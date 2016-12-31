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
 * @brief Headers of gum::PRMParameter.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_PARAMETER_H
#define GUM_PARAMETER_H

#include <agrum/PRM/elements/PRMClassElement.h>

namespace gum {
  namespace prm {

    /**
     * @class PRMParameter parameter.h <agrum/PRM/parameter.h>
     * @brief PRMParameter is a member of a Class in a PRM.
     *
     * @see PRM PRMFactory Class PRMClassElement
     * @ingroup prm_group
     */
    template <typename GUM_SCALAR>
    class PRMParameter : public PRMClassElement<GUM_SCALAR> {
      public:
      enum ParameterType { INT, REAL };

      // ========================================================================
      /// @name Constructor & destructor.
      // ========================================================================
      /// @{

      /**
       * @brief Constructor used by gum::Class.
       *
       * @param name
       * @param type
       * @param value
       */
      PRMParameter( const std::string& name,
                    ParameterType      type,
                    GUM_SCALAR         value );

      /// Destructor.
      virtual ~PRMParameter();

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      /// See gum::PRMClassElement::elt_type().
      virtual typename PRMClassElement<GUM_SCALAR>::ClassElementType
      elt_type() const;

      GUM_SCALAR value() const;

      void value( GUM_SCALAR value );

      ParameterType valueType() const;

      /// See gum::PRMClassElement::type().
      virtual PRMType<GUM_SCALAR>& type();

      /// See gum::PRMClassElement::type().
      virtual const PRMType<GUM_SCALAR>& type() const;

      /// See gum::PRMClassElement::cpf().
      virtual Potential<GUM_SCALAR>& cpf();

      /// See gum::PRMClassElement::cpf().
      virtual const Potential<GUM_SCALAR>& cpf() const;

      /// See gum::PRMClassElement::addParent().
      void addParent( const gum::prm::PRMClassElement<GUM_SCALAR>& );

      /// See gum::PRMClassElement::addChild().
      void addChild( const gum::prm::PRMClassElement<GUM_SCALAR>& );

      /// @brief Raise a gum::OperationNotAllowed
      /// See gum::PRMClassElement::getCastDescendant()
      virtual PRMAttribute<GUM_SCALAR>* getCastDescendant() const;

      /// @}
      protected:
      /// Copy constructor. Don't use it.
      PRMParameter( const PRMParameter<GUM_SCALAR>& source );

      /// Copy operator. Don't use it.
      PRMParameter& operator=( const PRMParameter<GUM_SCALAR>& from );

      private:
      // ========================================================================
      /// @name Private members
      // ========================================================================
      /// @{

      ParameterType __type;

      GUM_SCALAR __value;

      /// @}
    };


    extern template class PRMParameter<float>;
    extern template class PRMParameter<double>;


  } /* namespace prm */
}  // namespace gum

#include <agrum/PRM/elements/PRMParameter_tpl.h>

#endif /* GUM_PARAMETER_H */
