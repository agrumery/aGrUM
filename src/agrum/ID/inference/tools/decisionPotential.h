/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief This file contains abstract class definitions influence diagrams
 *        inference classes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe Gonzalez(@
 */
#ifndef AGRUM_DECISIONPOTENTIAL_H
#define AGRUM_DECISIONPOTENTIAL_H

#include <agrum/tools/multidim/potential.h>

namespace gum {
  /**
   * @class DecisionPotential decisionPotential.h
   *<agrum/ID/inference/decisionPotential.h>
   * @brief Potential for optimization in LIMIDS (such as Single Policy Update)
   */
  template < typename GUM_SCALAR >
  class DecisionPotential {
    public:
    Potential< GUM_SCALAR > probPot;
    Potential< GUM_SCALAR > utilPot;

    explicit DecisionPotential() {
      GUM_CONSTRUCTOR(DecisionPotential);
      probPot.fillWith(GUM_SCALAR(1));
      utilPot.fillWith(GUM_SCALAR(0));
    }

    ~DecisionPotential() {
      GUM_DESTRUCTOR(DecisionPotential);
      ;
    }

    DecisionPotential(const Potential< GUM_SCALAR >& prob, const Potential< GUM_SCALAR >& util) :
        probPot(prob), utilPot(util) {
      GUM_CONSTRUCTOR(DecisionPotential);
    }

    DecisionPotential(const DecisionPotential< GUM_SCALAR >& dp) :
        probPot(dp.probPot), utilPot(dp.utilPot) {
      GUM_CONS_CPY(DecisionPotential);
    }

    void clear() {
      gum::Potential< GUM_SCALAR > p;
      p.fillWith(GUM_SCALAR(1));
      probPot = p;
      p.fillWith(GUM_SCALAR(0));
      utilPot = p;
    }

    DecisionPotential< GUM_SCALAR >& operator=(const DecisionPotential< GUM_SCALAR >& src) {
      GUM_OP_CPY(DecisionPotential);
      if (&src == this) return *this;
      probPot = src.probPot;
      utilPot = src.utilPot;
      return *this;
    }

    DecisionPotential(DecisionPotential< GUM_SCALAR >&& dp) :
        probPot(std::forward< Potential< GUM_SCALAR > >(dp.probPot)),
        utilPot(std::forward< Potential< GUM_SCALAR > >(dp.utilPot)) {
      GUM_CONS_MOV(DecisionPotential);
    }

    DecisionPotential< GUM_SCALAR >& operator=(DecisionPotential< GUM_SCALAR >&& src) {
      GUM_OP_MOV(DecisionPotential);
      if (&src == this) return *this;
      probPot = std::forward< Potential< GUM_SCALAR > >(src.probPot);
      utilPot = std::forward< Potential< GUM_SCALAR > >(src.utilPot);
      return *this;
    }

    bool operator==(const DecisionPotential< GUM_SCALAR >& p) const {
      // @see Evaluating Influence Diagrams using LIMIDS (2000) - section 3.3
      return ((p.probPot == this->probPot)
              && (p.probPot * p.utilPot == this->probPot * this->utilPot));
    }

    bool operator!=(const DecisionPotential< GUM_SCALAR >& p) const { return !operator==(p); }

    const DiscreteVariable* variable(const std::string& name) const {
      for (const auto& v: probPot.variablesSequence()) {
        if (v->name() == name) return v;
      }
      for (const auto& v: utilPot.variablesSequence()) {
        if (v->name() == name) return v;
      }

      GUM_ERROR(NotFound, "'" << name << "' can not be found in DecisionPotential.")
    }

    void insertProba(const gum::Potential< GUM_SCALAR >& proba) { probPot *= proba; }

    void insertUtility(const gum::Potential< GUM_SCALAR >& util) { utilPot += util; }

    DecisionPotential< GUM_SCALAR > operator*(const DecisionPotential< GUM_SCALAR >& dp1) const {
      return DecisionPotential< GUM_SCALAR >::combination(*this, dp1);
    }

    DecisionPotential< GUM_SCALAR > operator*=(const DecisionPotential< GUM_SCALAR >& dp1) {
      *this = DecisionPotential< GUM_SCALAR >::combination(*this, dp1);
      return *this;
    }

    DecisionPotential< GUM_SCALAR > operator^(const Set< const DiscreteVariable* >& onto) const {
      return DecisionPotential< GUM_SCALAR >::marginalization(*this, onto);
    }

    DecisionPotential< GUM_SCALAR > operator^(const std::vector< std::string >& ontonames) const {
      return DecisionPotential< GUM_SCALAR >::marginalization(*this, ontonames);
    }


    static Potential< GUM_SCALAR > divideEvenZero(const Potential< GUM_SCALAR >& p1,
                                                  const Potential< GUM_SCALAR >& p2) {
      Potential< GUM_SCALAR > res(p1);
      Instantiation           I(res);
      for (I.setFirst(); !I.end(); I.inc()) {
        if (p2[I] != 0) res.set(I, res[I] / p2[I]);
      }
      return res;
    }

    static DecisionPotential< GUM_SCALAR > combination(const DecisionPotential< GUM_SCALAR >& dp1,
                                                       const DecisionPotential< GUM_SCALAR >& dp2) {
      return DecisionPotential< GUM_SCALAR >(dp1.probPot * dp2.probPot, dp1.utilPot + dp2.utilPot);
    }

    static DecisionPotential< GUM_SCALAR >
       marginalization(const DecisionPotential< GUM_SCALAR >& dp,
                       const Set< const DiscreteVariable* >&  onto) {
      const auto pr = dp.probPot.margSumIn(onto);
      return DecisionPotential(pr, divideEvenZero((dp.probPot * dp.utilPot).margSumIn(onto), pr));
    }

    static DecisionPotential< GUM_SCALAR >
       marginalization(const DecisionPotential< GUM_SCALAR >& dp,
                       const std::vector< std::string >&      ontonames) {
      Set< const DiscreteVariable* > onto;
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
  std::ostream& operator<<(std::ostream& out, const DecisionPotential< GUM_SCALAR >& array) {
    out << array.toString();
    return out;
  }
}   // namespace gum

#endif   // AGRUM_DECISIONPOTENTIAL_H
