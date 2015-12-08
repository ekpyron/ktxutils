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
#include <getopt.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tables.h"
#include "ktx.h"

ktx_header_t header = { KTX_MAGIC, 0x04030201, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 };
ktx_header_t sourceheader;
FILE *f = NULL;

GLuint texture = 0;

int load_ktx_header (void)
{
	if (fread (&sourceheader, 1, sizeof (ktx_header_t), f) != sizeof (ktx_header_t)) {
		fprintf (stderr, "Cannot read KTX header.\n");
		return 0;
	}

	const uint8_t ktx_magic[] = KTX_MAGIC;
	if (memcmp (&sourceheader.identifier[0], &ktx_magic[0], sizeof (ktx_magic))) {
		fprintf (stderr, "Not a KTX file.\n");
		return 0;
	}

	if (fseek (f, sourceheader.bytesOfKeyValueData, SEEK_CUR)) {
		fprintf (stderr, "Could not skip key value data.\n");
		return 0;
	}

	return 1;
}


int compressed = 0;

int display = 0;

const char *source_filename = NULL;

const char *dest_filename = NULL;

float defaultalpha = 1.0f;

typedef struct keyvaluedata
{
	struct keyvaluedata *next;
	uint32_t len;
	char data[];
} keyvaluedata_t;
keyvaluedata_t *first_key_value_entry = NULL;
keyvaluedata_t *last_key_value_entry = NULL;

int SetType (const char *type_name)
{
	if (header.glType != 0)
	{
		fprintf (stderr, "Only one type can be specified.\n");
		return 0;
	}
	header.glType = table_lookup (type_table, type_name);
	if (header.glType != 0) return 1;
	fprintf (stderr, "Invalid type.\n");
	return 0;
}

int SetFormat (const char *format_name)
{
	if (header.glFormat != 0)
	{
		fprintf (stderr, "Only one format can be specified.\n");
		return 0;
	}
	header.glFormat = table_lookup (format_table, format_name);
	if (header.glFormat != 0) return 1;
	fprintf (stderr, "Invalid format.\n");
	return 0;
}

int SetInternalFormat (const char *format_name)
{
	if (header.glInternalFormat != 0)
	{
		fprintf (stderr, "Only one internal format can be specified.\n");
		return 0;
	}
	header.glInternalFormat = base_format_table_lookup (internal_format_table, format_name, &header.glBaseInternalFormat);
	if (header.glInternalFormat != 0) return 1;
	header.glInternalFormat = base_format_table_lookup (compressed_internal_format_table, format_name, &header.glBaseInternalFormat);
	if (header.glInternalFormat != 0) {
		compressed = 1;
		return 1;
	}
	fprintf (stderr, "Invalid internal format.\n");
	return 0;
}

int SetLevels (const char *levelstr)
{
	char *endptr;
	header.numberOfMipmapLevels = strtoul (levelstr, &endptr, 10);
	if (levelstr + strlen (levelstr) != endptr)
	{
		fprintf (stderr, "Invalid number of mipmap levels requested.\n");
		return 0;
	}
	return 1;
}

int SetDefaultAlpha (const char *alphastr)
{
	char *endptr;
	defaultalpha = strtof (alphastr, &endptr);
	if (alphastr + strlen (alphastr) != endptr)
	{
		fprintf (stderr, "Invalid default alpha value requested.\n");
		return 0;
	}
	return 1;
}

int AddKeyValueData (const char *key, const char *value)
{
	uint32_t keylen = strlen (key);
	uint32_t valuelen = strlen (value);
	uint32_t len = keylen + valuelen + 2;
	header.bytesOfKeyValueData += sizeof (uint32_t) + len + (3 - ((len + 3) % 4));

	keyvaluedata_t *data = (keyvaluedata_t*) malloc (sizeof (keyvaluedata_t) + len);
	data->next = NULL;
	data->len = len;
	memcpy (&data->data[0], key, keylen + 1);
	memcpy (&data->data[keylen + 1], value, valuelen + 1);
	if (first_key_value_entry == NULL)
		first_key_value_entry = data;
	if (last_key_value_entry == NULL)
		last_key_value_entry = data;
	else
	{
		last_key_value_entry->next = data;
		last_key_value_entry = data;
	}
	return 1;
}

