/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by*
 *   the Free Software Foundation; either version 2 of the License, or       *
 *   (at your option) any later version. *
 *                                                                                                               *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of    *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   *
 *   GNU General Public License for more details.                              *
 *                                                                                   *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                             *
 *   Free Software Foundation, Inc., *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.     *
 ***************************************************************************/
/**
 * @file
 * @brief Template Implementation of the FMDPFactory class.
 *
 * @author Jean-Christophe Magnan and Pierre-Henri WUILLEMIN
 */

#include <agrum/FMDP/fmdpFactory.h>

#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/potential.h>


// #define FMDP_VERBOSITY(x) {  if (isVerbose()) std::cerr << "[FMDP factory]
// "<< x << std::endl; }


namespace gum {

  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                                   Constructor &
   * Destructor                        **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Default constructor.
  // @param fmdp A pointer over the Factored Markov Decision Process filled by
  // this factory.

  template <typename GUM_SCALAR>
  INLINE FMDPFactory<GUM_SCALAR>::FMDPFactory( FMDP<GUM_SCALAR>* fmdp )
      : __fmdp( fmdp ) {

    GUM_CONSTRUCTOR( FMDPFactory );

    __states.push_back( FMDPfactory_state::NONE );
    resetVerbose();
    __actionIdcpt = 1;
  }


  // Destructor

  template <typename GUM_SCALAR>
  INLINE FMDPFactory<GUM_SCALAR>::~FMDPFactory() {

    GUM_DESTRUCTOR( FMDPFactory );
  }

  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                                        Getter and
   * setters                         **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Returns the IBayesNet created by this factory.

  template <typename GUM_SCALAR>
  INLINE FMDP<GUM_SCALAR>* FMDPFactory<GUM_SCALAR>::fmdp() const {

    if ( state() != FMDPfactory_state::NONE )
      GUM_ERROR( OperationNotAllowed,
                 "Illegal state to return the factored "
                 "markov decision process: it is not yet "
                 "finished." );

    return __fmdp;
  }


  // Returns the current state of the factory.

  template <typename GUM_SCALAR>
  INLINE FMDPfactory_state FMDPFactory<GUM_SCALAR>::state() const {
    // This is ok because there is alway at least the state
    // FMDPfactory_state::NONE in the stack.
    return __states.back();
  }


  // Returns a constant reference on a variable given it's name.
  // @throw NotFound Raised if no variable matches the name.

  template <typename GUM_SCALAR>
  INLINE const DiscreteVariable*
  FMDPFactory<GUM_SCALAR>::variable( const std::string& name ) const {

    for ( const auto& elt : __varNameMap )
      if ( elt.first.compare( name ) == 0 ) return elt.second;

    GUM_ERROR( NotFound, name );

    return nullptr;
  }


  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **   Variable declaration methods (FMDPfactory_state::NONE <->
   * FMDPfactory_state::VARIABLE)          **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Tells the factory that we're in a variable declaration.

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::startVariableDeclaration() {

    if ( state() != FMDPfactory_state::NONE )
      __illegalStateError( "startVariableDeclaration" );
    else {
      __states.push_back( FMDPfactory_state::VARIABLE );
      __stringBag.push_back( "name" );
      __stringBag.push_back( "desc" );
    }

    //       VERBOSITY ( "  starting variable" );
  }


  // Tells the factory the current variable's name.

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::variableName( const std::string& name ) {

    if ( state() != FMDPfactory_state::VARIABLE )
      __illegalStateError( "variableName" );
    else {

      if ( __varNameMap.exists( name ) )
        GUM_ERROR( DuplicateElement, "Name already used: " + name );

      __foo_flag = true;
      __stringBag[0] = name;
      //         VERBOSITY ( "  -- variable " << name );
    }
  }


  // Tells the factory the current variable's description.

  template <typename GUM_SCALAR>
  INLINE void
  FMDPFactory<GUM_SCALAR>::variableDescription( const std::string& desc ) {

    if ( state() != FMDPfactory_state::VARIABLE )
      __illegalStateError( "variableDescription" );
    else {
      __bar_flag = true;
      __stringBag[1] = desc;
    }
  }


