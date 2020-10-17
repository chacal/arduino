PROJECT_NAME       := thread_router
PROJECT_DIR_NAME   := $(shell dirname "$(strip $(MAKEFILE_LIST))")
DFU_APP_VERSION_STR:= 1.4.3
PLATFORM           := NRF52840_XXAA
# BOARD_PCA10056 (regular nRF52840) or BOARD_PCA10059 (USB dongle)
TARGET_BOARD       := BOARD_PCA10056
TARGETS            := $(PLATFORM)
OUTPUT_DIRECTORY   := build

ifeq ($(TARGET_BOARD),BOARD_PCA10056)
  LINKER_SCRIPT      = openthread_nrf52840.ld
else
  LINKER_SCRIPT      = openthread_nrf52840-usb.ld
endif

SDK_ROOT           := $(HOME)/nonbackupped/nRF5_SDK_for_Thread_and_Zigbee_v4.1.0
PROJ_DIR           := .
SRC_DIR            := $(PROJ_DIR)/src
TEMPLATE_PATH      := $(SDK_ROOT)/components/toolchain/gcc
SDK_CONFIG_FILE    := $(SRC_DIR)/sdk_config.h

include $(TEMPLATE_PATH)/Makefile.common.jihartik

DFU_FAMILY         := NRF52840
DFU_HW_VERSION     := 52
DFU_PRIVATE_KEY    := private.key

SRC_FILES += \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52840.S \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_uart.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/assert/assert.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd/nrf_nvic.c \
  $(SDK_ROOT)/modules/nrfx/hal/nrf_nvmc.c \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd/nrf_soc.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_nvmc.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52840.c \
  $(SDK_ROOT)/components/thread/utils/thread_utils.c \
  $(SDK_ROOT)/components/libraries/mem_manager/mem_manager.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/sha256/sha256.c \
  $(SDK_ROOT)/components/libraries/crc32/crc32.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_nvmc.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/iot/coap/coap.c \
  $(SDK_ROOT)/components/iot/coap/coap_block.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_settings.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_flash.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_validation.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_ver_validation.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_handling_error.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_utils.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/dfu-cc.pb.c \
  $(SDK_ROOT)/components/iot/background_dfu/background_dfu_state.c \
  $(SDK_ROOT)/components/iot/background_dfu/background_dfu_block.c \
  $(SDK_ROOT)/components/iot/background_dfu/background_dfu_operation.c \
  $(SDK_ROOT)/components/iot/background_dfu/transport/coap/coap_dfu.c \
  $(SDK_ROOT)/components/iot/coap/coap_message.c \
  $(SDK_ROOT)/components/iot/coap/coap_option.c \
  $(SDK_ROOT)/components/iot/coap/coap_queue.c \
  $(SDK_ROOT)/components/iot/coap/coap_resource.c \
  $(SDK_ROOT)/components/iot/coap/coap_transport_ot.c \
  $(SDK_ROOT)/external/nano-pb/pb_common.c \
  $(SDK_ROOT)/external/nano-pb/pb_decode.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_init.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_shared.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_rng.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_init.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_shared.c \
  $(SRC_DIR)/main.c \
  $(SRC_DIR)/thread.c \
  $(SRC_DIR)/dfu_public_key.c

