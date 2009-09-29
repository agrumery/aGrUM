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
 * @brief Class representing Bayesian networks
 *
 * @author Lionel Torti & Pierre-Henri Wuillemin
 *
 */
#ifndef GUM_BAYES_NET_H
#define GUM_BAYES_NET_H
// ============================================================================
#include <utility>
// ============================================================================
#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>
#include <agrum/core/hashTable.h>
// ============================================================================
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/undiGraph.h>
// ============================================================================
#include <agrum/multidim/multiDimNoisyOR.h>
#include <agrum/multidim/potential.h>
// ============================================================================
#include <agrum/BN/variableNodeMap.h>
// ============================================================================

namespace gum {
  template<typename T_DATA>

  class BayesNetFactory;

  /**
   * @class AbstractBayesNet BayesNet.h <agrum/BN/BayesNet.h>
   * Abstract class for representing a BayesNet.
   */
  template<typename T_DATA>
  class AbstractBayesNet {
    public:
      // ===========================================================================
      /// @name Constructors / Destructors
      // ===========================================================================
      /// @{

      /**
       * Default constructor.
       */
      AbstractBayesNet();

      /**
       * Destructor.
       */
      virtual ~AbstractBayesNet();

      /**
       * Copy constructor. Do nothing.
       */
      AbstractBayesNet( const AbstractBayesNet<T_DATA>& source );

      /// @}
      // ===========================================================================
      /// @name Getter and setters
      // ===========================================================================
      /// @{

      /**
       * Return the value of the property "name" of this BayesNet.
       * @throw NotFound Raised if no "name" property is found.
       */
      const std::string& property( const std::string& name ) const;

      /**
       * Add or change a property of this BayesNet.
       */
      void setProperty( const std::string& name, const std::string& value );

      /// @}
      // ===========================================================================
      /// @name Variable manipulation methods.
      // ===========================================================================
      /// @{

      /**
       * Returns the CPT of a variable.
       * @throw NotFound If no variable's id matches varId.
       */
      virtual const Potential<T_DATA>& cpt( NodeId varId ) const =0;

      /**
       * Returns a constant reference to the dag of this Bayes Net.
       */
      virtual const DAG& dag() const =0;

      /**
      * Returns a constant reference to the VariableNodeMap of thisBN
      */
      virtual const VariableNodeMap& variableNodeMap() const =0;

      /**
       * Returns the number of variables in this bayes net.
       */
      virtual Idx size() const =0;

      /**
       * Retursn true if this bayes net is empty.
       */
      virtual bool empty() const =0;

      /**
       * Shortcut for this->dag().beginNodes().
       */
      const DAG::NodeIterator beginNodes() const;

      /**
       * Shortcut for this->dag().endNodes().
       */
      const DAG::NodeIterator endNodes() const;

      /**
      * Returns a constant reference over a variabe given it's node id.
      * @throw NotFound If no variable's id matches varId.
      */
      virtual const DiscreteVariable& variable( NodeId id ) const =0;

      /**
      * Return id node from discrete var pointer.
      * @throw NotFound If no variable matches var.
      */
      virtual NodeId nodeId( const DiscreteVariable &var ) const =0;

      /// Getter by name
      /// @throw NotFound if no such name exists in the graph.
      virtual NodeId idFromName( const std::string& name ) const =0;

      /// Getter by name
      /// @throw NotFound if no such name exists in the graph.
      virtual const DiscreteVariable& variableFromName( const std::string& name ) const =0;

      /// @}
      // ===========================================================================
      /// @name Arc manipulation methods.
      // ===========================================================================
      /// @{

      /**
       * Shortcut for this->dag().beginArcs().
       */
      const DAG::ArcIterator beginArcs() const;

      /**
       * Shortcut for this->dag().endArcs().
       */
      const DAG::ArcIterator& endArcs() const;

      /// @}
      // ===========================================================================
      /// @name Graphical methods
      // ===========================================================================
      /// @{

      /**
       * The node's id are coherent with the variables and nodes of the topology.
       * @param clear If false returns the previously created moral graph.
       */
      virtual const UndiGraph& moralGraph( bool clear = true ) const =0;

      /**
       * The topological order stays the same as long as no variable or arcs are
       * added or erased from the topology.
       * @param clear If false returns the previously created topology.
       */
      virtual const Sequence<NodeId>& getTopologicalOrder( bool clear = true ) const =0;

      /// @}

      /// @return Returns a string representation of this BayesNet.
      std::string toString( void ) const;

      /// @return Returns a dot representation of this BayesNet.
      std::string toDot( void ) const;

      /// @return Returns true if the from and this are equal.
      bool operator==(const AbstractBayesNet<T_DATA>& from) const;

