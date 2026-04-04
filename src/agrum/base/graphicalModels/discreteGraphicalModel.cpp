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


#include <agrum/base/graphicalModels/discreteGraphicalModel.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphicalModels/discreteGraphicalModel_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  DiscreteGraphicalModel::DiscreteGraphicalModel() { GUM_CONSTRUCTOR(DiscreteGraphicalModel); }

  DiscreteGraphicalModel::DiscreteGraphicalModel(const DiscreteGraphicalModel& from) :
      GraphicalModel(from), varMap_(from.varMap_) {
    GUM_CONS_CPY(DiscreteGraphicalModel);
  }

  DiscreteGraphicalModel::DiscreteGraphicalModel(DiscreteGraphicalModel&& from) :
      GraphicalModel(std::move(from)), varMap_(std::move(from.varMap_)) {
    GUM_CONS_MOV(DiscreteGraphicalModel);
  }

  DiscreteGraphicalModel::~DiscreteGraphicalModel() { GUM_DESTRUCTOR(DiscreteGraphicalModel); }

  DiscreteGraphicalModel&
      DiscreteGraphicalModel::operator=(const DiscreteGraphicalModel& source) {
    if (this != &source) {
      GraphicalModel::operator=(source);
      varMap_ = source.varMap_;
      GUM_OP_CPY(DiscreteGraphicalModel);
    }
    return *this;
  }

  DiscreteGraphicalModel& DiscreteGraphicalModel::operator=(DiscreteGraphicalModel&& source) {
    if (this != &source) {
      GraphicalModel::operator=(std::move(source));
      varMap_ = std::move(source.varMap_);
      GUM_OP_MOV(DiscreteGraphicalModel);
    }
    return *this;
  }

}   // namespace gum
