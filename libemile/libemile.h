/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#ifndef _LIBEMILE_H
#define _LIBEMILE_H

#include <sys/stat.h>

static __attribute__((used)) char* libemile_header = "$CVSHeader$";

#define SCSI_SUPPORT

#include "../second/head.h"

#define EMILE_FIRST_TUNE_DRIVE	0x0001
#define EMILE_FIRST_TUNE_OFFSET	0x0002
#define EMILE_FIRST_TUNE_SIZE	0x0004

#define FLOPPY_SECTOR_SIZE	512
#define FIRST_LEVEL_SIZE	(FLOPPY_SECTOR_SIZE * 2)

enum {
	EEMILE_CANNOT_READ_FIRST	= -2,
	EEMILE_UNKNOWN_FIRST		= -3,
	EEMILE_CANNOT_WRITE_FIRST	= -4,
	EEMILE_MALLOC_ERROR		= -5,
	EEMILE_CANNOT_OPEN_FILE		= -6,
	EEMILE_CANNOT_WRITE_SECOND	= -7,
	EEMILE_CANNOT_WRITE_KERNEL	= -8,
	EEMILE_CANNOT_WRITE_RAMDISK	= -9,
	EEMILE_CANNOT_WRITE_PAD		= -10,
	EEMILE_CANNOT_CREATE_IMAGE	= -11,
	EEMILE_MISSING_FIRST		= -12,
	EEMILE_MISSING_SECOND		= -13,
	EEMILE_CANNOT_READ_SECOND	= -14,
	EEMILE_INVALID_SECOND		= -15,
	EEMILE_CANNOT_READ_KERNEL	= -16,
};

static inline unsigned long emile_file_get_size(char* file)
{
        struct stat result;

        stat(file, &result);

        return (result.st_size + FLOPPY_SECTOR_SIZE - 1)
                / FLOPPY_SECTOR_SIZE * FLOPPY_SECTOR_SIZE;
}

extern int emile_first_set_param(int fd, unsigned short tune_mask, 
				 int drive_num, int second_offset, 
				 int second_size);
extern int emile_first_get_param(int fd, int *drive_num, int *second_offset, 
				 int *second_size);
extern int emile_first_set_param_scsi(int fd, char *second_name);
extern int emile_second_get_output(int fd, unsigned int *console_mask,
				   unsigned int *bitrate0, int *datasize0,
				   int *parity0, int *stopbits0,
				   unsigned int *bitrate1, int *datasize1,
				   int *parity1, int *stopbits1,
				   int *gestaltid);
extern int emile_second_set_output(int fd,
				   unsigned int enable_mask, 
				   unsigned int disable_mask,
				   unsigned int bitrate0, int datasize0,
				   int parity0, int stopbits0,
				   unsigned int bitrate1, int datasize1,
				   int parity1, int stopbits1, int gestaltid);
extern int emile_second_set_cmdline(int fd, char* cmdline);
extern int emile_second_get_cmdline(int fd, char* cmdline);
extern int emile_second_set_kernel(int fd, char *kernel_image, 
				   unsigned int kernel_offset,
				   unsigned int buffer_size, char* ramdisk);
extern int emile_second_get_kernel(int fd, unsigned int *kernel_offset,
				   unsigned int *kernel_image_size,
				   unsigned int *buffer_size, 
				   unsigned int *ramdisk_offset,
				   unsigned int *ramdisk_size);
extern int emile_second_set_kernel_scsi(int fd, char *kernel_name);
extern int emile_floppy_create_image(char* first_level, char* second_level, 
				     char* kernel_image, char* ramdisk, 
				     unsigned long buffer_size, char* image);
extern int emile_scsi_create_container(int fd, 
				       struct emile_container* container);
#endif