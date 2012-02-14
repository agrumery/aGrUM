/**************************************************************************
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
 * @brief Inline implementation of gum::ClassElementContainer
 *
 * @author Lionel TORTI & PH Wuillemin
 */
// ============================================================================
#include <agrum/prm/classElementContainer.h>

namespace gum {

  namespace prm {
// ============================================================================

    INLINE
    ClassElementContainer::ClassElementContainer( const std::string& name ):
        PRMObject( name ) {
      GUM_CONSTRUCTOR( ClassElementContainer );
    }

    INLINE
    ClassElementContainer::~ClassElementContainer() {
      GUM_DESTRUCTOR( ClassElementContainer );
    }

    INLINE
    ClassElementContainer&
    ClassElementContainer::operator=( const ClassElementContainer& source ) {
      GUM_ERROR( FatalError, "illegal call to ClassElementContainer copy operator" );
    }

    INLINE
    ClassElementContainer::ClassElementContainer( const ClassElementContainer& source ):
        PRMObject( source ) {
      GUM_CONS_CPY( ClassElementContainer );
      GUM_ERROR( FatalError, "illegal call to ClassElementContainer copy constructor" );
    }

// INLINE
// Property<std::pair<bool, bool>*>::onNodes&
// ClassElementContainer::_IOFlags() { return _IOFlags; }
//
// INLINE
// const Property<std::pair<bool, bool>*>::onNodes&
// ClassElementContainer::_IOFlags() const { return _IOFlags; }
//
    INLINE
    bool
    ClassElementContainer::isInputNode( const ClassElement& elt ) const {
      try {
        return _getIOFlag( elt ).first;
      } catch ( NotFound& ) {
        return false;
      }
    }

    INLINE
    void
    ClassElementContainer::setInputNode( const ClassElement& elt, bool b ) {
      if ( not exists( elt.safeName() ) ) {
        GUM_ERROR( NotFound, ": <"+elt.safeName()+"> is not in <"+name()+">" );
      } else if ( ClassElement::isAttribute( elt ) or ClassElement::isAggregate( elt ) ) {
        try {
          _getIOFlag( elt ).second = b;
        } catch ( NotFound& ) {
          _setIOFlag( elt, std::make_pair( b, false ) );
        }
      } else {
        GUM_ERROR( WrongClassElement, "given id is not an Attribute or an Aggregate" );
      }
    }

    INLINE
    bool
    ClassElementContainer::isOutputNode( const ClassElement& elt ) const {
      try {
        return _getIOFlag( elt ).second;
      } catch ( NotFound& ) {
        return false;
      }
    }

    INLINE
    void
    ClassElementContainer::setOutputNode( const ClassElement& elt, bool b ) {
      if ( not exists( elt.safeName() ) ) {
        GUM_ERROR( NotFound, "<"+elt.safeName()+"> is not in <"+name()+">" );
      } else if ( ClassElement::isAttribute( elt ) or ClassElement::isAggregate( elt ) ) {
        try {
          _getIOFlag( elt ).second = b;
        } catch ( NotFound& ) {
          _setIOFlag( elt, std::make_pair( false, b ) );
        }

        if ( b ) {
          _updateDescendants( elt );
        }
      } else {
        GUM_ERROR( WrongClassElement, "given ClassElement is not an Attribute or an Aggregate" );
      }
    }

    INLINE
    bool
    ClassElementContainer::isInnerNode( const ClassElement& elt ) const {
      try {
        return not( _getIOFlag( elt ).first or _getIOFlag( elt ).second );
      } catch ( NotFound& ) {
        return true;
      }
    }

    INLINE
    bool
    ClassElementContainer::isSuperTypeOf( const ClassElementContainer& cec ) const {
      return cec.isSubTypeOf( *this );
    }

    INLINE
    std::pair<bool, bool>&
    ClassElementContainer::_getIOFlag( const ClassElement& elt ) {
      try {
        return __IOFlags[elt.safeName()];
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "this ClassElement does not have any IO flags" );
      }
    }

    INLINE
    const std::pair<bool, bool>&
    ClassElementContainer::_getIOFlag( const ClassElement& elt ) const {
      try {
        return __IOFlags[elt.safeName()];
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "this ClassElement does not have any IO flags" );
      }
    }

    INLINE
    void
    ClassElementContainer::_setIOFlag( const ClassElement& elt, const std::pair<bool, bool>& flags ) {
      try {
        __IOFlags[elt.safeName()] = flags;
      } catch ( NotFound& ) {
        __IOFlags.insert( elt.safeName(), flags );
      }
    }

    INLINE
    bool
    ClassElementContainer::exists( NodeId id ) const {
      return dag().exists( id );
    }

    INLINE
    bool
    ClassElementContainer::exists( const std::string& name ) const {
      try {
        get( name );
        return true;
      } catch ( NotFound& ) {
        return false;
      }
    }

    INLINE
    bool
    ClassElementContainer::belongsTo( const ClassElement& elt ) const {
      try {
        return &elt == &( get( elt.safeName() ) );
      } catch ( NotFound& ) {
        return false;
      }
    }

    INLINE
    const DAG&
    ClassElementContainer::dag() const {
      return _dag();
    }

// ============================================================================
  } /* namespace prm */
} /* namespace gum */
// ============================================================================
