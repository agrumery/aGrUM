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
 * @brief Headers of gum::Parameter.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_PARAMETER_H
#define GUM_PARAMETER_H

#include <agrum/PRM/elements/classElement.h>

namespace gum {
  namespace prm {

    /**
     * @class Parameter parameter.h <agrum/PRM/parameter.h>
     * @brief Parameter is a member of a Class in a PRM.
     *
     * @see PRM PRMFactory Class ClassElement
     * @ingroup prm_group
     */
    template <typename GUM_SCALAR>
    class Parameter : public ClassElement<GUM_SCALAR> {
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
      Parameter( const std::string& name,
                 ParameterType type,
                 GUM_SCALAR value );

      /// Destructor.
      virtual ~Parameter();

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      /// See gum::ClassElement::elt_type().
      virtual typename ClassElement<GUM_SCALAR>::ClassElementType
      elt_type() const;

      GUM_SCALAR value() const;

      void value( GUM_SCALAR value );

      ParameterType valueType() const;

      /// See gum::ClassElement::type().
      virtual Type<GUM_SCALAR>& type();

      /// See gum::ClassElement::type().
      virtual const Type<GUM_SCALAR>& type() const;

      /// See gum::ClassElement::cpf().
      virtual Potential<GUM_SCALAR>& cpf();

      /// See gum::ClassElement::cpf().
      virtual const Potential<GUM_SCALAR>& cpf() const;

      /// See gum::ClassElement::addParent().
      void addParent( const gum::prm::ClassElement<GUM_SCALAR>& );

      /// See gum::ClassElement::addChild().
      void addChild( const gum::prm::ClassElement<GUM_SCALAR>& );

      /// @brief Raise a gum::OperationNotAllowed
      /// See gum::ClassElement::getCastDescendant()
      virtual Attribute<GUM_SCALAR>* getCastDescendant() const;

      /// @}
      protected:
      /// Copy constructor. Don't use it.
      Parameter( const Parameter<GUM_SCALAR>& source );

      /// Copy operator. Don't use it.
      Parameter& operator=( const Parameter<GUM_SCALAR>& from );

      private:
      // ========================================================================
      /// @name Private members
      // ========================================================================
      /// @{

      ParameterType __type;

      GUM_SCALAR __value;

      /// @}
    };

#ifndef GUM_NO_FORWARD_DECLARATION
    extern template class Parameter<float>;
    extern template class Parameter<double>;
#endif // GUM_NO_FORWARD_DECLARATION

  } /* namespace prm */
}  // namespace gum

#include <agrum/PRM/elements/parameter.tcc>

#endif /* GUM_PARAMETER_H */
