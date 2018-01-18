/*
 * Copyright 2014 Daniel Kirchner
 *
 * This file is part of ktxutils.
 *
 * ktxutils is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ktxutils is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ktxutils.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "ktx.h"
#include "tables.h"

ktx_header_t header;

int load_ktx_header (FILE *f)
{
	if (fread (&header, 1, sizeof (ktx_header_t), f) != sizeof (ktx_header_t)) {
		fprintf (stderr, "Cannot read KTX header.\n");
		return 0;
	}

	const uint8_t ktx_magic[] = KTX_MAGIC;
	if (memcmp (&header.identifier[0], &ktx_magic[0], sizeof (ktx_magic))) {
		fprintf (stderr, "Not a KTX file.\n");
		return 0;
	}

	return 1;
}


int main (int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf (stderr, "Usage: %s ktxfile\n", argv[0]);
		return -1;
	}

	FILE *f = fopen (argv [1], "rb");
	if (!f)
	{
		fprintf (stderr, "Cannot open input file.\n");
		return -1;
	}

	if (!load_ktx_header (f)) {
		fclose (f);
		return -1;
	}

	int compressed = (header.glType == 0) ? 1 : 0;

	if (compressed)
	{
		printf ("Internal Format: %s\n", base_format_table_reverse_lookup (compressed_internal_format_table, header.glInternalFormat, 0));
	}
	else
	{
		printf ("Type: %s\n", table_reverse_lookup (type_table, header.glType));
		printf ("Format: %s\n", table_reverse_lookup (format_table, header.glFormat));
		printf ("Internal Format: %s\n", base_format_table_reverse_lookup (internal_format_table, header.glInternalFormat, 0));
	}
	printf ("Resolution: %d", header.pixelWidth);
	if (header.pixelHeight != 0)
	{
		printf (" x %d", header.pixelHeight);
		if (header.pixelDepth != 0)
			printf (" x %d", header.pixelDepth);
	}
	printf ("\n");
	printf ("Mipmap Levels: %d\n", header.numberOfMipmapLevels);
	printf ("Number of Array Elements: %d\n", header.numberOfArrayElements);
	printf ("Number of Faces: %d\n", header.numberOfFaces);

	return 0;
}
