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
 * @brief Implementation of gum::PRMParameter.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    PRMParameter<GUM_SCALAR>::PRMParameter( const std::string& name,
                                            ParameterType      type,
                                            GUM_SCALAR         value )
        : PRMClassElement<GUM_SCALAR>( name )
        , __type( type )
        , __value( value ) {
      GUM_CONSTRUCTOR( PRMParameter );
      this->_safeName = name;
    }

    template <typename GUM_SCALAR>
    PRMParameter<GUM_SCALAR>::PRMParameter(
        const PRMParameter<GUM_SCALAR>& source )
        : PRMClassElement<GUM_SCALAR>( source.name() )
        , __type( source.valueType() )
        , __value( source.value() ) {
      GUM_ERROR( OperationNotAllowed, "cannot copy Paramter<GUM_SCALAR" );
    }

    template <typename GUM_SCALAR>
    PRMParameter<GUM_SCALAR>& PRMParameter<GUM_SCALAR>::
    operator=( const PRMParameter<GUM_SCALAR>& from ) {
      GUM_ERROR( OperationNotAllowed, "cannot copy Paramter<GUM_SCALAR" );
    }

    template <typename GUM_SCALAR>
    PRMParameter<GUM_SCALAR>::~PRMParameter() {
      GUM_DESTRUCTOR( PRMParameter );
    }

    template <typename GUM_SCALAR>
    typename PRMClassElement<GUM_SCALAR>::ClassElementType
    PRMParameter<GUM_SCALAR>::elt_type() const {
      return PRMClassElement<GUM_SCALAR>::ClassElementType::prm_parameter;
    }

    template <typename GUM_SCALAR>
    GUM_SCALAR PRMParameter<GUM_SCALAR>::value() const {
      return __value;
    }

    template <typename GUM_SCALAR>
    void PRMParameter<GUM_SCALAR>::value( GUM_SCALAR value ) {
      __value = value;
    }

    template <typename GUM_SCALAR>
    typename PRMParameter<GUM_SCALAR>::ParameterType
    PRMParameter<GUM_SCALAR>::valueType() const {
      return __type;
    }

    template <typename GUM_SCALAR>
    PRMType<GUM_SCALAR>& PRMParameter<GUM_SCALAR>::type() {
      GUM_ERROR( OperationNotAllowed, "Parameters do not have a type" );
    }

    template <typename GUM_SCALAR>
    const PRMType<GUM_SCALAR>& PRMParameter<GUM_SCALAR>::type() const {
      GUM_ERROR( OperationNotAllowed, "Parameters do not have a type" );
    }

    template <typename GUM_SCALAR>
    Potential<GUM_SCALAR>& PRMParameter<GUM_SCALAR>::cpf() {
      GUM_ERROR( OperationNotAllowed, "Parameters do not have a cpf" );
    }

    template <typename GUM_SCALAR>
    const Potential<GUM_SCALAR>& PRMParameter<GUM_SCALAR>::cpf() const {
      GUM_ERROR( OperationNotAllowed, "Parameters do not have a cpf" );
    }

    template <typename GUM_SCALAR>
    void PRMParameter<GUM_SCALAR>::addParent(
        const gum::prm::PRMClassElement<GUM_SCALAR>& ) {
      GUM_ERROR( OperationNotAllowed, "Parameters can not have parents" );
    }

    template <typename GUM_SCALAR>
    void PRMParameter<GUM_SCALAR>::addChild(
        const gum::prm::PRMClassElement<GUM_SCALAR>& ) {
      GUM_ERROR( OperationNotAllowed, "Parameters can not have children" );
    }

    template <typename GUM_SCALAR>
    PRMAttribute<GUM_SCALAR>* PRMParameter<GUM_SCALAR>::getCastDescendant() const {
      GUM_ERROR( OperationNotAllowed, "Parameters can have cast descendants" );
    }

  } /* namespace prm */
}  // namespace gum
