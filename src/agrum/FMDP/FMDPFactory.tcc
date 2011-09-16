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
 * @brief Template Implementation of the FMDPFactory class.
 *
 * @author Jean-Christophe Magnan
 */
// ============================================================================
#include <agrum/FMDP/FMDPFactory.h>
// ============================================================================

#define VERBOSITY(x) {  if (isVerbose()) std::cerr << "[FMDP factory] "<< x << std::endl; }


namespace gum {

	/* **************************************************************************************************** **/
	/* **																									**/
	/* **									Constructor & Destructor										**/
	/* **																									**/
	/* **************************************************************************************************** **/
	
		// ==========================================================================================
		// Default constructor.
		// @param fmdp A pointer over the Factored Markov Decision Process filled by this factory.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		FMDPFactory<T_DATA>::FMDPFactory( FactoredMarkovDecisionProcess<T_DATA>* fmdp, MultiDimDecisionDiagramFactoryBase<T_DATA>* ddFactory ) : __fmdp(fmdp), __decisionDiagramFactory(ddFactory) {

			GUM_CONSTRUCTOR ( FMDPFactory );
			
			__states.push_back ( NONE );
			resetVerbose();
		}

		//~ // ===========================================================================================
		//~ // Copy constructor.
		//~ // The copy will have an exact copy of the constructed BayesNet in source.
		//~ // ===========================================================================================
		//~ template<typename T_DATA> INLINE
		//~ FMDPFactory<T_DATA>::FMDPFactory( const FMDPFactory<T_DATA>& source )  {
			//~ 
			//~ GUM_CONS_CPY ( FMDPFactory );
