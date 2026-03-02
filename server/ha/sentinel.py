import asyncio
import logging
from typing import Optional
import nats
from nats.js.api import KeyValueConfig
import nats.js.errors

logger = logging.getLogger("quanux.sentinel")

class GlobalSentinelLoop:
    """
    Anchors High Availability cluster state to NATS JetStream.
    Executes the 2000ms STONITH hard-timeout upon lock acquisition.
    """
    def __init__(self, node_id: str, nats_url: str = "nats://localhost:4222"):
        self.node_id = node_id
        self.nats_url = nats_url
        self.nc = None
        self.js = None
        self.kv = None
        self._is_leader = False
        self._running = False
        self._heartbeat_task: Optional[asyncio.Task] = None
        self._watcher_task: Optional[asyncio.Task] = None

    async def connect(self):
        self.nc = await nats.connect(self.nats_url)
        self.js = self.nc.jetstream()
        # Initialize KV store if not exists. ttl=5s enforcing quick failover
        self.kv = await self.js.create_key_value(
            KeyValueConfig(bucket="quanux_tier1", ttl=5)
        )
        self._running = True

    async def disconnect(self):
        self._running = False
        if self._heartbeat_task:
            self._heartbeat_task.cancel()
        if self._watcher_task:
            self._watcher_task.cancel()
        if self.nc:
            await self.nc.close()

    async def execute_stonith(self, old_leader_id: str):
        """
        Primary Mandate: Out-Of-Band hardware power-kill (Apoptosis) of the old Leader.
        Must enforce a 2000ms hard-timeout.
        """
        logger.warning(f"STONITH INITIATED Against {old_leader_id}. Firing OOB Kill Pill.")
        
        async def _kill_pill():
            # In a real system, this hits DigitalOcean API or IPMI to power kill
            # e.g., POST https://api.digitalocean.com/v2/droplets/{old_leader_id}/actions {"type":"power_off"}
            logger.info(f"OOB API Call: Power off node {old_leader_id}")
            await asyncio.sleep(0.5) # Simulate API latency
            logger.warning(f"Node {old_leader_id} successfully terminated.")

        try:
            # Enforce the strict 2000ms hard-timeout
            await asyncio.wait_for(_kill_pill(), timeout=2.0)
            logger.info("STONITH Completed Successfully. Split-brain avoided.")
        except asyncio.TimeoutError:
            logger.critical("STONITH FAILED: 2000ms Timeout Exceeded. Split-brain imminent!")
            raise RuntimeError("STONITH Hard Timeout Exceeded")

    async def watchdog(self):
        """
        Watcher loop. Subscribes to the kv lock and attempts to acquire it if it becomes free.
        """
        try:
            watcher = await self.kv.watch("quanux.tier1.leader")
            async for entry in watcher:
                if entry is None:
                    continue
                # If the lock is held by someone else
                if entry.operation == nats.js.api.KeyValue.Put:
                    leader_id = entry.value.decode()
                    if leader_id != self.node_id:
                        self._is_leader = False
                        logger.info(f"Follower state: Leader is currently {leader_id}")
                
                 # If lock is deleted/purged, try to claim it
                elif entry.operation in (nats.js.api.KeyValue.Delete, nats.js.api.KeyValue.Purge):
                    logger.warning("KV Lock dropped! Initiating Raft election override.")
                    await self._attempt_leadership()

        except asyncio.CancelledError:
            pass
        except Exception as e:
            logger.error(f"Watchdog error: {e}")

    async def _attempt_leadership(self):
        try:
            # Try to grab the lock state to ensure it's still available
            try:
                old_entry = await self.kv.get("quanux.tier1.leader")
            except nats.js.errors.KeyNotFoundError:
                old_entry = None
            
            old_leader = old_entry.value.decode() if old_entry else "UNKNOWN"
            
            # Use put/create based locking; JetStream KV has optimistic concurrency via revisions
            rev = old_entry.revision if old_entry else 0
            await self.kv.update("quanux.tier1.leader", self.node_id.encode(), rev)
            
            try:
                # Primary Mandate: STONITH the old leader if we aren't bootstrapping
                if old_leader not in ("UNKNOWN", self.node_id):
                    await self.execute_stonith(old_leader)

                # ONLY IF STONITH succeeds (or if the old leader was UNKNOWN/self)
                self._is_leader = True
                logger.warning(f"Successfully promoted to Leader. Old leader was {old_leader}.")
            
            except Exception as e:
                # The Law of Verified Death: If STONITH fails, drop the crown
                logger.critical(f"STONITH FAILED: {e}. Dropping lock and falling back to Follower state.")
                self._is_leader = False
                await self.kv.delete("quanux.tier1.leader")
                return

        except nats.js.errors.KeyWrongLastSequenceError:
            # Another node beat us to the update
            logger.info("Lost election tie-breaker to another node.")
        except Exception as e:
            logger.error(f"Failed to acquire leadership: {e}")

    async def heartbeat(self):
        """
        Emits heartbeat if we are the leader to keep the TTL lock alive.
        """
        while self._running:
            if self._is_leader:
                try:
                    # Refresh the lock
                    entry = await self.kv.get("quanux.tier1.leader")
                    if entry and entry.value.decode() == self.node_id:
                        await self.kv.update("quanux.tier1.leader", self.node_id.encode(), entry.revision)
                except nats.js.errors.KeyNotFoundError:
                    # We lost the lock natively? Re-init
                    await self.kv.put("quanux.tier1.leader", self.node_id.encode())
                except nats.js.errors.KeyWrongLastSequenceError:
                    self._is_leader = False
                    logger.warning("KV constraint violation. Demoting to Follower.")
            await asyncio.sleep(1) # Heartbeat interval

    async def start(self):
        await self.connect()
        self._watcher_task = asyncio.create_task(self.watchdog())
        self._heartbeat_task = asyncio.create_task(self.heartbeat())
        
        # Initial boot check - try to claim immediately if no one has it
        try:
            await self.kv.get("quanux.tier1.leader")
        except nats.js.errors.KeyNotFoundError:
            await self._attempt_leadership()