void usage (char *appname)
{
	fprintf (stdout, "Usage: %s [options] source dest\n"
			"Options:\n"
			"  -h, --help                Display this help message.\n"
			"  -t, --type [type]         Specify the component type for\n"
			"                            storing uncompressed image data.\n"
			"  -f, --format [format]     Specify the format for storing\n"
			"                            uncompressed image data.\n"
			"  -i, --internal [format]   Specify the internal storage format for\n"
			"                            uncompressed image data or alternatively\n"
			"                            a compressed storage format.\n"
			"  -l, --levels [levels]     Specify the number of mipmap levels to\n"
			"                            include in the output file.\n"
			"  -a, --alpha [value]       Specify the default alpha value to be used if\n"
			"                            the input image doesn't have an alpha channel\n"
			"  -d, --display             Displays the image rather than converting it.\n"
			"  -k, --key [key]           Specify a key for optional key value data.\n"
			"  -v, --value [value]       Specify a value for optional key value data.\n"
			"\n"
			"Arguments:\n"
			"  source                    Input image.\n"
			"  dest                      Output file name.\n", appname);
	exit (0);
}

int parse_options (int argc, char **argv)
{
	int c = 0;
	char *key = NULL;
	static struct option long_options[] = {
			{ "help", no_argument, 0, 'h' },
			{ "display", no_argument, 0, 'd' },
			{ "type", required_argument, 0, 't' },
			{ "format", required_argument, 0, 'f' },
			{ "internal", required_argument, 0, 'i' },
			{ "levels", required_argument, 0, 'l' },
			{ "alpha", required_argument, 0, 'a' },
			{ "key", required_argument, 0, 'k' },
			{ "value", required_argument, 0, 'v' },
			{ 0, 0, 0, 0 }
	};

	while (1)
	{
		int option_index = 0;
		c = getopt_long (argc, argv, "t:f:l:i:a:k:v:hd", long_options, &option_index);

		if (c== -1) break;

		switch (c)
		{
		case 'a':
			if (!SetDefaultAlpha (optarg)) return 0;
			break;
		case 'd':
			display = 1;
			break;
		case 'h':
			usage (argv[0]);
			break;
		case 't':
			if (!SetType (optarg)) return 0;
			break;
		case 'i':
			if (!SetInternalFormat (optarg)) return 0;
			break;
		case 'f':
			if (!SetFormat (optarg)) return 0;
			break;
		case 'l':
			if (!SetLevels (optarg)) return 0;
			break;
		case 'k':
			if (key != NULL)
			{
				fprintf (stderr, "A key without a value was specified.\n");
				return 0;
			}
			key = optarg;
			break;
		case 'v':
			if (key == NULL)
			{
				fprintf (stderr, "A value without a key was specified.\n");
				return 0;
			}
			if (!AddKeyValueData (key, optarg)) return 0;
			key = NULL;
			break;
		}
	}

	if (key != NULL)
	{
		fprintf (stderr, "A key without a value was specified.\n");
		return 0;
	}

	if (header.glInternalFormat == 0)
	{
		fprintf (stderr, "No internal format was specified.\n");
		return 0;
	}

	if (compressed && (header.glType != 0 || header.glFormat != 0))
	{
		fprintf (stderr, "No type and format can be specified with a compressed internal format.\n");
		return 0;
	}
	if (!compressed && (header.glType == 0 || header.glFormat == 0))
	{
		fprintf (stderr, "Type and format must be specified unless the internal format is a compressed format.\n");
		return 0;
	}

	if (display)
	{
		if (optind + 1 != argc)
		{
			fprintf (stderr, "Invalid number of arguments.\n");
			return 0;
		}
		else
		{
			source_filename = argv [optind];
			return 1;
		}
	}

	if (optind + 2 != argc)
	{
		fprintf (stderr, "Invalid number of arguments.\n");
		return 0;
	}

	source_filename = argv [optind];
	dest_filename = argv [optind + 1];
	return 1;
}


