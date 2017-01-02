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
 * @author Pierre-Henri WUILLEMIN and Lionel TORTI
 */

#include <limits>

#include <agrum/BN/IBayesNet.h>

#include <agrum/multidim/aggregators/and.h>
#include <agrum/multidim/aggregators/or.h>

#include <agrum/multidim/ICIModels/multiDimNoisyAND.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORNet.h>

#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/multidim/potential.h>

namespace gum {

  //                                  IBayesNet

  template <typename GUM_SCALAR>
  INLINE IBayesNet<GUM_SCALAR>::IBayesNet()
      : DAGmodel() {
    GUM_CONSTRUCTOR( IBayesNet );
  }

  template <typename GUM_SCALAR>
  INLINE IBayesNet<GUM_SCALAR>::IBayesNet( std::string name )
      : DAGmodel() {
    GUM_CONSTRUCTOR( IBayesNet );
    this->setProperty( "name", name );
  }

  template <typename GUM_SCALAR>
  IBayesNet<GUM_SCALAR>::IBayesNet( const IBayesNet<GUM_SCALAR>& source )
      : DAGmodel( source ) {
    GUM_CONS_CPY( IBayesNet );
  }

  template <typename GUM_SCALAR>
  IBayesNet<GUM_SCALAR>& IBayesNet<GUM_SCALAR>::
  operator=( const IBayesNet<GUM_SCALAR>& source ) {
    if ( this != &source ) {
      DAGmodel::operator=( source );
    }

    return *this;
  }

  template <typename GUM_SCALAR>
  IBayesNet<GUM_SCALAR>::~IBayesNet() {
    GUM_DESTRUCTOR( IBayesNet );
  }

  template <typename GUM_SCALAR>
  Size IBayesNet<GUM_SCALAR>::dim() const {
    Size dim = 0;

    for ( auto node : nodes() ) {
      Size q = 1;

      for ( auto parent : dag().parents( node ) )
        q *= variable( parent ).domainSize();

      dim += ( variable( node ).domainSize() - 1 ) * q;
    }

    return dim;
  }

  template <typename GUM_SCALAR>
  Size IBayesNet<GUM_SCALAR>::maxVarDomainSize() const {
    Size res = 0;
    for ( auto node : nodes() ) {
      auto v = variable( node ).domainSize();
      if ( v > res ) {
        res = v;
      }
    }
    return res;
  }

  template <typename GUM_SCALAR>
  GUM_SCALAR IBayesNet<GUM_SCALAR>::minParam() const {
    GUM_SCALAR res = 1.0;
    for ( auto node : nodes() ) {
      auto v = cpt( node ).min();
      if ( v < res ) {
        res = v;
      }
    }
    return res;
  }

  template <typename GUM_SCALAR>
  GUM_SCALAR IBayesNet<GUM_SCALAR>::maxParam() const {
    GUM_SCALAR res = 1.0;
    for ( auto node : nodes() ) {
      auto v = cpt( node ).max();
      if ( v > res ) {
        res = v;
      }
    }
    return res;
  }

  template <typename GUM_SCALAR>
  GUM_SCALAR IBayesNet<GUM_SCALAR>::minNonZeroParam() const {
    GUM_SCALAR res = 1.0;
    for ( auto node : nodes() ) {
      auto v = cpt( node ).minNonZero();
      if ( v < res ) {
        res = v;
      }
    }
    return res;
  }

  template <typename GUM_SCALAR>
  GUM_SCALAR IBayesNet<GUM_SCALAR>::maxNonOneParam() const {
    GUM_SCALAR res = 0.0;
    for ( auto node : nodes() ) {
      auto v = cpt( node ).maxNonOne();
      if ( v > res ) {
        res = v;
      }
    }
    return res;
  }

