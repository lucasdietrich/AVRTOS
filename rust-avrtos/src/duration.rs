pub struct Duration(u32);

impl Duration {
    pub fn from_millis(millis: u32) -> Self {
        Duration(millis)
    }

    pub fn from_micros(micros: u32) -> Self {
        Duration(micros / 1000)
    }

    pub fn from_secs(secs: u32) -> Self {
        Duration(secs * 1000)
    }

    pub fn as_millis(&self) -> u32 {
        self.0
    }

    pub fn as_ticks(&self) -> u32 {
        todo!()
    }
}
