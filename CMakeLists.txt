cmake_minimum_required(VERSION 3.13...3.27)

# verify if the environment variables are setted correctly
message("PICO_SDK_PATH is set to: ${PICO_SDK_PATH}")
message("PICO_BOARD: ${PICO_BOARD}")

# set the SOURCE_CODE variable to the name of the C file
SET(SOURCE_CODE "code")

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(${SOURCE_CODE}	C CXX ASM)

# Initialize the Raspberry Pi Pico PICO_SDK_PATH
pico_sdk_init()

add_executable(${SOURCE_CODE}
	${SOURCE_CODE}.c
)

pico_enable_stdio_usb(${SOURCE_CODE} 1)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(${SOURCE_CODE}
						pico_stdlib
						pico_time
						hardware_timer
						hardware_clocks
						hardware_pll
						hardware_irq
						hardware_uart
						hardware_spi
						hardware_i2c
						hardware_pwm
						hardware_adc

					  )

# Create map/bin/hex/uf2 file in addition to ELF
pico_add_extra_outputs(${SOURCE_CODE})











