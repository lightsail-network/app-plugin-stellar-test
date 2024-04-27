APPNAME = "StellarTest"

# Application version
APPVERSION_M = 1
APPVERSION_N = 0
APPVERSION_P = 0

ifeq ($(BOLOS_SDK),)
$(error Environment variable BOLOS_SDK is not set)
endif

include $(BOLOS_SDK)/Makefile.defines

APPVERSION ?= "$(APPVERSION_M).$(APPVERSION_N).$(APPVERSION_P)"

# Application source files
APP_SOURCE_PATH += src

# Application icons following guidelines:
# https://developers.ledger.com/docs/embedded-app/design-requirements/#device-icon
NORMAL_NAME ?= $(shell echo -n "$(APPNAME)" | tr " ." "_" | tr "[:upper:]" "[:lower:]")
ICON_NANOS = icons/nanos_app_$(NORMAL_NAME).gif
ICON_NANOX = icons/nanox_app_$(NORMAL_NAME).gif
ICON_NANOSP = $(ICON_NANOX)
ICON_STAX = icons/stax_app_$(NORMAL_NAME).gif

ifeq ($(TARGET_NAME),TARGET_STAX)
    DEFINES += ICONGLYPH=C_stax_$(NORMAL_NAME)_64px
    DEFINES += ICONBITMAP=C_stax_$(NORMAL_NAME)_64px_bitmap
endif

VARIANT_PARAM = COIN
VARIANT_VALUES ?= $(NORMAL_NAME)

HAVE_APPLICATION_FLAG_LIBRARY = 1
a
DISABLE_STANDARD_APP_FILES = 1
DISABLE_STANDARD_SNPRINTF = 1
DISABLE_STANDARD_USB = 1
DISABLE_STANDARD_WEBUSB = 1
DISABLE_STANDARD_BAGL_UX_FLOW = 1
DISABLE_DEBUG_LEDGER_ASSERT = 1
DISABLE_DEBUG_THROW = 1

include $(BOLOS_SDK)/Makefile.standard_app
