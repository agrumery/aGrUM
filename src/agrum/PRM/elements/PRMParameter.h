/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Headers of gum::PRMParameter.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_PARAMETER_H
#define GUM_PARAMETER_H


#include <agrum/PRM/elements/PRMType.h>
#include <agrum/PRM/elements/PRMAttribute.h>


namespace gum {
  namespace prm {

    /**
     * @class PRMParameter
     * @headerfile parameter.h <agrum/PRM/parameter.h>
     * @brief PRMParameter is a member of a Class in a PRM.
     *
     * @see PRM PRMFactory Class PRMClassElement
     * @ingroup prm_group
     */
    template < typename GUM_SCALAR >
    class PRMParameter: public PRMClassElement< GUM_SCALAR > {
      public:
      enum ParameterType {
        INT,
        REAL
      };

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
      PRMParameter(const std::string& name, ParameterType type, GUM_SCALAR value);

      /// Destructor.
      virtual ~PRMParameter();

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      /// See gum::PRMClassElement::elt_type().
      virtual typename PRMClassElement< GUM_SCALAR >::ClassElementType elt_type() const;

      GUM_SCALAR value() const;

      void value(GUM_SCALAR value);

      ParameterType valueType() const;

      /// See gum::PRMClassElement::type().
      virtual PRMType& type();

      /// See gum::PRMClassElement::type().
      virtual const PRMType& type() const;

      /// See gum::PRMClassElement::cpf().
      virtual Potential< GUM_SCALAR >& cpf();

      /// See gum::PRMClassElement::cpf().
      virtual const Potential< GUM_SCALAR >& cpf() const;

      /// See gum::PRMClassElement::addParent().
      void addParent(const gum::prm::PRMClassElement< GUM_SCALAR >&);

      /// See gum::PRMClassElement::addChild().
      void addChild(const gum::prm::PRMClassElement< GUM_SCALAR >&);

      /// @brief Raise a gum::OperationNotAllowed
      /// See gum::PRMClassElement::getCastDescendant()
      virtual PRMAttribute< GUM_SCALAR >* getCastDescendant() const;

      /// @}
      protected:
      /// Copy constructor. Don't use it.
      PRMParameter(const PRMParameter< GUM_SCALAR >& source);

      /// Copy operator. Don't use it.
      PRMParameter< GUM_SCALAR >& operator=(const PRMParameter< GUM_SCALAR >& from);

      private:
      // ========================================================================
      /// @name Private members
      // ========================================================================
      /// @{

      ParameterType _type_;

      GUM_SCALAR _value_;

      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMParameter< double >;
#endif


  } /* namespace prm */
}   // namespace gum

#include <agrum/PRM/elements/PRMParameter_tpl.h>

#endif /* GUM_PARAMETER_H */
