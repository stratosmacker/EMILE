#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP = $(shell pwd)
VPATH=$(TOP)

TARGET = native
CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic
CPPFLAGS = 

LIBRARY = libiso9660.a

SOURCES = iso9660_mount.c iso9660_opendir.c \
	  iso9660_closedir.c iso9660_readdir.c \
	  iso9660_is_directory.c iso9660_open.c \
	  iso9660_read.c iso9660_close.c iso9660_init.c \
	  iso9660_lseek.c

HEADERS = libiso9660.h

DISTFILES = $(SOURCES) $(HEADERS)

all:
	test -d $(TARGET) || mkdir $(TARGET)
	cd $(TARGET) && make -f $(TOP)/Makefile $(LIBRARY) TOP=$(TOP)

include $(TOP)/../Rules.mk