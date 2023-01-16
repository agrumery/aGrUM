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
 * @brief This file contains Gibbs sampling (for BNs) class definitions
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */


#ifndef GUM_GIBBS_OPERATOR_H
#define GUM_GIBBS_OPERATOR_H

#include <agrum/BN/IBayesNet.h>

namespace gum {

  /**
   * @class GibbsOperator
   * @headerfile gibbsOperator.h <agrum/BN/inference/gibbsOperator.h>
   * @brief class containing all variables and methods required for Gibbssampling
   *
   * @ingroup bn_approximation
   *
   */

  template < typename GUM_SCALAR >
  class GibbsOperator {
    public:
    /**
     *  constructor
     */
    GibbsOperator(const IBayesNet< GUM_SCALAR >& BN,
                  const NodeProperty< Idx >*     hardEv,
                  Size                           nbr      = 1,
                  bool                           atRandom = false);

    /**
     * Destructor.
     */
    virtual ~GibbsOperator();

    /** Getters and setters*/
    Size nbrDrawnVar() const { return nbr_; }

    void setNbrDrawnVar(Size nbr) { nbr_ = nbr; }

    bool isDrawnAtRandom() const { return atRandom_; }

    void setDrawnAtRandom(bool atRandom) { atRandom_ = atRandom; }

    /// draws a Monte Carlo sample
    Instantiation monteCarloSample();

    /// draws next sample of Gibbs sampling
    Instantiation nextSample(Instantiation prev);

    protected:
    Size                           counting_;
    const IBayesNet< GUM_SCALAR >& samplingBn_;
    const NodeProperty< Idx >*     hardEv_;
    Sequence< NodeId >             samplingNodes_;

    Size nbr_;
    bool atRandom_;

    private:
    void _updateSamplingNodes_();

    // adds a node to current instantiation
    void _drawVarMonteCarlo_(NodeId nod, Instantiation* I);

    void _GibbsSample_(NodeId id, Instantiation* I);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS

  extern template class GibbsOperator< double >;

#endif

}   // namespace gum

#include <agrum/BN/inference/tools/gibbsOperator_tpl.h>

#endif