  template <typename GUM_SCALAR>
  INLINE std::string IBayesNet<GUM_SCALAR>::toString( void ) const {
    Size   param = 0;
    double dSize = log10DomainSize();

    for ( auto node : nodes() )
      param += cpt( node ).content()->realSize();

    double compressionRatio = log10( 1.0 * param ) - dSize;

    std::stringstream s;
    s << "BN{nodes: " << size() << ", arcs: " << dag().sizeArcs() << ", ";

    if ( dSize > 6 )
      s << "domainSize: 10^" << dSize;
    else
      s << "domainSize: " << std::round( std::pow( 10.0, dSize ) );

    s << ", parameters: " << param << ", compression ratio: ";

    if ( compressionRatio > -3 )
      s << trunc( 100.0 - std::pow( 10.0, compressionRatio + 2.0 ) );
    else
      s << "100-10^" << compressionRatio + 2.0;

    s << "% }";

    return s.str();
  }

  template <typename GUM_SCALAR>
  std::string IBayesNet<GUM_SCALAR>::toDot( void ) const {
    std::stringstream output;
    output << "digraph \"";

    std::string bn_name;

    try {
      bn_name = this->property( "name" );
    } catch ( NotFound& ) {
      bn_name = "no_name";
    }

    output << bn_name << "\" {" << std::endl;
    output << "  graph [bgcolor=transparent,label=\"" << bn_name << "\"];"
           << std::endl;
    output << "  node [style=filled fillcolor=\"#ffffaa\"];" << std::endl
           << std::endl;

    for ( auto node : nodes() )
      output << "\"" << variable( node ).name() << "\" [comment=\"" << node << ":"
             << variable( node ).toStringWithDescription() << "\"];" << std::endl;

    output << std::endl;

    std::string tab = "  ";

    for ( auto node : nodes() ) {
      if ( dag().children( node ).size() > 0 ) {
        for ( auto child : dag().children( node ) ) {
          output << tab << "\"" << variable( node ).name() << "\" -> "
                 << "\"" << variable( child ).name() << "\";" << std::endl;
        }
      } else if ( dag().parents( node ).size() == 0 ) {
        output << tab << "\"" << variable( node ).name() << "\";" << std::endl;
      }
    }

    output << "}" << std::endl;

    return output.str();
  }

  /// Compute a parameter of the joint probability for the BN (given an
  /// instantiation
  /// of the vars)
  template <typename GUM_SCALAR>
  GUM_SCALAR
  IBayesNet<GUM_SCALAR>::jointProbability( const Instantiation& i ) const {
    GUM_SCALAR value = (GUM_SCALAR)1.0;

    GUM_SCALAR tmp;

    for ( auto node : nodes() ) {
      if ( ( tmp = cpt( node )[i] ) == (GUM_SCALAR)0 ) {
        return (GUM_SCALAR)0;
      }

      value *= tmp;
    }

    return value;
  }

  /// Compute a parameter of the joint probability for the BN (given an
  /// instantiation
  /// of the vars)
  template <typename GUM_SCALAR>
  GUM_SCALAR
  IBayesNet<GUM_SCALAR>::log2JointProbability( const Instantiation& i ) const {
    GUM_SCALAR value = (GUM_SCALAR)0.0;

    GUM_SCALAR tmp;

    for ( auto node : nodes() ) {
      if ( ( tmp = cpt( node )[i] ) == (GUM_SCALAR)0 ) {
        return ( GUM_SCALAR )( -std::numeric_limits<double>::infinity() );
      }

      value += log2( cpt( node )[i] );
    }

    return value;
  }

