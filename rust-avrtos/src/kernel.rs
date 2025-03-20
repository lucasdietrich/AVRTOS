use avrtos_sys::{self as ll, k_msleep, z_irq_lock, z_irq_unlock, z_yield};

use crate::{critical_section::critical_section, duration::Duration};

static mut KERNEL_INITIALIZED: bool = false;

#[derive(Debug, Clone, Copy)]
pub struct KernelParams {
    pub serial_baud: Option<u32>,
}

impl Default for KernelParams {
    fn default() -> Self {
        Self {
            serial_baud: Some(115200),
        }
    }
}

pub struct Kernel {}

impl Kernel {
    pub fn init() -> Option<Self> {
        Self::init_with_params(KernelParams::default())
    }

    pub fn init_with_params(opts: KernelParams) -> Option<Self> {
        unsafe {
            if KERNEL_INITIALIZED {
                return None;
            }

            KERNEL_INITIALIZED = true;
        }

        unsafe {
            ll::z_avrtos_init();
        }

        if let Some(baud) = opts.serial_baud {
            unsafe {
                ll::serial_init_baud(baud);
            }
        }

        Some(Self {})
    }
}

#[inline(always)]
pub fn sleep(duration: Duration) {
    unsafe {
        k_msleep(duration.as_millis());
    }
}

#[inline(always)]
pub fn yeet() {
    critical_section(|| unsafe { z_yield() });
}
