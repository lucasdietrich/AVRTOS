use core::fmt::{Arguments, Write};

use crate::serial::Serial;

pub fn write_args(args: Arguments<'_>, nl: bool) {
    Serial.write_fmt(args).unwrap();

    if nl {
        Serial.write_str("\n").unwrap();
    }
}

#[macro_export]
macro_rules! print {
    () => {};
    ($($arg:tt)*) => {{
        $crate::stdio::write_args(format_args!($($arg)*), false)
    }};
}

#[macro_export]
macro_rules! println {
    () => {
        $crate::stdio::write_args(format_args!("\n"), false)
    };
    ($($arg:tt)*) => {{
        $crate::stdio::write_args(format_args!($($arg)*), true)
    }}
}
