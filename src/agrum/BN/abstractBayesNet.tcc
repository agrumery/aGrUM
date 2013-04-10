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
#include <math.h>

#include <agrum/BN/abstractBayesNet.h>
#include <agrum/BN/generator/defaultCPTGenerator.h>

// ============================================================================

namespace gum {
// ============================================================================
//                              AbstractBayesNet
// ============================================================================

  template <typename GUM_SCALAR> INLINE
  AbstractBayesNet<GUM_SCALAR>::AbstractBayesNet() :
    __propertiesMap ( 0 ) {
    GUM_CONSTRUCTOR ( AbstractBayesNet );
  }

  template <typename GUM_SCALAR> INLINE
  AbstractBayesNet<GUM_SCALAR>::AbstractBayesNet ( const AbstractBayesNet<GUM_SCALAR>& from ) :
    __propertiesMap ( 0 ) {
    GUM_CONS_CPY ( AbstractBayesNet );

    if ( from.__propertiesMap != 0 ) {
      __propertiesMap = new HashTable<std::string, std::string> ( * ( from.__propertiesMap ) );
    }
  }

  template <typename GUM_SCALAR> INLINE
  AbstractBayesNet<GUM_SCALAR>::~AbstractBayesNet() {
    GUM_DESTRUCTOR ( AbstractBayesNet );
    // Removing previous properties

    if ( __propertiesMap != 0 ) {
      delete __propertiesMap;
    }

  }

  template <typename GUM_SCALAR>
  AbstractBayesNet<GUM_SCALAR>&
  AbstractBayesNet<GUM_SCALAR>::operator= ( const AbstractBayesNet<GUM_SCALAR>& source ) {
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

  template<typename GUM_SCALAR> INLINE
  const std::string&
  AbstractBayesNet<GUM_SCALAR>::property ( const std::string& name ) const {
    try {
      return ( __properties() ) [name];
    } catch ( NotFound& ) {
      std::string msg = "The following property does not exists: ";
      GUM_ERROR ( NotFound, msg + name );
    }
  }

  template<typename GUM_SCALAR> INLINE
  void
  AbstractBayesNet<GUM_SCALAR>::setProperty ( const std::string& name, const std::string& value ) {
    try {
      __properties() [name] = value;
    } catch ( NotFound& ) {
      __properties().insert ( name, value );
    }
  }

  template<typename GUM_SCALAR> INLINE
  const DAG::NodeIterator
  AbstractBayesNet<GUM_SCALAR>::beginNodes() const {
    return dag().beginNodes();
  }

  template<typename GUM_SCALAR> INLINE
  const DAG::NodeIterator
  AbstractBayesNet<GUM_SCALAR>::endNodes() const {
    return dag().endNodes();
  }

  template<typename GUM_SCALAR> INLINE
  const DAG::ArcIterator
  AbstractBayesNet<GUM_SCALAR>::beginArcs() const {
    return dag().beginArcs();
  }

  template<typename GUM_SCALAR> INLINE
  const DAG::ArcIterator&
  AbstractBayesNet<GUM_SCALAR>::endArcs() const {
    return dag().endArcs();
  }

  template <typename GUM_SCALAR>
  void
  AbstractBayesNet<GUM_SCALAR>::_moralGraph ( UndiGraph& graph ) const {
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

  template <typename GUM_SCALAR>
  void
  AbstractBayesNet<GUM_SCALAR>::_topologicalOrder ( Sequence<NodeId>& topo ) const {
    DAG dag = this->dag();
    std::vector<NodeId> roots;

    for ( DAG::NodeIterator n = dag.beginNodes(); n != dag.endNodes(); ++n )
      if ( dag.parents ( *n ).empty() )
        roots.push_back ( *n );

    while ( roots.size() ) {
      topo.insert ( roots.back() );
      roots.pop_back();

      while ( dag.children ( topo.back() ).size() ) {
        NodeId child = * ( dag.children ( topo.back() ).begin() );
        dag.eraseArc ( Arc ( topo.back(), child ) );

        if ( dag.parents ( child ).empty() )
          roots.push_back ( child );
      }
    }

    GUM_ASSERT ( dag.sizeArcs() == 0 );

    GUM_ASSERT ( topo.size() == dag.size() );
  }

  template<typename GUM_SCALAR> INLINE
  HashTable<std::string, std::string>&
  AbstractBayesNet<GUM_SCALAR>::__properties() const {
    if ( __propertiesMap == 0 ) {
      __propertiesMap = new HashTable<std::string, std::string>();
    }

    return *__propertiesMap;
  }

  template<typename GUM_SCALAR> INLINE
  double
  AbstractBayesNet<GUM_SCALAR>::log10DomainSize ( void ) const {
    double dSize = 0.0;

    for ( DAG::NodeIterator it = beginNodes(); it != endNodes(); ++it ) {
      dSize += log10 ( variable ( *it ).domainSize() );
    }

    return dSize;
  }

  template<typename GUM_SCALAR> INLINE
  std::string
  AbstractBayesNet<GUM_SCALAR>::toString ( void ) const {
    Size param = 0;

    double dSize=log10DomainSize();

    for ( DAG::NodeIterator it = beginNodes(); it != endNodes(); ++it ) {
      param += ( ( const MultiDimImplementation<GUM_SCALAR> & ) cpt ( *it ).getMasterRef() ).realSize();
    }

    double compressionRatio = log10 ( 1.0*param )-dSize;

    std::stringstream s;
    s << "BN{nodes: " << size() << ", arcs: " << dag().sizeArcs() << ", ";

    if ( dSize>6 )
      s<<"domainSize: 10^" << dSize;
    else
      s<<"domainSize: " << round ( pow ( 10.0,dSize ) );

    s<< ", parameters: " << param << ", compression ratio: ";

    if ( compressionRatio>-3 )
      s<<trunc ( 100.0-pow ( 10.0,compressionRatio+2.0 ) );
    else
      s<<"100-10^" << compressionRatio+2.0;

    s<< "% }";

    return s.str();
  }

  template <typename GUM_SCALAR>
  bool
  AbstractBayesNet<GUM_SCALAR>::operator== ( const AbstractBayesNet<GUM_SCALAR>& from ) const {
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

          if ( std::pow ( cpt ( *node ).get ( i ) - from.cpt ( *node ).get ( j ), ( GUM_SCALAR ) 2 ) > ( GUM_SCALAR ) 1e-6 ) {
            return false;
          }
        }
      }

      return true;
    }

