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


#include <agrum/base/graphicalModels/graphicalModel.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphicalModels/graphicalModel_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {
  GraphicalModel::GraphicalModel() { GUM_CONSTRUCTOR(GraphicalModel) }

  GraphicalModel::GraphicalModel(const GraphicalModel& from) { GUM_CONS_CPY(GraphicalModel); }

  GraphicalModel::~GraphicalModel() { GUM_DESTRUCTOR(GraphicalModel); }

  GraphicalModel& GraphicalModel::operator=(const GraphicalModel& source) {
    if (this != &source) { _propertiesMap_ = source._propertiesMap_; }
    return *this;
  }

  NodeSet GraphicalModel::nodeset(const std::vector< std::string >& names) const {
    NodeSet res;
    for (const auto& name: names) {
      res.insert(idFromName(name));
    }
    return res;
  }

  void
      GraphicalModel::spaceCplxToStream(std::stringstream& s, double dSize, int dim, Size usedMem) {
    if (dSize > 6) s << "domainSize: 10^" << dSize;
    else s << "domainSize: " << std::round(std::pow(10.0, dSize));

    s << ", dim: " << dim << ", mem: ";

    if (const Size go = usedMem / (1024 * 1024 * 1024); go > 0) s << go << "Go ";
    if (const Size mo = (usedMem / (1024 * 1024)) % 1024; mo > 0) s << mo << "Mo ";
    if (const Size ko = (usedMem / 1024) % 1024; ko > 0) s << ko << "Ko ";
    s << usedMem % 1024 << "o";
  }
}   // namespace gum
