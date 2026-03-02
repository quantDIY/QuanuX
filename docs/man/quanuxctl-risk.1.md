.\" Manpage for quanuxctl-risk.
.\" Contact quantDIY@duncan.com for corrections.
.TH QUANUXCTL-RISK 1 "March 2026" "1.0" "QuanuX Institutional Manual"
.SH NAME
quanuxctl-risk \- Control surface for the QuanuX Risk Kernel and Hydration Gate
.SH SYNOPSIS
\fBquanuxctl risk\fR [\fICOMMAND\fR] [\fIOPTIONS\fR] [\fIARGS\fR]
.SH DESCRIPTION
The \fBquanuxctl risk\fR command group provides a strictly stateless interface to manage and audit global capital exposure across the QuanuX execution mesh. It interacts solely with the CNATS JetStream unified log and Key-Value (KV) stores. It never connects directly to database endpoints or interrupts C++ sovereign execution threads via RPC.
.SH COMMANDS
.TP
\fBview-state\fR
Subscribes to the NATS JetStream KV store (Bucket: \fBRISK_STATE\fR) and pulls the real-time aggregated global exposure value (\fIglobal.notional.exposure\fR).
.br
This command proves the global state without ever locking C++ hot path memory or hitting a database query layer.
.TP
\fBupdate-cap\fR \fI<new_cap_value>\fR
Publishes a new daily notional cap limit to the subject \fIquanux.control.risk.cap\fR.
.br
The C++ \fBRiskKernel\fR instances employ asynchronous background threads that subscribe to this subject, ingesting the new cap and safely pushing it to the \fIdaily_notional_cap_\fR local RAM matrix variable without suspending the primary Execution Loop. The value must be formatted as an \fIint64\fR implied 2-decimal scalar (e.g., $1,000,000.00 is represented as 100000000).
.TP
\fBforce-hydrate\fR \fI<node_id>\fR
Commands a specific node (e.g., \fISFO-EXEC-01\fR) to execute the pessimistic Cold Gate. 
.br
The target node drops its \fIis_hot_\fR boolean to \fIfalse\fR, suspending local trading, performs a blocking thread read from the JetStream KV global state to re-seed its RAM matrix, and evaluates the SHA-256 hash before restoring execution.

.SH EXAMPLES
.PP
Audit the current JetStream KV Risk State:
.RS 4
$ quanuxctl risk view-state
.RE
.PP
Dynamically increase the global notional limit to $25,000,000:
.RS 4
$ quanuxctl risk update-cap 2500000000
.RE
.PP
Force a newly promoted Standby Node to re-poll and re-synchronize its local execution RAM matrix against the global JetStream record post-STONITH:
.RS 4
$ quanuxctl risk force-hydrate LON-EXEC-02
.RE

.SH AUTHOR
Architected by the QuanuX Intelligence Core natively for Operation Aegis.
.SH SEE ALSO
\fBquanuxctl-obs\fR(1), \fBquanuxctl\fR(1)
