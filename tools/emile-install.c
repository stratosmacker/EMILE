/*
 *
 * (c) 2004-2008 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "libemile.h"
#include "libconfig.h"

enum {
	ARG_NONE = 0,
	ARG_HELP ='h',
	ARG_VERBOSE = 'v',
	ARG_FIRST = 'f',
	ARG_SECOND = 's',
	ARG_KERNEL = 'k',
	ARG_RAMDISK = 'r',
	ARG_GETINFO = 'g',
	ARG_CONFIG = 'c',
};

static struct option long_options[] =
{
	{"help",	0, NULL,	ARG_HELP	},
	{"verbose",	0, NULL,	ARG_VERBOSE	},
	{"first",	1, NULL,	ARG_FIRST	},
	{"second",	1, NULL,	ARG_SECOND	},
	{"kernel",	1, NULL,	ARG_KERNEL	},
	{"ramdisk",	1, NULL,	ARG_RAMDISK	},
	{"getinfo",	1, NULL,	ARG_GETINFO	},
	{"config",	1, NULL,	ARG_CONFIG	},
	{NULL,		0, NULL,	0		},
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s [OPTION] <image>\n", argv[0]);
	fprintf(stderr, "Create an EMILE bootable floppy disk\n");
	fprintf(stderr, "EMILE allows you to boot linux from a floppy disk\n");
	fprintf(stderr, "   -h, --help         display this text\n");
	fprintf(stderr, "   -v, --verbose      verbose mode\n");
	fprintf(stderr, "   -f, --first=PATH   "
	                    "first level to copy to floppy\n");
	fprintf(stderr, "   -s, --second=PATH  "
	                    "second level to copy to floppy\n");
	fprintf(stderr, "   -k, --kernel=PATH  kernel to copy to floppy\n");
	fprintf(stderr, "   -r, --ramdisk=PATH ramdisk to copy to floppy\n");
	fprintf(stderr, "   -g, --getinfo      get information from <image>\n");
	fprintf(stderr, "   -c, --config=FILE  "
			"set configuration according to a config file\n");
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

static int get_info(char *image, int verbose)
{
	int fd;
	int drive_num;
	int second_offset;
	int second_size;
	int8_t * configuration;
	char property[1024];
	char title[1024];
	int index;
	char *known_properties[] = {
		"root",
		"kernel",
		"initrd",
		"args",
		"chainloader",
		NULL
	};
	int i;
	int current;
	int res;
	int ret;

	fd = open(image, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "ERROR: cannot open \"%s\"\n",
				image);
		return 2;
	}

	/* first level info */

	ret = emile_first_get_param(fd, &drive_num, &second_offset,
				    &second_size);

	if (ret != 0)
	{
		printf("EMILE is not installed in this bootblock\n");
		close(fd);
		return 0;
	}

	printf("EMILE boot block identified\n\n");
	printf("Drive number:        %d\n", drive_num);
	printf("Second level offset: %d\n", second_offset);
	printf("Second level size:   %d\n", second_size);

	/* second level info */

	printf("EMILE second level information\n");

	configuration = emile_second_get_configuration(fd);
	if (configuration == NULL)
	{
		fprintf(stderr, "ERROR: cannot read second level\n");
		return 3;
	}

	if (verbose)
	{
		printf("%s\n", configuration);
		return 0;
	}

	if (config_get_property(configuration, 
				"gestaltID", property) != -1)
		printf("User forces gestalt ID to %ld\n",
			strtol(property, NULL, 0));

	if (config_get_property(configuration, 
				"default", property) != -1)
		printf("default %ld\n", strtol(property, NULL, 0));

	if (config_get_property(configuration, 
				"timeout", property) != -1)
		printf("timeout %ld\n", strtol(property, NULL, 0));

	if (config_get_property(configuration, 
				"vga", property) != -1)
		printf("vga %s\n", property);

	if (config_get_property(configuration, 
				"modem", property) != -1)
		printf("modem %s\n", property);

	if (config_get_property(configuration, 
				"printer", property) != -1)
		printf("printer %s\n", property);

	current = 0;
	for (index = 0; index < 20; index++)
	{
		res = config_get_property(configuration + current,
					  "title", title);
		if (res == -1)
		{
			if (index)
				break;
		}
		else {
			printf("title %s\n", title);
			current += res;
			current = config_get_next_property(
						configuration,
						current,
						NULL, NULL);
		}
		for (i = 0; known_properties[i] != NULL; i++)
		{
			if (config_get_indexed_property(
					configuration,
					(res == -1) ? NULL : "title",
					title, 
					known_properties[i],
					property) != -1)
				printf( "    %s %s\n", 
					known_properties[i], 
					property);
		}
	}

	free(configuration);

	close(fd);
	return 0;
}

