# Development STM32F4 blackpill using CubeMx

Main program is in [project.cc](Project/project.cc).
You can modify ioc file and [periph](Project/periph) folder to suit your needs.

## Build
````
mkdir build
cmake -B build
make -C build
````

## flash (st-link)
````
make flash -C build
````

## flash (DFU)
````
make dfu -C build
````