set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.16)

# specify cross-compilers and tools
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# project settings
project(blackpill C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# user defines
add_definitions(
    -DPERIPH_SYSTICK_TIM_BASE_SOURCE=TIM11
    -DETL_ASYNC_N_CHANNELS=8             # number of tasks that handle asynchronous functions
    -DETL_ASYNC_TASK_THREAD_SIZE=2048    # the size of each task
)

# enable assembler files preprocessing
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

# optimization flags
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g)
endif ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions -fpermissive")

# garbage collection
add_compile_options(-ffunction-sections -fdata-sections -fno-common)
add_link_options(-ffunction-sections -fdata-sections -fno-common)

# hardware floating point
add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)

# cpu
add_compile_options(-mcpu=cortex-m4 -mthumb -mthumb-interwork)
add_link_options(-mcpu=cortex-m4 -mthumb -mthumb-interwork)

# warnings
add_compile_options(-Wall -Wextra)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-literal-suffix -Wno-attributes")

# defines
add_definitions(-DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE)

# library
add_link_options(-specs=nano.specs -lm -lc)

# linker script
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F411CEUX_FLASH.ld)
set(MAP ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${MAP},--script=${LINKER_SCRIPT})

# include
include_directories(
    ${CMAKE_HOME_DIRECTORY}
    Core/Inc 
    Drivers/CMSIS/Device/ST/STM32F4xx/Include 
    Drivers/CMSIS/Include
    Drivers/STM32F4xx_HAL_Driver/Inc 
    Drivers/STM32F4xx_HAL_Driver/Inc/Legacy 
    Middlewares/Third_Party/FreeRTOS/Source/include 
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F 
    Middlewares/ST/STM32_USB_Device_Library/Core/Inc 
    Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc 
    USB_DEVICE/App 
    USB_DEVICE/Target 
    Project
)

# source files
file(GLOB_RECURSE SOURCES 
    Core/*.*
    Middlewares/ST/*.*
    Middlewares/Third_Party/FreeRTOS/*.*
    Drivers/*.*
    USB_DEVICE/*.*
    Project/*.*
)

# build elf
add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

# add etl
add_subdirectory(Middlewares/Third_Party/etl)
target_link_libraries(${PROJECT_NAME}.elf etl)

# add periph
add_subdirectory(Middlewares/Third_Party/stm32_hal_interface)
target_link_libraries(${PROJECT_NAME}.elf periph)

# add oled
add_subdirectory(Middlewares/Third_Party/stm32_oled)
target_link_libraries(${PROJECT_NAME}.elf oled)

# add wizchip
add_subdirectory(Middlewares/Third_Party/stm32_wizchip)
target_link_libraries(${PROJECT_NAME}.elf wizchip)

# add modbus
add_subdirectory(Middlewares/Third_Party/stm32_modbus)
target_link_libraries(${PROJECT_NAME}.elf modbus)

# build hex and bin files
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
add_custom_command(
    TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
    COMMENT "Building ${HEX_FILE}\nBuilding ${BIN_FILE}"
)

add_custom_target(flash
    COMMAND st-flash write ${BIN_FILE} 0x08000000
    DEPENDS ${BIN_FILE}
)

add_custom_target(dfu
    COMMAND dfu-util -a 0 -D ${BIN_FILE} -s 0x08000000:leave
    DEPENDS ${BIN_FILE}
)