  // Adds a modality to the current variable.
  // @throw DuplicateElement If the current variable already has a modality
  //                         with the same name.

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::addModality( const std::string& name ) {

    if ( state() != FMDPfactory_state::VARIABLE )
      __illegalStateError( "addModality" );
    else {
      __checkModalityInBag( name );
      __stringBag.push_back( name );
    }
  }


  // Check if in __stringBag there is no other modality with the same name.

  template <typename GUM_SCALAR>
  INLINE void
  FMDPFactory<GUM_SCALAR>::__checkModalityInBag( const std::string& mod ) {

    for ( size_t i = 2; i < __stringBag.size(); ++i )
      if ( mod == __stringBag[i] ) GUM_ERROR( DuplicateElement, mod );
  }


  // Tells the factory that we're out of a variable declaration.

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::endVariableDeclaration() {

    if ( state() != FMDPfactory_state::VARIABLE )
      __illegalStateError( "endVariableDeclaration" );
    else if ( __foo_flag && ( __stringBag.size() > 3 ) ) {

      LabelizedVariable* var = new LabelizedVariable(
          __stringBag[0], ( __bar_flag ) ? __stringBag[1] : "", 0 );

      for ( size_t i = 2; i < __stringBag.size(); ++i ) {
        var->addLabel( __stringBag[i] );
      }

      __fmdp->addVariable( var );
      __varNameMap.insert( var->name(), var );
      __varNameMap.insert( __fmdp->main2prime( var )->name(),
                           __fmdp->main2prime( var ) );

      __resetParts();
      __states.pop_back();

      //         VERBOSITY ( "  variable " << var->name() << " OK" );

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

      GUM_ERROR( OperationNotAllowed, msg.str() );
    }
  }


  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **    Action declaration methods (FMDPfactory_state::NONE <->
   * FMDPfactory_state::ACTION)             **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Tells the factory that we're declaring action

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::startActionDeclaration() {
    if ( state() != FMDPfactory_state::NONE )
      __illegalStateError( "startActionDeclaration" );
    else {
      __foo_flag = true;
      __states.push_back( FMDPfactory_state::ACTION );
    }

    //       VERBOSITY ( "starting action declaration" );
  }


  // Tells the factory to add an action

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::addAction( const std::string& action ) {
    if ( state() != FMDPfactory_state::ACTION )
      __illegalStateError( "addAction" );
    else {
      __stringBag.push_back( action );
      __fmdp->addAction( __actionIdcpt++, action );
    }
  }


