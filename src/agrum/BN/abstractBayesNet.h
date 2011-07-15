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
* @brief Class representing Abstract Bayesian networks
*
* @author Lionel Torti & Pierre-Henri Wuillemin
*
*/
#ifndef GUM_ABSTRACT_BAYES_NET_H
#define GUM_ABSTRACT_BAYES_NET_H

// ============================================================================
#include <list>
// ============================================================================
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/undiGraph.h>
// ============================================================================
#include <agrum/multidim/multiDimNoisyORCompound.h>
#include <agrum/multidim/multiDimNoisyORNet.h>
#include <agrum/multidim/potential.h>

namespace gum {

  /**
  * @class AbstractBayesNet abstractBayesNet.h <agrum/BN/abstractBayesNet.h>
   * Abstract class for representing a BayesNet.
   */
  template<typename T_DATA>

  class AbstractBayesNet {

    public:
      /// @name Constructors / Destructors
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
      /// @name Getter and setters
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
      /// @name Variable manipulation methods.
      /// @{

      /**
       * Returns the CPT of a variable.
       * @throw NotFound If no variable's id matches varId.
       */
      virtual const Potential<T_DATA>& cpt( NodeId varId ) const = 0;

      /**
       * Returns a constant reference to the dag of this Bayes Net.
       */
      virtual const DAG& dag() const = 0;

      /**
      * Returns a constant reference to the VariableNodeMap of thisBN
      */
      virtual const VariableNodeMap& variableNodeMap() const = 0;

      /**
       * Returns the number of variables in this bayes net.
       */
      virtual Idx size() const = 0;

      /**
       * Retursn true if this bayes net is empty.
       */
      virtual bool empty() const = 0;

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
      virtual const DiscreteVariable& variable( NodeId id ) const = 0;

      /**
      * Return id node src discrete var pointer.
      * @throw NotFound If no variable matches var.
      */
      virtual NodeId nodeId( const DiscreteVariable &var ) const = 0;

      /// Getter by name
      /// @throw NotFound if no such name exists in the graph.
      virtual NodeId idFromName( const std::string& name ) const = 0;

      /// Getter by name
      /// @throw NotFound if no such name exists in the graph.
      virtual const DiscreteVariable& variableFromName( const std::string& name ) const = 0;

      /// synchronize in (on this) with External
      void synchroInstantiations( Instantiation& inst,const Instantiation& external,bool sameLabelsOrder=true ) const;

      /// Get an instantiation over all the variables of the BN
      virtual void completeInstantiation( Instantiation& I ) const;
      /// @}
      /// @name Arc manipulation methods.
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
      /// @name Graphical methods
      /// @{

      /**
       * The node's id are coherent with the variables and nodes of the topology.
       * @param clear If false returns the previously created moral graph.
       */
      virtual const UndiGraph& moralGraph( bool clear = true ) const = 0;

      /**
       * The topological order stays the same as long as no variable or arcs are
       * added or erased src the topology.
       * @param clear If false returns the previously created topology.
       */
      virtual const Sequence<NodeId>& getTopologicalOrder( bool clear = true ) const = 0;

      /// @}

      /// @return Returns the log10 domain size of the joint probabilty for the BN
      double log10DomainSize( void ) const;

      /// @return Returns a string representation of this BayesNet.
      std::string toString( void ) const;

      /// @return Returns a dot representation of this BayesNet.
      virtual std::string toDot( void ) const=0;

      /// @return Returns true if the src and this are equal.
      bool operator==( const AbstractBayesNet<T_DATA>& src ) const;

      /// @return Returns false if the src and this are equal.
      bool operator!=( const AbstractBayesNet<T_DATA>& src ) const;

    protected:
      /**
       * Private copy operator.
       */
      AbstractBayesNet<T_DATA>& operator=( const AbstractBayesNet<T_DATA>& source );

      /// Returns the moral graph of this BayesNet.
      void _moralGraph( UndiGraph& graph ) const;

      /// Returns a topological order of this BayesNet.
      void _topologicalOrder( Sequence<NodeId>& order ) const;

    private:
      /// The properties of this BayesNet.
      /// Initialized using a lazy instantiation.
      mutable HashTable<std::string, std::string>* __propertiesMap;

      /// Return the properties of this BayesNet and initialize the hash table is
      /// necessary.
      HashTable<std::string, std::string>& __properties() const;
  };
} // gum

#include <agrum/BN/abstractBayesNet.tcc>

#endif /* GUM_ABSTRACT_BAYES_NET_H */
