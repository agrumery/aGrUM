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

/**
 * @file
 * @brief This file gives access to (too complex) gum::InformationTheory
 *
 * @author Pierre-Henri WUILLEMIN
 */
#include <agrum/agrum.h>

#include <agrum/base/graphicalModels/algorithms/informationTheory.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/MRF/inference/ShaferShenoyMRFInference.h>

#include "helpers.h"

#ifndef AGRUM_INFORMATIONTHEORY_H
#  define AGRUM_INFORMATIONTHEORY_H

class InformationTheory {
  public:
  InformationTheory(gum::LazyPropagation< double >& engine,
                    PyObject*                       Xnames,
                    PyObject*                       Ynames,
                    PyObject*                       Znames = nullptr) {
    gum::NodeSet X;
    gum::NodeSet Y;
    gum::NodeSet Z;
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(X,
                                                              Xnames,
                                                              engine.model().variableNodeMap());
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(Y,
                                                              Ynames,
                                                              engine.model().variableNodeMap());
    if (Znames)
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(Z,
                                                                Znames,
                                                                engine.model().variableNodeMap());
    _bnIT_  = new gum::InformationTheory(engine, X, Y, Z);
    _mrfIT_ = nullptr;
  }

  InformationTheory(gum::ShaferShenoyMRFInference< double >& engine,
                    PyObject*                                Xnames,
                    PyObject*                                Ynames,
                    PyObject*                                Znames = nullptr) {
    gum::NodeSet X;
    gum::NodeSet Y;
    gum::NodeSet Z;
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(X,
                                                              Xnames,
                                                              engine.model().variableNodeMap());
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(Y,
                                                              Ynames,
                                                              engine.model().variableNodeMap());
    if (Znames)
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(Z,
                                                                Znames,
                                                                engine.model().variableNodeMap());
    _bnIT_  = nullptr;
    _mrfIT_ = new gum::InformationTheory(engine, X, Y, Z);
  }

#  define DEFINE_IT_FUNC(func) \
    double func() { return (_bnIT_ != nullptr) ? _bnIT_->func() : _mrfIT_->func(); }

  DEFINE_IT_FUNC(entropyXY)
  DEFINE_IT_FUNC(entropyX)
  DEFINE_IT_FUNC(entropyY)
  DEFINE_IT_FUNC(entropyXgivenY)
  DEFINE_IT_FUNC(entropyYgivenX)
  DEFINE_IT_FUNC(mutualInformationXY)
  DEFINE_IT_FUNC(variationOfInformationXY)
  DEFINE_IT_FUNC(entropyXYgivenZ)
  DEFINE_IT_FUNC(mutualInformationXYgivenZ)

#  undef DEFINT_IT_FUNC

  ~InformationTheory() {
    delete _bnIT_;
    delete _mrfIT_;
  }

  private:
  gum::InformationTheory< gum::LazyPropagation, double >*          _bnIT_;
  gum::InformationTheory< gum::ShaferShenoyMRFInference, double >* _mrfIT_;
};
#endif   // AGRUM_INFORMATIONTHEORY_H
