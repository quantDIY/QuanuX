import struct
from omega_decoder import PyDecoderRing

def test_decoder():
    # Construct a mock CME MDP3 Execution Report byte buffer.
    # Layout matches CmeExecutionReportSbe:
    # 64-bit seq(1234), 64-bit ord(5678), 64-bit exec(9012)
    # 20-byte string cl_ord_id
    # 64-bit transact_time
    # 8-bit template_id, 8-bit ord_status
    # 16-bit unmapped_native_code
    
    seq = 1234
    ord_id = 5678
    exec_id = 9012
    cl_ord = b"ORD-999".ljust(20, b'\x00')
    time_nanos = 167888999000000000
    tid = 1
    status = 2 # Filled
    unmapped = 42
    
    # Pack exactly as binary
    raw_payload = struct.pack(f"<QQQ20sQBBH", seq, ord_id, exec_id, cl_ord, time_nanos, tid, status, unmapped)
    
    print(f"Mocking payload bytes: {len(raw_payload)} bytes")
    
    # Pass to DecoderRing 
    # VenueId::CmeMdp3 = 1
    result = PyDecoderRing.decode_payload(1, raw_payload)
    
    if result:
        print("[SUCCESS] Annex successfully invoked Omega DecoderRing and generated an OmegaEventEnvelope off the hot path.")
    else:
        print("[FAILED] DecoderRing rejected the payload or venue.")

if __name__ == "__main__":
    test_decoder()