      /// @return Returns false if the from and this are equal.
      bool operator!=(const AbstractBayesNet<T_DATA>& from) const;

    protected:
      /**
       * Private copy operator.
       */
      AbstractBayesNet<T_DATA>& operator=( const AbstractBayesNet<T_DATA>& source );

      /// Returns the moral graph of this BayesNet.
      void _moralGraph(UndiGraph& graph) const;

      /// Returns a topological order of this BayesNet.
      void _topologicalOrder(Sequence<NodeId>& order) const;

    private:
      /// The properties of this BayesNet.
      /// Initialized using a lazy instantiation.
      mutable HashTable<std::string, std::string>* __propertiesMap;

      /// Return the properties of this BayesNet and initialize the hash table is
      /// necessary.
      HashTable<std::string, std::string>& __properties() const;
  };

  /**
   * @class BayesNet BayesNet.h <agrum/BN/BayesNet.h>
   * @brief Class representing a Bayesian Network.
   * @ingroup bn_group
   *
   * After a variable is added to the BN, if it's domain size changes, then the
   * data in it's CPT is lost.
   *
   * We recommend you to use the gum::BayesNetFactory class to build a BayesNet.
   *
   * Don't forget that you can print a BayesNet using
   * gum::operator<<(std::ostream&, const BayesNet<T_DATA>&).
   */
  template<typename T_DATA>
  class BayesNet: public AbstractBayesNet<T_DATA> {

      friend class BayesNetFactory<T_DATA>;

    public:
      // ===========================================================================
      /// @name Constructors / Destructors
      // ===========================================================================
      /// @{

      /**
       * Default constructor.
       */
      BayesNet();

      /**
       * Destructor.
       */
      virtual ~BayesNet();

      /**
       * Copy constructor.
       */
      BayesNet( const BayesNet<T_DATA>& source );

      /**
       * Copy operator.
       */
      BayesNet<T_DATA>& operator=( const BayesNet<T_DATA>& source );

      /// @}
      // ===========================================================================
      /// @name Variable manipulation methods.
      // ===========================================================================
      /// @{

      /**
       * Returns the CPT of a variable.
       * @throw NotFound If no variable's id matches varId.
       */
      virtual const Potential<T_DATA>& cpt( NodeId varId ) const;

      /**
       * Returns a constant reference to the dag of this Bayes Net.
       */
      virtual const DAG& dag() const;

      /**
      * Returns a constant reference to the VariableNodeMap of thisBN
      */
      virtual const VariableNodeMap& variableNodeMap() const;

      /**
       * Returns the number of variables in this bayes net.
       */
      virtual Idx size() const;

      /**
       * Retursn true if this bayes net is empty.
       */
      virtual bool empty() const;

      /**
       * Add a variable, it's associate node and it's CPT. The id of the new
       * variable is automatically generated.
       *
       * The implementation of the Potential is by default a MultiDimArray.
       *
       * @param variable The variable added by copy.
       * @param id The chosen id. If 0, the NodeGraphPart will choose.
       * @warning give an id (not 0) should be reserved for rare and specific situations !!!
       * @return the id of the added variable.
       * @throws DuplicateElement if id(<>0) is already used
       */
      NodeId add( const DiscreteVariable& variable, NodeId id = 0 );

      /**
       * Add a variable, it's associate node and it's CPT. The id of the new
       * variable is automatically generated.
       *
       * @param variable The variable added by copy.
       * @param aContent The content used for the variable potential.
       * @param id The chosen id. If 0, the NodeGraphPart will choose.
       * @warning give an id (not 0) should be reserved for rare and specific situations !!!
       * @return the id of the added variable.
       * @throws DuplicateElement if id(<>0) is already used
       */
      NodeId add( const DiscreteVariable& variable,
                  MultiDimImplementation<T_DATA> *aContent, NodeId id = 0 );

      /**
       * Erase a Variable from the network and remove the variable from
       * all childs of id.
       * If no variable matches the id, then nothing is done.
       *
       * @param id The id of the variable to erase.
       */
      void erase( NodeId id );

      /**
       * Erase a Variable from the network and remove the variable from
       * all childs of var.
       * If no variable matches, then nothing is done.
       *
       * @param var The reference on the variable to remove.
       */
      void erase( const DiscreteVariable& var );

      /**
      * Returns a constant reference over a variabe given it's node id.
      * @throw NotFound If no variable's id matches varId.
      */
      virtual const DiscreteVariable& variable( NodeId id ) const;

      /** we allow the user to change the name of a variable
      * @throws DuplicateLabel if this name already exists
      * @throws NotFound Raised if no nodes matches id.
      */
      void changeVariableName(NodeId id,const std::string& new_name);