//~ 
			//~ if ( source.state() != NONE ) {
				//~ GUM_ERROR ( OperationNotAllowed, "Illegal state to proceed make a copy." );
			//~ } else {
				//~ __states = source.__states;
				//~ __bn = new FactoredMarkovDecisionProcess<T_DATA> ( * ( source.__bn ) );
			//~ }
		//~ }

		// ===========================================================================================
		// Destructor
		// ===========================================================================================
		template<typename T_DATA> INLINE
		FMDPFactory<T_DATA>::~FMDPFactory() {
			
			GUM_DESTRUCTOR ( FMDPFactory );
			
		}
		
		
		template<typename T_DATA> INLINE
		void FMDPFactory<T_DATA>::sayHelloToTheWorld() {
			GUM_TRACE( "Hello to the World!" );
		}
	/* **************************************************************************************************** **/
	/* **																									**/
	/* **									Getter and setters												**/
	/* **																									**/
	/* **************************************************************************************************** **/
	
		// ==========================================================================================
		// Returns the BayesNet created by this factory.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		FactoredMarkovDecisionProcess<T_DATA>*
		FMDPFactory<T_DATA>::FMDP() const {
			
			if ( state() != NONE )
				GUM_ERROR(OperationNotAllowed, "Illegal state to return the factored markov decision process: it is not yet finished.");
				
			return __fmdp;
		}

		// ==========================================================================================
		// Returns the current state of the factory.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		FMDPfactory_state
		FMDPFactory<T_DATA>::state() const {
			// This is ok because there is alway at least the state NONE in the stack.
			return __states.back();
		}

		// ==========================================================================================
		// Returns a constant reference on a variable given it's name.
		// @throw NotFound Raised if no variable matches the name.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		const DiscreteVariable*
		FMDPFactory<T_DATA>::variable ( const std::string& name ) const {
			
			for( HashTableConstIterator< std::string, const DiscreteVariable* > iterM = __varNameMap.begin(); iterM != __varNameMap.end(); ++iterM )
				if( iterM.key().compare( name ) == 0 )
					return *iterM;
					
			
			GUM_TRACE( __varNameMap );
			
			GUM_ERROR ( NotFound, name );
			
			return NULL;
		}
		

	/* **************************************************************************************************** **/
	/* **																									**/
	/* **							Network declaration methods (NONE <-> PROPERTY)							**/
	/* **																									**/
	/* **************************************************************************************************** **/
	
		// ==========================================================================================
		// Tells the factory that we're in a fmdp declaration.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::startPropertyDeclaration() {
			
			if ( state() != NONE )
				__illegalStateError ( "startPropertyDeclaration" );
			else
				__states.push_back ( PROPERTY );

			VERBOSITY ( "starting property" );
		}
	
		// ==========================================================================================
		// Tells the factory to add a property to the current fmdp.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::addProperty ( const std::string& propName, const std::string& propValue ) {
			
			if ( state() != PROPERTY )
				__illegalStateError ( "addProperty" );
			else
				__fmdp->setProperty ( propName, propValue );
				
		}
	
		// ==========================================================================================
		// Tells the factory that we're out of a fmdp declaration.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::endPropertyDeclaration() {
			
			if ( state() != PROPERTY )
				__illegalStateError ( "endPropertyDeclaration" );
			else
			__states.pop_back();

			VERBOSITY ( "property OK" );
		}
		

	/* **************************************************************************************************** **/
	/* **																									**/
	/* **							Variable declaration methods (NONE <-> VARIABLE)						**/
	/* **																									**/
	/* **************************************************************************************************** **/
	
		// ==========================================================================================
		// Tells the factory that we're in a variable declaration.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::startVariableDeclaration() {
			
			if ( state() != NONE )
				__illegalStateError ( "startVariableDeclaration" );
			else {
				__states.push_back ( VARIABLE );
				__stringBag.push_back ( "name" );
				__stringBag.push_back ( "desc" );
			}

			VERBOSITY ( "  starting variable" );
		}
	
		// ==========================================================================================
		// Tells the factory the current variable's name.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::variableName ( const std::string& name ) {
			
			if ( state() != VARIABLE )
				__illegalStateError ( "variableName" );
			else {
				
				if ( __varNameMap.exists ( name ) )
					GUM_ERROR ( DuplicateElement, "Name already used: " + name );

				__foo_flag = true;
				__stringBag[0] = name;
				VERBOSITY ( "  -- variable " << name );
			}
		}
	
		// ==========================================================================================
		// Tells the factory the current variable's description.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::variableDescription ( const std::string& desc ) {
			
			if ( state() != VARIABLE )
				__illegalStateError ( "variableDescription" );
			else {
				__bar_flag = true;
				__stringBag[1] = desc;
			}
		}
	
		// ==========================================================================================
		// Adds a modality to the current variable.
		// @throw DuplicateElement If the current variable already has a modality
		//                         with the same name.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::addModality ( const std::string& name ) {
			
			if ( state() != VARIABLE )
				__illegalStateError ( "addModality" );
			else {
				__checkModalityInBag ( name );
				__stringBag.push_back ( name );
			}
		}
	
		// ==========================================================================================
		// Check if in __stringBag there is no other modality with the same name.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::__checkModalityInBag ( const std::string& mod ) {
			
			for ( size_t i = 2; i < __stringBag.size(); ++i )
				if ( mod == __stringBag[i] )
					GUM_ERROR ( DuplicateElement, mod );
		}
	
		// ==========================================================================================
		// Tells the factory that we're out of a variable declaration.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::endVariableDeclaration() {
			
			if ( state() != VARIABLE )
				__illegalStateError ( "endVariableDeclaration" );
			else if ( __foo_flag and ( __stringBag.size() > 3 ) ) {
			
				LabelizedVariable* var = new LabelizedVariable ( __stringBag[0], ( __bar_flag ) ? __stringBag[1] : "", 0 );			
				LabelizedVariable* varPrime = new LabelizedVariable ( __stringBag[0] + "'", ( __bar_flag ) ? __stringBag[1] : "", 0 );

				for ( size_t i = 2; i < __stringBag.size(); ++i ){
					var->addLabel ( __stringBag[i] );
					varPrime->addLabel( __stringBag[i] );
				}

				__fmdp->addVariable( var );
				__varNameMap.insert( var->name(), var );
				__fmdp->addPrimedVariable( varPrime, var );
				__varNameMap.insert( varPrime->name() , varPrime );
				
				//~ __mainVarSeq.insert( var );
				//~ __primedVarSeq.insert( varPrime );
				
				__resetParts();
				__states.pop_back();

				VERBOSITY ( "  variable " << var->name() << " OK" );
				
			} else {
				
				std::stringstream msg;
				msg << "Not enough modalities (";

				if ( __stringBag.size() > 2 )
					msg << __stringBag.size() - 2;
				else
					msg << 0;

				msg << ") declared for variable ";

				if ( __foo_flag )
					msg << __stringBag[0];
				else
					msg << "unknown";

				__resetParts();
				__states.pop_back();
				
				GUM_ERROR ( OperationNotAllowed, msg.str() );
			}
		}
		

	/* **************************************************************************************************** **/
	/* **																									**/
	/* **							Action declaration methods (NONE <-> ACTION)							**/
	/* **																									**/
	/* **************************************************************************************************** **/
	
		// ==========================================================================================
		// Tells the factory that we're declaring action
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::startActionDeclaration ( ) {
			if ( state() != NONE )
				__illegalStateError ( "startActionDeclaration" );
			else {
				__foo_flag = true;
				__states.push_back ( ACTION );
			}
			
				
			//~ for( SequenceIterator< const DiscreteVariable* > primedIter = __primedVarSeq.begin(); primedIter != __primedVarSeq.end(); ++primedIter )
				//~ if( !__mainVarSeq.exists( *primedIter ) )
					//~ __mainVarSeq.insert( *primedIter );
				//~ else
					//~ break;
					

			VERBOSITY ( "starting action declaration" );
		}
	
		// ==========================================================================================
		// Tells the factory to add an action
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::addAction ( const std::string& action ) {
			if ( state() != ACTION )
				__illegalStateError ( "addAction" );
			else {
				__stringBag.push_back ( action );
				__fmdp->addAction( action );
			}
		}
	
		// ==========================================================================================
		// Tells the factory that we're out of an action declaration.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::endActionDeclaration() {
			
			if ( state() != ACTION )
				__illegalStateError ( "endActionDeclaration" );
			else {
				__states.pop_back();
				__resetParts();
			}

			VERBOSITY ( "action OK" );
		}
		

	/* **************************************************************************************************** **/
	/* **																									**/
	/* **					Transition declaration methods (NONE <-> TRANSITION <-> ACTION)					**/
	/* **																									**/
	/* **************************************************************************************************** **/
	
		// ==========================================================================================
		// Tells the factory that we're declaring transition
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::startTransitionDeclaration ( ) {
			if ( state() != NONE && state() != ACTION )
				__illegalStateError ( "startTransitionDeclaration" );
			else 
				__states.push_back ( TRANSITION );
					
			//~ this->__decisionDiagramFactory->setVariablesSequence( __mainVarSeq );

			VERBOSITY ( "starting transition declaration" );
		}
	
		// ==========================================================================================
		// Tells the factory to add an action
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::addTransition ( const std::string& var, const MultiDimAdressable* transition ) {
			
			const MultiDimImplementation<T_DATA>* t = reinterpret_cast<const MultiDimImplementation<T_DATA>*>( transition );
			
			if ( state() != TRANSITION )
				__illegalStateError ( "addTransition" );
			else 
				if( __foo_flag )
					__fmdp->addTransitionForAction( __varNameMap[var], t, __stringBag[0] );
				else
					__fmdp->addTransition( __varNameMap[var], t );
					
		}
	
		// ==========================================================================================
		// Tells the factory to add a transition table to the current fmdp.
		// This transition table will be extracted from incorporated multiDimDecisionDiagram.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::addTransition ( const std::string& var ){
			
			if ( state() != TRANSITION )
				__illegalStateError ( "addTransition" );
			else {
				if( __foo_flag )
					__fmdp->addTransitionForAction( __varNameMap[var], this->__decisionDiagramFactory->getMultiDimDecisionDiagram(), __stringBag[0] );
				else
					__fmdp->addTransition( __varNameMap[var], this->__decisionDiagramFactory->getMultiDimDecisionDiagram() );
				//~ this->__decisionDiagramFactory->showProperties();
				this->__decisionDiagramFactory->clear();
			}
					
		}
	
		// ==========================================================================================
		// Tells the factory that we're out of a transition declaration.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::endTransitionDeclaration() {
			
			if ( state() != TRANSITION )
				__illegalStateError ( "endTransitionDeclaration" );
			else
				__states.pop_back();
			VERBOSITY ( "transition OK" );
		}
		

	/* **************************************************************************************************** **/
	/* **																									**/
	/* **					Cost declaration methods (NONE <-> COST <-> ACTION)								**/
	/* **																									**/
	/* **************************************************************************************************** **/
	
		// ==========================================================================================
		// Tells the factory that we're declaring cost
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::startCostDeclaration ( ) {
			if ( state() != NONE && state() != ACTION )
				__illegalStateError ( "startTransitionDeclaration" );
			else 
				__states.push_back ( COST );
					
			this->__decisionDiagramFactory->setVariablesSequence( __mainVarSeq );

			VERBOSITY ( "starting Cost declaration" );
		}
	
		// ==========================================================================================
		// Tells the factory to add a cost
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::addCost ( const MultiDimAdressable* cost ) {
			
			const MultiDimImplementation<T_DATA>* c = reinterpret_cast<const MultiDimImplementation<T_DATA>*>( cost );
			
			if ( state() != COST )
				__illegalStateError ( "addCost" );
			else 
				if( __foo_flag )
					__fmdp->addCostForAction( c, __stringBag[0] );
				else
					__fmdp->addCost( c );
		}
	
		// ==========================================================================================
		// Tells the factory to add a cost
		// This cost table will be extracted from incorporated multiDimDecisionDiagram.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::addCost ( ) {
			if ( state() != COST )
				__illegalStateError ( "addCost" );
			else {
				if( __foo_flag )
					__fmdp->addCostForAction( this->__decisionDiagramFactory->getMultiDimDecisionDiagram(), __stringBag[0] );
				else
					__fmdp->addCost( this->__decisionDiagramFactory->getMultiDimDecisionDiagram() );
				GUM_TRACE( "BOUH!" );
				//~ this->__decisionDiagramFactory->showProperties();
				this->__decisionDiagramFactory->clear();
			}
		}
	
		// ==========================================================================================
		// Tells the factory that we're out of a cost declaration.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::endCostDeclaration() {
			
			if ( state() != COST )
				__illegalStateError ( "endCostDeclaration" );
			else
				__states.pop_back();

			VERBOSITY ( "Cost OK" );
		}
		

	/* **************************************************************************************************** **/
	/* **																									**/
	/* **					Reward declaration methods (NONE <-> REWARD <-> ACTION)							**/
	/* **																									**/
	/* **************************************************************************************************** **/
	
		// ==========================================================================================
		// Tells the factory that we're declaring reward
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::startRewardDeclaration ( ) {
			if ( state() != NONE && state() != ACTION )
				__illegalStateError ( "startRewardDeclaration" );
			else 
				__states.push_back ( REWARD );
					
			this->__decisionDiagramFactory->setVariablesSequence( __mainVarSeq );

			VERBOSITY ( "starting reward declaration" );
		}
	
		// ==========================================================================================
		// Tells the factory to add a reward
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::addReward ( const MultiDimAdressable* reward ) {
			
			const MultiDimImplementation<T_DATA>* r = reinterpret_cast<const MultiDimImplementation<T_DATA>*>( reward );
			
			if ( state() != REWARD )
				__illegalStateError ( "addReward" );
			else 
				__fmdp->addReward( r );
		}
	
		// ==========================================================================================
		// Tells the factory to add a reward
		// This reward table will be extracted from incorporated multiDimDecisionDiagram.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::addReward ( ) {
			if ( state() != REWARD )
				__illegalStateError ( "addReward" );
			else {
				__fmdp->addReward( this->__decisionDiagramFactory->getMultiDimDecisionDiagram() );
				//~ this->__decisionDiagramFactory->showProperties();
				this->__decisionDiagramFactory->clear();
			}
		}
	
		// ==========================================================================================
		// Tells the factory that we're out of a reward declaration.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::endRewardDeclaration() {
			
			if ( state() != REWARD )
				__illegalStateError ( "endRewardDeclaration" );
			else
				__states.pop_back();

			VERBOSITY ( "reward OK" );
		}
		

	/* **************************************************************************************************** **/
	/* **																									**/
	/* **					Discount declaration methods (NONE <-> DISCOUNT <-> ACTION)						**/
	/* **																									**/
	/* **************************************************************************************************** **/
	
		// ==========================================================================================
		// Tells the factory that we're declaring discount
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::startDiscountDeclaration ( ) {
			if ( state() != NONE )
				__illegalStateError ( "startDiscountDeclaration" );
			else 
				__states.push_back ( DISCOUNT );

			VERBOSITY ( "starting discount declaration" );
		}
	
		// ==========================================================================================
		// Tells the factory to add a discount
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::addDiscount (float discount ) {
			if ( state() != DISCOUNT )
				__illegalStateError ( "addDiscount" );
			else 
				__fmdp->addDiscount( (T_DATA) discount );
		}
	
		// ==========================================================================================
		// Tells the factory that we're out of a discount declaration.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::endDiscountDeclaration() {
			
			if ( state() != DISCOUNT )
				__illegalStateError ( "endDiscountDeclaration" );
			else
				__states.pop_back();

			VERBOSITY ( "discount OK" );
		}
		

	/* **************************************************************************************************** **/
	/* **																									**/
	/* **					Discount declaration methods (NONE <-> DISCOUNT <-> ACTION)						**/
	/* **																									**/
	/* **************************************************************************************************** **/
	
		// ==========================================================================================
		// Insert in diagram a non terminal node
		// ==========================================================================================
		template<typename T_DATA> INLINE
		NodeId
		FMDPFactory<T_DATA>::addNonTerminalNode( std::string name_of_var ){
			
			return __decisionDiagramFactory->addNonTerminalNode( variable( name_of_var ) );
		}
	
		// ==========================================================================================
		// Insert in diagram a terminal node
		// ==========================================================================================
		template<typename T_DATA> INLINE
		NodeId
		FMDPFactory<T_DATA>::addTerminalNode( float value ){
			
			return __decisionDiagramFactory->addTerminalNode( (T_DATA) value );
		}
	
		// ==========================================================================================
		// Insert an arc in diagram
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::insertArc( NodeId from, NodeId to, Idx modality ){
			
			__decisionDiagramFactory->insertArc( from, to, modality );
		}

	/* **************************************************************************************************** **/
	/* **																									**/
	/* **									Various Private Methods											**/
	/* **																									**/
	/* **************************************************************************************************** **/
	
		// ==========================================================================================
		// Raise an OperationNotAllowed with the message "Illegal state."
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::__illegalStateError ( const std::string& s ) {
			
			std::string msg = "Illegal state call (";
			msg += s;
			msg += ") in state ";

			switch ( state() ) {

				case NONE: 			msg += "NONE";
									break;
				case PROPERTY:		msg += "PROPERTY";
									break;
				case VARIABLE:		msg += "VARIABLE";
									break;
				case ACTION:		msg += "ACTION";
									break;
				case TRANSITION:	msg += "TRANSITION";
									break;
				case COST:			msg += "COST";
									break;
				case REWARD:		msg += "REWARD";
									break;
				case DISCOUNT:		msg += "DISCOUNT";
									break;
				default:			msg += "Unknown state";
			}

			GUM_ERROR ( OperationNotAllowed, msg );
		}
		
		// ==========================================================================================
		// Reset the different parts used to constructed the BayesNet.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		void
		FMDPFactory<T_DATA>::__resetParts() {
			__foo_flag = false;
			__bar_flag = false;
			__stringBag.clear();
		}
	
		//~ // ==========================================================================================
		//~ // Check if a variable with the given name exists, if not raise an NotFound
		//~ // exception.
		//~ // ==========================================================================================
		//~ template<typename T_DATA> INLINE
		//~ void
		//~ FMDPFactory<T_DATA>::__checkVariableName ( const std::string& name ) {
			//~ if ( !__varNameMap.exists ( name ) )
				//~ GUM_ERROR ( NotFound, name );
		//~ }
	
		// ==========================================================================================
		// Copy operator is illegal, use only copy constructor.
		// ==========================================================================================
		template<typename T_DATA> INLINE
		FMDPFactory<T_DATA>&
		FMDPFactory<T_DATA>::operator= ( const FMDPFactory<T_DATA>& source ) {
			GUM_ERROR ( FatalError, "Illegal!" );
			// For noisy compilers
			return *this;
		}
} /* namespace gum */
