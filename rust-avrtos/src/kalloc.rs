use core::alloc::{GlobalAlloc, Layout};

use crate::{println, sys as ll};

struct KernelAlloc;

unsafe impl GlobalAlloc for KernelAlloc {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        println!(
            "z_malloc size: {}, align: {}",
            layout.size(),
            layout.align()
        );
        let ptr = ll::z_malloc(layout.size() as u16, layout.align() as u8);
        ptr as *mut u8
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        ll::k_free(ptr as *mut core::ffi::c_void);
    }
}

#[global_allocator]
static Z_KERNEL_ALLOCATOR: KernelAlloc = KernelAlloc;

pub fn heap_stats() -> (u16, u16, u16) {
    let mut total: u16 = 0;
    let mut used: u16 = 0;
    let mut free: u16 = 0;

    unsafe {
        ll::k_global_allocator_stats_get(&mut total, &mut used, &mut free);
    }

    (total, used, free)
}
