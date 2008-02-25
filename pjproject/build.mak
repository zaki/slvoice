# build.mak.  Generated from build.mak.in by configure.
export MACHINE_NAME := auto
export OS_NAME := auto
export HOST_NAME := unix
export CC_NAME := gcc
export TARGET_NAME := i686-pc-linux-gnu
export CROSS_COMPILE := 
export LINUX_POLL := select 

# Determine which party libraries to use
export APP_THIRD_PARTY_LIBS := -lresample-$(TARGET_NAME) -lmilenage-$(TARGET_NAME)

ifneq (,1)
APP_THIRD_PARTY_LIBS += -lgsmcodec-$(TARGET_NAME)
endif

ifneq (,1)
APP_THIRD_PARTY_LIBS += -lspeex-$(TARGET_NAME)
endif

ifneq (,1)
APP_THIRD_PARTY_LIBS += -lilbccodec-$(TARGET_NAME)
endif

ifneq ($(findstring pa,pa_unix),)
APP_THIRD_PARTY_LIBS += -lportaudio-$(TARGET_NAME)
endif


# CFLAGS, LDFLAGS, and LIBS to be used by applications
export PJDIR := /home/ryanm/Code/voice/pjproject
export APP_CC := $(CROSS_COMPILE)$(CC_NAME)
export APP_CFLAGS := -DPJ_AUTOCONF=1\
	-O2\
	-I$(PJDIR)/pjlib/include\
	-I$(PJDIR)/pjlib-util/include\
	-I$(PJDIR)/pjnath/include\
	-I$(PJDIR)/pjmedia/include\
	-I$(PJDIR)/pjsip/include
export APP_CXXFLAGS := $(APP_CFLAGS)
export APP_LDFLAGS := -L$(PJDIR)/pjlib/lib\
	-L$(PJDIR)/pjlib-util/lib\
	-L$(PJDIR)/pjnath/lib\
	-L$(PJDIR)/pjmedia/lib\
	-L$(PJDIR)/pjsip/lib\
	-L$(PJDIR)/third_party/lib\
	
export APP_LDLIBS := -lpjsua-$(TARGET_NAME)\
	-lpjsip-ua-$(TARGET_NAME)\
	-lpjsip-simple-$(TARGET_NAME)\
	-lpjsip-$(TARGET_NAME)\
	-lpjmedia-codec-$(TARGET_NAME)\
	-lpjmedia-$(TARGET_NAME)\
	-lpjnath-$(TARGET_NAME)\
	-lpjlib-util-$(TARGET_NAME)\
	$(APP_THIRD_PARTY_LIBS)\
	-lpj-$(TARGET_NAME)\
	-lm -luuid -lnsl -lrt -lpthread  -lasound -lssl -lcrypto

export PJ_DIR := $(PJDIR)
export PJ_CC := $(APP_CC)
export PJ_CFLAGS := $(APP_CFLAGS)
export PJ_CXXFLAGS := $(APP_CXXFLAGS)
export PJ_LDFLAGS := $(APP_LDFLAGS)
export PJ_LDLIBS := $(APP_LDLIBS)

