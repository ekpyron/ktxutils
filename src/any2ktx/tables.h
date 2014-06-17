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
#ifndef TABLES_H
#define TABLES_H

#include <GL/glew.h>

#define TABLE_ENTRY(x) { #x, x }

typedef struct table_entry {
	const char *name;
	GLenum value;
} table_entry_t;

typedef struct base_format_table_entry {
	GLenum baseformat;
	table_entry_t *formats;
} base_format_table_entry_t;

extern table_entry_t type_table[];
extern table_entry_t format_table[];
extern base_format_table_entry_t internal_format_table[];
extern base_format_table_entry_t compressed_internal_format_table[];

GLenum table_lookup (const table_entry_t *table, const char *name);
GLenum base_format_table_lookup (const base_format_table_entry_t *table, const char *name, GLenum *baseformat);

#endif /* TABLES_H */
