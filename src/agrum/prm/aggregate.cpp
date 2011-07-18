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
 * @brief Source implementation of gum::Aggregate
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
#include <agrum/prm/aggregate.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/aggregate.inl>
#endif //GUM_NO_INLINE
// ============================================================================
namespace gum {
  namespace prm {

    Aggregate::Aggregate( const std::string& name, AggregateType aggType, const Type& rvType ):
        ClassElement( name ), __agg_type( aggType ),  __type( new Type( rvType ) ),
        __label( rvType.variable().domainSize() + 1 ) {
      GUM_CONSTRUCTOR( Aggregate );
      _safeName = ClassElement::LEFT_CAST() + __type->name() + ClassElement::RIGHT_CAST() + name;
    }

    Aggregate::Aggregate( const std::string& name, AggregateType aggType, const Type& rvType, Idx label ):
        ClassElement( name ), __agg_type( aggType ), __type( new Type( rvType ) ),
        __label( label ) {
      GUM_CONSTRUCTOR( Aggregate );
      _safeName = ClassElement::LEFT_CAST() + __type->name() + ClassElement::RIGHT_CAST() + name;
    }

    Aggregate::~Aggregate() {
      GUM_DESTRUCTOR( Aggregate );
      delete __type;
    }

    Aggregate::Aggregate( const Aggregate& source ):
        ClassElement( source ), __agg_type( source.__agg_type ), __type( new Type( source.type() ) ),
        __label( source.__label ) {
      GUM_CONS_CPY( Aggregate );
      GUM_ERROR( FatalError, "illegal call to gum::Aggregate copy constructor." );
    }

    Aggregate&
    Aggregate::operator=( const Aggregate& source ) {
      GUM_ERROR( FatalError, "illegal call to gum::Aggregate copy operator." );
    }

// ============================================================================
  } /* namespace prm */
} /* namespace gum */
// ============================================================================
