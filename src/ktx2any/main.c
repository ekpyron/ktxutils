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
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ktx.h"
#include <string.h>
#define MAGICKCORE_QUANTUM_DEPTH 32
#define MAGICKCORE_HDRI_ENABLE 1
#include <wand/MagickWand.h>

GLFWwindow *window = NULL;
FILE *f = NULL;
ktx_header_t header;
GLuint texture = 0;
float *imagedata = NULL;

int load_texture (void)
{
	glGenTextures (1, &texture);

	glBindTexture (GL_TEXTURE_2D, texture);

	int level;

	for (level = 0; level < ((header.numberOfMipmapLevels == 0) ? 1 : header.numberOfMipmapLevels); level++)
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

		if (header.glType != 0)
		{
			glTexImage2D (GL_TEXTURE_2D, level, header.glInternalFormat, (header.pixelWidth >> level), (header.pixelHeight >> level), 0,
					header.glFormat, header.glType, data);
		}
		else
		{
			glCompressedTexImage2D (GL_TEXTURE_2D, level, header.glInternalFormat,
					(header.pixelWidth >> level), (header.pixelHeight >> level), 0,
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

	if (header.numberOfMipmapLevels == 0) {
		glGenerateMipmap (GL_TEXTURE_2D);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else if (header.numberOfMipmapLevels == 1) {
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	} else {
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, header.numberOfMipmapLevels);
	}

	return 1;
}

int load_ktx_header (void)
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

	if (fseek (f, header.bytesOfKeyValueData, SEEK_CUR)) {
		fprintf (stderr, "Could not skip key value data.\n");
		return 0;
	}

	return 1;
}

int create_window (void)
{
	glfwWindowHint (GLFW_VISIBLE, GL_FALSE);
	window = glfwCreateWindow (64, 64, "ktxviewer", NULL, NULL);
    if (!window) {
    	fprintf (stderr, "Cannot open window.\n");
    	return 0;
    }
    glfwMakeContextCurrent (window);

    if (glewInit () != GLEW_OK) {
    	fprintf (stderr, "Cannot initialize GLEW.\n");
    	return 0;
    }

    return 1;
}


void cleanup (void)
{
	if (imagedata != NULL) free (imagedata);
	if (texture != 0) glDeleteTextures (1, &texture);
    if (window != NULL) glfwDestroyWindow (window);
	if (f != NULL) fclose (f);
	glfwTerminate ();
}

void WandException (MagickWand *wand)
{
	char *desc;
	ExceptionType type;
	desc = MagickGetException (wand, &type);
	fprintf(stderr,"%s %s %lu %s\n", GetMagickModule (), desc);
	MagickRelinquishMemory (desc);
}

int image_save (const char *filename) {
	MagickWand *wand;
	PixelWand *color;

	MagickWandGenesis ();
	wand = NewMagickWand ();

	color = NewPixelWand ();
	PixelSetColor (color, "black");

	if (MagickNewImage (wand, header.pixelWidth, header.pixelHeight, color) != MagickTrue) {
		WandException (wand);
		DestroyMagickWand (wand);
		MagickWandTerminus ();
		return 0;
	}

	if (MagickImportImagePixels (wand, 0, 0, header.pixelWidth, header.pixelHeight, "RGBA", FloatPixel, imagedata) != MagickTrue) {
		WandException (wand);
		DestroyMagickWand (wand);
		MagickWandTerminus ();
		return 0;
	}

	if (MagickWriteImage (wand, filename) != MagickTrue) {
		WandException (wand);
		DestroyMagickWand (wand);
		MagickWandTerminus ();
		return 0;
	}

	DestroyMagickWand (wand);
	MagickWandTerminus ();

	return 1;
}

int main (int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf (stderr, "Usage: %s ktxfile outputfile\n", argv[0]);
		return 1;
	}

	if (!glfwInit ())
	{
		fprintf (stderr, "Cannot initialize GLFW.\n");
		return 1;
	}

	f = fopen (argv [1], "rb");
	if (!f)
	{
		fprintf (stderr, "Cannot open input file.\n");
		cleanup ();
		return 1;
	}

	if (!load_ktx_header ()) {
		cleanup ();
		return 1;
	}

	if (!create_window ()) {
		cleanup ();
		return 1;
	}

	if (!load_texture ()) {
		cleanup ();
		return 1;
	}

	imagedata = (float*) malloc (header.pixelWidth * header.pixelHeight * 4 * sizeof (float));
	glGetTexImage (GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &imagedata[0]);

	if (!image_save (argv[2])) {
		cleanup ();
		return 1;
	}

	cleanup ();

	return 0;
}
