/* MDB Tools - A library for reading MS Access database files
 * Copyright (C) 2000 Brian Bruns
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "mdbtools.h"

MdbHandle *mdb_alloc_handle()
{
MdbHandle *mdb;

	mdb = (MdbHandle *) malloc(sizeof(MdbHandle));
	memset(mdb, '\0', sizeof(MdbHandle));

	return mdb;	
}
void mdb_free_handle(MdbHandle *mdb)
{
	if (!mdb) return;	

	if (mdb->filename) free(mdb->filename);
	if (mdb->catalog) mdb_free_catalog(mdb);
	free(mdb);
}
void mdb_free_catalog(MdbHandle *mdb)
{
GList *l;
MdbCatalogEntry *entryp;

	for (l=g_list_first(mdb->catalog);l;l=g_list_next(l)) {
		entryp = l->data;
		g_free(entryp);
	}
}