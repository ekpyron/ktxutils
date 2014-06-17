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
#include "tables.h"

table_entry_t type_table[] = {
		TABLE_ENTRY (GL_UNSIGNED_BYTE),
		TABLE_ENTRY (GL_BYTE),
		TABLE_ENTRY (GL_UNSIGNED_SHORT),
		TABLE_ENTRY (GL_SHORT),
		TABLE_ENTRY (GL_UNSIGNED_INT),
		TABLE_ENTRY (GL_INT),
		TABLE_ENTRY (GL_FLOAT),
		TABLE_ENTRY (GL_UNSIGNED_BYTE_3_3_2),
		TABLE_ENTRY (GL_UNSIGNED_BYTE_2_3_3_REV),
		TABLE_ENTRY (GL_UNSIGNED_SHORT_5_6_5),
		TABLE_ENTRY (GL_UNSIGNED_SHORT_5_6_5_REV),
		TABLE_ENTRY (GL_UNSIGNED_SHORT_4_4_4_4),
		TABLE_ENTRY (GL_UNSIGNED_SHORT_4_4_4_4_REV),
		TABLE_ENTRY (GL_UNSIGNED_SHORT_5_5_5_1),
		TABLE_ENTRY (GL_UNSIGNED_SHORT_1_5_5_5_REV),
		TABLE_ENTRY (GL_UNSIGNED_INT_8_8_8_8),
		TABLE_ENTRY (GL_UNSIGNED_INT_8_8_8_8_REV),
		TABLE_ENTRY (GL_UNSIGNED_INT_10_10_10_2),
		TABLE_ENTRY (GL_UNSIGNED_INT_2_10_10_10_REV),
		{ NULL, 0 }
};

table_entry_t format_table[] = {
		TABLE_ENTRY (GL_RED),
		TABLE_ENTRY (GL_RG),
		TABLE_ENTRY (GL_RGB),
		TABLE_ENTRY (GL_BGR),
		TABLE_ENTRY (GL_RGBA),
		TABLE_ENTRY (GL_BGRA),
		TABLE_ENTRY (GL_RED_INTEGER),
		TABLE_ENTRY (GL_RG_INTEGER),
		TABLE_ENTRY (GL_RGB_INTEGER),
		TABLE_ENTRY (GL_BGR_INTEGER),
		TABLE_ENTRY (GL_RGBA_INTEGER),
		TABLE_ENTRY (GL_BGRA_INTEGER),
		TABLE_ENTRY (GL_STENCIL_INDEX),
		TABLE_ENTRY (GL_DEPTH_COMPONENT),
		TABLE_ENTRY (GL_DEPTH_STENCIL),
		{ NULL, 0 }
};

