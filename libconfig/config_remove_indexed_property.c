/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

void config_remove_indexed_property(char *configuration, char *index_name,
				    char *index_property, char *name)
{
	int last_index;
	int index;
	int len = 0;

	last_index = config_find_indexed_property(configuration, index_name,
					     index_property, name,  NULL);
	if (last_index == -1)
		return;
	index = config_get_next_property(configuration, 
					 last_index, NULL, NULL);
	if (index != -1)
	{
		len = strlen(configuration + index);
		memmove(configuration + last_index, configuration + index, len);
	}

	if (configuration[index + len - 1] == '\n')
		len--;
	configuration[index + len] = 0;
}