ifeq ($(TARGET_BOARD),BOARD_PCA10059)
SRC_FILES += \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_power.c \
  $(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_usbd.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_power.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_systick.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_trigger_usb.c \
  $(SDK_ROOT)/components/libraries/usbd/app_usbd.c \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm/app_usbd_cdc_acm.c \
  $(SDK_ROOT)/components/libraries/usbd/class/nrf_dfu_trigger/app_usbd_nrf_dfu_trigger.c \
  $(SDK_ROOT)/components/libraries/usbd/app_usbd_core.c \
  $(SDK_ROOT)/components/libraries/usbd/app_usbd_serial_num.c \
  $(SDK_ROOT)/components/libraries/usbd/app_usbd_string_desc.c \
  $(SDK_ROOT)/external/utf_converter/utf.c
endif


INC_FOLDERS += \
  $(SDK_ROOT)/external/openthread/project/config \
  $(SDK_ROOT)/external/nrf_security/config \
  $(SDK_ROOT)/external/nrf_security/nrf_cc310_plat/include \
  $(SDK_ROOT)/integration/nrfx \
  $(SDK_ROOT)/external/openthread/project/nrf52840 \
  $(SDK_ROOT)/external/nrf_security/include \
  $(SDK_ROOT)/components/thread/utils \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/libraries/atomic_fifo \
  $(SDK_ROOT)/external/nRF-IEEE-802.15.4-radio-driver/src/fem \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/integration/nrfx/legacy \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/modules/nrfx/hal \
  $(SDK_ROOT)/components/libraries/mem_manager \
  $(SDK_ROOT)/components/libraries/bsp \
  $(SDK_ROOT)/components/libraries/sortlist \
  $(SDK_ROOT)/modules/nrfx/drivers/include \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/external/fprintf \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/modules/nrfx \
  $(SDK_ROOT)/external/openthread/include \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/external/nRF-IEEE-802.15.4-radio-driver/src/fem/three_pin_gpio \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/external/nrf_security/mbedtls_plat_config \
  $(SDK_ROOT)/components/libraries/mutex \
  $(SDK_ROOT)/components/libraries/pwr_mgmt \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/external/nrf_tls \
  $(SDK_ROOT)/components/iot/coap \
  $(SDK_ROOT)/components/iot/common \
  $(SDK_ROOT)/components/softdevice/mbr/headers \
  $(SDK_ROOT)/components/libraries/stack_info \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/libraries/bootloader \
  $(SDK_ROOT)/components/libraries/bootloader/dfu \
  $(SDK_ROOT)/components/iot/background_dfu \
  $(SDK_ROOT)/components/iot/background_dfu/transport \
  $(SDK_ROOT)/components/iot/background_dfu/transport/coap \
  $(SDK_ROOT)/external/nano-pb \
  $(SDK_ROOT)/components/libraries/crypto \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310 \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon \
  $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc \
  $(SDK_ROOT)/components/libraries/crypto/backend/optiga \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_sw \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw \
  $(SDK_ROOT)/components/libraries/crypto/backend/cifra \
  $(SDK_ROOT)/external/nrf_cc310_bl/include \
  $(SDK_ROOT)/components/libraries/sha256 \
  $(SDK_ROOT)/components/libraries/usbd \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm \
  $(SDK_ROOT)/components/libraries/usbd/class/nrf_dfu_trigger \
  $(SDK_ROOT)/components/libraries/block_dev \
  $(SDK_ROOT)/external/utf_converter \
  $(SRC_DIR) \
  $(PROJ_DIR) \

#
# Original Nordic SDK Openthread libraries
#
#LIB_FILES += \
#  $(SDK_ROOT)/external/openthread/lib/nrf52840/gcc/libopenthread-cli-ftd.a \
#  $(SDK_ROOT)/external/openthread/lib/nrf52840/gcc/libopenthread-ftd.a \
#  $(SDK_ROOT)/external/openthread/lib/nrf52840/gcc/libopenthread-nrf52840-transport.a \
#  $(SDK_ROOT)/external/openthread/lib/nrf52840/gcc/libopenthread-platform-utils.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue_cc310.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue_vanilla.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_vanilla_backend.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_tls_vanilla.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_x509_vanilla.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libnrf_cc310_platform_0.9.2.a \
#  $(SDK_ROOT)/external/openthread/lib/nrf52840/gcc/libopenthread-nrf52840-sdk.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_cc310_backend.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue_cc310.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue_vanilla.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_base_vanilla.a \
#  $(SDK_ROOT)/external/openthread/lib/nrf52840/gcc/libnordicsemi-nrf52840-radio-driver.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_cc310_backend.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_vanilla_backend.a \
#  $(SDK_ROOT)/external/openthread/lib/nrf52840/gcc/libopenthread-nrf52840-transport.a \
#  $(SDK_ROOT)/external/openthread/lib/nrf52840/gcc/libopenthread-platform-utils.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_base_vanilla.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_x509_vanilla.a \
#  $(SDK_ROOT)/external/nrf_security/lib/libnrf_cc310_platform_0.9.2.a \
#  $(SDK_ROOT)/external/openthread/lib/nrf52840/gcc/libopenthread-ftd.a \
#  $(SDK_ROOT)/external/nrf_cc310_bl/lib/cortex-m4/hard-float/libnrf_cc310_bl_0.9.12.a \


#
# Self compiled more minimal libraries built with build_library_chacal.sh and build_dongle_chacal.sh
# Built from https://github.com/chacal/openthread/commit/900a704d384d8e94749af755bf329808165944c4
#
# NOTE! Includes are still used from the Nordic SDK!
#
ifeq ($(TARGET_BOARD),BOARD_PCA10056)
LIB_FILES += \
  $(PROJ_DIR)/openthread-3bb0872b/nrf52840/lib/libopenthread-ftd.a \
  $(PROJ_DIR)/openthread-3bb0872b/nrf52840/lib/libopenthread-nrf52840-transport.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue_cc310.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue_vanilla.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_vanilla_backend.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_tls_vanilla.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_x509_vanilla.a \
  $(PROJ_DIR)/openthread-3bb0872b/nrf52840/lib/libopenthread-nrf52840-sdk.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_cc310_backend.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue_cc310.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue_vanilla.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_base_vanilla.a \
  $(PROJ_DIR)/openthread-3bb0872b/nrf52840/lib/libnordicsemi-nrf52840-radio-driver.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_cc310_backend.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_vanilla_backend.a \
  $(PROJ_DIR)/openthread-3bb0872b/nrf52840/lib/libopenthread-platform-utils.a \
  $(PROJ_DIR)/openthread-3bb0872b/nrf52840/lib/libopenthread-nrf52840-transport.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_base_vanilla.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_x509_vanilla.a \
  $(SDK_ROOT)/external/nrf_security/lib/libnrf_cc310_platform_0.9.2.a \
  $(PROJ_DIR)/openthread-3bb0872b/nrf52840/lib/libopenthread-ftd.a \
  $(SDK_ROOT)/external/nrf_cc310_bl/lib/cortex-m4/hard-float/libnrf_cc310_bl_0.9.12.a
else
LIB_FILES += \
  $(PROJ_DIR)/openthread-usb-3bb0872b/nrf52840/lib/libopenthread-ftd.a \
  $(PROJ_DIR)/openthread-usb-3bb0872b/nrf52840/lib/libopenthread-cli-ftd.a \
  $(PROJ_DIR)/openthread-usb-3bb0872b/nrf52840/lib/libopenthread-nrf52840-transport.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue_cc310.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue_vanilla.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_vanilla_backend.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_tls_vanilla.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_x509_vanilla.a \
  $(PROJ_DIR)/openthread-usb-3bb0872b/nrf52840/lib/libopenthread-nrf52840-sdk.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_cc310_backend.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue_cc310.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_glue_vanilla.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_base_vanilla.a \
  $(PROJ_DIR)/openthread-usb-3bb0872b/nrf52840/lib/libnordicsemi-nrf52840-radio-driver.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_cc310_backend.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedcrypto_vanilla_backend.a \
  $(PROJ_DIR)/openthread-usb-3bb0872b/nrf52840/lib/libopenthread-platform-utils.a \
  $(PROJ_DIR)/openthread-usb-3bb0872b/nrf52840/lib/libopenthread-nrf52840-transport.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_base_vanilla.a \
  $(SDK_ROOT)/external/nrf_security/lib/libmbedtls_x509_vanilla.a \
  $(SDK_ROOT)/external/nrf_security/lib/libnrf_cc310_platform_0.9.2.a \
  $(PROJ_DIR)/openthread-usb-3bb0872b/nrf52840/lib/libopenthread-ftd.a \
  $(SDK_ROOT)/external/nrf_cc310_bl/lib/cortex-m4/hard-float/libnrf_cc310_bl_0.9.12.a
endif

# Optimization flags
OPT = -O3 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto


# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -D$(TARGET_BOARD)
CFLAGS += -DAPP_TIMER_V2
CFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
CFLAGS += -DMBEDTLS_CONFIG_FILE=\"nrf-config.h\"
CFLAGS += -DMBEDTLS_USER_CONFIG_FILE=\"nrf52840-mbedtls-config.h\"
CFLAGS += -DOPENTHREAD_FTD=1
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -D$(PLATFORM)
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DUART0_ENABLED=0
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums
CFLAGS += -D__HEAP_SIZE=0
CFLAGS += -D__STACK_SIZE=8192
CFLAGS += -DNRF_DFU_SETTINGS_VERSION=1
CFLAGS += -DNRF_DFU_NO_TRANSPORT
CFLAGS += -DAPP_VERSION='"$(DFU_APP_VERSION_STR)"'
ifeq ($(TARGET_BOARD),BOARD_PCA10059)
CFLAGS += -DUSB_CDC_AS_SERIAL_TRANSPORT=1
CFLAGS += -DUSE_APP_CONFIG=1
endif

# C++ flags common to all targets
CXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DBOARD_PCA10056
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -D$(PLATFORM)
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DUART0_ENABLED=0
ASMFLAGS += -D__HEAP_SIZE=0
ASMFLAGS += -D__STACK_SIZE=8192
ASMFLAGS += -DNRF_DFU_SETTINGS_VERSION=1
ASMFLAGS += -DNRF_DFU_NO_TRANSPORT


# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
LDFLAGS += -Wl,--gc-sections
LDFLAGS += --specs=nano.specs -lc -lnosys -lm -lstdc++

.PHONY: $(TARGETS) default all clean flash erase

# Make default (=first) target -> needs to be before including Makefile.common
default: $(OUTPUT_DIRECTORY)/app_and_bootloader_settings.hex

include $(TEMPLATE_PATH)/Makefile.common
$(foreach target, $(TARGETS), $(call define_target, $(target)))

build_app: clean_linked_binaries
	@$(MAKE) -j12 $(PLATFORM)

$(OUTPUT_DIRECTORY)/bootloader_settings.hex: build_app
	$(NRFUTIL) settings generate --family $(DFU_FAMILY) --application $(OUTPUT_FILE).hex --application-version-string $(DFU_APP_VERSION_STR) --bootloader-version 1 --bl-settings-version 1 $@

dfu_pkg: build_app
	$(NRFUTIL) pkg generate --hw-version $(DFU_HW_VERSION) --application-version-string "$(DFU_APP_VERSION_STR)" --application $(OUTPUT_FILE).hex --sd-req 0x00 --key-file $(DFU_PRIVATE_KEY) $(PROJECT_NAME)_dfu_package_$(DFU_APP_VERSION_STR).zip

flash_dfu_settings: $(OUTPUT_DIRECTORY)/bootloader_settings.hex
	@echo Flashing bootloader settings..
	$(call flash,$<)

# Flash the program
flash_app: build_app
	$(call flash,$(OUTPUT_FILE).hex)

$(OUTPUT_DIRECTORY)/app_and_bootloader_settings.hex: build_app $(OUTPUT_DIRECTORY)/bootloader_settings.hex
	$(MERGEHEX) -m $(OUTPUT_DIRECTORY)/bootloader_settings.hex $(OUTPUT_FILE).hex -o $@

flash: default
	$(call flash,$(OUTPUT_DIRECTORY)/app_and_bootloader_settings.hex)

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
