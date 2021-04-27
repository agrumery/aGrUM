/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Template implementations for the LeastSquareTestPolicy class.
 *
 * @author Jean-Christophe MAGNAN
 */
// =========================================================================
#include <agrum/FMDP/learning/core/testPolicy/leastSquareTestPolicy.h>
// =========================================================================


namespace gum {

  template < typename GUM_SCALAR >
  LeastSquareTestPolicy< GUM_SCALAR >::~LeastSquareTestPolicy() {
    for (auto obsIter = this->obsTable__.cbeginSafe();
         obsTable__.cendSafe() != obsIter;
         ++obsIter)
      delete obsIter.val();

    GUM_DESTRUCTOR(LeastSquareTestPolicy);
  }


  // ##########################################################################
  //
  // ##########################################################################

  // ==========================================================================
  //
  // ==========================================================================
  template < typename GUM_SCALAR >
  void LeastSquareTestPolicy< GUM_SCALAR >::addObservation(Idx        attr,
                                                           GUM_SCALAR value) {
    ITestPolicy< GUM_SCALAR >::addObservation(attr, value);
    sumO__ += value;

    if (sumAttrTable__.exists(attr))
      sumAttrTable__[attr] += value;
    else
      sumAttrTable__.insert(attr, value);

    if (nbObsTable__.exists(attr))
      nbObsTable__[attr]++;
    else
      nbObsTable__.insert(attr, 1);

    if (!obsTable__.exists(attr))
      obsTable__.insert(attr, new LinkedList< double >());
    obsTable__[attr]->addLink(value);
  }


  // ############################################################################
  // @name Test result
  // ############################################################################

  // ============================================================================
  // Computes the GStat of current variable according to the test
  // ============================================================================
  template < typename GUM_SCALAR >
  void LeastSquareTestPolicy< GUM_SCALAR >::computeScore() {
    ITestPolicy< GUM_SCALAR >::computeScore();
    double mean         = sumO__ / (double)this->nbObservation();
    double errorO       = 0.0;
    double sumErrorAttr = 0.0;
    for (auto attrIter = sumAttrTable__.cbeginSafe();
         attrIter != sumAttrTable__.cendSafe();
         ++attrIter) {
      Idx    key       = attrIter.key();
      double meanAttr  = sumAttrTable__[key] / (double)nbObsTable__[key];
      double errorAttr = 0.0;

      const Link< double >* linky = obsTable__[key]->list();
      while (linky) {
        errorAttr += std::pow(linky->element() - meanAttr, 2);
        errorO += std::pow(linky->element() - mean, 2);
        linky = linky->nextLink();
      }

      sumErrorAttr += ((double)nbObsTable__[key] / (double)this->nbObservation())
                    * errorAttr;
    }
    score__ = errorO - sumErrorAttr;
  }

  // ============================================================================
  // Returns the performance of current variable according to the test
  // ============================================================================
  template < typename GUM_SCALAR >
  double LeastSquareTestPolicy< GUM_SCALAR >::score() {
    if (this->isModified_()) computeScore();
    return score__;
  }

  // ============================================================================
  // Returns a second criterion to severe ties
  // ============================================================================
  template < typename GUM_SCALAR >
  double LeastSquareTestPolicy< GUM_SCALAR >::secondaryscore() const {
    if (this->isModified_()) computeScore();
    return score__;
  }

  template < typename GUM_SCALAR >
  void LeastSquareTestPolicy< GUM_SCALAR >::add(const LeastSquareTestPolicy& src) {
    ITestPolicy< GUM_SCALAR >::add(src);

    for (auto obsIter = src.nbObsTable().cbeginSafe();
         obsIter != src.nbObsTable().cendSafe();
         ++obsIter)
      if (nbObsTable__.exists(obsIter.key()))
        nbObsTable__[obsIter.key()] += obsIter.val();
      else
        nbObsTable__.insert(obsIter.key(), obsIter.val());

    for (auto attrIter = src.sumAttrTable().cbeginSafe();
         attrIter != src.sumAttrTable().cendSafe();
         ++attrIter)
      if (sumAttrTable__.exists(attrIter.key()))
        sumAttrTable__[attrIter.key()] += attrIter.val();
      else
        sumAttrTable__.insert(attrIter.key(), attrIter.val());

    for (auto obsIter = src.obsTable().cbeginSafe();
         obsIter != src.obsTable().cendSafe();
         ++obsIter) {
      if (!obsTable__.exists(obsIter.key()))
        obsTable__.insert(obsIter.key(), new LinkedList< double >());
      const Link< double >* srcLink = obsIter.val()->list();
      while (srcLink) {
        obsTable__[obsIter.key()]->addLink(srcLink->element());
        srcLink = srcLink->nextLink();
      }
    }
  }

}   // End of namespace gum
