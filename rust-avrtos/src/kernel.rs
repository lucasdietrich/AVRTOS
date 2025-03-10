use avrtos_sys as ll;

static mut KERNEL_INITIALIZED: bool = false;

#[derive(Debug, Clone, Copy)]
pub struct KernelInitParams {
    pub serial_baud: Option<u32>,
}

impl Default for KernelInitParams {
    fn default() -> Self {
        Self {
            serial_baud: Some(115200),
        }
    }
}

pub struct Kernel {}

impl Kernel {
    pub fn init(opts: KernelInitParams) -> Option<Self> {
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
