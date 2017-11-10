PROJECT_NAME       := nrf5-scanner-test
DFU_APP_VERSION    := 1
DFU_APP_VERSION_STR:= 1.0.0
PLATFORM_FAMILY    := NRF52
PLATFORM           := NRF52832
#PLATFORM_FAMILY    := NRF51
#PLATFORM           := NRF51822

TARGETS            := $(PLATFORM)_xxaa
OUTPUT_DIRECTORY   := build
BMP_DEV            := /dev/cu.usbmodemBEE7BD01
ifeq ($(PLATFORM),NRF51822)
LINKER_SCRIPT      := gcc_nrf51_s130.ld
else
LINKER_SCRIPT      := gcc_nrf52_s132.ld
endif

SDK_ROOT           := $(HOME)/nonbackupped/nRF5_SDK_12.3.0
ifeq ($(PLATFORM),NRF51822)
SOFTDEVICE_HEX     := $(SDK_ROOT)/components/softdevice/s130/hex/s130_nrf51_2.0.1_softdevice.hex
else
SOFTDEVICE_HEX     := $(SDK_ROOT)/components/softdevice/s132/hex/s132_nrf52_3.0.0_softdevice.hex
endif
PROJ_DIR           := .
SRC_DIR            := $(PROJ_DIR)/src
TEMPLATE_PATH      := $(SDK_ROOT)/components/toolchain/gcc

