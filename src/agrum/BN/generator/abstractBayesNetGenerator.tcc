/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Source implementation of AbstractBayesNetGenerator
 *
 * @author Christophe GONZALES, Pierre-Henri WUILLEMIN, Lionel Torti and Ariele-Paolo MAESANO
 *
 */
// ============================================================================
#include <agrum/BN/generator/abstractBayesNetGenerator.h>

// ============================================================================
namespace gum {

// Default constructor.
// Use the DefaultCPTGenerator for generating the BNs CPT.
  template <typename GUM_SCALAR, template<class> class ICPTGenerator> INLINE
  AbstractBayesNetGenerator<GUM_SCALAR,ICPTGenerator>::AbstractBayesNetGenerator(Size nbrNodes, Size maxArcs , Size maxModality): _bayesNet() {
        GUM_CONSTRUCTOR(AbstractBayesNetGenerator);
    _nbrNodes = nbrNodes;
    if (maxArcs < nbrNodes - 1 || maxArcs > (nbrNodes * (nbrNodes - 1)) / 2 ) 
    GUM_ERROR(OperationNotAllowed, " maxArcs value not possible ");
    if (maxModality < 2)GUM_ERROR(OperationNotAllowed, " maxModality must be at least equal to two ");
    _maxArcs = maxArcs;
    _maxModality = maxModality;
  }


// Destructor.
  template <typename GUM_SCALAR, template<class> class ICPTGenerator> INLINE
  AbstractBayesNetGenerator<GUM_SCALAR,ICPTGenerator>::~AbstractBayesNetGenerator() {
    GUM_DESTRUCTOR(AbstractBayesNetGenerator);
//    delete _cptGenerator;
  }

 template <typename GUM_SCALAR, template<class> class ICPTGenerator>
 void  AbstractBayesNetGenerator<GUM_SCALAR,ICPTGenerator>::fillCPT() {
  for ( DAG::NodeIterator iter = _bayesNet.beginNodes(); iter != _bayesNet.endNodes(); ++iter ) {
//    AbstractBayesNetGenerator<GUM_SCALAR,ICPTGenerator>::_cptGenerator->generateCPT ( 
    ICPTGenerator<GUM_SCALAR>::generateCPT (_bayesNet.cpt(*iter).pos(_bayesNet.variable(*iter)),  _bayesNet.cpt(*iter)); //TODO ASSERT THE LINE
   }
 }
  
 template<typename GUM_SCALAR, template<class> class ICPTGenerator> INLINE
   Size AbstractBayesNetGenerator<GUM_SCALAR,ICPTGenerator>::maxModality() const{
       return _maxModality;
 }
 
 template<typename GUM_SCALAR, template<class> class ICPTGenerator> INLINE
      Size AbstractBayesNetGenerator<GUM_SCALAR,ICPTGenerator>::nbrNodes() const{
       return _nbrNodes; 
 }
      
      
 template<typename GUM_SCALAR, template<class> class ICPTGenerator> INLINE
   Size AbstractBayesNetGenerator<GUM_SCALAR,ICPTGenerator>::maxArcs() const{
       return _maxArcs;
 }
 
      
      template<typename GUM_SCALAR, template<class> class ICPTGenerator> INLINE
      void AbstractBayesNetGenerator<GUM_SCALAR,ICPTGenerator>::setMaxModality(Size maxModality){
        if (maxModality < 2)GUM_ERROR(OperationNotAllowed, " maxModality must be at least equal to two ");
        _maxModality = maxModality;
      }
      template<typename GUM_SCALAR, template<class> class ICPTGenerator> INLINE
      void AbstractBayesNetGenerator<GUM_SCALAR,ICPTGenerator>::setNbrNodes(Size nbrNodes){
       if (_maxArcs < nbrNodes - 1 || _maxArcs > (nbrNodes * (nbrNodes - 1)) / 2 ) 
    GUM_ERROR(OperationNotAllowed, " nbrNodes value not possible ");
        _nbrNodes = nbrNodes;
      }

            template<typename GUM_SCALAR, template<class> class ICPTGenerator> INLINE
      void AbstractBayesNetGenerator<GUM_SCALAR,ICPTGenerator>::setMaxArcs(Size maxArcs){
        if (maxArcs < _nbrNodes - 1 || maxArcs > (_nbrNodes * (_nbrNodes - 1)) / 2 ) 
    GUM_ERROR(OperationNotAllowed, " maxArcs value not possible ");
        _maxArcs = maxArcs;
      }
      
      
// Generates a bayesian network using floats.
// @param nbrNodes The number of nodes in the generated BN.
// @param density The probability of adding an arc between two nodes.
// @return A BNs randomly generated.


} /* namespace gum */
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;
