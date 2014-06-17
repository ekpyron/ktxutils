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

#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

typedef struct image {
	size_t width;
	size_t height;
	float *data;
} image_t;

image_t *load_image (const char *filename);
void free_image (image_t *image);


#endif /* IMAGE_H */
