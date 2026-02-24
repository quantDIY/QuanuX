use bytemuck::{Pod, Zeroable};
use serde::Serialize;

#[repr(C, align(64))]
#[derive(Debug, Copy, Clone, Pod, Zeroable, Serialize)]
pub struct MarketTick {
    pub local_rec_ts: u64,
    pub exchange_ts: u64,
    pub price: f64,
    pub size: u32,
    pub flags: u32,
    pub instrument_id: u32,
    // Explicit padding to ensure bytemuck can derive Pod safely
    #[serde(skip)]
    pub _padding1: u32,
    pub internal_arrival_ts: u64,
    pub processing_start_ts: u64,
    #[serde(skip)]
    pub _pad: [u8; 8],
}
