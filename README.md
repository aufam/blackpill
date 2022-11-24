## Development STM32F4 blackpill using CubeMx

Main program is in [project.cc](Project/project.cc).
You can still modify [ioc file](blackpill.ioc) and [periph folder](Project/periph) to suit your needs.

### Prerequisites
1. ARM toolchain
2. cmake minimum version 3.16
3. st-link (optional)
4. dfu-util (optional)
5. CubeMX (optional)

### Build
````
mkdir build
cmake -B build
make -C build
````

### flash (st-link)
````
make flash -C build
````

### flash (DFU)
````
make dfu -C build
````