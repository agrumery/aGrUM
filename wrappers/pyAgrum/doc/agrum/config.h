/***************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
*   {prenom.nom}_at_lip6.fr                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

// This file is generated by cmake.
// Please do not change its content.
// Please do not add it to the repository.

// To ease the process (especially for pyAgrum and other apps which directly
// use agrum source instead of linking with agrum shared library), this file is
// generated in source directory :
// it should not be different between release and debug versions

#ifndef GUM_CONFIG_H
#define GUM_CONFIG_H

#define GUM_MAJOR_VERSION 0
#define GUM_MINOR_VERSION 11
#define GUM_PATCH_VERSION 2.9

#define GUM_RANDOMSEED 0

#define GUM_SRC_PATH /home/phw/Documents/gits/lip6/aGrUM-dev/src
#define GUM_VERSION "0.11.2.9"

/* #undef GUM_TRACE_ON */

#define GUM_CHAR_SIZE   1
#define GUM_INT_SIZE    4
#define GUM_LONG_SIZE   8
#define GUM_FLOAT_SIZE  4
#define GUM_DOUBLE_SIZE 8

#define HAVE_MKSTEMP 1
#define HAVE_DIRENT_H 1
#define HAVE_UNISTD_H 1
/* #undef HAVE_SYS_TIME_H */
#define HAVE_EXECINFO_H 1


#define GUM_AT_LEAST(x,y,z) (GUM_MAJOR_VERSION>x || (GUM_MAJOR_VERSION>=x && \
                             (GUM_MINOR_VERSION>y || (GUM_MINOR_VERSION>=y && \
                                 GUM_PATCH_VERSION>=z))))

#ifdef __GNUC__
#define GNUC_AT_LEAST(x,y) ((__GNUC__==x && __GNUC_MINOR__>=y) || __GNUC__>x)
#else
#define GNUC_AT_LEAST(x,y) 0
#endif

#ifdef __GNUC__
#define GNUC_AT_MOST(x,y) ((__GNUC__==x && __GNUC_MINOR__<=y) || __GNUC__<x)
#else
#define GNUC_AT_MOST(x,y) 0
#endif

#ifdef __GNUC__
#define GUM_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define GUM_DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define GUM_DEPRECATED(func) func
#endif

#define GUM_UNUSED(expr) do { (void)(expr); } while (0)

#ifdef _MSC_VER

#pragma warning(disable : 4503) // removing warning about the truncated names of type (>4096)... stupid compiler

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS // removing warning about using (non safe) methods instead of (safe but m$-specific) versions.
#endif

#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE  // removing warning about POSIX name (dup instead of _dup)
#endif

#endif


#include <agrum/core/debug.h>
#include <agrum/core/inline.h>
#include <agrum/core/types.h>
#include <agrum/core/exceptions.h>
#include <agrum/core/utils_misc.h>

#endif // GUM_CONFIG_H