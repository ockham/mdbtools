lib_LTLIBRARIES	=	libmdbsql.la
libmdbsql_la_SOURCES=	mdbsql.c parser.y lexer.l 
libmdbsql_la_LDFLAGS = -version-info 1:0:0
DISTCLEANFILES = parser.c parser.h lexer.c
AM_CPPFLAGS	=	-I$(top_srcdir)/include $(GLIB_CFLAGS)
LIBS	=	$(GLIB_LIBS)
libmdbsql_la_LIBADD =	../libmdb/libmdb.la 
YACC = @YACC@ -d

dist-hook:
	rm -f $(distdir)/parser.c $(distdir)/parser.h $(distdir)/lexer.c
