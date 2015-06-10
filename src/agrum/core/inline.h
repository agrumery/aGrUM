/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
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
/** @file
 * @brief aGrUM's inline/outline selection
 *
 * aGrUM's INLINE is a substitute to the usual C++ inline. It enables compiling
 * aGrUM with or without inline functions, depending on whether we wish to
 * debug parts of aGrUM.
 *
 * From eigen library, aGrUM proposes now:
 * INLINE : the compiler has to try to inline
 * STRONG_INLINE : the compiler has to try harder toinline
 * ALWAYS_INLINE : the compiler has to inline
 * NO_INLINE : the compiler should not inline
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_INLINE_H
#define GUM_INLINE_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef GUM_NO_INLINE

#define INLINE inline

#define ENFORCED_INLINE inline
// ENFORCED_INLINE is a stronger version of the inline, using __forceinline on MSVC,
// but it still doesn't use GCC's always_inline. This is useful in (common)
// situations where MSVC needs forceinline
// but GCC is still doing fine with just inline.
// (from eigen library)
#if (defined _MSC_VER) || (defined __INTEL_COMPILER)
#define ENFORCED_INLINE __forceinline
#else
#define ENFORCED_INLINE inline
#endif

// ALWAYS_INLINE is the strongest, it has the effect of making the function inline
// and adding every possible
// attribute to maximize inlining. This should only be used when really necessary: in
// particular,
// it uses __attribute__((always_inline)) on GCC, which most of the time is useless
// and can severely harm compile times.
// FIXME with the always_inline attribute,
// gcc 3.4.x reports the following compilation error:
//   Eval.h:91: sorry, unimplemented: inlining failed in call to '...' : function
//   body not available
// (from eigen library)
#if GNUC_AT_LEAST(4, 0)
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#else
#define ALWAYS_INLINE EIGEN_STRONG_INLINE
#endif

#if (defined __GNUC__)
#define NO_INLINE __attribute__((noinline))
#elif(defined _MSC_VER)
#define NEVER_INLINE __declspec(noinline)
#else
#define NEVER_INLINE
#endif
#else
#define INLINE
#define STRONG_INLINE
#define ALWAYS_INLINE
#define NEVER_INLINE
#endif /* GUM_NO_INLINE */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#endif /* GUM_INLINE_H */
