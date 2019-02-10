PROJECT_NAME       := network-display
PROJECT_DIR_NAME   := $(shell dirname "$(strip $(MAKEFILE_LIST))")
DFU_APP_VERSION_STR:= 1.0.0
PLATFORM           := NRF52840_XXAA

TARGETS            := $(PLATFORM)
OUTPUT_DIRECTORY   := build
LINKER_SCRIPT      := openthread_nrf52840.ld

SDK_ROOT           := $(HOME)/nonbackupped/nRF5_SDK_for_Thread_and_Zigbee_2.0.0
PROJ_DIR           := .
SRC_DIR            := $(PROJ_DIR)/src
TEMPLATE_PATH      := $(SDK_ROOT)/components/toolchain/gcc
SDK_CONFIG_FILE    := $(SRC_DIR)/sdk_config.h

U8G2_PATH          := $(HOME)/arduino/u8g2
U8G2_SOURCES       := $(wildcard $(U8G2_PATH)/csrc/*.c)

HFSM_PATH          := $(HOME)/Documents/dev/HFSM

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
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52840.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spim.c\
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
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_init.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_shared.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecdh.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_sw/nrf_sw_backend_hash.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_settings.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_flash.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_validation.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_ver_validation.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_handling_error.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_utils.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/dfu-cc.pb.c \
  $(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_spi.c \
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
  $(U8G2_SOURCES) \
  $(SRC_DIR)/main.cpp \
  $(SRC_DIR)/thread.cpp \
  $(SRC_DIR)/dfu_public_key.c \
  $(SRC_DIR)/periodic_timer.cpp \
  $(SRC_DIR)/coap_service.cpp \
  $(SRC_DIR)/coap_helpers.cpp \
  $(SRC_DIR)/cmd_protocol.cpp \
  $(SRC_DIR)/display.cpp \
  $(SRC_DIR)/nrf_u8g2_adapter.cpp \
  $(SRC_DIR)/power_manager.cpp \
  $(SRC_DIR)/display_command_handler.cpp \
  $(SRC_DIR)/rendering.cpp \

INC_FOLDERS += \
  $(SRC_DIR) \
  $(SRC_DIR)/states \
  $(SDK_ROOT)/external/fprintf \
  $(PROJ_DIR) \
  $(SDK_ROOT)/components/thread/utils \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/external/nrf_cc310/include \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/integration/nrfx \
  $(SDK_ROOT)/modules/nrfx/drivers/include \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/modules/nrfx \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/modules/nrfx/hal \
  $(SDK_ROOT)/external/openthread/include \
  $(SDK_ROOT)/components/libraries/bsp \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/integration/nrfx/legacy \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/libraries/sha256 \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/softdevice/mbr/nrf52840/headers \
  $(SDK_ROOT)/components/libraries/mem_manager \
  $(SDK_ROOT)/components/libraries/mutex \
  $(SDK_ROOT)/components/libraries/pwr_mgmt \
  $(SDK_ROOT)/components/libraries/bootloader \
  $(SDK_ROOT)/components/libraries/bootloader/dfu \
  $(SDK_ROOT)/components/libraries/crypto \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310 \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon \
  $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_sw \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw \
  $(SDK_ROOT)/components/libraries/crypto/backend/cifra \
  $(SDK_ROOT)/components/iot/coap \
  $(SDK_ROOT)/components/iot/common \
  $(SDK_ROOT)/components/iot/background_dfu \
  $(SDK_ROOT)/components/iot/background_dfu/transport \
  $(SDK_ROOT)/components/iot/background_dfu/transport/coap \
  $(SDK_ROOT)/external/nrf_tls \
  $(SDK_ROOT)/external/nano-pb \
  $(SDK_ROOT)/external/micro-ecc/micro-ecc \
  $(U8G2_PATH)/csrc \
  $(HFSM_PATH)/include \

#
# Original Nordic SDK Openthread libraries
#
#LIB_FILES += \
#  $(SDK_ROOT)/external/openthread/lib/gcc/libopenthread-cli-ftd.a \
#  $(SDK_ROOT)/external/openthread/lib/gcc/libopenthread-diag.a \
#  $(SDK_ROOT)/external/openthread/lib/gcc/libopenthread-ftd.a \
#  $(SDK_ROOT)/external/openthread/lib/gcc/libopenthread-platform-utils.a \
#  $(SDK_ROOT)/external/openthread/lib/gcc/libmbedcrypto.a \
#  $(SDK_ROOT)/external/openthread/lib/gcc/libopenthread-nrf52840-sdk.a \
#  $(SDK_ROOT)/external/micro-ecc/nrf52hf_armgcc/armgcc/micro_ecc_lib_nrf52.a \
#  $(SDK_ROOT)/external/nrf_cc310/lib/libnrf_cc310_0.9.10.a \
#  -lstdc++

#
# Self compiled more minimal libraries, configured with COAP=1 DNS_CLIENT=1 DISABLE_SPI=1 (Openthread git commit 41d8e4a73b4de..)
#
# NOTE! Includes are still used from the Nordic SDK!
#
LIB_FILES += \
  $(PROJ_DIR)/openthread/nrf52840/lib/libopenthread-diag.a \
  $(PROJ_DIR)/openthread/nrf52840/lib/libopenthread-ftd.a \
  $(PROJ_DIR)/openthread/nrf52840/lib/libopenthread-platform-utils.a \
  $(PROJ_DIR)/openthread/nrf52840/lib/libmbedcrypto.a \
  $(PROJ_DIR)/openthread/nrf52840/lib/libopenthread-nrf52840-sdk.a \
  $(SDK_ROOT)/external/micro-ecc/nrf52hf_armgcc/armgcc/micro_ecc_lib_nrf52.a \
  $(SDK_ROOT)/external/nrf_cc310/lib/libnrf_cc310_0.9.10.a \
  -lstdc++

# Optimization flags
OPT = -O3 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto


# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DBOARD_PCA10056
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
CFLAGS += -DNRF_DFU_SETTINGS_VERSION=1
CFLAGS += -DNRF_DFU_NO_TRANSPORT
CFLAGS += -DuECC_ENABLE_VLI_API=0
CFLAGS += -DuECC_OPTIMIZATION_LEVEL=3
CFLAGS += -DuECC_SQUARE_FUNC=0
CFLAGS += -DuECC_SUPPORT_COMPRESSED_POINT=0
CFLAGS += -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1
CFLAGS += -DAPP_VERSION='"$(DFU_APP_VERSION_STR)"'
CFLAGS += -DDEBUG

# C++ flags common to all targets
CXXFLAGS += $(OPT)
CXXFLAGS += -std=c++17 -Wno-register
CXXFLAGS += -fno-threadsafe-statics
CXXFLAGS += -DDEBUG

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
ASMFLAGS += -DNRF_DFU_SETTINGS_VERSION=1
ASMFLAGS += -DNRF_DFU_NO_TRANSPORT
ASMFLAGS += -DuECC_ENABLE_VLI_API=0
ASMFLAGS += -DuECC_OPTIMIZATION_LEVEL=3
ASMFLAGS += -DuECC_SQUARE_FUNC=0
ASMFLAGS += -DuECC_SUPPORT_COMPRESSED_POINT=0
ASMFLAGS += -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1


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
	$(NRFUTIL) pkg generate --hw-version $(DFU_HW_VERSION) --application-version-string "$(DFU_APP_VERSION_STR)" --application $(OUTPUT_FILE).hex --sd-req 0x00 --key-file $(DFU_PRIVATE_KEY) app_dfu_package_$(DFU_APP_VERSION_STR).zip

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
