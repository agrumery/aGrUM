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
 * @brief Inline implementation of gum::Attribute
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

namespace gum {
  namespace prm {

    INLINE
    ClassElement::ClassElementType
    Attribute::elt_type() const { return prm_attribute; }

    INLINE
    Type&
    Attribute::type() { return *__type; }

    INLINE
    const Type&
    Attribute::type() const { return *__type; }

    INLINE
    Potential<prm_float>&
    Attribute::cpf() { return *__cpf; }

    INLINE
    const Potential<prm_float>&
    Attribute::cpf() const { return *__cpf; }

    INLINE
    void
    Attribute::addParent( const ClassElement& elt ) {
      try {
        __cpf->add( elt.type().variable() );
      } catch ( DuplicateElement& ) {
        std::stringstream msg;
        msg << ": " << elt.name() << " as parent of " << name();
        GUM_ERROR( DuplicateElement, msg.str() );
      } catch ( OperationNotAllowed& ) {
        std::stringstream msg;
        msg << ": " << elt.name() << " of wrong type as parent of " << name();
        GUM_ERROR( OperationNotAllowed, msg.str() );
      }
    }

// See gum::ClassElement::_addChild().
    INLINE
    void
    Attribute::addChild( const ClassElement& elt ) { }

    INLINE
    FuncAttribute::FuncAttribute( const std::string& name,
                                  const Type& type,
                                  MultiDimImplementation<prm_float>* impl ):
      Attribute( name, type, impl ) {
      GUM_CONSTRUCTOR( FuncAttribute );
    }

    INLINE
    FuncAttribute::FuncAttribute( const std::string& name,
                                  Type* type, Potential<prm_float>* cpf,
                                  bool delete_type ):
      Attribute( name, type, cpf, delete_type ) {
      GUM_CONSTRUCTOR( FuncAttribute );
    }

    INLINE
    FuncAttribute::~FuncAttribute() {
      GUM_DESTRUCTOR( FuncAttribute );
    }

    INLINE
    void
    FuncAttribute::addParent( const ClassElement& elt ) { }

    INLINE
    void
    FuncAttribute::addChild( const ClassElement& elt ) { }

    INLINE
    FuncAttribute::FuncAttribute( const FuncAttribute& source ):
      Attribute( source ) {
      GUM_CONS_CPY( FuncAttribute );
      GUM_ERROR( FatalError, "illegal call to copy constructor" );
    }

    INLINE
    FuncAttribute&
    FuncAttribute::operator=( const FuncAttribute& from ) {
      GUM_ERROR( FatalError, "illegal call to copy operator" );
    }


  } /* namespace prm */
} /* namespace gum */

