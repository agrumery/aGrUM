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
 * @brief Inline implementation of gum::ScalarAttribute
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <sstream>

#include <agrum/PRM/utils_prm.h>

#include <agrum/PRM/elements/attribute.h>
#include <agrum/PRM/elements/type.h>

namespace gum {
  namespace prm {

    template<typename GUM_SCALAR>
    ScalarAttribute<GUM_SCALAR>::ScalarAttribute ( const std::string& name,
                                                   const Type<GUM_SCALAR>& type,
                                                   MultiDimImplementation<GUM_SCALAR>* impl ) :
      Attribute<GUM_SCALAR> ( name ),
      __type ( new Type<GUM_SCALAR> ( type ) ),
      __cpf ( new Potential<GUM_SCALAR> ( impl ) ),
      __delete_type ( true ) 
    {
      std::stringstream str;
      str << "In ScalarAttribute(" << name << ", " << type.name() << ", " << impl << ")"; 
      GUM_TRACE( str.str() );
      GUM_CONSTRUCTOR ( ScalarAttribute );
      __cpf->add( type.variable() );

      this->_safeName = PRMObject::LEFT_CAST() + __type->name() + PRMObject::RIGHT_CAST() + name;
    }

    template<typename GUM_SCALAR>
    ScalarAttribute<GUM_SCALAR>::ScalarAttribute ( const std::string& name,
                                                   Type<GUM_SCALAR>* type,
                                                   Potential<GUM_SCALAR>* cpf,
                                                   bool delete_type ) :
      Attribute<GUM_SCALAR> ( name ),
      __type ( type ),
      __cpf ( cpf ),
      __delete_type ( delete_type )
    {
      GUM_CONSTRUCTOR ( ScalarAttribute );

      if ( not __cpf->variablesSequence().exists ( & ( type->variable() ) ) ) {
        auto msg = "the given Potential does not contain the type of this ScalarAttribute";
        GUM_ERROR ( OperationNotAllowed, msg );
      }

      this->_safeName = PRMObject::LEFT_CAST() + __type->name() + PRMObject::RIGHT_CAST() + name;
    }

    template<typename GUM_SCALAR>
    ScalarAttribute<GUM_SCALAR>::ScalarAttribute ( const ScalarAttribute<GUM_SCALAR>& source ) :
      Attribute<GUM_SCALAR> ( source ),
      __type (0),
      __cpf (0)
    {
      GUM_CONS_CPY ( ScalarAttribute );
      GUM_ERROR ( FatalError, "Illegal call to the copy constructor of gum::ScalarAttribute" );
    }

    template<typename GUM_SCALAR>
    ScalarAttribute<GUM_SCALAR>::~ScalarAttribute() {

      GUM_DESTRUCTOR ( ScalarAttribute );
      delete __cpf;

      if ( __delete_type ) {
        delete __type;
      }

    }

    template<typename GUM_SCALAR>
    Attribute<GUM_SCALAR>*
    ScalarAttribute<GUM_SCALAR>::newFactory() const {
      auto impl = static_cast<MultiDimImplementation<GUM_SCALAR>*>( this->cpf().content()->newFactory() );
      return new ScalarAttribute<GUM_SCALAR> ( this->name(), this->type(), impl );
    }

    template<typename GUM_SCALAR>
    Attribute<GUM_SCALAR>*
    ScalarAttribute<GUM_SCALAR>::copy( Bijection<const DiscreteVariable*, const DiscreteVariable*> bij) const
    {
      std::stringstream str;
      str << "In ScalarAttribute::copy(" << this->name() << ", " << bij.size() << ")";
      GUM_TRACE(str.str());

      auto copy = new ScalarAttribute<GUM_SCALAR> ( this->name(), this->type() );

      if ( not bij.existsFirst( & ( type().variable() ) ) ) {
          bij.insert( &(type().variable()), &(copy->type().variable()) );
      }

      delete copy->__cpf;
      copy->__cpf = copyPotential ( bij, cpf() );

      return copy;
    } 

    template<typename GUM_SCALAR>
    void
    ScalarAttribute<GUM_SCALAR>::copyCpf(const Bijection<const DiscreteVariable *, const DiscreteVariable *>& bij,
                                         const Attribute<GUM_SCALAR> & source) 
    {
      GUM_TRACE( "In copyCpf(" + this->_safeName + ")" );

      for ( const auto & var: __cpf->variablesSequence() ) {
        GUM_TRACE( var->name() );
      }
      for ( const auto & var: source.cpf().variablesSequence() ) {
        GUM_TRACE( var->name() );
      }

      delete __cpf;
      __cpf = copyPotential( bij, source.cpf() );

      GUM_TRACE( "Out copyCpf(" + this->_safeName + ")" );
    }

    template<typename GUM_SCALAR> ScalarAttribute<GUM_SCALAR>&
    ScalarAttribute<GUM_SCALAR>::operator= ( const ScalarAttribute<GUM_SCALAR>& from ) {
      GUM_ERROR ( FatalError, "Illegal call to the copy operator of gum::ScalarAttribute" );
    }