  template <typename GUM_SCALAR>
  bool IBayesNet<GUM_SCALAR>::operator==( const IBayesNet& from ) const {
    if ( size() != from.size() ) {
      return false;
    }

    if ( sizeArcs() != from.sizeArcs() ) {
      return false;
    }

    // alignment of variables between the 2 BNs
    Bijection<const DiscreteVariable*, const DiscreteVariable*> alignment;

    for ( auto node : nodes() ) {
      try {
        alignment.insert( &variable( node ),
                          &from.variableFromName( variable( node ).name() ) );
      } catch ( NotFound& ) {
        // a name is not found in from
        return false;
      }
    }

    for ( auto node : nodes() ) {
      NodeId fromnode = from.idFromName( variable( node ).name() );

      if ( cpt( node ).nbrDim() != from.cpt( fromnode ).nbrDim() ) {
        return false;
      }

      if ( cpt( node ).domainSize() != from.cpt( fromnode ).domainSize() ) {
        return false;
      }

      Instantiation i( cpt( node ) );
      Instantiation j( from.cpt( fromnode ) );

      for ( i.setFirst(); !i.end(); i.inc() ) {
        for ( Idx indice = 0; indice < cpt( node ).nbrDim(); ++indice ) {
          const DiscreteVariable* p = &( i.variable( indice ) );
          j.chgVal( *( alignment.second( p ) ), i.val( *p ) );
        }

        if ( std::pow( cpt( node ).get( i ) - from.cpt( fromnode ).get( j ),
                       (GUM_SCALAR)2 ) > (GUM_SCALAR)1e-6 ) {
          return false;
        }
      }
    }

    return true;
  }

  template <typename GUM_SCALAR>
  bool IBayesNet<GUM_SCALAR>::operator!=( const IBayesNet& from ) const {
    return !this->operator==( from );
  }

  // visit the nodes and add some of node from soids in minimal
  template <typename GUM_SCALAR>
  void IBayesNet<GUM_SCALAR>::__minimalCondSetVisitUp(
      NodeId         node,
      const NodeSet& soids,
      NodeSet&       minimal,
      NodeSet&       alreadyVisitedUp,
      NodeSet&       alreadyVisitedDn ) const {
    if ( alreadyVisitedUp.contains( node ) ) return;
    alreadyVisitedUp << node;

    if ( soids.contains( node ) ) {
      minimal << node;
    } else {
      for ( auto fath : _dag.parents( node ) )
        __minimalCondSetVisitUp(
            fath, soids, minimal, alreadyVisitedUp, alreadyVisitedDn );
      for ( auto chil : _dag.children( node ) )
        __minimalCondSetVisitDn(
            chil, soids, minimal, alreadyVisitedUp, alreadyVisitedDn );
    }
  }

  // visit the nodes and add some of node from soids in minimal
  template <typename GUM_SCALAR>
  void IBayesNet<GUM_SCALAR>::__minimalCondSetVisitDn(
      NodeId         node,
      const NodeSet& soids,
      NodeSet&       minimal,
      NodeSet&       alreadyVisitedUp,
      NodeSet&       alreadyVisitedDn ) const {
    if ( alreadyVisitedDn.contains( node ) ) return;
    alreadyVisitedDn << node;

    if ( soids.contains( node ) ) {
      minimal << node;
      for ( auto fath : _dag.parents( node ) )
        __minimalCondSetVisitUp(
            fath, soids, minimal, alreadyVisitedUp, alreadyVisitedDn );
    } else {
      for ( auto chil : _dag.children( node ) )
        __minimalCondSetVisitDn(
            chil, soids, minimal, alreadyVisitedUp, alreadyVisitedDn );
    }
  }


  template <typename GUM_SCALAR>
  NodeSet IBayesNet<GUM_SCALAR>::minimalCondSet( NodeId         target,
                                                 const NodeSet& soids ) const {
    if ( soids.contains( target ) ) return NodeSet( {target} );

    NodeSet res;
    NodeSet alreadyVisitedUp;
    NodeSet alreadyVisitedDn;
    alreadyVisitedDn << target;
    alreadyVisitedUp << target;

    for ( auto fath : _dag.parents( target ) )
      __minimalCondSetVisitUp(
          fath, soids, res, alreadyVisitedUp, alreadyVisitedDn );
    for ( auto chil : _dag.children( target ) )
      __minimalCondSetVisitDn(
          chil, soids, res, alreadyVisitedUp, alreadyVisitedDn );
    return res;
  }

  template <typename GUM_SCALAR>
  INLINE std::ostream& operator<<( std::ostream&                output,
                                   const IBayesNet<GUM_SCALAR>& bn ) {
    output << bn.toString();
    return output;
  }

} /* namespace gum */
