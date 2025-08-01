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
#ifndef GUM_CAUSAL_MODEL_H
#define GUM_CAUSAL_MODEL_H

#include <agrum/BN/BayesNet.h>
#include <agrum/base/graphs/DAG.h>

namespace gum {
  /// CausalModel is a class representing a causal model, which is a directed acyclic graph (DAG)
  /// where nodes represent variables and edges represent causal relationships.
  template <typename GUM_SCALAR>
  class CausalModel {
    private:
    /// The underlying BayesNet representing the observed part of the model
    BayesNet<GUM_SCALAR> _observedBN_;
    /// The underlying DAG representing the causal structure of the model
    DAG _causalDAG_;

   public:
    /// Default constructor
    CausalModel() = delete;

    // constructor with no latent variables
    explicit CausalModel(const BayesNet<GUM_SCALAR>& observedBN)
        : _observedBN_(observedBN), _causalDAG_(observedBN.dag()) {}

    /// Copy constructor
    CausalModel(const CausalModel& other) = default;

    /// Move constructor
    CausalModel(CausalModel&& other) noexcept = default;

    /// Destructor
    ~CausalModel() = default;

    /// Assignment operator
    CausalModel& operator=(const CausalModel& other) = default;

    /// Move assignment operator
    CausalModel& operator=(CausalModel&& other) noexcept = default;
        
    /// Returns the underlying BayesNet representing the observed part of the model
    const BayesNet<GUM_SCALAR>& observedBayesNet() const {
      return _observedBN_;
    }
    /// Returns the underlying DAG representing the causal structure of the model
    const DAG& causalDAG() const {
      return _causalDAG_;
    }
  };

} // namespace gum

#endif // GUM_CAUSAL_MODEL_H