  // Tells the factory that we're out of an action declaration.

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::endActionDeclaration() {

    if ( state() != FMDPfactory_state::ACTION )
      __illegalStateError( "endActionDeclaration" );
    else {
      __states.pop_back();
      __resetParts();
    }

    //       VERBOSITY ( "action OK" );
  }


  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                 Transition declaration methods **/
  /* **      (FMDPfactory_state::NONE <-> FMDPfactory_state::TRANSITION <->
   * FMDPfactory_state::ACTION)    **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Tells the factory that we're declaring transition

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::startTransitionDeclaration() {
    if ( state() != FMDPfactory_state::NONE &&
         state() != FMDPfactory_state::ACTION )
      __illegalStateError( "startTransitionDeclaration" );
    else
      __states.push_back( FMDPfactory_state::TRANSITION );

    //       VERBOSITY ( "starting transition declaration" );
    this->__initializeFunctionGraph();
  }


  // Tells the factory to add an action

  template <typename GUM_SCALAR>
  INLINE void
  FMDPFactory<GUM_SCALAR>::addTransition( const std::string&        var,
                                          const MultiDimAdressable* transition ) {

    const MultiDimImplementation<GUM_SCALAR>* t =
        reinterpret_cast<const MultiDimImplementation<GUM_SCALAR>*>( transition );

    if ( state() != FMDPfactory_state::TRANSITION )
      __illegalStateError( "addTransition" );
    else if ( __foo_flag )
      __fmdp->addTransitionForAction(
          __fmdp->actionId( __stringBag[0] ), __varNameMap[var], t );
    else
      __fmdp->addTransition( __varNameMap[var], t );
  }


  // Tells the factory to add a transition table to the current fmdp.
  // This transition table will be extracted from incorporated
  // multiDimFunctionGraph.

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::addTransition( const std::string& var ) {

    if ( state() != FMDPfactory_state::TRANSITION )
      __illegalStateError( "addTransition" );
    else {

      this->__finalizeFunctionGraph();

      if ( __foo_flag ) {
        this->__FunctionGraph->setTableName( "ACTION :" + __stringBag[0] +
                                             " - VARIABLE : " + var );
        __fmdp->addTransitionForAction( __fmdp->actionId( __stringBag[0] ),
                                        __varNameMap[var],
                                        this->__FunctionGraph );
      } else {
        __fmdp->addTransition( __varNameMap[var], this->__FunctionGraph );
      }
    }
  }


  // Tells the factory that we're out of a transition declaration.

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::endTransitionDeclaration() {

    if ( state() != FMDPfactory_state::TRANSITION )
      __illegalStateError( "endTransitionDeclaration" );
    else
      __states.pop_back();

    //       VERBOSITY ( "transition OK" );
  }


  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                  Cost declaration methods **/
  /* **        (FMDPfactory_state::NONE <-> FMDPfactory_state::COST <->
   * FMDPfactory_state::ACTION)        **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Tells the factory that we're declaring cost

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::startCostDeclaration() {
    if ( state() != FMDPfactory_state::NONE &&
         state() != FMDPfactory_state::ACTION )
      __illegalStateError( "startTransitionDeclaration" );
    else
      __states.push_back( FMDPfactory_state::COST );

    //       VERBOSITY ( "starting Cost declaration" );
    this->__initializeFunctionGraph();
  }


  // Tells the factory to add a cost

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::addCost( const MultiDimAdressable* cost ) {

    const MultiDimImplementation<GUM_SCALAR>* c =
        reinterpret_cast<const MultiDimImplementation<GUM_SCALAR>*>( cost );

    if ( state() != FMDPfactory_state::COST )
      __illegalStateError( "addCost" );
    else if ( __foo_flag )
      __fmdp->addCostForAction( __fmdp->actionId( __stringBag[0] ), c );
    else
      __fmdp->addCost( c );
  }


  // Tells the factory to add a cost
  // This cost table will be extracted from incorporated multiDimFunctionGraph.

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::addCost() {
    if ( state() != FMDPfactory_state::COST )
      __illegalStateError( "addCost" );
    else {

      this->__finalizeFunctionGraph();

      if ( __foo_flag )
        __fmdp->addCostForAction( __fmdp->actionId( __stringBag[0] ),
                                  this->__FunctionGraph );
      else
        __fmdp->addCost( this->__FunctionGraph );
    }
  }


  // Tells the factory that we're out of a cost declaration.

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::endCostDeclaration() {

    if ( state() != FMDPfactory_state::COST )
      __illegalStateError( "endCostDeclaration" );
    else
      __states.pop_back();

    //       VERBOSITY ( "Cost OK" );
  }


  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                 Reward declaration methods **/
  /* **       (FMDPfactory_state::NONE <-> FMDPfactory_state::REWARD <->
   * FMDPfactory_state::ACTION)       **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Tells the factory that we're declaring reward

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::startRewardDeclaration() {
    if ( state() != FMDPfactory_state::NONE &&
         state() != FMDPfactory_state::ACTION )
      __illegalStateError( "startRewardDeclaration" );
    else
      __states.push_back( FMDPfactory_state::REWARD );

    //       VERBOSITY ( "starting reward declaration" );
    this->__initializeFunctionGraph();
  }


  // Tells the factory that we're in a reward declaration mode where the global
  // reward diagram is an operation between simplier dd

  template <typename GUM_SCALAR>
  INLINE void
  FMDPFactory<GUM_SCALAR>::setOperationModeOn( std::string operationType ) {
    __foo_flag = true;
    std::string ot( operationType );
    __stringBag.push_back( ot );
  }


  // Tells the factory to add a reward

  template <typename GUM_SCALAR>
  INLINE void
  FMDPFactory<GUM_SCALAR>::addReward( const MultiDimAdressable* reward ) {

    const MultiDimImplementation<GUM_SCALAR>* r =
        reinterpret_cast<const MultiDimImplementation<GUM_SCALAR>*>( reward );

    if ( state() != FMDPfactory_state::REWARD )
      __illegalStateError( "addReward" );
    else
      __fmdp->addReward( r );
  }


  // Tells the factory to add a reward
  // This reward table will be extracted from incorporated
  // multiDimFunctionGraph.

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::addReward() {
    if ( state() != FMDPfactory_state::REWARD )
      __illegalStateError( "addReward" );
    else {

      this->__finalizeFunctionGraph();
      __FunctionGraph->setTableName( "Reward" );

      if ( __foo_flag )
        __ddBag.push_back( this->__FunctionGraph );
      else
        __fmdp->addReward( this->__FunctionGraph );
    }
  }


  // Tells the factory that we're out of a reward declaration.

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::endRewardDeclaration() {

    if ( state() != FMDPfactory_state::REWARD )
      __illegalStateError( "endRewardDeclaration" );
    else {
      if ( __foo_flag ) {
        MultiDimImplementation<GUM_SCALAR>* res = nullptr;
        MultiDimImplementation<GUM_SCALAR>* temp;

        for ( const auto elt : __ddBag ) {

          temp = res;

          switch ( __stringBag[0][0] ) {
            case '+':
              res = add2MultiDimFunctionGraphs( res, elt );
              break;

            case '-':
              res = subtract2MultiDimFunctionGraphs( res, elt );
              break;

            case '*':
              res = multiply2MultiDimFunctionGraphs( res, elt );
              break;

            case '/':
              res = divide2MultiDimFunctionGraphs( res, elt );
              break;

            default:
              break;
          }

          delete elt;

          if ( temp != nullptr ) delete temp;
        }
        reinterpret_cast<MultiDimFunctionGraph<GUM_SCALAR>*>( res )->setTableName(
            "Reward" );
        __fmdp->addReward( res );
      }

      __resetParts();
      __states.pop_back();
    }
    //       VERBOSITY ( "reward OK" );
  }


  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                Discount declaration methods **/
  /* **    (FMDPfactory_state::NONE <-> FMDPfactory_state::DISCOUNT <->
   * FMDPfactory_state::ACTION)        **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Tells the factory that we're declaring discount
  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::startDiscountDeclaration() {
    if ( state() != FMDPfactory_state::NONE )
      __illegalStateError( "startDiscountDeclaration" );
    else
      __states.push_back( FMDPfactory_state::DISCOUNT );

    //       VERBOSITY ( "starting discount declaration" );
  }


  // Tells the factory to add a discount

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::addDiscount( float discount ) {
    if ( state() != FMDPfactory_state::DISCOUNT )
      __illegalStateError( "addDiscount" );
    //    else
    //      __fmdp->setDiscount ( ( GUM_SCALAR ) discount );
  }


  // Tells the factory that we're out of a discount declaration.

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::endDiscountDeclaration() {

    if ( state() != FMDPfactory_state::DISCOUNT )
      __illegalStateError( "endDiscountDeclaration" );
    else
      __states.pop_back();

    //       VERBOSITY ( "discount OK" );
  }


  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                 Discount declaration methods **/
  /* **       (FMDPfactory_state::NONE <-> FMDPfactory_state::DISCOUNT <->
   * FMDPfactory_state::ACTION)     **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Insert in diagram a non terminal node

  template <typename GUM_SCALAR>
  INLINE NodeId
  FMDPFactory<GUM_SCALAR>::addInternalNode( std::string name_of_var ) {
    return __FunctionGraph->manager()->addInternalNode( variable( name_of_var ) );
  }


  // Insert in diagram a terminal node

  template <typename GUM_SCALAR>
  INLINE NodeId FMDPFactory<GUM_SCALAR>::addTerminalNode( float value ) {
    return __FunctionGraph->manager()->addTerminalNode( (GUM_SCALAR)value );
  }


  // Insert an arc in diagram
  template <typename GUM_SCALAR>
  INLINE void
  FMDPFactory<GUM_SCALAR>::addArc( NodeId from, NodeId to, Idx modality ) {
    __FunctionGraph->manager()->setSon( from, modality, to );
  }


  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::setRoot( NodeId rootId ) {
    __FunctionGraph->manager()->setRootNode( rootId );
  }

  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                  Various Private Methods **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Raise an OperationNotAllowed with the message "Illegal state."

  template <typename GUM_SCALAR>
  INLINE void
  FMDPFactory<GUM_SCALAR>::__illegalStateError( const std::string& s ) {

    std::string msg = "Illegal state call (";
    msg += s;
    msg += ") in state ";

    switch ( state() ) {

      case FMDPfactory_state::NONE:
        msg += "FMDPfactory_state::NONE";
        break;

      case FMDPfactory_state::VARIABLE:
        msg += "FMDPfactory_state::VARIABLE";
        break;

      case FMDPfactory_state::ACTION:
        msg += "FMDPfactory_state::ACTION";
        break;

      case FMDPfactory_state::TRANSITION:
        msg += "FMDPfactory_state::TRANSITION";
        break;

      case FMDPfactory_state::COST:
        msg += "FMDPfactory_state::COST";
        break;

      case FMDPfactory_state::REWARD:
        msg += "FMDPfactory_state::REWARD";
        break;

      case FMDPfactory_state::DISCOUNT:
        msg += "FMDPfactory_state::DISCOUNT";
        break;

      default:
        msg += "Unknown state";
    }

    GUM_ERROR( OperationNotAllowed, msg );
  }


  // Reset the different parts used to constructed the IBayesNet.

  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::__resetParts() {
    __foo_flag = false;
    __bar_flag = false;
    __stringBag.clear();
    __ddBag.clear();
  }


  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::__initializeFunctionGraph() {

    this->__FunctionGraph =
        MultiDimFunctionGraph<GUM_SCALAR>::getReducedAndOrderedInstance();
    // Recopie des variables principales dans le graphe de décision
    for ( auto varIter = __fmdp->beginVariables();
          varIter != __fmdp->endVariables();
          ++varIter ) {
      __FunctionGraph->add( **varIter );
    }

    // Recopie des version primes des variables dans le graphe de décision
    for ( auto varIter = __fmdp->beginVariables();
          varIter != __fmdp->endVariables();
          ++varIter ) {
      __FunctionGraph->add( *( __fmdp->main2prime( *varIter ) ) );
    }
  }


  template <typename GUM_SCALAR>
  INLINE void FMDPFactory<GUM_SCALAR>::__finalizeFunctionGraph() {
    this->__FunctionGraph->manager()->reduce();
    this->__FunctionGraph->manager()->clean();
  }

  //~ ==============
  //~ // Check if a variable with the given name exists, if not raise an
  // NotFound
  //~ // exception.
  //~ ==============
  //~ template<typename GUM_SCALAR> INLINE
  //~ void
  //~ FMDPFactory<GUM_SCALAR>::__checkVariableName ( const std::string& name ) {
  //~ if ( !__varNameMap.exists ( name ) )
  //~ GUM_ERROR ( NotFound, name );
  //~ }


  // Copy operator is illegal, use only copy constructor.

  template <typename GUM_SCALAR>
  INLINE FMDPFactory<GUM_SCALAR>& FMDPFactory<GUM_SCALAR>::
  operator=( const FMDPFactory<GUM_SCALAR>& source ) {
    GUM_ERROR( FatalError, "Illegal!" );
    // For noisy compilers
    return *this;
  }
} /* namespace gum */
