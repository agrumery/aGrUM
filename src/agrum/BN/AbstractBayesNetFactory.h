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
/**
 * @file
 * @brief Headers of the BayesNetFactory class.
 *
 * @author Lionel Torti & Pierre-Henri Wuillemin
 */
#ifndef GUM_ABSTRACT_BAYESNET_FACTORY_H
#define GUM_ABSTRACT_BAYESNET_FACTORY_H

#include <string>
#include <vector>

#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/multiDimAdressable.h>
#include <agrum/graphicalModels/variableNodeMap.h>

namespace gum {
  /**
   * @class AbstractBayesNetFactory
   * This class is used to implement the factory_state enumeration because
   * Cpp is a pain in the ***.
   */

  class AbstractBayesNetFactory {
    public:
      /**
       * @brief The enumeration of states in which the factory can be in.
       *
       * Every documentation section's name indicates from which state you can
       * call it's methods, and in which state it places the factory.
       *
       * There is an exception for the delegated CPT definition methods which do
       * not changes the state of the factory.
       */
      typedef enum { NONE, NETWORK, VARIABLE, PARENTS, RAW_CPT, FACT_CPT, FACT_ENTRY}
      factory_state;

// just to make some compilers happy
	virtual ~AbstractBayesNetFactory() {};

      /// @{
      /// @brief Please @see BayesNetFactory<T_DATA> for documentation.
      virtual const DiscreteVariable& varInBN( NodeId id ) =0;
      virtual factory_state state() const =0;
      virtual NodeId variableId( const std::string& name ) const =0;
			virtual Size cptDomainSize(const NodeId n) const =0;

			virtual void startNetworkDeclaration() =0;
      virtual void addNetworkProperty( const std::string& propName,const std::string& propValue ) =0;
      virtual void endNetworkDeclaration() =0;
      virtual void startVariableDeclaration() =0;
      virtual void variableName( const std::string& name ) =0;
      virtual void variableDescription( const std::string& desc ) =0;
      virtual void addModality( const std::string& name ) =0;
      virtual void setVariableCPTImplementation( MultiDimAdressable* impl ) =0;
      virtual NodeId endVariableDeclaration() =0;
      virtual void startParentsDeclaration( const std::string& var ) =0;
      virtual void addParent( const std::string& var ) =0;
      virtual void endParentsDeclaration() =0;
      virtual void startRawProbabilityDeclaration( const std::string& var ) =0;
      virtual void rawConditionalTable( const std::vector<std::string>& variables,const std::vector<float>& rawTable ) =0;
      virtual void rawConditionalTable( const std::vector<float>& rawTable ) =0;
      virtual void endRawProbabilityDeclaration() =0;
      virtual void startFactorizedProbabilityDeclaration( const std::string& var ) =0;
      virtual void startFactorizedEntry() =0;
      virtual void endFactorizedEntry() =0;
      virtual void setParentModality( const std::string& parent,const std::string& modality ) =0;
      virtual void setVariableValuesUnchecked( const std::vector<float>& values ) =0;
      virtual void setVariableValues( const std::vector<float>& values ) =0;
      virtual void endFactorizedProbabilityDeclaration() =0;
      virtual void setVariable( const DiscreteVariable& var ) =0;
      virtual void setVariableCPT( const std::string& varName, MultiDimAdressable* table,bool redefineParents=false ) =0;
			/// @}
  };
} /* namespace gum */

// ============================================================================
#endif // GUM_ASBTRACT_BAYESNET_FACTORY_H
// ============================================================================
