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


/** @file
 * @brief source file for virtual Base classes for mixed graphs listener
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/graphs/parts/listeners/mixedGraphListener.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/parts/listeners/mixedGraphListener_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  MixedGraphListener::MixedGraphListener(const MixedGraphListener& d) {
    GUM_CONS_CPY(MixedGraphListener);
    GUM_ERROR(OperationNotAllowed, "No copy constructor for MixedGraphListener")
  }

  MixedGraphListener& MixedGraphListener::operator=(const MixedGraphListener& d) {
    GUM_OP_CPY(MixedGraphListener);
    GUM_ERROR(OperationNotAllowed, "No copy operator for MixedGraphListener")
  }

  MixedGraphListener::MixedGraphListener(MixedGraph* g) {
    if (!g) { GUM_ERROR(OperationNotAllowed, "A graph listener need a graph to listen to") }

    GUM_CONSTRUCTOR(MixedGraphListener);
    graph_ = g;

    GUM_CONNECT((*graph_), onNodeAdded, (*this), MixedGraphListener::whenNodeAdded);
    GUM_CONNECT((*graph_), onNodeDeleted, (*this), MixedGraphListener::whenNodeDeleted);
    GUM_CONNECT((*graph_), onArcAdded, (*this), MixedGraphListener::whenArcAdded);
    GUM_CONNECT((*graph_), onArcDeleted, (*this), MixedGraphListener::whenArcDeleted);
    GUM_CONNECT((*graph_), onEdgeAdded, (*this), MixedGraphListener::whenEdgeAdded);
    GUM_CONNECT((*graph_), onEdgeDeleted, (*this), MixedGraphListener::whenEdgeDeleted);
  }

  MixedGraphListener::~MixedGraphListener() {
    GUM_DESTRUCTOR(MixedGraphListener);
    ;
  }

}   // namespace gum
