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

#include "image.h"
#define MAGICKCORE_QUANTUM_DEPTH 32
#define MAGICKCORE_HDRI_ENABLE 1
#include <wand/MagickWand.h>
#include <stdlib.h>

void WandException (MagickWand *wand)
{
	char *desc;
	ExceptionType type;
	desc = MagickGetException (wand, &type);
	fprintf(stderr,"%s %s %lu %s\n", GetMagickModule (), desc);
	MagickRelinquishMemory (desc);
}

image_t *load_image (const char *filename)
{
	MagickWand *wand;
	MagickBooleanType status;

	MagickWandGenesis ();
	wand = NewMagickWand ();

	status = MagickReadImage (wand, filename);
	if (status != MagickTrue)
	{
		WandException (wand);
		DestroyMagickWand (wand);
		MagickWandTerminus ();
		return NULL;
	}

	image_t *image = (image_t*) malloc (sizeof (image_t));

	image->width = MagickGetImageWidth (wand);
	image->height = MagickGetImageHeight (wand);

	image->data = (float*) malloc (image->width * image->height * 4 * sizeof (float));

	status = MagickExportImagePixels (wand, 0, 0, image->width, image->height, "RGBA", FloatPixel, image->data);

	if (status != MagickTrue)
	{
		free (image->data);
		free (image);
		WandException (wand);
		DestroyMagickWand (wand);
		MagickWandTerminus ();
		return NULL;
	}

	DestroyMagickWand (wand);
	MagickWandTerminus ();

	return image;
}

void free_image (image_t *image)
{
	if (image) {
		free (image->data);
		free (image);
	}
}
