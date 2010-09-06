/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Template implementation of bns/bayesNet.h classes.
 *
 * @author Lionel Torti and Pierre-Henri Wuillemin
 */
// ============================================================================
#include <agrum/BN/abstractBayesNet.h>
// ============================================================================

namespace gum {
// ============================================================================
//                              AbstractBayesNet
// ============================================================================

  template <typename T_DATA> INLINE
  AbstractBayesNet<T_DATA>::AbstractBayesNet() :
      __propertiesMap ( 0 ) {
    GUM_CONSTRUCTOR ( AbstractBayesNet );
  }

  template <typename T_DATA> INLINE
  AbstractBayesNet<T_DATA>::AbstractBayesNet ( const AbstractBayesNet<T_DATA>& from ) :
      __propertiesMap ( 0 ) {
    GUM_CONS_CPY ( AbstractBayesNet );

    if ( from.__propertiesMap != 0 ) {
      __propertiesMap = new HashTable<std::string, std::string> ( * ( from.__propertiesMap ) );
    }
  }

  template <typename T_DATA> INLINE
  AbstractBayesNet<T_DATA>::~AbstractBayesNet() {
    GUM_DESTRUCTOR ( AbstractBayesNet );
    // Removing previous properties

    if ( __propertiesMap != 0 ) {
      delete __propertiesMap;
    }

  }

  template <typename T_DATA>
  AbstractBayesNet<T_DATA>&
  AbstractBayesNet<T_DATA>::operator= ( const AbstractBayesNet<T_DATA>& source ) {
    if ( this != &source ) {
      if ( __propertiesMap != 0 ) {
        delete __propertiesMap;
        __propertiesMap = 0;
      }

      if ( source.__propertiesMap != 0 ) {
        __propertiesMap = new HashTable<std::string, std::string> ( * ( source.__propertiesMap ) );
      }
    }

    return *this;
  }

  template<typename T_DATA> INLINE
  const std::string&
  AbstractBayesNet<T_DATA>::property ( const std::string& name ) const {
    try {
      return ( __properties() ) [name];
    } catch ( NotFound& ) {
      std::string msg = "The following property does not exists: ";
      GUM_ERROR ( NotFound, msg + name );
    }
  }

  template<typename T_DATA> INLINE
  void
  AbstractBayesNet<T_DATA>::setProperty ( const std::string& name, const std::string& value ) {
    try {
      __properties() [name] = value;
    } catch ( NotFound& ) {
      __properties().insert ( name, value );
    }
  }

  template<typename T_DATA> INLINE
  const DAG::NodeIterator
  AbstractBayesNet<T_DATA>::beginNodes() const {
    return dag().beginNodes();
  }

  template<typename T_DATA> INLINE
  const DAG::NodeIterator
  AbstractBayesNet<T_DATA>::endNodes() const {
    return dag().endNodes();
  }

  template<typename T_DATA> INLINE
  const DAG::ArcIterator
  AbstractBayesNet<T_DATA>::beginArcs() const {
    return dag().beginArcs();
  }

  template<typename T_DATA> INLINE
  const DAG::ArcIterator&
  AbstractBayesNet<T_DATA>::endArcs() const {
    return dag().endArcs();
  }

  template <typename T_DATA>
  void
  AbstractBayesNet<T_DATA>::_moralGraph ( UndiGraph& graph ) const {
    graph.populateNodes ( dag() );
    // transform the arcs into edges

    for ( DAG::ArcIterator iter = dag().beginArcs(); iter != dag().endArcs(); ++iter ) {
      graph.insertEdge ( iter->first(), iter->second() );
    }

    // mary the parents
    for ( DAG::NodeIterator iter = beginNodes(); iter != endNodes(); ++iter ) {
      const NodeSet& parents = dag().parents ( *iter );

      for ( NodeSetIterator it1 = parents.begin(); it1 != parents.end(); ++it1 ) {
        NodeSetIterator it2 = it1;

        for ( ++it2; it2 != parents.end(); ++it2 ) {
          // will automatically check if this edge already exists
          graph.insertEdge ( *it1, *it2 );
        }
      }
    }
  }

