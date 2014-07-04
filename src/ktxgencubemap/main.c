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
#include <string.h>
#include "ktx.h"

void usage (char *appname)
{
	fprintf (stdout, "Usage: %s [options] sourcefiles dest\n"
			"Options:\n"
			"  -h, --help                Display this help message.\n"
			"\n"
			"Arguments:\n"
			"  sourcefiles               Six input images.\n"
			"  dest                      Output file name.\n", appname);
	exit (0);
}

int load_ktx_header (FILE *f, ktx_header_t *header)
{
	if (fread (header, 1, sizeof (ktx_header_t), f) != sizeof (ktx_header_t)) {
		fprintf (stderr, "Cannot read KTX header.\n");
		return 0;
	}

	const uint8_t ktx_magic[] = KTX_MAGIC;
	if (memcmp (&header->identifier[0], &ktx_magic[0], sizeof (ktx_magic))) {
		fprintf (stderr, "Not a KTX file.\n");
		return 0;
	}

	if (fseek (f, header->bytesOfKeyValueData, SEEK_CUR)) {
		fprintf (stderr, "Could not skip key value data.\n");
		return 0;
	}

	return 1;
}

ktx_header_t header;

FILE *faces[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
FILE *output = NULL;

int load_headers (int i, const char *filename)
{
	ktx_header_t h;
	faces[i] = fopen (filename, "rb");
	if (!faces[i]) {
		fprintf (stderr, "Cannot open input file: %s\n", filename);
		return 0;
	}

	if (!load_ktx_header (faces[i], &h)) {
		return 0;
	}

	if (h.numberOfArrayElements > 1 || h.numberOfFaces > 1) {
		fprintf (stderr, "Invalid input format: %s\n", filename);
		return 0;
	}

	h.bytesOfKeyValueData = 0;
	if (i == 0)
	{
		memcpy (&header, &h, sizeof (ktx_header_t));
	}
	else if (memcmp (&h, &header, sizeof (ktx_header_t)))
	{
		fprintf (stderr, "Incompatible input format: %s\n", filename);
		return 0;
	}

	return 1;
}

void cleanup (void)
{
	int i;
	for (i = 0; i < 6; i++)
	{
		if (faces[i] != NULL)
			fclose (faces[i]);
	}
	if (output != NULL)
		fclose (output);
}

int main (int argc, char *argv[])
{
	int i;
	if (argc != 8) {
		fprintf (stderr, "Usage: %s [input files] [output file]\n", argv[0]);
		return -1;
	}

	for (i = 0; i < 6; i++)
	{
		if (!load_headers (i, argv[i + 1]))
		{
			cleanup ();
			return -1;
		}
	}

	header.numberOfFaces = 6;

	FILE *output = fopen (argv[7], "wb");
	if (output == NULL)
	{
		cleanup ();
		fprintf (stderr, "Could not open output file: %s\n", argv[7]);
		return -1;
	}

	if (fwrite (&header, 1, sizeof (ktx_header_t), output) != sizeof (ktx_header_t))
	{
		cleanup ();
		fprintf (stderr, "Could not write ktx header.\n");
		return -1;
	}

	int level;
	for (level = 0; level < ((header.numberOfMipmapLevels == 0) ? 1 : header.numberOfMipmapLevels); level++)
	{
		uint32_t imageSize;
		for (i = 0; i < 6; i++)
		{
			uint32_t s;
			if (fread (&s, 1, sizeof (uint32_t), faces[i]) != sizeof (uint32_t))
			{
				cleanup ();
				fprintf (stderr, "Could not read image size.\n");
				return -1;
			}
			if (i == 0) { imageSize = s; }
			else if (s != imageSize) {
				cleanup ();
				fprintf (stderr, "Conflicting image sizes.\n");
				return -1;
			}
		}
		if (fwrite (&imageSize, 1, sizeof (uint32_t), output) != sizeof (uint32_t))
		{
			cleanup ();
			fprintf (stderr, "Could not write image size.\n");
			return -1;
		}
		for (i = 0; i < 6; i++)
		{
			int n;
			for (n = 0; n < imageSize; n++)
			{
				int c = fgetc (faces[i]);
				if (c == EOF)
				{
					cleanup ();
					fprintf (stderr, "Premature End Of File.\n");
					return -1;
				}
				if (fputc (c, output) == EOF)
				{
					cleanup ();
					fprintf (stderr, "Write error.\n");
					return -1;
				}
			}
			for (n = 0; n < 3 - ((imageSize + 3) % 4); n++)
			{
				if (fgetc (faces[i]) == EOF)
				{
					cleanup ();
					fprintf (stderr, "Premature End Of File.\n");
					return -1;
				}
				if (fputc (0, output) == EOF)
				{
					cleanup ();
					fprintf (stderr, "Write error.\n");
					return -1;
				}
			}
		}
	}

	cleanup ();
	return 0;
}
