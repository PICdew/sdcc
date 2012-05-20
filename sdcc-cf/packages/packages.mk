HOSTNAME := $(shell if [ $(shell expr $(shell hostname) : '.*\.') != '0' ]; then expr $(shell hostname) : '\([^.]*\).'; else echo $(shell hostname); fi)
LOCAL_DIR=$(HOME)/local-$(HOSTNAME)
SF_REPOSITORY=http://voxel.dl.sourceforge.net

OS = $(shell uname)

CFLAGS += -I$(HOME)/local-$(HOSTNAME)/include
CXXFLAGS += -I$(HOME)/local-$(HOSTNAME)/include
LDFLAGS += -L$(HOME)/local-$(HOSTNAME)/lib

ifeq ($(OS), Darwin)
  ifeq ($(HOSTNAME), mirror-doors)
    XCODE_ROOT = /Xcode2.5
  else
    XCODE_ROOT = /Developer
  endif
endif