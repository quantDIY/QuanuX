use bytemuck::{Pod, Zeroable};
use serde::{Deserialize, Serialize};

#[repr(C, align(64))]
#[derive(Debug, Copy, Clone, Pod, Zeroable, Serialize, Deserialize)]
pub struct CommandPacket {
    pub timestamp_tsc: u64,
    pub command_type: u32,
    pub signature: u32,
    #[serde(skip)]
    pub _pad1: [u8; 32],
    #[serde(skip)]
    pub _pad2: [u8; 16],
}
