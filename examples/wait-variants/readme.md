# Build for QEMU

Build in release mode for QEMU (debug won't work) but it compiles and runs:

    QEMU=ON SAMPLE=wait-variants CMAKE_BUILD_TYPE=Release make single