    template<typename GUM_SCALAR> INLINE
    void 
    ScalarAttribute<GUM_SCALAR>::_setCpf( Potential<GUM_SCALAR>* cpf) {
      delete __cpf;
      __cpf = cpf;
    }

    template<typename GUM_SCALAR> INLINE
    typename ClassElement<GUM_SCALAR>::ClassElementType
    ScalarAttribute<GUM_SCALAR>::elt_type() const { return this->prm_attribute; }

    template<typename GUM_SCALAR> INLINE
    Type<GUM_SCALAR>&
    ScalarAttribute<GUM_SCALAR>::type() { return *__type; }

    template<typename GUM_SCALAR> INLINE
    const Type<GUM_SCALAR>&
    ScalarAttribute<GUM_SCALAR>::type() const { return *__type; }

    template<typename GUM_SCALAR> INLINE
    Type<GUM_SCALAR>*
    ScalarAttribute<GUM_SCALAR>::type(Type<GUM_SCALAR>* type) {
      auto tmp = __type;
      __type = type;
      __cpf->erase( tmp->variable() );
      __cpf->add( __type->variable() );
      return tmp;
    }

    template<typename GUM_SCALAR> INLINE
    Potential<GUM_SCALAR>&
    ScalarAttribute<GUM_SCALAR>::cpf() { return *__cpf; }

    template<typename GUM_SCALAR> INLINE
    const Potential<GUM_SCALAR>&
    ScalarAttribute<GUM_SCALAR>::cpf() const { return *__cpf; }

    template<typename GUM_SCALAR> INLINE
    void
    ScalarAttribute<GUM_SCALAR>::addParent ( const ClassElement<GUM_SCALAR>& elt ) {
      try {
        __cpf->add( elt.type().variable() );
      } catch ( DuplicateElement& ) {
        std::stringstream msg;
        msg << ": " << elt.name() << " as parent of " << this->name();
        GUM_ERROR ( DuplicateElement, msg.str() );
      } catch ( OperationNotAllowed& ) {
        std::stringstream msg;
        msg << ": " << elt.name() << " of wrong type as parent of " << this->name();
        GUM_ERROR ( OperationNotAllowed, msg.str() );
      }
    }

    // See gum::ClassElement<GUM_SCALAR>::_addChild().
    template<typename GUM_SCALAR> INLINE
    void
    ScalarAttribute<GUM_SCALAR>::addChild ( const ClassElement<GUM_SCALAR>& elt ) { }

    template<typename GUM_SCALAR>
    Attribute<GUM_SCALAR>*
    ScalarAttribute<GUM_SCALAR>::getCastDescendant() const {
      ScalarAttribute<GUM_SCALAR>* cast = 0;

      try {
        cast = new ScalarAttribute<GUM_SCALAR> ( this->name(), type().super() );
      } catch ( NotFound& ) {
        GUM_ERROR ( OperationNotAllowed, "this ScalarAttribute can not have cast descendant" );
      }

      cast->addParent ( *this );
      const DiscreteVariable& my_var = type().variable();
      DiscreteVariable& cast_var = cast->type().variable();
      Instantiation inst ( cast->cpf() );

      for ( inst.setFirst(); not inst.end(); inst.inc() ) {
        if ( type().label_map() [inst.val ( my_var )] == inst.val ( cast_var ) ) {
          cast->cpf().set ( inst, 1 );
        } else {
          cast->cpf().set ( inst, 0 );
        }
      }

      return cast;
    }

    template<typename GUM_SCALAR>
    void
    ScalarAttribute<GUM_SCALAR>::setAsCastDescendant ( Attribute<GUM_SCALAR>* cast ) {
      try {
        cast->type().setSuper ( type() );
      } catch ( OperationNotAllowed& ) {
        GUM_ERROR ( OperationNotAllowed, "this ScalarAttribute can not have cast descendant" );
      } catch ( WrongType& ) {
        std::stringstream msg;
        msg << type().name() << " is not a subtype of " << cast->type().name();
        GUM_ERROR ( WrongType, msg.str() );
      }

      Potential<GUM_SCALAR>* cpf = new Potential<GUM_SCALAR>();
      cpf->add ( type().variable() );
      cpf->add ( cast->type().variable() );

      DiscreteVariable& my_var = cast->type().variable();
      DiscreteVariable& cast_var = type().variable();

      Instantiation inst ( *cpf );

      for ( inst.setFirst(); not inst.end(); inst.inc() ) {
        if ( type().label_map() [inst.pos ( my_var )] == inst.pos ( cast_var ) )
          cpf->set ( inst, 1 );
        else
          cpf->set ( inst, 0 );
      }

      delete __cpf;
      __cpf = cpf;
    }

    template<typename GUM_SCALAR>
    void
    ScalarAttribute<GUM_SCALAR>::swap(const Type<GUM_SCALAR>& old_type,
                                      const Type<GUM_SCALAR>& new_type) 
    {
      __cpf->swap( old_type.variable(), new_type.variable() );
    }

  } /* namespace prm */
} /* namespace gum */


