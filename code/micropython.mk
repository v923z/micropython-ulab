
USERMODULES_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(USERMODULES_DIR)/ndarray.c
SRC_USERMOD += $(USERMODULES_DIR)/linalg.c
SRC_USERMOD += $(USERMODULES_DIR)/vectorise.c
SRC_USERMOD += $(USERMODULES_DIR)/poly.c
SRC_USERMOD += $(USERMODULES_DIR)/fft.c
SRC_USERMOD += $(USERMODULES_DIR)/numerical.c
SRC_USERMOD += $(USERMODULES_DIR)/ulab.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(USERMODULES_DIR)
