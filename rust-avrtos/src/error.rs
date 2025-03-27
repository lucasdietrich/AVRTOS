use core::fmt;

use num_derive::FromPrimitive;
use num_traits::FromPrimitive;

#[repr(i8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, FromPrimitive)]
pub enum OsErr {
    EPERM = -1,
    ENOENT = -2,
    ESRCH = -3,
    EINTR = -4,
    EIO = -5,
    ENXIO = -6,
    E2BIG = -7,
    ENOEXEC = -8,
    EBADF = -9,
    ECHILD = -10,
    EAGAIN = -11,
    ENOMEM = -12,
    EACCES = -13,
    EFAULT = -14,
    ENOTBLK = -15,
    EBUSY = -16,
    EEXIST = -17,
    EXDEV = -18,
    ENODEV = -19,
    ENOTDIR = -20,
    EISDIR = -21,
    EINVAL = -22,
    ENFILE = -23,
    EMFILE = -24,
    ENOTTY = -25,
    ETXTBSY = -26,
    EFBIG = -27,
    ENOSPC = -28,
    ESPIPE = -29,
    EROFS = -30,
    EMLINK = -31,
    EPIPE = -32,
    EDOM = -33,
    ERANGE = -34,
    ENOMSG = -35,
    ENOTSUP = -95,
    ETIMEDOUT = -110,
    EALREADY = -114,
    EINPROGRESS = -115,
    ECANCELED = -125,
}

impl fmt::Display for OsErr {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?} ({})", self, *self as i8)
    }
}

impl core::error::Error for OsErr {}

impl OsErr {
    /// Converts an integer error code to an `OsErr` if it matches a known error.
    pub fn from_code(code: i8) -> Option<Self> {
        OsErr::from_i8(code)
    }

    /// Returns the integer representation of the error.
    pub fn to_code(self) -> i8 {
        self as i8
    }
}

pub fn os_error_to_result(err: i8) -> Result<(), OsErr> {
    match err {
        0 => Ok(()),
        _ => Err(OsErr::from_code(err).unwrap()),
    }
}
