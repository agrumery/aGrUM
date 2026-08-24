/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


/** @file
 * @brief Inline implementations of gum::KTBNModality's non-template members.
 *
 * KTBNModality is a plain struct, so these are neither templates (they would
 * belong in KTBN_tpl.h) nor ordinary out-of-line functions (KTBN.cpp): they are
 * two-line accessors on the path of fillCPT() and of every addObservation() /
 * addIntervention(), and are meant to stay inlined.
 *
 * @author Seth AGUILA & Anis KHACEF
 */

#include <agrum/KTBN/KTBN.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  INLINE KTBNModality::KTBNModality(const char* modality) : isLabel(true), label(modality) {}

  INLINE KTBNModality::KTBNModality(std::string modality) :
      isLabel(true), label(std::move(modality)) {}

  INLINE Idx KTBNModality::toIndex(const DiscreteVariable& var) const {
    if (isLabel) return var.index(label);   // throws on an unknown label
    if (index >= var.domainSize())
      GUM_ERROR(OutOfBounds,
                "modality index " << index << " is out of the domain of '" << var.name() << "'.")
    return index;
  }

}   // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
