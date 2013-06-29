
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
 * @brief Template methods of gum::MultiDimImplementation.
 *
 * @author Jean-Christophe Magnan
 *
 */
#include <agrum/multidim/multiDimDecisionGraph.h>

namespace gum {

  // ############################################################################
  /// @name Constructors / Destructors
  // ############################################################################
  /// @{
    // ============================================================================
    /// Default constructor.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraph<GUM_SCALAR>::MultiDimDecisionGraph(){

    }

    // ============================================================================
    /// Copy constructor.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraph<GUM_SCALAR>::MultiDimDecisionGraph( const MultiDimDecisionGraph<GUM_SCALAR>& from ){

    }

    // ============================================================================
    /// Copy Operator.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraph<GUM_SCALAR>& MultiDimDecisionGraph<GUM_SCALAR>::operator=(const MultiDimDecisionGraph<GUM_SCALAR>& from){

    }
    // ============================================================================
    /// Destructor.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraph<GUM_SCALAR>::~MultiDimDecisionGraph(){

    }
  /// @}

  // ============================================================================
  /**
   * This method creates a clone of this object, withouth its content
   * (including variable), you must use this method if you want to ensure
   * that the generated object has the same type than the object containing
   * the called newFactory()
   * For example :
   *   MultiDimArray<double> y;
   *   MultiDimContainer<double>* x = y.newFactory();
   * Then x is a MultiDimArray<double>*
   *
   * @warning you must desallocate by yourself the memory
   * @return an empty clone of this object with the same type
   */
  // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimContainer<GUM_SCALAR>* MultiDimDecisionGraph<GUM_SCALAR>::newFactory() const{

    }


  // ############################################################################
  /// @name Accessors / Modifiers herited from gum::MultiDimImplementation
  // ############################################################################
  /// @{

    // ============================================================================
    /// see gum::MultiDimImplementation
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const std::string& MultiDimDecisionGraph<GUM_SCALAR>::name() const{
        return "multidimdecisiongraph";
    }

    // ============================================================================
    /**
     * see gum::MultiDimImplementation::set ( const Instantiation &i, const GUM_SCALAR &value )
     *
     * @throw OperationNotAllowed. Decision Graph can't be edited so easily.
     * MultiDimDecisionGraphManager provides the framework to editate a Decision Graph.
     */
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::set ( const Instantiation &i, const GUM_SCALAR &value ) const{

    }

    // ============================================================================
    /**
     * see gum::MultiDimImplementation::fill( const GUM_SCALAR &d )
     *
     * @throw OperationNotAllowed. Decision Graph can't be edited so easily.
     * MultiDimDecisionGraphManager provides the framework to editate a Decision Graph.
     */
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::fill( const GUM_SCALAR &d ) const{

    }

    // ==============================================================================
    /**
     * see gum::MultiDimImplementation::fillWith ( const std::vector<GUM_SCALAR>& v )
     *
     * @throw OperationNotAllowed. Decision Graph can't be edited so easily.
     * MultiDimDecisionGraphManager provides the framework to editate a Decision Graph.
     */
    // ==============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::fillWith ( const std::vector<GUM_SCALAR>& v ) const{

    }

  /// @}


  // ############################################################################
  /// @name Implementation of MultiDimInterface
  // ############################################################################
  /// @{

    // ============================================================================
    /// See gum::MultiDimInterface::erase(const DiscreteVariable& v)
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::erase( const DiscreteVariable& v ){

    }

    // ============================================================================
    /// see gum::MultiDimImplementation::realSize()
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    Size MultiDimDecisionGraph<GUM_SCALAR>::realSize() const{
      return __sonsMap.size() + __valueMap.size();
    }

  /// @}

  // ############################################################################
  /// @name Slave management and extension due to slave management
  // ############################################################################
  /// @{
    // ============================================================================
    /** see gum::MultiDimImplementation::changeNotification( Instantiation& i,
     *                                                       const DiscreteVariable* const var,
     *                                                       const Idx& oldval,const Idx& newval )
     */
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::changeNotification( Instantiation& i,
                                 const DiscreteVariable* const var,
                                 const Idx& oldval,const Idx& newval ){

    }

    // ============================================================================
    /// see gum::MultiDimImplementation::setFirstNotification( Instantiation& i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setFirstNotification( Instantiation& i ){

    }

    // ============================================================================
    /// see gum::MultiDimImplementation::setLastNotification( Instantiation& i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setLastNotification( Instantiation& i ){

    }

    // ============================================================================
    /// see gum::MultiDimImplementation::setIncNotification( Instantiation& i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setIncNotification( Instantiation& i ){

    }

    // ============================================================================
    /// see gum::MultiDimImplementation::setDecNotification( Instantiation& i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setDecNotification( Instantiation& i ){

    }

    // ============================================================================
    /// see gum::MultiDimImplementation::setChangeNotification( Instantiation& i )
    // ===========================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::setChangeNotification( Instantiation& i ){

    }

    // ============================================================================
    /// see gum::MultiDimImplementation::toString( const Instantiation *i )
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const std::string MultiDimDecisionGraph<GUM_SCALAR>::toString( const Instantiation *i ) const{
        std::stringstream sBuff;
        sBuff << ( *i ) << " = " << get( *i );
        return sBuff.str();
    }

  /// @}


  // ############################################################################
  /// @name Copy methods.
  // ############################################################################
  /// @{
    // ============================================================================
    /**
     * @brief Basic copy src a gum::MultiDimContainer.
     * This method is virtual because it should be optimized in certain
     * gum::MultiDimContainer.
     *
     * @todo specific versions for decorator and for MultiDimArray
     *
     * @param src The gum::MultiDimContainer src which values are copied.
     * @param p_i Give the order to iterate in this gum::MultiDimContainer during
     *            the copy (natural order if null).
     * @throw OperationNotAllowed Raised if src does not have the same domain size
     *                            than this gum::MultiDimContainer.
     */
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::copyFrom ( const MultiDimContainer<GUM_SCALAR>& src,
                                                       Instantiation *p_i ) const {

    }

    // ============================================================================
    /**
     * Removes all variables in this gum::MultiDimContainer and copy the content
     * of src, variables included.
     */
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::copy ( const MultiDimContainer<GUM_SCALAR>& src ){
    }

  /// @}


  // ############################################################################
  /// @name Graph Handlers.
  // ############################################################################
  /// @{
    // ============================================================================
    /// Indicates if given node is terminal or not
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    bool MultiDimDecisionGraph<GUM_SCALAR>::isTerminalNode(const NodeId& node) const{
      return __valueMap.existsFirst(node);
    }

    // ============================================================================
    /// Returns value associated to given node
    /// @throw InvalidNode if node isn't terminal
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const GUM_SCALAR& MultiDimDecisionGraph<GUM_SCALAR>::nodeValue( NodeId n ) const {
      if ( !isTerminalNode( n ) )
        GUM_ERROR( InvalidNode, " Node " << n << " is a non terminal node. " );
      return this->unsafeNodeValue( n );
    }

    template<typename GUM_SCALAR>
    INLINE
    const GUM_SCALAR& MultiDimDecisionGraph<GUM_SCALAR>::unsafeNodeValue( NodeId n ) const{
      return this->__valueMap.second( n );
    }

    // ============================================================================
    /// Returns associated variable of given node
    /// @throw InvalidNode if Node is terminal
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const DiscreteVariable* MultiDimDecisionGraph<GUM_SCALAR>::nodeVariable( NodeId n ) const{
      if ( this->isTerminalNode( n ) )
        GUM_ERROR( InvalidNode, " Node " << n << " is a terminal node. " );
      return this->unsafeNodeVariable( n );
    }

    template<typename GUM_SCALAR>
    INLINE
    const DiscreteVariable* MultiDimDecisionGraph<GUM_SCALAR>::unsafeNodeVariable( NodeId n ) const{
      return this->__variableMap[ n ];
    }

    // ============================================================================
    /// Returns node's sons map
    /// @throw InvalidNode if node is terminal
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const NodeId* MultiDimDecisionGraph<GUM_SCALAR>::nodeSons( NodeId n ) const{
      if ( this->isTerminalNode( n ) )
        GUM_ERROR( InvalidNode, " Node " << n << " is a terminal node. " );

      return this->unsafeNodeSons( n );
    }

    template<typename GUM_SCALAR>
    INLINE
    const NodeId* MultiDimDecisionGraph<GUM_SCALAR>::unsafeNodeSons( NodeId n ) const{
      if ( !this->__arcMap.exists( n ) )
        return NULL;
      return this->__arcMap[ n ];
    }

    // ============================================================================
    /// Returns true if node has a default son
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    bool MultiDimDecisionGraph<GUM_SCALAR>::hasNodeDefaultSon( NodeId n ) const{
      return this->__defaultArcMap.exists( n );
    }

    // ============================================================================
    /// Returns node's default son
    /// @throw InvalidNode if node is terminal
    /// @throw NotFound if node doesn't have a default son
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const NodeId MultiDimDecisionGraph<GUM_SCALAR>::nodeDefaultSon( NodeId n ) const{
      if ( this->isTerminalNode( n ) )
        GUM_ERROR( InvalidNode, " Node " << n << " is a terminal node. " );
      if( !this->__defaultArcMap.exists( n ) )
        GUM_ERROR( NotFound, " Node " <<  n << " doesn't have a default son." );
      return this->unsafeNodeDefaultSon( n );
    }

    template<typename GUM_SCALAR>
    INLINE
    const NodeId MultiDimDecisionGraph<GUM_SCALAR>::unsafeNodeDefaultSon( NodeId n ) const{
      return this->__defaultArcMap[ n ];
    }

    // ============================================================================
    /// Returns associated nodes of the variable pointed by the given node
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const List< NodeId >* MultiDimDecisionGraph<GUM_SCALAR>::variableNodes( const DiscreteVariable* v ) const{
      if ( ! this->__var2NodeIdMap.exists( v ) )
        return NULL;
      return this->__var2NodeIdMap[ v ];
    }

    // ============================================================================
    /// Returns associated nodes of the variable pointed by the given node
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const Idx* MultiDimDecisionGraph<GUM_SCALAR>::variableUsedModalities( const DiscreteVariable* v ) const{
      if ( ! this->__varUsedModalitiesMap.exists( v ) )
        return NULL;
      return this->__varUsedModalitiesMap[ v ];
    }

    // ============================================================================
    /// Returns true if variable is in diagram
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    bool MultiDimDecisionGraph<GUM_SCALAR>::isInDiagramVariable( const DiscreteVariable* v ) const{
      return( __var2NodeIdMap.exists( v ) );
    }

  /// @}

    // ============================================================================
    /// @brief Replace variable x by y.
    /// Technically this should be call by any subclass overloading this method
    /// to proceed with the changes in this class containers.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::_swap( const DiscreteVariable* x, const DiscreteVariable* y ){

    }

    // ============================================================================
    /**
     * @brief Return a data, given a Instantiation.
     *
     * Note that get allows to change a value in the container.
     * The method is still tagged as const.
     *
     * @warning If i variables set is disjoint with this gum::MultiDimContainer
     * then 0 is assumed for dimensions (i.e. variables) not prensent in the
     * instantiation.
     *
     * @param i The instantiation used to find the data.
     */
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    GUM_SCALAR & MultiDimDecisionGraph<GUM_SCALAR>::_get ( const Instantiation &inst ) const{
        NodeId current_Node = __root;
        while ( ! isTerminalNode( current_Node ) ) {
          if ( __sonsMap[current_Node][ inst.val( *__variableMap[current_Node] ) ] != 0 )
            current_Node = __sonsMap[current_Node][ inst.val( *__variableMap[current_Node] ) ];
          else
            current_Node = __defaultSonMap[current_Node];
        }
        return this->__valueMap.second( current_Node );
    }

} //gum