    return false;
  }

  template <typename GUM_SCALAR>
  bool
  AbstractBayesNet<GUM_SCALAR>::operator!= ( const AbstractBayesNet<GUM_SCALAR>& from ) const {
    return not this->operator== ( from );
  }

  // we are certain that Iq and Ip consist of variables with the same names and with the same labels.
  // But the order may be different ... :(
  template<typename GUM_SCALAR>
  void
  AbstractBayesNet<GUM_SCALAR>::synchroInstantiations ( Instantiation& inst,const Instantiation& external,bool sameLabelsOrder ) const {
    for ( Idx i=0; i<external.nbrDim(); i++ ) {
      const std::string& v_name=external.variable ( i ).name();
      const std::string& v_label=external.variable ( i ).label ( external.val ( i ) );
      const DiscreteVariable& vq=variableFromName ( v_name );
      inst.chgVal ( vq,vq[v_label] );
    }
  }

  template<typename GUM_SCALAR>
  void
  AbstractBayesNet<GUM_SCALAR>::completeInstantiation ( Instantiation& I ) const {
    I.clear();

    for ( DAG::NodeIterator node_iter = dag().beginNodes(); node_iter != dag().endNodes(); ++node_iter )
      I << variable ( *node_iter );
  }


  template<typename GUM_SCALAR>
  void
  AbstractBayesNet<GUM_SCALAR>::generateCPTs() {
    DefaultCPTGenerator<GUM_SCALAR> generator;

    for ( DAG::NodeIterator iter = beginNodes(); iter != endNodes(); ++iter ) {
      generator.generateCPT ( cpt ( *iter ).pos ( variable ( *iter ) ),  cpt ( *iter ) );
    }
  }

} /* namespace gum */

// ============================================================================
// kate: indent-mode cstyle; indent-width 1; replace-tabs on; ;
