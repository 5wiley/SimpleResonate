# Project Name
TARGET = YourProjectName

LIBDAISY_DIR = lib/libDaisy
DAISYSP_DIR = lib/DaisySP
STMLIB_Dir = lib/stmlib
CMSIS_DSP_SRC_DIR = ${LIBDAISY_DIR}/Drivers/CMSIS-DSP/Source

C_DEFS = -DTARGET_DAISY

# Use QSPI Flash instead of internal 128KB flash
APP_TYPE = BOOT_SRAM

C_INCLUDES = \
	-Isource/

C_SOURCES = \
	${CMSIS_DSP_SRC_DIR}/CommonTables/arm_common_tables.c \
	${CMSIS_DSP_SRC_DIR}/ControllerFunctions/arm_sin_cos_f32.c

CPP_SOURCES = \
	main.cpp \
	source/Controls.cpp \
	source/Engine.cpp \

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
