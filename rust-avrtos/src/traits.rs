use crate::{duration::Duration, error::OsErr};

/// A synchronization primitive that allows threads to synchronize access to shared resources.
///
/// Implementations of this trait define how threads can synchronize using swap values,
/// enabling the creation of synchronization mechanisms like mutexes, semaphores, etc.
pub trait SyncPrimitive {
    /// The type of value that is passed between threads during synchronization.
    ///
    /// This type must implement the `Swappable` trait.
    type Swap;

    /// Attempts to acquire the synchronization primitive for the given thread.
    ///
    /// If the primitive is available, the thread acquires it and may receive a swap value.
    /// If the primitive is not available, the thread is made to wait until it can be notified.
    ///
    /// # Parameters
    ///
    /// - `thread`: A reference to the thread attempting to acquire the synchronization primitive.
    ///
    /// # Returns
    ///
    /// - `Ok(Self::Swap)` if the primitive was acquired and a swap value is available.
    /// - `Err(OsErr)` if an error occurred while attempting to acquire.
    fn acquire(&self, duration: Duration) -> Result<Self::Swap, OsErr>;

    /// Releases the synchronization primitive with the given swap value as
    /// no thread was waiting for the primitive.
    ///
    /// Typically called when a thread has finished using the resource protected
    /// by the synchronization primitive and no thread is waiting to acquire it.
    /// So the swap value is given back to the primitive.
    fn release(&self, released: Self::Swap) -> Result<(), Self::Swap>;
}
