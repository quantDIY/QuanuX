import asyncio
import os
import nats
import h5py
import numpy as np
import time

# Pillar III & IV: The Sovereign Vault
# HDF5 SWMR Binary Journaler for 1:1 Reconciliation Parity

VAULT_PATH = "critique_vault.h5"

async def main():
    print("[Annex] Booting quanux-vault (The Sovereign Vault).")
    
    # Initialize HDF5 file in SWMR mode
    # SWMR demands the file to be created using the 'latest' library format bounds
    f = h5py.File(VAULT_PATH, 'a', libver='latest')
    
    # Enable Single Writer Multiple Reader mode
    # This guarantees zero-lock concurrent reading by auditing processes while the QuanuX-Sentinel is firing.
    f.swmr_mode = True
    print("[Annex] HDF5 Vault established in SWMR mode. Lock-free reading enabled.")

    # Datatype for raw binary packets (approx 64 bytes)
    # Storing as opaque vectors to strictly preserve the L1 alignment physics
    dt = h5py.vlen_dtype(np.dtype('uint8'))

    try:
        nc = await nats.connect("nats://localhost:4222")
        print("[Annex] Connected to NATS.")

        async def message_handler(msg):
            subject = msg.subject  # e.g., MARKET.BIN.ES_M4 or EXECUTION.BIN
            data = msg.data
            
            # Schema: Group datasets by Epoch -> InstrumentID
            epoch_str = time.strftime("epoch_%Y%m%d")
            safe_subject = subject.replace('.', '_')
            dataset_path = f"/{epoch_str}/{safe_subject}"
            
            # Ensure hierarchical structure exists
            if f"/{epoch_str}" not in f:
                f.create_group(f"/{epoch_str}")
                
            if dataset_path not in f:
                # Create flexible dataset capable of infinite dimension scaling
                grp = f[f"/{epoch_str}"]
                dset = grp.create_dataset(safe_subject, shape=(0,), maxshape=(None,), dtype=dt, chunks=True)
            else:
                dset = f[dataset_path]

            # Append the raw binary sequence exactly as ingested off the Spreader wire
            curr_len = dset.shape[0]
            dset.resize((curr_len + 1,))
            dset[curr_len] = np.frombuffer(data, dtype=np.uint8)
            
            # Flush pipeline to strictly commit bit-state for parallel SWMR Readers
            dset.flush()

        await nc.subscribe("MARKET.BIN.>", cb=message_handler)
        await nc.subscribe("EXECUTION.BIN.>", cb=message_handler)
        print("[Annex] Sovereignty Bound. Ingesting MARKET.BIN.> and EXECUTION.BIN.>")

        while True:
            await asyncio.sleep(1)

    except Exception as e:
        print(f"[Annex] Vault Seal Compromised: {e}")
    finally:
        f.close()

if __name__ == '__main__':
    asyncio.run(main())
