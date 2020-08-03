PROJECT_NAME       := nrf5-bt-sensor-receiver
PROJECT_DIR_NAME   := $(shell dirname "$(strip $(MAKEFILE_LIST))")
PLATFORM_FAMILY    := NRF52
PLATFORM           := NRF52832_XXAA

TARGETS            := $(PLATFORM)
OUTPUT_DIRECTORY   := build
LINKER_SCRIPT      := gcc_nrf52_no_softdevice.ld

SDK_ROOT           := $(HOME)/nonbackupped/nRF5_SDK_17.0.0
PROJ_DIR           := .
SRC_DIR            := $(PROJ_DIR)/src
TEMPLATE_PATH      := $(SDK_ROOT)/components/toolchain/gcc
SDK_CONFIG_FILE    := $(SRC_DIR)/sdk_config.h

include $(TEMPLATE_PATH)/Makefile.common.jihartik

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uarte.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_wdt.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52.S \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c \
  $(SRC_DIR)/main.cpp \
  $(SRC_DIR)/radio.cpp \
  $(SRC_DIR)/util.cpp \
  $(SRC_DIR)/channel_resolver.cpp \
  $(SRC_DIR)/packet_queue.cpp \

INC_FOLDERS += \
  $(SDK_ROOT)/components/softdevice/s132/headers \
  $(SDK_ROOT)/components/softdevice/s132/headers/nrf52 \

INC_FOLDERS += \
  $(SRC_DIR) \
  $(SDK_ROOT)/components/ble/ble_services/ble_ancs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias_c \
  $(SDK_ROOT)/components/ble/ble_advertising \
  $(SDK_ROOT)/components/ble/ble_db_discovery \
  $(SDK_ROOT)/components/libraries/pwm \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/generic \
  $(SDK_ROOT)/components/libraries/usbd/class/msc \
  $(SDK_ROOT)/components/libraries/usbd/class/hid \
  $(SDK_ROOT)/components/ble/ble_services/ble_gls \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/libraries/gpiote \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs_c \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/ble/ble_dtm \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs_c \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/kbd \
  $(SDK_ROOT)/components/ble/ble_services/ble_lls \
  $(SDK_ROOT)/components/libraries/bsp \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/ble/ble_services/ble_ans_c \
  $(SDK_ROOT)/components/libraries/slip \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/libraries/csense_drv \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/mouse \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs \
  $(SDK_ROOT)/components/ble/ble_services/ble_hts \
  $(SDK_ROOT)/components/libraries/crc16 \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc \
  $(SDK_ROOT)/components/libraries/csense \
  $(SDK_ROOT)/components/libraries/low_power_pwm \
  $(SDK_ROOT)/components/libraries/hardfault \
  $(SDK_ROOT)/components/ble/ble_services/ble_cscs \
  $(SDK_ROOT)/components/libraries/uart \
  $(SDK_ROOT)/components/libraries/hci \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/libraries/led_softblink \
  $(SDK_ROOT)/components/ble/ble_services/ble_cts_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus \
  $(SDK_ROOT)/components/ble/ble_services/ble_hids \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/usbd/class/audio \
  $(SDK_ROOT)/components/ble/peer_manager \
  $(SDK_ROOT)/components/libraries/mem_manager \
  $(SDK_ROOT)/components/ble/ble_services/ble_tps \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/libraries/usbd \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs_c \
  $(SDK_ROOT)/components/ble/ble_racp \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/libraries/fds \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus \
  $(SDK_ROOT)/components/libraries/pwr_mgmt \
  $(SDK_ROOT)/components/libraries/ecc \
  $(SDK_ROOT)/components/libraries/sha256 \
  $(SDK_ROOT)/components/libraries/svc \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/external/fprintf \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components/softdevice/common \
  $(SDK_ROOT)/components/libraries/mutex \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(SDK_ROOT)/modules/nrfx \
  $(SDK_ROOT)/integration/nrfx \
  $(SDK_ROOT)/modules/nrfx/drivers/include

LIB_FILES += \
  -lstdc++ \

# Optimization flags
OPT = -Wall -O3 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -D$(PLATFORM_FAMILY)
CFLAGS += -D$(PLATFORM)
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -D__HEAP_SIZE=0
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums
CFLAGS += -DSVC_INTERFACE_CALL_AS_NORMAL_FUNCTION

# C++ flags common to all targets
CXXFLAGS += -std=c++17
CXXFLAGS += -Wno-register

# Assembler flags common to all targets
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -D$(PLATFORM_FAMILY)
ASMFLAGS += -D$(PLATFORM)
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DBOARD_CUSTOM
ASMFLAGS += -DBSP_DEFINES_ONLY
ASMFLAGS += -D__HEAP_SIZE=0
ASMFLAGS += -DSVC_INTERFACE_CALL_AS_NORMAL_FUNCTION

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT) -L$(SDK_ROOT)/modules/nrfx/mdk
LDFLAGS += -Wl,--gc-sections
LDFLAGS += --specs=nano.specs -lc -lnosys
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16


.PHONY: $(TARGETS) default all clean flash erase

# Make default (=first) target -> needs to be before including Makefile.common
default: clean_linked_binaries
	@$(MAKE) -j8 $(PLATFORM)

include $(TEMPLATE_PATH)/Makefile.common
$(foreach target, $(TARGETS), $(call define_target, $(target)))

# Flash the program
flash_app: default
	$(call flash,$(OUTPUT_FILE).hex)

flash: flash_app

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
