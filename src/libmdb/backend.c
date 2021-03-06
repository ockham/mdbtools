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

#ifdef JAVA
#include "javadefines.h"
#define MdbBackendType_STRUCT_ELEMENT(a,b,c,d) new MdbBackendType(a,b,c,d)
#else
#define MdbBackendType_STRUCT_ELEMENT(a,b,c,d) {a,b,c,d}
/*
** functions to deal with different backend database engines
*/
#
#include "mdbtools.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#endif   /* JAVA */

static int is_init;
GHashTable *mdb_backends;

   /*    Access data types */
static MdbBackendType mdb_access_types[] = {
		MdbBackendType_STRUCT_ELEMENT("Unknown 0x00", 0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Boolean", 0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Byte", 0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Integer", 0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Long Integer", 0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Currency", 0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Single", 0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Double", 0,0,0),
		MdbBackendType_STRUCT_ELEMENT("DateTime (Short)", 0,0,1),
		MdbBackendType_STRUCT_ELEMENT("Binary", 0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Text", 1,0,1),
		MdbBackendType_STRUCT_ELEMENT("OLE", 1,0,1),
		MdbBackendType_STRUCT_ELEMENT("Memo/Hyperlink",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("Unknown 0x0d",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Unknown 0x0e",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Replication ID",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Numeric",1,1,0)
};

/*    Oracle data types */
static MdbBackendType mdb_oracle_types[] = {
		MdbBackendType_STRUCT_ELEMENT("Oracle_Unknown 0x00",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("NUMBER",1,0,0),
		MdbBackendType_STRUCT_ELEMENT("NUMBER",1,0,0),
		MdbBackendType_STRUCT_ELEMENT("NUMBER",1,0,0),
		MdbBackendType_STRUCT_ELEMENT("NUMBER",1,0,0),
		MdbBackendType_STRUCT_ELEMENT("NUMBER",1,0,0),
		MdbBackendType_STRUCT_ELEMENT("FLOAT",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("FLOAT",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("DATE",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Oracle_Unknown 0x09",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("VARCHAR2",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("BLOB",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("CLOB",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("Oracle_Unknown 0x0d",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Oracle_Unknown 0x0e",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("NUMBER",1,0,0),
		MdbBackendType_STRUCT_ELEMENT("NUMBER",1,0,0),
};

/*    Sybase/MSSQL data types */
static MdbBackendType mdb_sybase_types[] = {
		MdbBackendType_STRUCT_ELEMENT("Sybase_Unknown 0x00",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("bit",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("char",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("smallint",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("int",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("money",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("real",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("float",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("smalldatetime",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Sybase_Unknown 0x09",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("varchar",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("varbinary",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("text",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("Sybase_Unknown 0x0d",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Sybase_Unknown 0x0e",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Sybase_Replication ID",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("numeric",1,1,0),
};

/*    Postgres data types */
static MdbBackendType mdb_postgres_types[] = {
		MdbBackendType_STRUCT_ELEMENT("Postgres_Unknown 0x00",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("BOOL",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("SMALLINT",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("INTEGER",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("INTEGER",0,0,0), /* bigint */
		MdbBackendType_STRUCT_ELEMENT("NUMERIC(15,2)",0,0,0), /* money deprecated ? */
		MdbBackendType_STRUCT_ELEMENT("REAL",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("DOUBLE PRECISION",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("TIMESTAMP WITHOUT TIME ZONE",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("BYTEA",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("VARCHAR",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("Postgres_Unknown 0x0b",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("TEXT",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Postgres_Unknown 0x0d",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Postgres_Unknown 0x0e",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("UUID",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("Postgres_Unknown 0x10",0,0,0),
};
/*    MySQL data types */
static MdbBackendType mdb_mysql_types[] = {
		MdbBackendType_STRUCT_ELEMENT("Text",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("char",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("int",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("int",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("int",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("float",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("float",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("float",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("date",0,0,1),
		MdbBackendType_STRUCT_ELEMENT("varchar",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("varchar",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("varchar",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("text",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("blob",0,0,0),
		MdbBackendType_STRUCT_ELEMENT("text",1,0,1),
		MdbBackendType_STRUCT_ELEMENT("numeric",1,1,0),
		MdbBackendType_STRUCT_ELEMENT("numeric",1,1,0),
};
#ifndef JAVA
static gboolean mdb_drop_backend(gpointer key, gpointer value, gpointer data);

char* sanitize_name(const char* str)
{
	char *result = malloc(256);
	char *p = result;

	if (*str) {
		*p = isalpha(*str) ? *str : '_';
		p++;
        if (!isdigit(*str))  /* if it was a digit, keep it */
		str++;
	}

	while (*str) {
		*p = isalnum(*str) ? *str : '_';
		p++;
		str++;
	}

	*p = 0;

	return result;
}

static char* quote_name_with_brackets(const char* name)
{
	char *result = malloc(strlen(name)+3);
	sprintf(result, "[%s]", name);
	return result;
}

static char* quote_name_with_dquotes(const char* name)
{
	char *result = malloc(2*strlen(name)+3);
	char *p = result;
	*p++ = '"';
	while (*name) {
		*p++ = *name;
		if (*name == '"')
			*p++ = *name; /* double it */
		name ++;
	}
	*p++ = '"';
	*p++ = 0;
	return result;
}

static char* quote_name_with_rquotes(const char* name)
{
	return (char*)g_strconcat("`", name, "`", NULL);
}

char *mdb_get_coltype_string(MdbBackend *backend, int col_type)
{
	static char buf[16];

	if (col_type > 0x10 ) {
   		// return NULL;
		snprintf(buf,sizeof(buf), "type %04x", col_type);
		return buf;
	} else {
		return backend->types_table[col_type].name;
	}
}

int mdb_coltype_takes_length(MdbBackend *backend, int col_type)
{
	return backend->types_table[col_type].needs_length;
}

/**
 * mdb_init_backends
 *
 * Initializes the mdb_backends hash and loads the builtin backends.
 * Use mdb_remove_backends() to destroy this hash when done.
 */
void mdb_init_backends()
{
	mdb_backends = g_hash_table_new(g_str_hash, g_str_equal);

	mdb_register_backend(mdb_access_types, quote_name_with_brackets, "access");
	mdb_register_backend(mdb_sybase_types, quote_name_with_dquotes, "sybase");
	mdb_register_backend(mdb_oracle_types, quote_name_with_dquotes, "oracle");
	mdb_register_backend(mdb_postgres_types, quote_name_with_dquotes, "postgres");
	mdb_register_backend(mdb_mysql_types, quote_name_with_rquotes, "mysql");
}
void mdb_register_backend(MdbBackendType *backend_type, char* (*quote_name)(const char*), char *backend_name)
{
	MdbBackend *backend = (MdbBackend *) g_malloc0(sizeof(MdbBackend));
	backend->types_table = backend_type;
	backend->quote_name  = quote_name;
	g_hash_table_insert(mdb_backends, backend_name, backend);
}

/**
 * mdb_remove_backends
 *
 * Removes all entries from and destroys the mdb_backends hash.
 */
void mdb_remove_backends()
{
	g_hash_table_foreach_remove(mdb_backends, mdb_drop_backend, NULL);
	g_hash_table_destroy(mdb_backends);
}
static gboolean mdb_drop_backend(gpointer key, gpointer value, gpointer data)
{
	MdbBackend *backend = (MdbBackend *)value;
	g_free (backend);
	return TRUE;
}

/**
 * mdb_set_default_backend
 * @mdb: Handle to open MDB database file
 * @backend_name: Name of the backend to set as default
 *
 * Sets the default backend of the handle @mdb to @backend_name.
 *
 * Returns: 1 if successful, 0 if unsuccessful.
 */
int mdb_set_default_backend(MdbHandle *mdb, const char *backend_name)
{
	MdbBackend *backend;

	backend = (MdbBackend *) g_hash_table_lookup(mdb_backends, backend_name);
	if (backend) {
		mdb->default_backend = backend;
		mdb->backend_name = (char *) g_strdup(backend_name);
		is_init = 0;
		return 1;
	} else {
		return 0;
	}
}

/**
 * mdb_get_sequences
 * @mdb: Handle to open MDB database file
 *
 * Generates sequences and set default values
 *
 * Returns: a string stating that relationships are not supported for the
 *   selected backend, or a string containing SQL commands for setting up
 *   the sequences, tailored for the selected backend.
 *   Returns NULL on last iteration.
 *   The caller is responsible for freeing this string.
 */
char *mdb_get_sequences(MdbCatalogEntry *entry, char *namespace, int sanitize)
{
	MdbTableDef *table;
	MdbHandle *mdb = entry->mdb;
	int i;
	int backend = 0;  /* Backends: 1=oracle, 2=postgres */
	const char *quoted_table_name;
	char *result = NULL;
	char tmp[4*512+512]; /* maximum size is 4 quoted names + some constants */


	if (is_init == 1) { /* the second time through */
		is_init = 0;
		return NULL;
	}
	is_init = 1;
	/* the first time through */

	if (!strcmp(mdb->backend_name, "postgres")) {
		backend = 2;
	} else {
		return (char *) g_strconcat(
			"-- sequences are not supported for ",
			mdb->backend_name, NULL);
	}

	/* get the columns */
	table = mdb_read_table (entry);

	/* get the columns */
	mdb_read_columns (table);

	if (sanitize)
		quoted_table_name = sanitize_name(table->name);
	else
		quoted_table_name = mdb->default_backend->quote_name(table->name);

	for (i = 0; i < table->num_cols; i++) {
		MdbColumn *col;
		col = g_ptr_array_index (table->columns, i);
		if (col->is_long_auto) {
			const char *quoted_column_name;
			char sequence_name[256+1+256+4+1];
			const char *quoted_sequence_name;
			quoted_column_name = mdb->default_backend->quote_name(col->name);
			sprintf(sequence_name, "%s_%s_seq", entry->object_name, col->name);
			quoted_sequence_name = mdb->default_backend->quote_name(sequence_name);
			sprintf (tmp, "CREATE SEQUENCE %s OWNED BY %s.%s;\n", quoted_sequence_name, quoted_table_name, quoted_column_name);
			if (result) {
				result = realloc(result, strlen(result) + strlen(tmp) + 1); /* sentry */
				strcat(result, tmp);
			} else
				result = strdup(tmp);
			/* after that point, result can't be NULL any more */

			sprintf (tmp, "ALTER TABLE %s ALTER COLUMN %s SET DEFAULT pg_catalog.nextval('%s');\n\n",
				quoted_table_name, quoted_column_name, quoted_sequence_name);
			result = realloc(result, strlen(result) + strlen(tmp) + 1); /* sentry */
			strcat(result, tmp);

			free((void*)quoted_column_name);
			free((void*)quoted_sequence_name);
		}
	}

	if (!result)
		is_init = 0;
	return result;
}


/**
 * mdb_get_relationships
 * @mdb: Handle to open MDB database file
 *
 * Generates relationships by reading the MSysRelationships table.
 *   'szColumn' contains the column name of the child table.
 *   'szObject' contains the table name of the child table.
 *   'szReferencedColumn' contains the column name of the parent table.
 *   'szReferencedObject' contains the table name of the parent table.
 *   'grbit' contains integrity constraints.
 *
 * Returns: a string stating that relationships are not supported for the
 *   selected backend, or a string containing SQL commands for setting up
 *   the relationship, tailored for the selected backend.
 *   Returns NULL on last iteration.
 *   The caller is responsible for freeing this string.
 */
char *mdb_get_relationships(MdbHandle *mdb)
{
	unsigned int i;
	gchar *text = NULL;  /* String to be returned */
	static char *bound[5];  /* Bound values */
	static MdbTableDef *table;  /* Relationships table */
	int backend = 0;  /* Backends: 1=oracle, 2=postgres */
	char *quoted_table_1, *quoted_column_1,
	     *quoted_table_2, *quoted_column_2,
	     *index_name, *quoted_index_name,
	     *constraint_name, *quoted_constraint_name;
	long grbit;

	if (!strcmp(mdb->backend_name, "oracle")) {
		backend = 1;
	} else if (!strcmp(mdb->backend_name, "postgres")) {
		backend = 2;
	} else {
		if (is_init == 0) { /* the first time through */
			is_init = 1;
			return (char *) g_strconcat(
				"-- relationships are not supported for ",
				mdb->backend_name, NULL);
		} else { /* the second time through */
			is_init = 0;
			return NULL;
		}
	}

	if (is_init == 0) {
		table = mdb_read_table_by_name(mdb, "MSysRelationships", MDB_TABLE);
		if ((!table) || (table->num_rows == 0)) {
			fprintf(stderr, "No MSysRelationships\n");
			return NULL;
		}

		mdb_read_columns(table);
		for (i=0;i<5;i++) {
			bound[i] = (char *) g_malloc0(MDB_BIND_SIZE);
		}
		mdb_bind_column_by_name(table, "szColumn", bound[0], NULL);
		mdb_bind_column_by_name(table, "szObject", bound[1], NULL);
		mdb_bind_column_by_name(table, "szReferencedColumn", bound[2], NULL);
		mdb_bind_column_by_name(table, "szReferencedObject", bound[3], NULL);
		mdb_bind_column_by_name(table, "grbit", bound[4], NULL);
		mdb_rewind_table(table);

		is_init = 1;
	}
	else {
		if (!table) {
			fprintf(stderr, "table is NULL\n");
		}
	    if (table->cur_row >= table->num_rows) {  /* past the last row */
			for (i=0;i<5;i++)
				g_free(bound[i]);
			is_init = 0;
			return NULL;
		}
	}

	if (!mdb_fetch_row(table)) {
		for (i=0;i<5;i++)
			g_free(bound[i]);
		is_init = 0;
		return NULL;
	}

	quoted_table_1 = mdb->default_backend->quote_name(bound[1]);
	quoted_column_1 = mdb->default_backend->quote_name(bound[0]);
	quoted_table_2 = mdb->default_backend->quote_name(bound[3]);
	quoted_column_2 = mdb->default_backend->quote_name(bound[2]);
	grbit = atoi(bound[4]);
	constraint_name = g_strconcat(bound[1], "_", bound[0], "_fk", NULL);
	quoted_constraint_name = mdb->default_backend->quote_name(constraint_name);
	free(constraint_name);
	index_name = g_strconcat(bound[3], "_", bound[2], "_idx", NULL);
	quoted_index_name = mdb->default_backend->quote_name(index_name);
	free(index_name);

	if (grbit & 0x00000002) {
		text = g_strconcat(
			"-- Relationship from ", quoted_table_1,
			" (", quoted_column_1, ")"
			" to ", quoted_table_2, "(", quoted_column_2, ")",
			" does not enforce integrity.\n", NULL);
	} else {
		switch (backend) {
		  case 1:  /* oracle */
		  case 2:  /* postgres */
			text = g_strconcat(
				"ALTER TABLE ", quoted_table_1,
				" ADD CONSTRAINT ", quoted_constraint_name,
				" FOREIGN KEY (", quoted_column_1, ")"
				" REFERENCES ", quoted_table_2, "(", quoted_column_2, ")",
				(grbit & 0x00000100) ? " ON UPDATE CASCADE" : "",
				(grbit & 0x00001000) ? " ON DELETE CASCADE" : "",
				";\n", NULL);

			break;
		}
	}
	free(quoted_table_1);
	free(quoted_column_1);
	free(quoted_table_2);
	free(quoted_column_2);
	free(quoted_constraint_name);
	free(quoted_index_name);

	return (char *)text;
}
#endif