base_format_table_entry_t internal_format_table[] = {
		{
				GL_RED, (table_entry_t[]) {
						TABLE_ENTRY (GL_R8),
						TABLE_ENTRY (GL_R8_SNORM),
						TABLE_ENTRY (GL_R16),
						TABLE_ENTRY (GL_R16_SNORM),
						TABLE_ENTRY (GL_R16F),
						TABLE_ENTRY (GL_R32F),
						TABLE_ENTRY (GL_R8I),
						TABLE_ENTRY (GL_R8UI),
						TABLE_ENTRY (GL_R16I),
						TABLE_ENTRY (GL_R16UI),
						TABLE_ENTRY (GL_R32I),
						TABLE_ENTRY (GL_R32UI),
						{ NULL, 0 }
				},
		},
		{
				GL_RG, (table_entry_t[]) {
						TABLE_ENTRY (GL_RG8),
						TABLE_ENTRY (GL_RG8_SNORM),
						TABLE_ENTRY (GL_RG16),
						TABLE_ENTRY (GL_RG16_SNORM),
						TABLE_ENTRY (GL_RG16F),
						TABLE_ENTRY (GL_RG32F),
						TABLE_ENTRY (GL_RG8I),
						TABLE_ENTRY (GL_RG8UI),
						TABLE_ENTRY (GL_RG16I),
						TABLE_ENTRY (GL_RG16UI),
						TABLE_ENTRY (GL_RG32I),
						TABLE_ENTRY (GL_RG32UI),
						{ NULL, 0 }
				}
		},
		{
				GL_RGB, (table_entry_t[]) {
						TABLE_ENTRY (GL_R3_G3_B2),
						TABLE_ENTRY (GL_RGB4),
						TABLE_ENTRY (GL_RGB5),
						TABLE_ENTRY (GL_RGB8),
						TABLE_ENTRY (GL_RGB8_SNORM),
						TABLE_ENTRY (GL_RGB10),
						TABLE_ENTRY (GL_RGB12),
						TABLE_ENTRY (GL_RGB16_SNORM),
						TABLE_ENTRY (GL_SRGB8),
						TABLE_ENTRY (GL_RGB16F),
						TABLE_ENTRY (GL_RGB32F),
						TABLE_ENTRY (GL_R11F_G11F_B10F),
						TABLE_ENTRY (GL_RGB9_E5),
						TABLE_ENTRY (GL_RGB8I),
						TABLE_ENTRY (GL_RGB8UI),
						TABLE_ENTRY (GL_RGB16I),
						TABLE_ENTRY (GL_RGB16UI),
						TABLE_ENTRY (GL_RGB32I),
						TABLE_ENTRY (GL_RGB32UI),
						{ NULL, 0 }
				}
		},
		{
				GL_RGBA, (table_entry_t[]) {
						TABLE_ENTRY (GL_RGBA2),
						TABLE_ENTRY (GL_RGBA4),
						TABLE_ENTRY (GL_RGB5_A1),
						TABLE_ENTRY (GL_RGBA8),
						TABLE_ENTRY (GL_RGBA8_SNORM),
						TABLE_ENTRY (GL_RGB10_A2),
						TABLE_ENTRY (GL_RGB10_A2UI),
						TABLE_ENTRY (GL_RGBA12),
						TABLE_ENTRY (GL_RGBA16),
						TABLE_ENTRY (GL_SRGB8_ALPHA8),
						TABLE_ENTRY (GL_RGBA16F),
						TABLE_ENTRY (GL_RGBA32F),
						TABLE_ENTRY (GL_RGBA8I),
						TABLE_ENTRY (GL_RGBA8UI),
						TABLE_ENTRY (GL_RGBA16I),
						TABLE_ENTRY (GL_RGBA16UI),
						TABLE_ENTRY (GL_RGBA32I),
						TABLE_ENTRY (GL_RGBA32UI),
						{ NULL, 0 }
				}
		},
		{
				GL_DEPTH_COMPONENT, (table_entry_t[]) {
						TABLE_ENTRY (GL_DEPTH_COMPONENT)
				}
		},
		{
				GL_DEPTH_STENCIL, (table_entry_t[]) {
						TABLE_ENTRY (GL_DEPTH_STENCIL)
				}
		},
		{ 0, NULL }
};

base_format_table_entry_t compressed_internal_format_table[] = {
		{
				GL_RED, (table_entry_t[]) {
						TABLE_ENTRY (GL_COMPRESSED_RED),
						TABLE_ENTRY (GL_COMPRESSED_RED_RGTC1),
						TABLE_ENTRY (GL_COMPRESSED_SIGNED_RED_RGTC1),
						{ NULL, 0 }
				},
		},
		{
				GL_RG, (table_entry_t[]) {
						TABLE_ENTRY (GL_COMPRESSED_RG),
						TABLE_ENTRY (GL_COMPRESSED_RG_RGTC2),
						TABLE_ENTRY (GL_COMPRESSED_SIGNED_RG_RGTC2),
						{ NULL, 0 }
				}
		},
		{
				GL_RGB, (table_entry_t[]) {
						TABLE_ENTRY (GL_COMPRESSED_RGB),
						TABLE_ENTRY (GL_COMPRESSED_SRGB),
						TABLE_ENTRY (GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT),
						TABLE_ENTRY (GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT),
						{ NULL, 0 }
				}
		},
		{
				GL_RGBA, (table_entry_t[]) {
						TABLE_ENTRY (GL_COMPRESSED_RGBA),
						TABLE_ENTRY (GL_COMPRESSED_SRGB_ALPHA),
						TABLE_ENTRY (GL_COMPRESSED_RGBA_BPTC_UNORM),
						TABLE_ENTRY (GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM),
						{ NULL, 0 }
				}
		},
		{ 0, NULL }
};

GLenum table_lookup (const table_entry_t *table, const char *name)
{
	const table_entry_t *entry;
	for (entry = table; entry->name != NULL; entry++)
	{
		if (!strcmp (name, entry->name))
		{
			return entry->value;
		}
	}
	return 0;
}

GLenum base_format_table_lookup (const base_format_table_entry_t *table, const char *name, GLenum *baseformat)
{
	const base_format_table_entry_t *entry;
	for (entry = table; entry->formats != NULL; entry++)
	{
		GLenum result = table_lookup (entry->formats, name);
		if (result != 0)
		{
			*baseformat = entry->baseformat;
			return result;
		}
	}
	return 0;
}