      /**
      * Return id node from discrete var pointer.
      * @throw NotFound If no variable matches var.
      */
      virtual NodeId nodeId( const DiscreteVariable &var ) const ;

      /// Getter by name
      /// @throw NotFound if no such name exists in the graph.
      virtual NodeId idFromName( const std::string& name ) const;

      /// Getter by name
      /// @throw NotFound if no such name exists in the graph.
      virtual const DiscreteVariable& variableFromName( const std::string& name ) const;

      /// @}
      // ===========================================================================
      /// @name Arc manipulation methods.
      // ===========================================================================
      /// @{

      /**
       * Add an arc in the BN, and update arc.head's CPT.
       *
       * @param head and
       * @param tail as NodeId
       * @throw InvalidEdge If arc.tail and/or arc.head are not in the BN.
       */
      void insertArc( NodeId tail, NodeId head );

      /**
       * Removes an arc in the BN, and update head's CTP.
       *
       * If (tail, head) doesn't exist, the nothing happens.
       * @param arc The arc removed.
       */
      void eraseArc( const Arc& arc );

      /**
      * Removes an arc in the BN, and update head's CTP.
      *
      * If (tail, head) doesn't exist, the nothing happens.
      * @param head and
      * @param tail as NodeId
      */
      void eraseArc( NodeId tail, NodeId head );

      /// @}
      // ===========================================================================
      /// @name Graphical methods
      // ===========================================================================
      /// @{

      /**
       * The node's id are coherent with the variables and nodes of the topology.
       * @param clear If false returns the previously created moral graph.
       */
      virtual const UndiGraph& moralGraph( bool clear = true ) const;

      /**
       * The topological order stays the same as long as no variable or arcs are
       * added or erased from the topology.
       * @param clear If false returns the previously created topology.
       */
      virtual const Sequence<NodeId>& getTopologicalOrder( bool clear = true ) const;

      /// @}
      // ===========================================================================
      /// @name Accessor for node with NoisyOR implementation
      // ===========================================================================
      /// @{

      /**
      * Add a variable, it's associate node and a noisyOR implementation. The id of the new
      * variable is automatically generated.
      *
      * @param variable The variable added by copy.
      * @param externalWeight @see gum::MultiDimNoisyOR
      * @param id The chosen id. If 0, the NodeGraphPart will choose.
      * @warning give an id (not 0) should be reserved for rare and specific situations !!!
      * @return the id of the added variable.
      * @throws DuplicateElement if id(<>0) is already used
      */
      NodeId addNoisyOR( const DiscreteVariable& variable, T_DATA externalWeight, NodeId id = 0 );

      /**
      * Add an arc in the BN, and update arc.head's CPT.
      *
      * @param head and
      * @param tail as NodeId
      * @param causalWeight @see gum::MultiDimNoisyOR
      * @throw InvalidArc If arc.tail and/or arc.head are not in the BN.
      * @throw InvalidArc If variable in arc.head is not a NoisyOR variable.
      */
      void insertArcNoisyOR( NodeId tail, NodeId head, T_DATA causalWeight );

      /// @}
      // ===========================================================================
      /// @name Deprecated methods
      // ===========================================================================
      /// @{

      /**
       * @warning Deprecated: use add(const DiscreteVariable&).
       */
      NodeId addVariable( const DiscreteVariable& variable );

      /**
       * @warning Deprecated: use add(const DiscreteVariable&, MultiDimImplementation*)
       */
      NodeId addVariable( const DiscreteVariable& variable,
                          MultiDimImplementation<T_DATA> *aContent );

      /**
       * @warning Deprecated: use erase(NodeId) instead.
       */
      void eraseVariable( NodeId id );

      /// @}

    private:

      /// The DAG of this bayes net.
      DAG __dag;

      /// the map between variable and id
      VariableNodeMap __varMap;

      /// Mapping between the variable's id and their CPT.
      //Property< Potential< T_DATA >* >::onNodes __probaMap;
      HashTable<NodeId, Potential<T_DATA>* > __probaMap;

      /// The moral graph of this bayes net.
      mutable UndiGraph* __moralGraph;

      /// The topology sequence of this bayes net.
      mutable Sequence<NodeId>* __topologicalOrder;

  };

  /// Prints map's DAG in output using the Graphviz-dot format.
  template <typename T_DATA>
  std::ostream&
  operator<<( std::ostream& output, const AbstractBayesNet<T_DATA>& map );

} /* namespace gum */
// ============================================================================
#include <agrum/BN/BayesNet.tcc>
// ============================================================================
#endif /* GUM_BAYES_NET_H */
// ============================================================================
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
