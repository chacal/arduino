PROJECT_NAME       := nrf5-autopilot-remote
PROJECT_DIR_NAME   := $(shell dirname "$(strip $(MAKEFILE_LIST))")
PLATFORM_FAMILY    := NRF52
PLATFORM           := NRF52832_XXAA

TARGETS            := $(PLATFORM)
OUTPUT_DIRECTORY   := build
LINKER_SCRIPT      := gcc_nrf52_s132.ld

SDK_ROOT           := $(HOME)/nonbackupped/nRF5_SDK_17.0.0
SOFTDEVICE_HEX     := $(SDK_ROOT)/components/softdevice/s132/hex/s132_nrf52_7.0.1_softdevice.hex
PROJ_DIR           := .
SRC_DIR            := $(PROJ_DIR)/src
TEMPLATE_PATH      := $(SDK_ROOT)/components/toolchain/gcc
SDK_CONFIG_FILE    := $(SRC_DIR)/sdk_config.h

include $(TEMPLATE_PATH)/Makefile.common.jihartik

DFU_HW_VERSION     := 52
# S132 v7.0.1, see https://github.com/NordicSemiconductor/pc-nrfutil#commands
DFU_SOFT_DEVICE_ID := 0xCB
DFU_PRIVATE_KEY    := private.key

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52.S \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_saadc.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_uart.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/components/libraries/crc32/crc32.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer2.c \
  $(SDK_ROOT)/components/libraries/timer/drv_rtc.c \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/components/libraries/sortlist/nrf_sortlist.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SRC_DIR)/ble.cpp \
  $(SRC_DIR)/buttons.cpp \
  $(SRC_DIR)/vcc_measurement.cpp \
  $(SRC_DIR)/main.cpp \

INC_FOLDERS += \
  ${SDK_ROOT}/modules/nrfx \
  ${SDK_ROOT}/modules/nrfx/mdk \
  ${SDK_ROOT}/modules/nrfx/hal \
  ${SDK_ROOT}/modules/nrfx/drivers/include \
  ${SDK_ROOT}/integration/nrfx \
  ${SDK_ROOT}/integration/nrfx/legacy \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/ble/common \
  ${SDK_ROOT}/components/libraries/util \
  ${SDK_ROOT}/components/libraries/strerror \
  ${SDK_ROOT}/components/libraries/delay \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/libraries/sortlist \
  $(SDK_ROOT)/components/libraries/atomic_fifo \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/softdevice/common \
  $(SDK_ROOT)/components/softdevice/s132/headers \
  $(SDK_ROOT)/components/softdevice/s132/headers/nrf52 \
  $(SDK_ROOT)/external/fprintf \
  $(SDK_ROOT)/external/segger_rtt \
  $(SRC_DIR) \

LIB_FILES += \
  -lstdc++ \
  -lm

# Optimization flags
OPT = -Wall -O3 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
# CFLAGS += -DDEBUG
CFLAGS += -DAPP_TIMER_V2
CFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -D$(PLATFORM_FAMILY)
CFLAGS += -D$(PLATFORM)
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DNRF52_PAN_74
CFLAGS += -DNRF_SD_BLE_API_VERSION=7
CFLAGS += -DS132
CFLAGS += -DSOFTDEVICE_PRESENT

CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -D__HEAP_SIZE=0
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)
CXXFLAGS += -std=c++17
CXXFLAGS += -Wno-register

# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DAPP_TIMER_V2
ASMFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
ASMFLAGS += -DBOARD_CUSTOM
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -D$(PLATFORM_FAMILY)
ASMFLAGS += -D$(PLATFORM)
ASMFLAGS += -DNRF52_PAN_74
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=7
ASMFLAGS += -DS132
ASMFLAGS += -DSOFTDEVICE_PRESENT

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -Wl,--gc-sections
LDFLAGS += --specs=nano.specs -lc -lnosys
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16


.PHONY: $(TARGETS) default all clean flash erase

# Make default (=first) target -> needs to be before including Makefile.common
default: build_app

include $(TEMPLATE_PATH)/Makefile.common
$(foreach target, $(TARGETS), $(call define_target, $(target)))

build_app: clean_linked_binaries
	@$(MAKE) -j12 $(TARGETS)

# Flash the program
flash_app: build_app
	$(call flash,$(OUTPUT_FILE).hex)

flash_softdevice:
	@echo Flashing Soft Device..
	$(call flash,$(SOFTDEVICE_HEX))

flash: flash_softdevice flash_app

erase:
	$(call erase)

$(OUTPUT_DIRECTORY)/.clean_linked_binaries: $(PROJ_DIR)/$(LINKER_SCRIPT)
	@echo Cleaning linked binaries..
	@rm -f $(OUTPUT_DIRECTORY)/*.hex
	@rm -f $(OUTPUT_DIRECTORY)/*.bin
	@rm -f $(OUTPUT_DIRECTORY)/*.out
	@touch $@

clean_linked_binaries: $(OUTPUT_DIRECTORY) $(OUTPUT_DIRECTORY)/.clean_linked_binaries

sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
