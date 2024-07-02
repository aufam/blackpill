## STM32 Blackpill Template Project
Template project for STM32 blackpill Development

### Prerequisites
1. GNU ARM toolchain
    * Download the latest version:
    ```bash
    curl -O "https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz"
    ```
    * Extract to /usr/share/
    ```bash
    sudo tar xf arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz -C /usr/share/
    ```
    * Create links so that binaries are accessible system-wide
    ```bash
    sudo ln -s /usr/share/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc 
    sudo ln -s /usr/share/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-g++ /usr/bin/arm-none-eabi-g++
    sudo ln -s /usr/share/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gdb /usr/bin/arm-none-eabi-gdb
    sudo ln -s /usr/share/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-size /usr/bin/arm-none-eabi-size
    sudo ln -s /usr/share/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-ar /usr/bin/arm-none-eabi-ar
    sudo ln -s /usr/share/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-nm /usr/bin/arm-none-eabi-nm
    sudo ln -s /usr/share/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-objcopy /usr/bin/arm-none-eabi-objcopy
    sudo ln -s /usr/share/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-objdump /usr/bin/arm-none-eabi-objdump
    ```
2. CMake
    ```bash
    sudo apt install cmake
    ```
3. st-link
    ```bash
    sudo apt install stlink-tools
    ```
4. [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) (optional)

### Project structure
    .
    ├── CMakeLists.txt              # Build configuration
    ├── README.md                   # Project documentation
    ├── {$PROJECT_NAME}.ioc         # CubeMX generated code
    ├── Core/                       # CubeMX generated code
    ├── Drivers/                    # CubeMX generated code
    │ ├── ST/                       # CubeMX generated code
    │ ├── Third_Party/              # Submodules
    ├── USB_DEVICE/                 # CubeMX generated code
    ├── Project/                    # Kernel and apps
    │ ├── apps/                     # Apps source
    │ ├── main.cpp                  # Kernel init
    │ ├── main.hpp                  # Kernel header

### CubeMX Integration
You can modify the CubeMX-generated code by editing the ioc file and regenerating the code as needed using STM32CubeMX. 
This allows customization of hardware configurations and peripheral setups.

### Kernel Initialization
The kernel initialization is defined in Project/main.cpp and Project/main.hpp. 
You can modify these files to customize startup routines, configure peripherals, or initialize system-wide settings.

### Adding Application Sources
Additional application-specific source files can be added under Project/apps/. These files can contain your custom application logic, task definitions, or any other functionalities specific to your project.

### Build
```bash
mkdir build
cmake -B build
cmake --build build
```

### Flash (st-link)
```bash
cmake --build build --target flash
```

### Flash (DFU)
```bash
cmake --build build --target dfu
```