GLFWwindow *window = NULL;


int create_context (void)
{
	if (display) {
		window = glfwCreateWindow (sourceheader.pixelWidth, sourceheader.pixelHeight, "ktx2ktx", NULL, NULL);
	} else {
		glfwWindowHint (GLFW_VISIBLE, GL_FALSE);
		window = glfwCreateWindow (64, 64, "ktx2ktx", NULL, NULL);
	}
    if (!window) {
    	fprintf (stderr, "Cannot open window.\n");
    	return 0;
    }
    glfwMakeContextCurrent (window);

    if (glewInit () != GLEW_OK) {
    	fprintf (stderr, "Cannot initialize GLEW.\n");
    	return 0;
    }
    if (compressed && !GLEW_ARB_texture_compression) {
    	fprintf (stderr, "Texture compression requested, but not supported.\n");
    	return 0;
    }
    glHint (GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);
    return 1;
}

void cleanup (void)
{
	while (first_key_value_entry != NULL)
	{
		keyvaluedata_t *data = first_key_value_entry;
		first_key_value_entry = first_key_value_entry->next;
		free (data);
	}
    if (texture)
    	glDeleteTextures (1, &texture);

    if (window != NULL)
        glfwDestroyWindow (window);

	if (f != NULL)
		fclose (f);

	glfwTerminate ();
}

unsigned int intlog2 (unsigned int v)
{
	unsigned int r = 0;
	while (v >>= 1) r++;
	return r;
}

int load_texture (void)
{
	glGenTextures (1, &texture);

	glBindTexture (GL_TEXTURE_2D, texture);

	int level;

	for (level = 0; level < ((sourceheader.numberOfMipmapLevels == 0) ? 1 : sourceheader.numberOfMipmapLevels); level++)
	{
		uint32_t imageSize = 0;
		if (fread (&imageSize, 1, sizeof (uint32_t), f) != sizeof (uint32_t)) {
			fprintf (stderr, "Could not read image size\n");
			return 0;
		}
		void *data = malloc (imageSize);

		if (fread (data, 1, imageSize, f) != imageSize) {
			fprintf (stderr, "Could not read image data\n");
			free (data);
			return 0;
		}

		if (sourceheader.glType != 0)
		{
			glTexImage2D (GL_TEXTURE_2D, level, sourceheader.glInternalFormat, (sourceheader.pixelWidth >> level), (sourceheader.pixelHeight >> level), 0,
						  sourceheader.glFormat, sourceheader.glType, data);
		}
		else
		{
			glCompressedTexImage2D (GL_TEXTURE_2D, level, sourceheader.glInternalFormat,
									(sourceheader.pixelWidth >> level), (sourceheader.pixelHeight >> level), 0,
									imageSize, data);
		}

		free (data);

		{
			int skip = (3 - ((imageSize + 3) % 4));
			if (skip > 0)
			{
				if (fseek (f, skip, SEEK_CUR))
				{
					fprintf (stderr, "Could not skip padding bytes\n");
					return 0;
				}
			}
		}
	}

	if (sourceheader.numberOfMipmapLevels == 0) {
		glGenerateMipmap (GL_TEXTURE_2D);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else if (header.numberOfMipmapLevels == 1) {
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	} else {
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, sourceheader.numberOfMipmapLevels);
	}

	return texture;
}


