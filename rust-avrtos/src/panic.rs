use core::panic::PanicInfo;

const RUST_PANIC_CODE: u8 = 6;

#[inline(never)]
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    unsafe {
        avrtos_sys::__fault(RUST_PANIC_CODE);
        // __fault() should not return
    }
    loop {}
}
