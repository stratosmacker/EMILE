#
#  (c) 2005-2007 Laurent Vivier <Laurent@Vivier.EU>
#

TOP = $(shell pwd)
VPATH=$(TOP)

CFLAGS	+= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -g -O2 -Os
#-m68000 -Wa,-m68000
CPPFLAGS = -I$(TOP)/../libmacos -I$(TOP) -I$(TOP)/../libgzip

LIBRARIES = scsi/libstream.a floppy/libstream.a full/libstream.a

SOURCES = stream_close.c stream_open.c stream_read.c stream_lseek.c \
	  stream_uncompress.c gzio.c stream_fstat.c stream_set_default.c

HEADERS = libstream.h

SCSI_FLAGS      = -DSCSI_SUPPORT -I$(TOP)/../libscsi
ISO9660_FLAGS   = -DISO9660_SUPPORT -I$(TOP)/../libiso9660
EXT2_FLAGS      = -DEXT2_SUPPORT -I$(TOP)/../libext2
CONTAINER_FLAGS = -DCONTAINER_SUPPORT -I$(TOP)/../libcontainer
FLOPPY_FLAGS    = -DFLOPPY_SUPPORT -I$(TOP)/../libfloppy
BLOCK_FLAGS     = -DBLOCK_SUPPORT -I$(TOP)/../libblock
MAP_FLAGS	= -DMAP_SUPPORT -I$(TOP)/../libmap -I$(TOP)/../libemile

all: $(LIBRARIES)

scsi/libstream.a::
	test -e scsi || mkdir scsi
	$(MAKE) -C scsi -f $(TOP)/Makefile TOP=$(TOP) LIBRARY=libstream.a CPPFLAGS="$(CPPFLAGS) \
	$(SCSI_FLAGS) $(CONTAINER_FLAGS) $(BLOCK_FLAGS) $(ISO9660_FLAGS) $(MAP_FLAGS) \
	$(EXT2_FLAGS)" libstream.a

floppy/libstream.a::
	test -e floppy || mkdir floppy
	$(MAKE) -C floppy -f $(TOP)/Makefile TOP=$(TOP) LIBRARY=libstream.a \
	CPPFLAGS="$(CPPFLAGS) $(FLOPPY_FLAGS) $(BLOCK_FLAGS)" libstream.a

full/libstream.a::
	test -e full || mkdir full
	$(MAKE) -C full -f $(TOP)/Makefile TOP=$(TOP) LIBRARY=libstream.a \
	CPPFLAGS="$(CPPFLAGS) $(SCSI_FLAGS) $(CONTAINER_FLAGS) \
	$(ISO9660_FLAGS) $(FLOPPY_FLAGS) $(BLOCK_FLAGS) $(MAP_FLAGS) \
	$(EXT2_FLAGS)" libstream.a

include $(TOP)/../tools.mk
include $(TOP)/../Rules.mk
