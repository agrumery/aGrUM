/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
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
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_INLINE_H
#define GUM_INLINE_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  ifndef GUM_NO_INLINE

#    define INLINE inline

// ENFORCED_INLINE is a stronger version of the inline, using  _forceinline_ on
// MSVC, but it still doesn't use GCC's always_inline. This is useful in
// (common) situations where MSVC needs forceinline but GCC is still doing fine
// with just inline.  (from eigen library)
#    if (defined _MSC_VER) || (defined _INTEL_COMPILER_)
#      define ENFORCED_INLINE __forceinline
#    else
#      define ENFORCED_INLINE inline
#    endif

// ALWAYS_INLINE is the strongest, it has the effect of making the function
// inline and adding every possible attribute to maximize inlining. This should
// only be used when really necessary: in particular, it uses
//  __attribute__((always_inline)) on GCC, which most of the time is useless and
// can severely harm compile times.
// @warning with the always_inline attribute, gcc 3.4.x reports the following
// compilation error:
//   Eval.h:91: sorry, unimplemented: inlining failed in call to '...' :
//   function body not available (from eigen library)
#    if GNUC_AT_LEAST(4, 0)
#      define ALWAYS_INLINE __attribute__((always_inline)) inline
#    else
#      define ALWAYS_INLINE ENFORCED_INLINE
#    endif

#    if (defined __GNUC__)
#      define NO_INLINE __attribute__((noinline))
#    elif (defined _MSC_VER)
#      define NEVER_INLINE _declspec_(noinline)
#    else
#      define NEVER_INLINE
#    endif
#  else   // GUM_NO_INLINE
#    define INLINE
#    define STRONG_INLINE
#    define ALWAYS_INLINE
#    define NEVER_INLINE
#  endif /* GUM_NO_INLINE */

#endif   /* DOXYGEN_SHOULD_SKIP_THIS */

#endif   /* GUM_INLINE_H */
