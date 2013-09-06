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
 * @brief Inline implementation of InstanceBayesNet.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/prm/instanceBayesNet.h>// to ease IDE parser

namespace gum {
  namespace prm {

    template<typename GUM_SCALAR>
    INLINE
    InstanceBayesNet<GUM_SCALAR>::InstanceBayesNet( const Instance& i ):
      IBayesNet<GUM_SCALAR>(), __inst( &i ) {
      GUM_CONSTRUCTOR( InstanceBayesNet );
      __init( i );
    }

    template<typename GUM_SCALAR>
    INLINE
    InstanceBayesNet<GUM_SCALAR>::InstanceBayesNet( const InstanceBayesNet& from ):
      IBayesNet<GUM_SCALAR>( from ), __varNodeMap( from.__varNodeMap ),
      __inst( from.__inst ) {
      GUM_CONS_CPY( InstanceBayesNet );

    }

    template<typename GUM_SCALAR>
    INLINE
    InstanceBayesNet<GUM_SCALAR>::~InstanceBayesNet() {
      GUM_DESTRUCTOR( InstanceBayesNet );
    }

    template<typename GUM_SCALAR>
    INLINE
    InstanceBayesNet<GUM_SCALAR>&
    InstanceBayesNet<GUM_SCALAR>::operator=( const InstanceBayesNet& from ) {
      if ( this != &from ) {
        IBayesNet<GUM_SCALAR>::operator=( from );

        __varNodeMap = from.__varNodeMap;
      }

      return *this;
    }

    template<typename GUM_SCALAR>
    INLINE
    const Potential<GUM_SCALAR>&
    InstanceBayesNet<GUM_SCALAR>::cpt( NodeId varId ) const {
      return __get( varId ).cpf();
    }

    template<typename GUM_SCALAR>
    INLINE
    const VariableNodeMap&
    InstanceBayesNet<GUM_SCALAR>::variableNodeMap() const {
      GUM_ERROR( NotFound, "no VariableNodeMap in an InstanceBayesNet" );
    }

    template<typename GUM_SCALAR>
    INLINE
    const DiscreteVariable&
    InstanceBayesNet<GUM_SCALAR>::variable( NodeId id ) const {
      return __get( id ).type().variable();
    }

    template<typename GUM_SCALAR>
    INLINE
    NodeId
    InstanceBayesNet<GUM_SCALAR>::nodeId( const DiscreteVariable& var ) const {
      return __varNodeMap[&var]->id();
    }

    template<typename GUM_SCALAR>
    INLINE
    NodeId
    InstanceBayesNet<GUM_SCALAR>::idFromName( const std::string& name ) const {
      return __get( name ).id();
    }

    template<typename GUM_SCALAR>
    INLINE
    const DiscreteVariable&
    InstanceBayesNet<GUM_SCALAR>::variableFromName( const std::string& name ) const {
      return __get( name ).type().variable();
    }


    template<typename GUM_SCALAR>
    INLINE
    const ClassElement&
    InstanceBayesNet<GUM_SCALAR>::__get( NodeId id ) const {
      return __inst->get( id );
    }

    template<typename GUM_SCALAR>
    INLINE
    const ClassElement&
    InstanceBayesNet<GUM_SCALAR>::__get( const std::string& name ) const {
      try {
        return __inst->get( name );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no element found with that name" );
      }
    }

    template<typename GUM_SCALAR>
    INLINE
    const Property<unsigned int>::onNodes&
    InstanceBayesNet<GUM_SCALAR>::modalities() const {
      if ( __modalities.empty() ) {
        for ( DAG::NodeIterator node = this->dag().beginNodes(); node != this->dag().endNodes(); ++node ) {
          __modalities.insert( *node, ( unsigned int ) variable( *node ).domainSize() );
        }
      }

      return __modalities;
    }

    template<typename GUM_SCALAR>
    INLINE
    std::string
    InstanceBayesNet<GUM_SCALAR>::toDot() const {
      std::string tab = "  ";
      std::stringstream output;
      output << "digraph \"";
      output << __inst->name() << "\" {" << std::endl;

      for ( DAG::NodeIterator node_iter = this->dag().beginNodes(); node_iter != this->dag().endNodes(); ++node_iter ) {
        if ( this->dag().children( *node_iter ).size() > 0 ) {
          const NodeSet& children = this->dag().children( *node_iter );

          for ( NodeSetIterator arc_iter = children.begin();
                arc_iter != children.end(); ++arc_iter ) {
            output << tab << "\"" << variable( *node_iter ).name() << "\" -> ";
            output << "\"" << variable( *arc_iter ).name() << "\";" << std::endl;
          }
        } else if ( this->dag().parents( *node_iter ).size() == 0 ) {
          output << tab << "\"" << variable( *node_iter ).name() << "\";" << std::endl;
        }
      }

      output << "}" << std::endl;
      return output.str();
    }

  } /* namespace prm */
} /* namespace gum */