int main (int argc, char *argv[])
{
	if (!parse_options (argc, argv)) {
		fprintf (stderr, "Invalid arguments. For help type %s -h.\n", argv[0]);
		return -1;
	}

	if (!glfwInit ())
	{
		fprintf (stderr, "Cannot initialize GLFW.\n");
		return -1;
	}

	f = fopen (source_filename, "rb");
	if (!f)
	{
		fprintf (stderr, "Cannot open input file.\n");
		cleanup ();
		return -1;
	}

	if (!load_ktx_header ()) {
		cleanup ();
		return -1;
	}

	header.pixelWidth = sourceheader.pixelWidth;
	header.pixelHeight = sourceheader.pixelHeight;

	if (header.numberOfMipmapLevels > intlog2 (header.pixelWidth) + 1)
		header.numberOfMipmapLevels = intlog2 (header.pixelWidth) + 1;
	if (header.numberOfMipmapLevels > intlog2 (header.pixelHeight) + 1)
		header.numberOfMipmapLevels = intlog2 (header.pixelHeight) + 1;

	if (!create_context ())
	{
		cleanup ();
		return -1;
	}

	texture = load_texture ();
	if (!texture)
	{
		cleanup ();
		return -1;
	}

	if (display) {
		while (!glfwWindowShouldClose (window)) {
			int w, h;
			glfwGetWindowSize (window, &w, &h);
			glViewport (0, 0, w, h);

			glEnable (GL_TEXTURE_2D);

			glClearColor (0, 0, 0, 0);
			glClear (GL_COLOR_BUFFER_BIT);
			glBegin (GL_QUADS);
			glTexCoord2f (0, 0);
			glVertex3f (-1, -1, 0);
			glTexCoord2f (1, 0);
			glVertex3f (1, -1, 0);
			glTexCoord2f (1, 1);
			glVertex3f (1, 1, 0);
			glTexCoord2f (0, 1);
			glVertex3f (-1, 1, 0);
			glEnd ();

			glfwSwapBuffers (window);
	        glfwPollEvents ();
		}
	}
	else
	{
		FILE *f = fopen (dest_filename, "wb");
		if (!f) {
			fprintf (stderr, "Cannot open output file for writing.\n");
			cleanup ();
			return -1;
		}

		if (fwrite (&header, 1, sizeof (header), f) != sizeof (header)) {
			fclose (f);
			fprintf (stderr, "Could not write ktx header.\n");
			cleanup ();
			return -1;
		}

		{
			keyvaluedata_t *data;
			for (data = first_key_value_entry; data != NULL; data = data->next)
			{
				int i;
				if (fwrite (&data->len, 1, sizeof (uint32_t) + data->len, f) != sizeof (uint32_t) + data->len) {
					fclose (f);
					fprintf (stderr, "Could not write key value pair.\n");
					cleanup ();
					return -1;
				}
				for (i = 0; i < (3 - ((data->len + 3) % 4)); i++)
					fputc (0, f);
			}
		}

		if (compressed)
		{
			glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);
			if (!compressed) {
				fclose (f);
				fprintf (stderr, "Compressed texture format requested, but OpenGL reports an uncompressed texture.\n");
				cleanup ();
				return -1;
			}
			glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &header.glInternalFormat);

			int level;
			for (level = 0; level < ((header.numberOfMipmapLevels == 0) ? 1 : header.numberOfMipmapLevels); level++)
			{
				uint32_t imageSize = 0;
				glGetTexLevelParameteriv (GL_TEXTURE_2D, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &imageSize);

				if (fwrite (&imageSize, 1, sizeof (uint32_t), f) != sizeof (uint32_t)) {
					fclose (f);
					fprintf (stderr, "Could not write image size.\n");
					cleanup ();
					return -1;
				}

				void *data = malloc (imageSize);
				glGetCompressedTexImage (GL_TEXTURE_2D, level, data);
				if (fwrite (data, 1, imageSize, f) != imageSize) {
					free (data);
					fclose (f);
					fprintf (stderr, "Could not write image data.\n");
					cleanup ();
					return -1;
				}
				free (data);

				{
					int i;
					for (i = 0; i < (3 - ((imageSize + 3) % 4)); i++) {
						fputc (0, f);
					}
				}
			}

			fclose (f);

		}
		else
		{
			int pixelSize = 0;
			int components = 0;
			switch (header.glBaseInternalFormat)
			{
			case GL_RED:
			case GL_DEPTH_COMPONENT:
				components = 1;
				break;
			case GL_RG:
			case GL_DEPTH_STENCIL:
				components = 2;
				break;
			case GL_RGB:
				components = 3;
				break;
			case GL_RGBA:
				components = 4;
				break;
			default:
				fclose (f);
				fprintf (stderr, "Invalid base internal format.\n");
				cleanup ();
				return -1;
			}
			switch (header.glType)
			{
			case GL_UNSIGNED_BYTE:
			case GL_BYTE:
				pixelSize = components;
				header.glTypeSize = 1;
				break;
			case GL_UNSIGNED_SHORT:
			case GL_SHORT:
				pixelSize = components * 2;
				header.glTypeSize = 2;
				break;
			case GL_UNSIGNED_INT:
			case GL_INT:
			case GL_FLOAT:
				pixelSize = components * 4;
				header.glTypeSize = 4;
				break;
			case GL_UNSIGNED_BYTE_3_3_2:
			case GL_UNSIGNED_BYTE_2_3_3_REV:
				if (components != 3) {
					fclose (f);
					fprintf (stderr, "Base internal format conflicts with type.\n");
					cleanup ();
					return -1;
				}
				pixelSize = 1;
				header.glTypeSize = 1;
				break;
			case GL_UNSIGNED_SHORT_5_6_5:
			case GL_UNSIGNED_SHORT_5_6_5_REV:
				if (components != 3) {
					fclose (f);
					fprintf (stderr, "Base internal format conflicts with type.\n");
					cleanup ();
					return -1;
				}
				pixelSize = 2;
				header.glTypeSize = 2;
				break;
			case GL_UNSIGNED_SHORT_4_4_4_4:
			case GL_UNSIGNED_SHORT_4_4_4_4_REV:
			case GL_UNSIGNED_SHORT_5_5_5_1:
			case GL_UNSIGNED_SHORT_1_5_5_5_REV:
				if (components != 4) {
					fclose (f);
					fprintf (stderr, "Base internal format conflicts with type.\n");
					cleanup ();
					return -1;
				}
				pixelSize = 2;
				header.glTypeSize = 2;
				break;
			case GL_UNSIGNED_INT_8_8_8_8:
			case GL_UNSIGNED_INT_8_8_8_8_REV:
			case GL_UNSIGNED_INT_10_10_10_2:
			case GL_UNSIGNED_INT_2_10_10_10_REV:
				if (components != 4) {
					fclose (f);
					fprintf (stderr, "Base internal format conflicts with type.\n");
					cleanup ();
					return -1;
				}
				pixelSize = 4;
				header.glTypeSize = 4;
				break;
			default:
				fclose (f);
				fprintf (stderr, "Invalid type.\n");
				cleanup ();
				return -1;
			}

			int level;
			void *data = malloc ((header.pixelWidth * header.pixelHeight * pixelSize + 0xFF) & ~0xFF);
			for (level = 0; level < ((header.numberOfMipmapLevels == 0) ? 1 : header.numberOfMipmapLevels); level++)
			{
				uint32_t imageSize = (header.pixelWidth >> level) * (header.pixelHeight >> level) * pixelSize;
				if (fwrite (&imageSize, 1, sizeof (uint32_t), f) != sizeof (uint32_t)) {
					free (data);
					fclose (f);
					fprintf (stderr, "Could not write image size.\n");
					cleanup ();
					return -1;
				}

				glGetTexImage (GL_TEXTURE_2D, level, header.glFormat, header.glType, data);
				if (fwrite (data, 1, imageSize, f) != imageSize) {
					free (data);
					fclose (f);
					fprintf (stderr, "Could not write image data.\n");
					cleanup ();
					return -1;
				}

				{
					int i;
					for (i = 0; i < (3 - ((imageSize + 3) % 4)); i++) {
						fputc (0, f);
					}
				}
			}
			free (data);
		}
	}

	cleanup ();
	return 0;
}
