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

### Used Peripherals
1. USB CDC (PA11, PA12)
2. UART1 (PA9, PA10)
3. SPI1 (PA5, PA6, PA7)
4. WIZCHIP 3300 (SPI=SPI1, CS=PA4, RST=PA1)

### Default Ethernet configuration
1. IP: `10.20.30.2`
2. Gateway: `10.20.30.1`
3. DNS: `10.20.30.1`

You can change the configuration in [main.cpp](Project/main.cpp)

### [Terminal](Project/apps/terminal.cpp)
Provides a terminal-based application for handling various commands 
and communication protocols.

Example terminal commands:
1. `tasks`
    * returns: number of available tasks
2. `echo ${1}`
    * returns: the first argument
3. `heap`
    * returns: FreeRTOS heap information
4. `async_test`
    * description: example for non blocking execution
    * returns: Ok if the tasks is available otherwise error
5. `add ${1} ${2}`
    * description: add two integers
6. `div ${1} ${2}`
    * description: add two integers
    * returns: `${1}` divided by `${2}` if `${2}` is not zero, otherwise zero division error

You can access the terminal via:
1. USB Serial, usually through `/dev/ttyACM0`
2. UART
3. UDP through `10.20.30.2:12345`

### CubeMX Integration
You can modify the CubeMX-generated code by editing the ioc file and regenerating the code as needed using STM32CubeMX. 
This allows customization of hardware configurations and peripheral setups.

### Kernel Initialization
The kernel initialization is defined in [main.cpp](Project/main.cpp). 
You can modify these files to customize startup routines, configure peripherals, or initialize system-wide settings.

### Adding Application Sources
Additional application-specific source files can be added under [apps](Project/apps/) folder. 
These files can contain your custom application logic, task definitions, or any other functionalities specific to your project.

### Clone
```bash
git clone https://github.com/aufam/blackpill.git --recurse
cmake -B build
cmake --build build
```

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
