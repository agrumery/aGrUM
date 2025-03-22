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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief This file contains abstract class definitions influence diagrams
 *        inference classes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe Gonzalez(@
 */
#ifndef AGRUM_DECISIONTENSOR_H
#define AGRUM_DECISIONTENSOR_H

#include <agrum/base/multidim/tensor.h>

namespace gum {
  /**
   * @class DecisionTensor decisionTensor.h
   *<agrum/ID/inference/decisionTensor.h>
   * @brief Tensor for optimization in LIMIDS (such as Single Policy Update)
   */
  template < typename GUM_SCALAR >
  class DecisionTensor {
    public:
    Tensor< GUM_SCALAR > probPot;
    Tensor< GUM_SCALAR > utilPot;

    explicit DecisionTensor() {
      GUM_CONSTRUCTOR(DecisionTensor);
      probPot.fillWith(GUM_SCALAR(1));
      utilPot.fillWith(GUM_SCALAR(0));
    }

    ~DecisionTensor() {
      GUM_DESTRUCTOR(DecisionTensor);
      ;
    }

    DecisionTensor(const Tensor< GUM_SCALAR >& prob, const Tensor< GUM_SCALAR >& util) :
        probPot(prob), utilPot(util) {
      GUM_CONSTRUCTOR(DecisionTensor);
    }

    DecisionTensor(const DecisionTensor< GUM_SCALAR >& dp) :
        probPot(dp.probPot), utilPot(dp.utilPot) {
      GUM_CONS_CPY(DecisionTensor);
    }

    void clear() {
      gum::Tensor< GUM_SCALAR > p;
      p.fillWith(GUM_SCALAR(1));
      probPot = p;
      p.fillWith(GUM_SCALAR(0));
      utilPot = p;
    }

    DecisionTensor< GUM_SCALAR >& operator=(const DecisionTensor< GUM_SCALAR >& src) {
      GUM_OP_CPY(DecisionTensor);
      if (&src == this) return *this;
      probPot = src.probPot;
      utilPot = src.utilPot;
      return *this;
    }

    DecisionTensor(DecisionTensor< GUM_SCALAR >&& dp) :
        probPot(std::forward< Tensor< GUM_SCALAR > >(dp.probPot)),
        utilPot(std::forward< Tensor< GUM_SCALAR > >(dp.utilPot)) {
      GUM_CONS_MOV(DecisionTensor);
    }

    DecisionTensor< GUM_SCALAR >& operator=(DecisionTensor< GUM_SCALAR >&& src) {
      GUM_OP_MOV(DecisionTensor);
      if (&src == this) return *this;
      probPot = std::forward< Tensor< GUM_SCALAR > >(src.probPot);
      utilPot = std::forward< Tensor< GUM_SCALAR > >(src.utilPot);
      return *this;
    }

    bool operator==(const DecisionTensor< GUM_SCALAR >& p) const {
      // @see Evaluating Influence Diagrams using LIMIDS (2000) - section 3.3
      return ((p.probPot == this->probPot)
              && (p.probPot * p.utilPot == this->probPot * this->utilPot));
    }

    bool operator!=(const DecisionTensor< GUM_SCALAR >& p) const { return !operator==(p); }

    const DiscreteVariable* variable(const std::string& name) const {
      for (const auto& v: probPot.variablesSequence()) {
        if (v->name() == name) return v;
      }
      for (const auto& v: utilPot.variablesSequence()) {
        if (v->name() == name) return v;
      }

      GUM_ERROR(NotFound, "'" << name << "' can not be found in DecisionTensor.")
    }

    void insertProba(const gum::Tensor< GUM_SCALAR >& proba) { probPot *= proba; }

    void insertUtility(const gum::Tensor< GUM_SCALAR >& util) { utilPot += util; }

    DecisionTensor< GUM_SCALAR > operator*(const DecisionTensor< GUM_SCALAR >& dp1) const {
      return DecisionTensor< GUM_SCALAR >::combination(*this, dp1);
    }

    DecisionTensor< GUM_SCALAR > operator*=(const DecisionTensor< GUM_SCALAR >& dp1) {
      *this = DecisionTensor< GUM_SCALAR >::combination(*this, dp1);
      return *this;
    }

    DecisionTensor< GUM_SCALAR > operator^(const gum::VariableSet& onto) const {
      return DecisionTensor< GUM_SCALAR >::marginalization(*this, onto);
    }

    DecisionTensor< GUM_SCALAR > operator^(const std::vector< std::string >& ontonames) const {
      return DecisionTensor< GUM_SCALAR >::marginalization(*this, ontonames);
    }

    static Tensor< GUM_SCALAR > divideEvenZero(const Tensor< GUM_SCALAR >& p1,
                                               const Tensor< GUM_SCALAR >& p2) {
      Tensor< GUM_SCALAR > res(p1);
      Instantiation        I(res);
      for (I.setFirst(); !I.end(); I.inc()) {
        if (p2[I] != 0) res.set(I, res[I] / p2[I]);
      }
      return res;
    }

    static DecisionTensor< GUM_SCALAR > combination(const DecisionTensor< GUM_SCALAR >& dp1,
                                                    const DecisionTensor< GUM_SCALAR >& dp2) {
      return DecisionTensor< GUM_SCALAR >(dp1.probPot * dp2.probPot, dp1.utilPot + dp2.utilPot);
    }

    static DecisionTensor< GUM_SCALAR > marginalization(const DecisionTensor< GUM_SCALAR >& dp,
                                                        const gum::VariableSet&             onto) {
      const auto pr = dp.probPot.sumIn(onto);
      return DecisionTensor(pr, divideEvenZero((dp.probPot * dp.utilPot).sumIn(onto), pr));
    }

    static DecisionTensor< GUM_SCALAR >
        marginalization(const DecisionTensor< GUM_SCALAR >& dp,
                        const std::vector< std::string >&   ontonames) {
      gum::VariableSet onto;
      for (const auto& varname: ontonames) {
        onto.insert(dp.variable(varname));
      }
      return marginalization(dp, onto);
    }

    std::pair< GUM_SCALAR, GUM_SCALAR > meanVar() {
      const auto       tmp = probPot * utilPot;
      const GUM_SCALAR s   = probPot.sum();
      const double     m   = tmp.sum() / s;
      const double     m2  = (tmp * utilPot).sum() / s;
      double           var = m2 - m * m;
      if (var < 0.0) var = 0.0;   // var is a small number<0 due to computation errors
      return std::pair< GUM_SCALAR, GUM_SCALAR >(m, var);
    }

    virtual std::string toString() const {
      return "prob : " + probPot.toString() + "    util:" + utilPot.toString();
    }
  };

  template < typename GUM_SCALAR >
  std::ostream& operator<<(std::ostream& out, const DecisionTensor< GUM_SCALAR >& array) {
    out << array.toString();
    return out;
  }
}   // namespace gum

#endif   // AGRUM_DECISIONTENSOR_H