  template <typename T_DATA>
  void
  AbstractBayesNet<T_DATA>::_topologicalOrder ( Sequence<NodeId>& order ) const {
    // Add all root nodes in the list
    NodeSet nodeList = dag().nodes();

    for ( NodeSetIterator iter = nodeList.begin(); iter != nodeList.end(); ++iter ) {
      if ( dag().parents ( *iter ).empty() ) {
        order.insert ( *iter );
        nodeList.erase ( *iter );
      }
    }

    // Check if the graph has at least one root node
    if ( order.empty() ) {
      GUM_ERROR ( OperationNotAllowed, "No root node in DAG." );
    }

    // Add nodes in topological order
    while ( not nodeList.empty() ) {
      bool add;
      // Parsing all nodes in nodeList

      for ( NodeSetIterator node = nodeList.begin(); node != nodeList.end(); ++node ) {
        add = true;
        // Parsing all parents of current node
        const NodeSet& parents = dag().parents ( *node );

        for ( NodeSetIterator arc = parents.begin(); arc != parents.end(); ++arc ) {
          add = add && order.exists ( *arc );
        }

        // If current node's parent are all in order. then we add it
        if ( add ) {
          order.insert ( *node );
          nodeList.erase ( *node );
        }
      }
    }
  }

  template<typename T_DATA> INLINE
  HashTable<std::string, std::string>&
  AbstractBayesNet<T_DATA>::__properties() const {
    if ( __propertiesMap == 0 ) {
      __propertiesMap = new HashTable<std::string, std::string>();
    }

    return *__propertiesMap;
  }

  template<typename T_DATA> INLINE
  std::string
  AbstractBayesNet<T_DATA>::toString ( void ) const {
    Size dSize = 1;
    Size param = 0;

    for ( DAG::NodeIterator it = beginNodes();it != endNodes();++it ) {
      dSize *= variable ( *it ).domainSize();
      param += ( ( const MultiDimImplementation<T_DATA> & ) cpt ( *it ).getMasterRef() ).realSize();
    }

    int compressionRatio = ( int ) ( 100.0 * ( ( float ) 1.0 - ( ( float ) param ) / ( ( float ) dSize ) ) );

    std::stringstream s;
    s << "BN{nodes: " << size() << ", arcs: " << dag().sizeArcs() << ", domainSize: " << dSize << ", parameters: " << param << ", compression ratio: " << compressionRatio << "% }";
    return s.str();
  }

  template <typename T_DATA>
  bool
  AbstractBayesNet<T_DATA>::operator== ( const AbstractBayesNet<T_DATA>& from ) const {
    if ( dag() == from.dag() ) {
      for ( DAG::NodeIterator node = beginNodes(); node != endNodes(); ++node ) {
        // We don't use Potential::operator== because BN's don't share
        // DiscreteVariable's pointers.
        Bijection<const DiscreteVariable*, const DiscreteVariable*> bijection;
        bijection.insert ( & ( variable ( *node ) ), & ( from.variable ( *node ) ) );
        const NodeSet& parents = dag().parents ( *node );

        for ( NodeSetIterator arc = parents.begin(); arc != parents.end(); ++arc ) {
          bijection.insert ( & ( variable ( *arc ) ), & ( from.variable ( *arc ) ) );
        }

        Instantiation i ( cpt ( *node ) );

        Instantiation j ( from.cpt ( *node ) );

        for ( i.setFirst(); not i.end(); i.inc() ) {
          typedef Bijection<const DiscreteVariable*, const DiscreteVariable*>::iterator BiIter;

          for ( BiIter iter = bijection.begin(); iter != bijection.end(); ++iter ) {
            j.chgVal ( * ( iter.second() ), i.val ( * ( iter.first() ) ) );
          }

          if ( std::pow ( cpt ( *node ).get ( i ) - from.cpt ( *node ).get ( j ), ( T_DATA ) 2 ) > ( T_DATA ) 1e-6 ) {
            return false;
          }
        }
      }

      return true;
    }

    return false;
  }

  template <typename T_DATA>
  bool
  AbstractBayesNet<T_DATA>::operator!= ( const AbstractBayesNet<T_DATA>& from ) const {
    return not this->operator== ( from );
  }

} /* namespace gum */

// ============================================================================
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;   replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
