.TH QUANUXCTL-CLUSTER 1 "March 2026" "QuanuX" "User Commands"
.SH NAME
quanuxctl-cluster \- Manage the QuanuX Tier 1 High Availability Cluster
.SH SYNOPSIS
.B quanuxctl cluster
.IR command
[ \fB\-\-help\fR ]
.SH DESCRIPTION
.B quanuxctl cluster
provides a direct interface to the NATS JetStream Control Plane and allows sysadmins to manually manage Raft elections, override KV locks, and enforce STONITH (Shoot The Other Node In The Head) fencing upon misbehaving nodes.
.SH COMMANDS
.TP
.B status
Queries the NATS JetStream `quanux.tier1.leader` lock to show real-time Leader/Follower telemetry across all cluster nodes. Returns current heartbeats and the locked Leader ID.
.TP
.B promote <node_id>
Forces a Raft election override, promoting the specified Follower \fInode_id\fR to Leader. Use this command if a Leader is dead but the automatic lock transition is hung.
.TP
.B demote
Forces the current Leader to drop the KV lock and step down, triggering standard election procedures.
.TP
.B fence <node_id>
Manually fires the Out-Of-Band (OOB) STONITH kill-pill API call to physically power off the specified \fInode_id\fR. Mandatory procedure for split-brain resolution.
.SH EXPERIMENTAL VALIDATION
These tools were directly verified via physical kinetic testing during the "DigitalOcean NYC/LON/SFO Chaos Experiment". For a complete repeatable tutorial of simulating a catastrophic Leader failure, BGP convergence ("The Long-Dark"), and the "Control Plane Genesis" (handling NATS BucketNotFoundError on edge node boot), refer to the 
.B HA_RUNBOOK.md
in the documentation repository.
.SH SEE ALSO
.BR quanuxctl (1),
.BR docs/operations/HA_RUNBOOK.md
