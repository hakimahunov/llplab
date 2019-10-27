/* include/config.h.  Generated from config.h.in by configure.  */
/*
 * Copyright (c) 2002 Silicon Graphics, Inc.  All Rights Reserved.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @configure_input@
 */

/* Define if you have the attr/error_context.h header */
#define HAVE_ATTR_ERROR_CONTEXT_H 1

/* Define if you want Posix compliant getfacl and setfacl utilities
   without extensions */
/* #undef POSIXLY_CORRECT */

/* The number of bytes in a int.  */
/* #undef SIZEOF_INT */

/* The number of bytes in a long.  */
/* #undef SIZEOF_LONG */

/* The number of bytes in a short.  */
/* #undef SIZEOF_SHORT */

/* Define if you have attribute((visibility(hidden))) in gcc. */
#define HAVE_VISIBILITY_ATTRIBUTE /**/

/* Define if you want gettext (I18N) support */
#define ENABLE_GETTEXT 1

#ifdef ENABLE_GETTEXT
# include <libintl.h>
# define _(x)			gettext(x)
#else
# define _(x)			(x)
# define textdomain(d)		do { } while (0)
# define bindtextdomain(d,dir)	do { } while (0)
#endif
#include <locale.h>

/* On GNU/kFreeBSD, ENODATA is not defined in the system headers */
#include <errno.h>
#ifndef ENODATA
# define ENODATA ENOATTR
#endif
