
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


/** @file
 * @brief Source implementation of MaxInducedWidthMCBayesNetGenerator
 *
 * @author Pierre-Henri WUILLEMIN and Ariele Maesano
 *
 */

#include <agrum/BN/generator/maxInducedWidthMCBayesNetGenerator.h>

namespace gum {
#ifdef _MSC_VER
#  define MCBG MCBayesNetGenerator
#  define IBNG IBayesNetGenerator
#else
#  define MCBG MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >
#  define IBNG IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >
#endif
  // Default constructor.
  // Use the SimpleCPTGenerator for generating the BNs CPT.
  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE MaxInducedWidthMCBayesNetGenerator<
     GUM_SCALAR,
     ICPTGenerator,
     ICPTDisturber >::MaxInducedWidthMCBayesNetGenerator(Size nbrNodes,
                                                         Size maxArcs,
                                                         Size maxModality,
                                                         Size maxInducedWidth,
                                                         Idx  iteration,
                                                         Idx  p,
                                                         Idx  q) :
      MCBG(nbrNodes, maxArcs, maxModality, iteration, p, q) {
    if (maxInducedWidth == 0)
      GUM_ERROR(OperationNotAllowed,
                "maxInducedWidth must be at least equal "
                "to 1 to have a connexe graph");

    _maxlog10InducedWidth = maxInducedWidth;
    GUM_CONSTRUCTOR(MaxInducedWidthMCBayesNetGenerator);
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE MaxInducedWidthMCBayesNetGenerator< GUM_SCALAR,
                                             ICPTGenerator,
                                             ICPTDisturber >::
     MaxInducedWidthMCBayesNetGenerator(BayesNet< GUM_SCALAR > bayesNet,
                                        Size                   maxInducedWidth,
                                        Idx                    iteration,
                                        Idx                    p,
                                        Idx                    q) :
      MCBG(bayesNet, iteration, p, q) {
    _maxlog10InducedWidth = maxInducedWidth;
    GUM_CONSTRUCTOR(MaxInducedWidthMCBayesNetGenerator);
  }

  // Use this constructor if you want to use a different policy for generating
  // CPT than the default one.
  // The cptGenerator will be erased when the destructor is called.
  // @param cptGenerator The policy used to generate CPT.
  /*template<typename GUM_SCALAR, template<class> class ICPTGenerator,
  template<class> class ICPDisturber>
  MaxInducedWidthMCBayesNetGenerator<GUM_SCALAR,ICPTGenerator,ICPTDisturber>::MaxInducedWidthMCBayesNetGenerator(
  CPTGenerator* cptGenerator,Size nbrNodes,  Idx p,Idx q,Idx iteration,float
  maxDensity , Size max_modality, Size maxInducedWidth):
  MCBG<GUM_SCALAR,ICPTGenerator,ICPTDisturber>(cptGenerator,
  nbrNodes,p,q,iteration, maxDensity,max_modality, maxInducedWidth){
      GUM_CONSTRUCTOR(MaxInducedWidthMCBayesNetGenerator);
  }*/

  // Destructor.
  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE MaxInducedWidthMCBayesNetGenerator<
     GUM_SCALAR,
     ICPTGenerator,
     ICPTDisturber >::~MaxInducedWidthMCBayesNetGenerator() {
    GUM_DESTRUCTOR(MaxInducedWidthMCBayesNetGenerator);
    //    delete BayesNetGenerator<GUM_SCALAR>::_cptGenerator;
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  bool MaxInducedWidthMCBayesNetGenerator< GUM_SCALAR,
                                           ICPTGenerator,
                                           ICPTDisturber >::__checkConditions() {
    NodeProperty< Size > __modalitiesMap;

    for (auto node : this->_bayesNet.nodes())
      __modalitiesMap.insert(node, this->_bayesNet.variable(node).domainSize());

    DefaultTriangulation tri(&(this->_bayesNet.moralGraph()), &__modalitiesMap);

    if (tri.maxLog10CliqueDomainSize() > _maxlog10InducedWidth) return false;

    return MCBG::__checkConditions();
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE Size
         MaxInducedWidthMCBayesNetGenerator< GUM_SCALAR,
                                             ICPTGenerator,
                                             ICPTDisturber >::maxlog10InducedWidth()
        const {
    return _maxlog10InducedWidth;
  }
  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE void MaxInducedWidthMCBayesNetGenerator<
     GUM_SCALAR,
     ICPTGenerator,
     ICPTDisturber >::setMaxlog10InducedWidth(Size maxlog10InducedWidth) {
    if (maxlog10InducedWidth == 0)
      GUM_ERROR(OperationNotAllowed,
                "maxInducedWidth must be at least equal "
                "to 1 to have a connexe graph");

    _maxlog10InducedWidth = maxlog10InducedWidth;
  }
} /* namespace gum */
