
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */


#ifndef GUM_GIBBS_OPERATOR_H
#define GUM_GIBBS_OPERATOR_H

#include <agrum/BN/IBayesNet.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/potential.h>

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
                  Size                           nbr = 1,
                  bool                           atRandom = false);

    /**
     * Destructor.
     */
    virtual ~GibbsOperator();

    /** Getters and setters*/
    Size nbrDrawnVar() const { return _nbr; }

    void setNbrDrawnVar(Size nbr) { _nbr = nbr; }

    bool isDrawnAtRandom() const { return _atRandom; }

    void setDrawnAtRandom(bool atRandom) { _atRandom = atRandom; }

    /// draws a Monte Carlo sample
    Instantiation monteCarloSample();

    /// draws next sample of Gibbs sampling
    Instantiation nextSample(Instantiation prev);

    protected:
    /// number of samples drawn
    Size                           _counting;
    const IBayesNet< GUM_SCALAR >& _sampling_bn;
    const NodeProperty< Idx >*     _hardEv;
    Sequence< NodeId >             _samplingNodes;

    Size _nbr;

    protected:
    bool _atRandom;


    private:
    void __updateSamplingNodes();
    // adds a node to current instantiation
    void __drawVarMonteCarlo(NodeId nod, Instantiation* I);
    void __GibbsSample(NodeId id, Instantiation* I);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class GibbsOperator< double >;
#endif

}   // namespace gum

#include <agrum/BN/inference/tools/gibbsOperator_tpl.h>
#endif
