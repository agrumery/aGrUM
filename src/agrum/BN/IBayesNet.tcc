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

#include <agrum/multidim/aggregators/or.h>
#include <agrum/multidim/aggregators/and.h>

#include <agrum/multidim/CIModels/multiDimNoisyAND.h>
#include <agrum/multidim/CIModels/multiDimNoisyORNet.h>
#include <agrum/multidim/CIModels/multiDimNoisyORCompound.h>

#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/multidim/potential.h>


namespace gum {

//                                  IBayesNet


  template<typename GUM_SCALAR> INLINE
  IBayesNet<GUM_SCALAR>::IBayesNet() :
    DAGmodel() {
    GUM_CONSTRUCTOR ( IBayesNet );
  }

  template<typename GUM_SCALAR> INLINE
  IBayesNet<GUM_SCALAR>::IBayesNet ( std::string name ) :
    DAGmodel() {
    GUM_CONSTRUCTOR ( IBayesNet );
    this->setProperty ( "name", name );
  }

  template<typename GUM_SCALAR>
  IBayesNet<GUM_SCALAR>::IBayesNet ( const IBayesNet<GUM_SCALAR>& source ) :
    DAGmodel ( source ) {
    GUM_CONS_CPY ( IBayesNet );
  }

  template<typename GUM_SCALAR>
  IBayesNet<GUM_SCALAR>&
  IBayesNet<GUM_SCALAR>::operator= ( const IBayesNet<GUM_SCALAR>& source ) {
    if ( this != &source ) {
      DAGmodel::operator= ( source );
    }

    return *this;
  }

  template<typename GUM_SCALAR>
  IBayesNet<GUM_SCALAR>::~IBayesNet() {
    GUM_DESTRUCTOR ( IBayesNet );
  }

  template<typename GUM_SCALAR>
  Idx
  IBayesNet<GUM_SCALAR>::dim() const {
    Idx dim = 0;

    for ( const auto node : nodes() ) {
      Idx q = 1;

      for ( const auto parent : dag().parents ( node ) )
        q *= variable ( parent ).domainSize();

      dim += ( variable ( node ).domainSize() - 1 ) * q;
    }

    return dim;
  }


  template<typename GUM_SCALAR> INLINE
  std::string
  IBayesNet<GUM_SCALAR>::toString ( void ) const {
    Size param = 0;
    double dSize = log10DomainSize();

    for ( const auto it : nodes() ) {
      param += ( ( const MultiDimImplementation<GUM_SCALAR>& ) cpt ( it ).getMasterRef() ).realSize();
    }

    double compressionRatio = log10 ( 1.0 * param ) - dSize;

    std::stringstream s;
    s << "BN{nodes: " << size() << ", arcs: " << dag().sizeArcs() << ", ";

    if ( dSize > 6 )
      s << "domainSize: 10^" << dSize;
    else
      s << "domainSize: " << round ( pow ( 10.0, dSize ) );

    s << ", parameters: " << param << ", compression ratio: ";

    if ( compressionRatio > -3 )
      s << trunc ( 100.0 - pow ( 10.0, compressionRatio + 2.0 ) );
    else
      s << "100-10^" << compressionRatio + 2.0;

    s << "% }";

    return s.str();
  }

  template<typename GUM_SCALAR>
  std::string
  IBayesNet<GUM_SCALAR>::toDot ( void ) const {
    std::stringstream output;
    output << "digraph \"";

    std::string bn_name;

    try {
      bn_name = this->property ( "name" );
    } catch ( NotFound& ) {
      bn_name = "no_name";
    }


    output << bn_name << "\" {" << std::endl;
    output << "  graph [bgcolor=transparent,label=\"" << bn_name << "\"];" << std::endl;
    output << "  node [style=filled fillcolor=\"#ffffaa\"];" << std::endl << std::endl;

    for ( const auto node_iter : nodes() ) {
      output << "\"" << variable ( node_iter ).name() << "\" [comment=\"" << node_iter << ":" << variable ( node_iter ) << "\"];" << std::endl;
    }

    output << std::endl;

    std::string tab = "  ";

    for ( const auto node_iter : nodes() ) {
      if ( dag().children ( node_iter ).size() > 0 ) {
        //const NodeSet& children =  dag().children ( node_iter );

        for ( const auto child_iter : dag().children ( node_iter ) ) {
          output << tab << "\"" << variable ( node_iter ).name() << "\" -> "
                 << "\"" << variable ( child_iter ).name() << "\";" << std::endl;
        }
      } else if ( dag().parents ( node_iter ).size() == 0 ) {
        output << tab << "\"" << variable ( node_iter ).name() << "\";" << std::endl;
      }
    }

    output << "}" << std::endl;

    return output.str();
  }


  /// Compute a parameter of the joint probability for the BN (given an instantiation of the vars)
  template<typename GUM_SCALAR>
  GUM_SCALAR IBayesNet<GUM_SCALAR>::jointProbability ( const Instantiation& i ) const {
    GUM_SCALAR value = ( GUM_SCALAR ) 1.0;

    GUM_SCALAR tmp;

    for ( const auto node_iter : nodes() ) {
      if ( ( tmp = cpt ( node_iter ) [i] ) == ( GUM_SCALAR ) 0 ) {
        return ( GUM_SCALAR ) 0;
      }

      value *= tmp;
    }

    return value;
  }

  /// Compute a parameter of the joint probability for the BN (given an instantiation of the vars)
  template<typename GUM_SCALAR>
  GUM_SCALAR IBayesNet<GUM_SCALAR>::log2JointProbability ( const Instantiation& i ) const {
    GUM_SCALAR value = ( GUM_SCALAR ) 0.0;

    GUM_SCALAR tmp;

    for ( const auto node_iter : nodes() ) {
      if ( ( tmp = cpt ( node_iter ) [i] ) == ( GUM_SCALAR ) 0 ) {
        return ( GUM_SCALAR ) ( - std::numeric_limits<double>::infinity( ) );
      }

      value += log2 ( cpt ( node_iter ) [i] );
    }

    return value;
  }

  template <typename GUM_SCALAR>
  bool
  IBayesNet<GUM_SCALAR>::operator== ( const IBayesNet& from ) const {
    if ( dag() == from.dag() ) {
      for ( const auto node : nodes() ) {
        // We don't use Potential::operator== because BN's don't share
        // DiscreteVariable's pointers.
        Bijection<const DiscreteVariable*, const DiscreteVariable*> bijection;
        bijection.insert ( & ( variable ( node ) ), & ( from.variable ( node ) ) );

        for ( const auto arc : dag().parents ( node ) ) {
          bijection.insert ( & ( variable ( arc ) ), & ( from.variable ( arc ) ) );
        }

        Instantiation i ( cpt ( node ) );
        Instantiation j ( from.cpt ( node ) );

        for ( i.setFirst(); not i.end(); i.inc() ) {
          for ( auto iter = bijection.begin(); iter != bijection.end(); ++iter ) {
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
  IBayesNet<GUM_SCALAR>::operator!= ( const IBayesNet& from ) const {
    return not this->operator== ( from );
  }


  template<typename GUM_SCALAR> INLINE
  std::ostream&
  operator<< ( std::ostream& output, const IBayesNet<GUM_SCALAR>& bn ) {
    output << bn.toString();
    return output;
  }

} /* namespace gum */
