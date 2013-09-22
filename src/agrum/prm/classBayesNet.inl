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
 * @brief Inline implementation of ClassBayesNet.
 *
 * @author Lionel TORTI
 */
#include <agrum/core/inline.h>
#include <agrum/prm/classBayesNet.h>// to ease IDE parser

namespace gum {
  namespace prm {

    INLINE
    ClassBayesNet::ClassBayesNet( const Class& c ):
      BayesNet<prm_float>(), __class( &c ) {
      GUM_CONSTRUCTOR( ClassBayesNet );
      __init( c );
    }

    INLINE
    ClassBayesNet::ClassBayesNet( const ClassBayesNet& from ):
      BayesNet<prm_float>( from ), __class( from.__class ) {
      GUM_CONS_CPY( ClassBayesNet );
    }

    INLINE
    ClassBayesNet::~ClassBayesNet() {
      GUM_DESTRUCTOR( ClassBayesNet );
    }

    INLINE
    ClassBayesNet&
    ClassBayesNet::operator=( const ClassBayesNet& from ) {
      if ( this != &from ) {
        BayesNet<prm_float>::operator=( from );

        __class = from.__class;
      }

      return *this;
    }

    INLINE
    const Potential<prm_float>&
    ClassBayesNet::cpt( NodeId varId ) const {
      return __get( varId ).cpf();
    }

    INLINE
    const VariableNodeMap&
    ClassBayesNet::variableNodeMap() const {
      GUM_ERROR( FatalError, "Sorry no VarMap in a ClassBayesNet." );
    }


    INLINE
    const DiscreteVariable&
    ClassBayesNet::variable( NodeId id ) const {
      return __get( id ).type().variable();
    }

    INLINE
    NodeId
    ClassBayesNet::nodeId( const DiscreteVariable& var ) const {
      return __varNodeMap[&var]->id();
    }

    INLINE
    NodeId
    ClassBayesNet::idFromName( const std::string& name ) const {
      return __get( name ).id();
    }

    INLINE
    const DiscreteVariable&
    ClassBayesNet::variableFromName( const std::string& name ) const {
      return __get( name ).type().variable();
    }


    INLINE
    const ClassElement&
    ClassBayesNet::__get( NodeId id ) const {
      if ( _dag.exists( id ) ) {
        return __class->get( id );
      } else {
        GUM_ERROR( NotFound, "no element found with that id." );
      }
    }

    INLINE
    const ClassElement&
    ClassBayesNet::__get( const std::string& name ) const {
      try {
        return __class->get( name );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no element found with that id." );
      }
    }

    INLINE
    const Property<unsigned int>::onNodes&
    ClassBayesNet::modalities() const {
      if ( __modalities.empty() ) {
        for ( DAG::NodeIterator node = dag().beginNodes(); node != dag().endNodes(); ++node ) {
          __modalities.insert( *node, ( unsigned int ) variable( *node ).domainSize() );
        }
      }

      return __modalities;
    }

    INLINE
    std::string
    ClassBayesNet::toDot() const {
      std::string tab = "  ";
      std::stringstream output;
      output << "digraph \"";
      output << __class->name() << "\" {" << std::endl;

      for ( DAG::NodeIterator node_iter = dag().beginNodes(); node_iter != dag().endNodes(); ++node_iter ) {
        if ( dag().children( *node_iter ).size() > 0 ) {
          const NodeSet& children = dag().children( *node_iter );

          for ( NodeSetIterator arc_iter = children.begin();
                arc_iter != children.end(); ++arc_iter ) {
            output << tab << "\"" << variable( *node_iter ).name() << "\" -> ";
            output << "\"" << variable( *arc_iter ).name() << "\";" << std::endl;
          }
        } else if ( dag().parents( *node_iter ).size() == 0 ) {
          output << tab << "\"" << variable( *node_iter ).name() << "\";" << std::endl;
        }
      }

      output << "}" << std::endl;
      return output.str();
    }

  } /* namespace prm */
} /* namespace gum */

