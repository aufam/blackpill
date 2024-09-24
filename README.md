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
3. WIZCHIP 3300 (SPI=SPI1(PA5, PA6, PA7), CS=PA4, RST=PA1)

### [Ethernet configuration](Project/apps/ip_config.cpp)
Default settings:
1. MAC Address: `aa:bb:cc:dd:ee:ff`
2. IP Address: `10.20.30.2`
3. Subnet Mask (SN): `255.255.255.0`
4. Gateway: `10.20.30.1`
5. DNS: `10.20.30.1`

The configuration is stored in Flash memory. 
You can modify these settings using the following terminal commands: 
`set-mac`, `set-ip`, `set-sn`, `set-gw`, and `set-dns` respectively. 
Please note, changes will take effect only after rebooting the device.

### [Terminal command](Project/apps/terminal.cpp)
Provides a terminal-based application for handling various commands 
and communication protocols.

Example terminal commands:
1. `tasks`
    * returns: number of available tasks
2. `heap`
    * returns: FreeRTOS heap information
2. `reboot`
    * description: reboot the device
3. `echo ${1}`
    * returns: the first argument
4. `async_test`
    * description: example for non blocking execution
    * returns: Ok if the tasks is available otherwise error
5. `add ${1} ${2}`
    * description: add two integers
6. `div ${1} ${2}`
    * description: divide float `${1}` by `${2}` 
    * returns: `${1}` divided by `${2}` if `${2}` is not zero, otherwise zero division error
7. `dfu-bootloader`
    * description: jump to DFU bootloader

You can access the terminal via:
1. USB Serial, usually through `/dev/ttyACM0`
2. UART with baud rate 9600
3. UDP through `10.20.30.2:12345`

### CubeMX Integration
You can modify the CubeMX-generated code by editing the ioc file and regenerating the code as needed using STM32CubeMX. 
This allows customization of hardware configurations and peripheral setups.

### Kernel Initialization
The kernel initialization is defined in [main.cpp](Project/main.cpp). 
You can modify these files to customize startup routines, configure peripherals, or initialize system-wide settings.

### Adding Application Sources
Additional application-specific source files can be added under [apps](Project/apps/) folder. 
These files can contain your custom application logic, task definitions, 
or any other functionalities specific to your project. 
You can use the `APP` macro to ensure that the application logic is executed during initialization, 
or `APP_ASYNC` to run it in a separate thread. For example:
```c++
#include <apps/app.h>
#include <etl/async.h> // for APP_ASYNC
// include other necessary headers

APP(my_app) {
    // app logic here
}

APP_ASYNC(my_async_app) {
    // asynchronous app logic initialization
    for(;;) {
        // app logic loop
    }
}
```

### Clone
```bash
git clone https://github.com/aufam/blackpill.git --recurse
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
