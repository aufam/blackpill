## STM32 Blackpill Template Project

Template project for STM32 blackpill Development
using CubeMx, CMake and C++17.
Main program is in [project.cc](Project/project.cc).
You can modify [ioc file](blackpill.ioc) to suit your needs.

### Prerequisites
1. GNU ARM toolchain
    * Download the latest version from the
      [website](https://developer.arm.com/downloads/-/gnu-rm)
    * Extract to /usr/share/
    ```bash
    sudo tar xjf gcc-arm-none-eabi-VERSION.bz2 -C /usr/share/
    ```
    * Create links so that binaries are accessible system-wide
    ```bash
    sudo ln -s /usr/share/gcc-arm-none-eabi-VERSION/bin/arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc 
    sudo ln -s /usr/share/gcc-arm-none-eabi-VERSION/bin/arm-none-eabi-g++ /usr/bin/arm-none-eabi-g++
    sudo ln -s /usr/share/gcc-arm-none-eabi-VERSION/bin/arm-none-eabi-gdb /usr/bin/arm-none-eabi-gdb
    sudo ln -s /usr/share/gcc-arm-none-eabi-VERSION/bin/arm-none-eabi-size /usr/bin/arm-none-eabi-size
    sudo ln -s /usr/share/gcc-arm-none-eabi-VERSION/bin/arm-none-eabi-ar /usr/bin/arm-none-eabi-ar
    sudo ln -s /usr/share/gcc-arm-none-eabi-VERSION/bin/arm-none-eabi-nm /usr/bin/arm-none-eabi-nm
    sudo ln -s /usr/share/gcc-arm-none-eabi-VERSION/bin/arm-none-eabi-objcopy /usr/bin/arm-none-eabi-objcopy
    sudo ln -s /usr/share/gcc-arm-none-eabi-VERSION/bin/arm-none-eabi-objdump /usr/bin/arm-none-eabi-objdump
    ```
    * install dependencies (if any)
    ```bash
    sudo apt install libncurses-dev
    ```

2. CMake
    ```bash
    sudo apt install cmake
    ```
3. st-link
    ```bash
    git clone https://github.com/stlink-org/stlink
    cd stlink
    mkdir build
    cd build
    cmake ..
    make
    sudo make install
    ```
4. [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
   (optional)

### Build
```bash
mkdir build
cmake -B build
make -C build
```

### flash (st-link)
```bash
make flash -C build
```