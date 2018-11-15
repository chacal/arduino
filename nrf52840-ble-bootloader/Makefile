PROJECT_NAME       := nrf52840-ble-bootloader
PROJECT_DIR_NAME   := $(shell dirname "$(strip $(MAKEFILE_LIST))")
PLATFORM_FAMILY    := NRF52
PLATFORM           := NRF52840_XXAA

TARGETS            := $(PLATFORM)
OUTPUT_DIRECTORY   := build
LINKER_SCRIPT      := secure_bootloader_gcc_nrf52840.ld

SDK_ROOT           := $(HOME)/nonbackupped/nRF5_SDK_15.2.0
SOFTDEVICE_HEX     := $(SDK_ROOT)/components/softdevice/s140/hex/s140_nrf52_6.1.0_softdevice.hex
PROJ_DIR           := .
SRC_DIR            := $(PROJ_DIR)/src
TEMPLATE_PATH      := $(SDK_ROOT)/components/toolchain/gcc
SDK_CONFIG_FILE    := $(SRC_DIR)/sdk_config.h

include $(TEMPLATE_PATH)/Makefile.common.jihartik

SRC_FILES += \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52840.S \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/crc32/crc32.c \
  $(SDK_ROOT)/components/libraries/mem_manager/mem_manager.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_nvmc.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/queue/nrf_queue.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52840.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_init.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_shared.c \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \
  $(SDK_ROOT)/modules/nrfx/hal/nrf_nvmc.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_init.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_shared.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_svci_handler.c \
  $(SDK_ROOT)/components/libraries/svc/nrf_svc_handler.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/libraries/bootloader/nrf_bootloader.c \
  $(SDK_ROOT)/components/libraries/bootloader/nrf_bootloader_app_start.c \
  $(SDK_ROOT)/components/libraries/bootloader/nrf_bootloader_app_start_final.c \
  $(SDK_ROOT)/components/libraries/bootloader/nrf_bootloader_dfu_timers.c \
  $(SDK_ROOT)/components/libraries/bootloader/nrf_bootloader_fw_activation.c \
  $(SDK_ROOT)/components/libraries/bootloader/nrf_bootloader_info.c \
  $(SDK_ROOT)/components/libraries/bootloader/nrf_bootloader_wdt.c \
  $(SDK_ROOT)/external/nano-pb/pb_common.c \
  $(SDK_ROOT)/external/nano-pb/pb_decode.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/dfu-cc.pb.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu.c \
  $(SDK_ROOT)/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_flash.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_handling_error.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_mbr.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_settings.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_settings_svci.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_transport.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_utils.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_validation.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_ver_validation.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_chacha_poly_aead.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecdh.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_eddsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_hmac.c \
  $(SRC_DIR)/main.c \
  $(SRC_DIR)/dfu_public_key.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \

INC_FOLDERS += \
  $(SRC_DIR) \
  $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/components/softdevice/s140/headers/nrf52 \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/libraries/mem_manager \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/modules/nrfx \
  $(SDK_ROOT)/external/nrf_oberon/include \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon \
  $(SDK_ROOT)/components/libraries/crypto/backend/cifra \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/integration/nrfx \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl \
  $(SDK_ROOT)/external/nrf_cc310/include \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/libraries/bootloader/dfu \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/components/libraries/svc \
  $(SDK_ROOT)/components/libraries/stack_info \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/external/nrf_oberon \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310 \
  $(SDK_ROOT)/components/libraries/bootloader \
  $(SDK_ROOT)/components/softdevice/s140/headers \
  $(SDK_ROOT)/components/libraries/crypto \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/external/nrf_cc310_bl/include \
  $(SDK_ROOT)/modules/nrfx/hal \
  $(SDK_ROOT)/external/fprintf \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/atomic_fifo \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_sw \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(SDK_ROOT)/components/libraries/bootloader/ble_dfu \
  $(SDK_ROOT)/components/softdevice/common \
  $(SDK_ROOT)/external/nano-pb \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/libraries/mutex \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/external/segger_rtt \

LIB_FILES += \
  $(SDK_ROOT)/external/nrf_oberon/lib/nrf52/liboberon_2.0.5.a \
  $(SDK_ROOT)/external/nrf_cc310_bl/lib/libnrf_cc310_bl_0.9.10.a \

# Optimization flags
OPT = -Os -g3
# Uncomment the line below to enable link time optimization
OPT += -flto


# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DS140
CFLAGS += -DNRF_SD_BLE_API_VERSION=6
CFLAGS += -DSWI_DISABLE0
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -D$(PLATFORM)
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall
CFLAGS += -DNRF_DFU_SETTINGS_VERSION=1
CFLAGS += -DNRF_DFU_SVCI_ENABLED
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums
CFLAGS += -D__HEAP_SIZE=0

# C++ flags common to all targets
CXXFLAGS += $(OPT) \

# Assembler flags common to all targets


ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DS140
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=6
ASMFLAGS += -DNRF_DFU_SETTINGS_VERSION=1
ASMFLAGS += -DNRF_DFU_SVCI_ENABLED
ASMFLAGS += -D$(PLATFORM)
ASMFLAGS += -DBOARD_CUSTOM
ASMFLAGS += -D__HEAP_SIZE=0

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
LDFLAGS += -Wl,--gc-sections
LDFLAGS += --specs=nano.specs -lc -lnosys -lm

.PHONY: $(TARGETS) default all clean flash erase kuik

# Make default (=first) target -> needs to be before including Makefile.common
default: clean_linked_binaries
	@$(MAKE) -j12 $(PLATFORM)

include $(TEMPLATE_PATH)/Makefile.common
$(foreach target, $(TARGETS), $(call define_target, $(target)))

$(OUTPUT_DIRECTORY)/bootloader_and_softdevice.hex: default
	$(MERGEHEX) -m $(SOFTDEVICE_HEX) $(OUTPUT_FILE).hex -o $@

flash_bootloader_with_softdevice: $(OUTPUT_DIRECTORY)/bootloader_and_softdevice.hex
	@echo Flashing bootloader and softdevice..
	$(call flash,$<)

flash_bootloader: default
	$(call flash,$(OUTPUT_FILE).hex)

flash: flash_app flash_dfu_settings

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