static int set_config(char *image, int verbose, char *config_path, 
		      char *first_level, char *second_level)
{
	char property[1024];
	char property2[1024];
	char *ramdisk_ondisk, *kernel_ondisk;
	int8_t *configuration;
	int8_t *conffile;
	int fd;
	struct stat st;
	int i;
	int current;
	int res;
	int has_root;
	static char *prolog[] = {
		"gestaltID",
		"default",
		"timeout",
		"vga",
		"modem",
		"printer"
	};
	static char *known_properties[] ={
		"root",
		"kernel",
		"initrd",
		"args",
		"chainloader"
	};

	/* open configuration file */

	fd = open(config_path, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "ERROR: cannot open %s\n", config_path);
		return 5;
	}
	if (fstat(fd, &st) == -1)
	{
		fprintf(stderr, "ERROR: cannot fstat %s\n", config_path);
		return 5;
	}
	conffile = (int8_t*)malloc(st.st_size);
	if (conffile == NULL)
	{
		fprintf(stderr, "ERROR: cannot malloc() %s\n", config_path);
		return 5;
	}

	if (read(fd, conffile, st.st_size) != st.st_size)
	{
		fprintf(stderr, "ERROR: cannot read() %s\n", config_path);
		return 5;
	}
	close(fd);

	/* extract properties */

	if (first_level == NULL) {
		if (config_get_property(conffile,
					"first_level", property) == -1)
			first_level = PREFIX "/lib/emile/first_floppy";
		else
			first_level = property;
	}

	if (second_level == NULL) {
		if (config_get_property(conffile,
					"second_level", property2) == -1)
			second_level = PREFIX "/lib/emile/second_floppy";
		else
			second_level = property2;
	}

	/* create floppy, set first and second level position */

	fd = emile_floppy_create(image, first_level, second_level);
	if (fd < 0)
	{
		fprintf(stderr, "ERROR: cannot create %s\n", image);
		free(conffile);
		return 6;
	}

	/* create configuration information of the floppy image */

	configuration = malloc(65536);
	if (configuration == NULL)
	{
		fprintf(stderr, "ERROR: cannot initalize configuration\n");
		free(conffile);
		return 7;
	}
	memset(configuration, 0, 65536);

	/* copy prolog */

	for (i = 0; i < sizeof(prolog) / sizeof(char*); i++)
	{
		if (config_get_property(conffile,
					prolog[i], property) != -1)
			config_set_property(configuration, prolog[i], property);
	}

	/* get kernel properties */

	kernel_ondisk = NULL;
	ramdisk_ondisk = NULL;
	current = 0;
	while(1)
	{
		res = config_get_property(conffile + current,
					  "title", property);
		if (res == -1)
			break;

		config_add_property(configuration, "title", property);

		if (verbose)
			printf("title %s\n", property);

		current += res;
		current = config_get_next_property(conffile, current,
						   NULL, NULL);

		has_root = 0;
		for (i = 0; i < sizeof(known_properties) / sizeof(char*); i++)
		{
			res = config_get_indexed_property(conffile,
							  "title",
							  property,
							  known_properties[i],
							  property2);
			if (res == -1)
				continue;

			if (strcmp(known_properties[i], "root") == 0)
			{
				has_root = 1;
				if (verbose)
					printf("    %s %s\n",
					       known_properties[i], property2);

				config_set_indexed_property(configuration,
							"title", property,
							known_properties[i],
							property2);
			} else if (has_root || emile_is_url(property2))
			{
				if (verbose)
					printf("    %s %s\n",
					       known_properties[i], property2);

				config_set_indexed_property(configuration,
							"title", property,
							known_properties[i],
							property2);
			}
			else
			{
				char *url;
				if (strcmp(known_properties[i], "kernel") == 0)
				{
					if (kernel_ondisk == NULL)
						kernel_ondisk =
							emile_floppy_add(fd,
								property2);
					url = kernel_ondisk;
				} else if (strcmp(known_properties[i], "initrd") == 0)
				{
					if (ramdisk_ondisk == NULL)
						ramdisk_ondisk =
							emile_floppy_add(fd,
								property2);
					url = ramdisk_ondisk;
				} else
					url = property2;

				config_set_indexed_property(configuration,
						    "title", property,
						    known_properties[i], url);
				if (verbose)
					printf("    %s %s (%s)\n", 
						known_properties[i],
						property2, url);
			}
		}
	}
	if (ramdisk_ondisk != NULL)
		free(ramdisk_ondisk);
	if (kernel_ondisk != NULL)
		free(kernel_ondisk);

	if (strlen((char*)configuration) > 1023)
	{
		int tmpfd;
		char *conf;
		char *tmpfile = strdup("/tmp/emile-install-XXXXXX");

		fprintf(stderr, "WARNING: configuration doesn't fit in second level\n"
				"         will store it at end of floppy\n");

		tmpfd = mkstemp(tmpfile);
		write(tmpfd, configuration, strlen((char*)configuration) + 1);
		close(tmpfd);

		conf = emile_floppy_add(fd, tmpfile);
		free(configuration);

		unlink(tmpfile);
		free(tmpfile);

		configuration = malloc(1024);
		memset(configuration, 0, 1024);

		config_set_property(configuration, "configuration", conf);
		if (verbose)
			printf("configuration %s\n", conf);
	}

	emile_second_set_configuration(fd, configuration);
	emile_floppy_close(fd);

	free(conffile);
	free(configuration);

	return 0;
}

