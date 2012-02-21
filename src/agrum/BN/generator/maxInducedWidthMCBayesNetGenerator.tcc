/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN & Ariele-Paolo MAESANO   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief Source implementation of MaxInducedWidthMCBayesNetGenerator
 *
 * @author Pierre-Henri WUILLEMIN and Ariele Maesano
 *
 */
// ============================================================================
#include <agrum/BN/generator/maxInducedWidthMCBayesNetGenerator.h>

// ============================================================================
namespace gum {

// Default constructor.
// Use the DefaultCPTGenerator for generating the BNs CPT.
template<typename T_DATA, template<class> class ICPTGenerator, template<class> class ICPTDisturber> INLINE
MaxInducedWidthMCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::MaxInducedWidthMCBayesNetGenerator( Size nbrNodes,Size maxArcs , Size max_modality, Size maxInducedWidth, Idx iteration,  Idx p,Idx q): 
MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>(nbrNodes, maxArcs,max_modality,iteration ,p,q){
  if (maxInducedWidth == 0) GUM_ERROR(OperationNotAllowed,"maxInducedWidth must be at least equal to 1 to have a connexe graph");
   _maxlog10InducedWidth = maxInducedWidth;
    GUM_CONSTRUCTOR(MaxInducedWidthMCBayesNetGenerator);
}


template<typename T_DATA, template<class> class ICPTGenerator, template<class> class ICPTDisturber> INLINE
MaxInducedWidthMCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::MaxInducedWidthMCBayesNetGenerator( BayesNet<T_DATA> bayesNet, Size maxInducedWidth, Idx iteration,  Idx p,Idx q): 
MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>(bayesNet, iteration ,p,q){
   _maxlog10InducedWidth = maxInducedWidth;
    GUM_CONSTRUCTOR(MaxInducedWidthMCBayesNetGenerator);
}




// Use this constructor if you want to use a different policy for generating
// CPT than the default one.
// The cptGenerator will be erased when the destructor is called.
// @param cptGenerator The policy used to generate CPT.
/*template<typename T_DATA, template<class> class ICPTGenerator, template<class> class ICPDisturber> 
MaxInducedWidthMCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::MaxInducedWidthMCBayesNetGenerator( CPTGenerator* cptGenerator,Size nbrNodes,  Idx p,Idx q,Idx iteration,float maxDensity , Size max_modality, Size maxInducedWidth): 
MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>(cptGenerator, nbrNodes,p,q,iteration, maxDensity,max_modality, maxInducedWidth){
    GUM_CONSTRUCTOR(MaxInducedWidthMCBayesNetGenerator);
}*/

// Destructor.
template<typename T_DATA, template<class> class ICPTGenerator, template<class> class ICPTDisturber> INLINE
MaxInducedWidthMCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::~MaxInducedWidthMCBayesNetGenerator() {
    GUM_DESTRUCTOR(MaxInducedWidthMCBayesNetGenerator);
//    delete BayesNetGenerator<T_DATA>::_cptGenerator;
}

template<typename T_DATA, template<class> class ICPTGenerator, template<class> class ICPTDisturber> 
bool MaxInducedWidthMCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__checkConditions() {
  typename Property<unsigned int>::onNodes __modalitiesMap;

  for ( DAG::NodeIterator iter = AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.beginNodes();
  iter != AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.endNodes(); ++iter ) {
  __modalitiesMap.insert( *iter,  AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.variable( *iter ).domainSize() );
  }
  DefaultTriangulation tri( &(AbstractBayesNetGenerator<T_DATA,ICPTGenerator>::_bayesNet.moralGraph()), &__modalitiesMap );
  if (tri.maxLog10CliqueDomainSize() > _maxlog10InducedWidth) return false;
  return MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::__checkConditions();
}




    template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber> INLINE
 Size MaxInducedWidthMCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::maxlog10InducedWidth() const{
   return _maxlog10InducedWidth;
 }
      template<typename T_DATA, template<class> class ICPTGenerator, template <class> class ICPTDisturber> INLINE
      void MaxInducedWidthMCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>::setMaxlog10InducedWidth(Size maxlog10InducedWidth){
         if (maxlog10InducedWidth == 0) GUM_ERROR(OperationNotAllowed,"maxInducedWidth must be at least equal to 1 to have a connexe graph");
        _maxlog10InducedWidth = maxlog10InducedWidth;
      }
} /* namespace gum */
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;