GDB                 = "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gdb"
GDB_TARGET          = $(GDB) -ex "target extended-remote $(BMP_DEV)"

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/util/sdk_mapped_flags.c \
  $(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/util/sdk_errors.c \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c \
  $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd/nrf_nvic.c \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd/nrf_soc.c \
  $(SRC_DIR)/main.c \
  $(SRC_DIR)/vcc_measurement.c \
  $(SRC_DIR)/bmp180.c \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler/softdevice_handler.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/drivers_nrf/adc/nrf_drv_adc.c \
  $(SDK_ROOT)/components/drivers_nrf/hal/nrf_saadc.c \
  $(SDK_ROOT)/components/drivers_nrf/saadc/nrf_drv_saadc.c \
  $(SDK_ROOT)/components/drivers_nrf/twi_master/nrf_drv_twi.c \
  $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c \

ifeq ($(PLATFORM),NRF51822)
SRC_FILES += \
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf51.S \
  $(SDK_ROOT)/components/toolchain/system_nrf51.c
else
SRC_FILES += \
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf52.S \
  $(SDK_ROOT)/components/toolchain/system_nrf52.c
endif


ifeq ($(PLATFORM),NRF51822)
INC_FOLDERS += \
  $(SDK_ROOT)/components/softdevice/s130/headers \
  $(SDK_ROOT)/components/softdevice/s130/headers/nrf51 \
  $(SRC_DIR)/sdk_config_nrf51
else
INC_FOLDERS += \
  $(SDK_ROOT)/components/softdevice/s132/headers \
  $(SDK_ROOT)/components/softdevice/s132/headers/nrf52 \
  $(SRC_DIR)/sdk_config_nrf52
endif

# Include folders common to all targets
INC_FOLDERS += \
  $(SRC_DIR) \
  $(SDK_ROOT)/components/drivers_nrf/comp \
  $(SDK_ROOT)/components/drivers_nrf/twi_master \
  $(SDK_ROOT)/components/ble/ble_services/ble_ancs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias_c \
  $(SDK_ROOT)/components/ble/ble_advertising \
  $(SDK_ROOT)/components/ble/ble_db_discovery \
  $(SDK_ROOT)/components/libraries/pwm \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/generic \
  $(SDK_ROOT)/components/libraries/usbd/class/msc \
  $(SDK_ROOT)/components/libraries/usbd/class/hid \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/ble/ble_services/ble_gls \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/drivers_nrf/i2s \
  $(SDK_ROOT)/components/libraries/gpiote \
  $(SDK_ROOT)/components/drivers_nrf/gpiote \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/drivers_nrf/common \
  $(SDK_ROOT)/components/drivers_nrf/adc \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs_c \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/ble/ble_dtm \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs_c \
  $(SDK_ROOT)/components/drivers_nrf/uart \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/kbd \
  $(SDK_ROOT)/components/ble/ble_services/ble_lls \
  $(SDK_ROOT)/components/drivers_nrf/wdt \
  $(SDK_ROOT)/components/libraries/bsp \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/ble/ble_services/ble_ans_c \
  $(SDK_ROOT)/components/libraries/slip \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/libraries/csense_drv \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus_c \
  $(SDK_ROOT)/components/drivers_nrf/rtc \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/mouse \
  $(SDK_ROOT)/components/drivers_nrf/ppi \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu \
  $(SDK_ROOT)/components/drivers_nrf/twis_slave \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/drivers_nrf/usbd \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs \
  $(SDK_ROOT)/components/ble/ble_services/ble_hts \
  $(SDK_ROOT)/components/drivers_nrf/delay \
  $(SDK_ROOT)/components/libraries/crc16 \
  $(SDK_ROOT)/components/drivers_nrf/timer \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/drivers_nrf/pwm \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc \
  $(SDK_ROOT)/components/libraries/csense \
  $(SDK_ROOT)/components/drivers_nrf/rng \
  $(SDK_ROOT)/components/libraries/low_power_pwm \
  $(SDK_ROOT)/components/libraries/hardfault \
  $(SDK_ROOT)/components/ble/ble_services/ble_cscs \
  $(SDK_ROOT)/components/libraries/uart \
  $(SDK_ROOT)/components/libraries/hci \
  $(SDK_ROOT)/components/drivers_nrf/spi_slave \
  $(SDK_ROOT)/components/drivers_nrf/lpcomp \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/drivers_nrf/power \
  $(SDK_ROOT)/components/libraries/usbd/config \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/libraries/led_softblink \
  $(SDK_ROOT)/components/drivers_nrf/qdec \
  $(SDK_ROOT)/components/ble/ble_services/ble_cts_c \
  $(SDK_ROOT)/components/drivers_nrf/spi_master \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus \
  $(SDK_ROOT)/components/ble/ble_services/ble_hids \
  $(SDK_ROOT)/components/drivers_nrf/pdm \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/usbd/class/audio \
  $(SDK_ROOT)/components/ble/peer_manager \
  $(SDK_ROOT)/components/libraries/mem_manager \
  $(SDK_ROOT)/components/ble/ble_services/ble_tps \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis \
  $(SDK_ROOT)/components/device \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/libraries/usbd \
  $(SDK_ROOT)/components/drivers_nrf/saadc \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs_c \
  $(SDK_ROOT)/components/ble/ble_racp \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/libraries/fds \
  $(SDK_ROOT)/components/libraries/twi \
  $(SDK_ROOT)/components/drivers_nrf/clock \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs \
  $(SDK_ROOT)/components/drivers_nrf/swi \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus \
  $(SDK_ROOT)/components/libraries/pwr_mgmt \
  $(SDK_ROOT)/components/libraries/ecc \
  $(SDK_ROOT)/components/libraries/sha256 \
  $(SDK_ROOT)/components/libraries/svc \
  $(SDK_ROOT)/external/micro-ecc/micro-ecc

# Libraries common to all targets
ifeq ($(PLATFORM),NRF51822)
LIB_FILES += \
  $(SDK_ROOT)/external/micro-ecc/nrf51_armgcc/armgcc/micro_ecc_lib_nrf51.a
else
LIB_FILES += \
  $(SDK_ROOT)/external/micro-ecc/nrf52_armgcc/armgcc/micro_ecc_lib_nrf52.a
endif

# C flags common to all targets
ifeq ($(PLATFORM),NRF51822)
CFLAGS += -DS130
CFLAGS += -DNRF_SD_BLE_API_VERSION=2
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mfloat-abi=soft
else
CFLAGS += -DS132
CFLAGS += -DNRF_SD_BLE_API_VERSION=3
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
endif
CFLAGS += -D$(PLATFORM_FAMILY)
CFLAGS += -D$(PLATFORM)
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -O3 -g3
CFLAGS += -D__HEAP_SIZE=0
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums
CFLAGS += -DSVC_INTERFACE_CALL_AS_NORMAL_FUNCTION

# C++ flags common to all targets
CXXFLAGS += \

# Assembler flags common to all targets
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -D$(PLATFORM_FAMILY)
ASMFLAGS += -D$(PLATFORM)
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DBOARD_CUSTOM
ASMFLAGS += -DBSP_DEFINES_ONLY
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -D__HEAP_SIZE=0
ASMFLAGS += -DSVC_INTERFACE_CALL_AS_NORMAL_FUNCTION
ifeq ($(PLATFORM),NRF51822)
ASMFLAGS += -DS130
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=2
else
ASMFLAGS += -DS132
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=3
endif

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -Wl,--gc-sections
LDFLAGS += --specs=nano.specs -lc -lnosys
ifeq ($(PLATFORM),NRF51822)
LDFLAGS += -mcpu=cortex-m0
else
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
endif


.PHONY: $(TARGETS) default all clean flash erase

# Make default (=first) target -> needs to be before including Makefile.common
default: clean_linked_binaries
	@$(MAKE) -j8 $(PLATFORM)_xxaa

include $(TEMPLATE_PATH)/Makefile.common
$(foreach target, $(TARGETS), $(call define_target, $(target)))

$(OUTPUT_DIRECTORY)/bootloader_settings.hex: default
	nrfutil settings generate --family $(PLATFORM_FAMILY) --application $(OUTPUT_FILE).hex --application-version $(DFU_APP_VERSION) --application-version-string $(DFU_APP_VERSION_STR) --bootloader-version 1 --bl-settings-version 1 $@

# Flash the program
flash_app: default
	@echo Flashing: $(OUTPUT_FILE).hex
	$(GDB_TARGET) -ex load -ex kill -ex quit $(OUTPUT_FILE).hex

flash_softdevice:
	@echo Flashing Soft Device..
	$(GDB_TARGET) -ex load -ex kill -ex quit $(SOFTDEVICE_HEX)

flash_dfu_settings: $(OUTPUT_DIRECTORY)/bootloader_settings.hex
	@echo Flashing bootloader settings..
	$(GDB_TARGET) -ex load -ex kill -ex quit $<

flash: flash_app flash_dfu_settings

erase:
	$(GDB_TARGET) -ex "mon erase_mass" -ex detach -ex quit

$(OUTPUT_DIRECTORY)/.clean_linked_binaries: $(PROJ_DIR)/$(LINKER_SCRIPT)
	@echo Cleaning linked binaries..
	@rm -f $(OUTPUT_DIRECTORY)/*.hex
	@rm -f $(OUTPUT_DIRECTORY)/*.bin
	@rm -f $(OUTPUT_DIRECTORY)/*.out
	@touch $@

clean_linked_binaries: $(OUTPUT_DIRECTORY) $(OUTPUT_DIRECTORY)/.clean_linked_binaries