int main(int argc, char** argv)
{
	int verbose = 0;
	int option_index = 0;
	char* first_level = NULL;
	char* second_level = NULL;
	char* kernel_image = NULL;
	char* ramdisk = NULL;
	char* image = NULL;
	char* config_path = NULL;
	int action_getinfo = 0;
	int c;
	int ret;

	while(1)
	{
		c = getopt_long(argc, argv, "hvf:s:k:r:gc:", long_options, 
				&option_index);
		if (c == -1)
			break;
		switch(c)
		{
		case ARG_HELP:
			usage(argc, argv);
			return 0;
		case ARG_VERBOSE:
			verbose = 1;
			break;
		case ARG_FIRST:
			first_level = optarg;
			break;
		case ARG_SECOND:
			second_level = optarg;
			break;
		case ARG_KERNEL:
			kernel_image = optarg;
			break;
		case ARG_RAMDISK:
			ramdisk = optarg;
			break;
		case ARG_GETINFO:
			action_getinfo = 1;
			break;
		case ARG_CONFIG:
			config_path = optarg;
			break;
		}
	}

	if (optind < argc)
		image = argv[optind];

	if (image == NULL)
	{
		fprintf(stderr, 
		"ERROR: you must provide an image file or a block device.\n");
		usage(argc, argv);
		return 1;
	}

	if (action_getinfo)
		return get_info(image, verbose);

	if (config_path != NULL)
	{
		if (kernel_image || ramdisk)
		{
			fprintf(stderr, "ERROR: don't use --kernel"
					" or --ramdisk with --config\n");
			return 4;
		}
		return set_config(image, verbose, config_path, 
				  first_level, second_level);
	}

	if (first_level == NULL)
		first_level = PREFIX "/lib/emile/first_floppy";

	if (second_level == NULL)
		second_level = PREFIX "/lib/emile/second_floppy";

	if (kernel_image == NULL)
		kernel_image = PREFIX "/boot/vmlinuz";

	if (verbose)
	{
		printf("first:   %s\n", first_level);
		printf("second:  %s\n", second_level);
		printf("kernel:  %s\n", kernel_image);
		printf("ramdisk: %s\n", ramdisk);
		printf("image:   %s\n", image);
	}

	ret = emile_floppy_create_image(first_level, second_level, 
					kernel_image, ramdisk, image);
	switch(ret)
	{
	case 0:
		break;
	case EEMILE_CANNOT_WRITE_FIRST:
		fprintf(stderr, "ERROR: cannot write first\n");
		break;
	case EEMILE_CANNOT_WRITE_SECOND:
		fprintf(stderr, "ERROR: cannot write second\n");
		break;
	case EEMILE_CANNOT_WRITE_KERNEL:
		fprintf(stderr, "ERROR: cannot write kernel\n");
		break;
	case EEMILE_CANNOT_WRITE_RAMDISK:
		fprintf(stderr, "ERROR: cannot write ramdisk\n");
		break;
	case EEMILE_CANNOT_WRITE_PAD:
		fprintf(stderr, "ERROR: cannot write padding\n");
		break;
	case EEMILE_CANNOT_CREATE_IMAGE:
		fprintf(stderr, "ERROR: cannot create image\n");
		break;
	case EEMILE_CANNOT_OPEN_FILE:
		fprintf(stderr, "ERROR: cannot open one of provided files\n");
		break;
	default:
		fprintf(stderr, "ERROR: unknown error :-P\n");
		break;
	}

	return ret;
}
