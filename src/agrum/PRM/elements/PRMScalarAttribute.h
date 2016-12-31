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
 * @brief Headers of gum::PRMScalarAttribute.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SCALAR_ATTRIBUTE_H
#define GUM_SCALAR_ATTRIBUTE_H

#include <agrum/PRM/elements/PRMAttribute.h>

namespace gum {
  namespace prm {

    /**
     * @class PRMScalarAttribute scalarAttribute.h
     *<agrum/PRM/elements/scalarAttribute.h>
     * @brief PRMScalarAttribute is a member of a Class in a PRM.
     *
     * A PRMScalarAttribute is defined by its name, its containing class, its
     *type
     *and
     * by a Conditional Probability Function (aka CPF but represented by a
     * Potential).
     *
     * An attribute in a PRM is the equivalent of a random variable in a
     *bayesian
     * network.
     *
     * This class is constructed by a gum::PRMFactory and is deleted by its
     * gum::Class.
     *
     * Built-in copies (copy constructor and copy operator) are illegal due to
     * various problems raised by redondant information.
     *
     * @see PRM PRMFactory Class PRMClassElement PRMType<GUM_SCALAR> Potential
     * @ingroup prm_group
     */
    template <typename GUM_SCALAR>
    class PRMScalarAttribute : public PRMAttribute<GUM_SCALAR> {

      public:
      // ========================================================================
      /// @name Constructor & destructor.
      // ========================================================================
      /// @{

      /**
       * @brief Constructor used by gum::Class.
       * This will create an PRMAttribute with only one variable: type and with
       * the
       * given
       * implementation.
       * @param name The name of this PRMAttribute.
       * @param type The type of this PRMAttribute, it is copied.
       * @param impl The MultiDimImplementation used by the internal Potential
       * of this PRMAttribute.
       *             it will be deleted after the call of ~PRMAttribute.
       */
      PRMScalarAttribute( const std::string&                  name,
                          const PRMType<GUM_SCALAR>&          type,
                          MultiDimImplementation<GUM_SCALAR>* impl =
                              new MultiDimArray<GUM_SCALAR>() );

      /// Destructor.
      virtual ~PRMScalarAttribute();

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      virtual PRMAttribute<GUM_SCALAR>*
      newFactory( const PRMClass<GUM_SCALAR>& c ) const;
      virtual PRMAttribute<GUM_SCALAR>* copy(
          Bijection<const DiscreteVariable*, const DiscreteVariable*> bij ) const;

      virtual void copyCpf(
          const Bijection<const DiscreteVariable*, const DiscreteVariable*>& bif,
          const PRMAttribute<GUM_SCALAR>& source );

      /// See gum::PRMClassElement::elt_type().
      virtual typename PRMClassElement<GUM_SCALAR>::ClassElementType
      elt_type() const;

      /// See gum::PRMClassElement::type().
      virtual PRMType<GUM_SCALAR>& type();

      /// See gum::PRMClassElement::type().
      virtual const PRMType<GUM_SCALAR>& type() const;

      /// See gum::PRMClassElement::cpf().
      virtual const Potential<GUM_SCALAR>& cpf() const;

      /// See gum::PRMClassElement::_addParent().
      virtual void addParent( const PRMClassElement<GUM_SCALAR>& elt );

      /// See gum::PRMClassElement::_addChild().
      virtual void addChild( const PRMClassElement<GUM_SCALAR>& elt );

      virtual PRMAttribute<GUM_SCALAR>* getCastDescendant() const;
      virtual void setAsCastDescendant( PRMAttribute<GUM_SCALAR>* attr );
      virtual void becomeCastDescendant( PRMType<GUM_SCALAR>& subtype );

      /// Swap old_type with new_type in the PRMClassElement cpt.
      virtual void swap( const PRMType<GUM_SCALAR>& old_type,
                         const PRMType<GUM_SCALAR>& new_type );

      /// @}
      protected:
      /// Copy constructor. Don't use it.
      PRMScalarAttribute( const PRMScalarAttribute& source );

      /// Copy operator. Don't use it.
      PRMScalarAttribute& operator=( const PRMScalarAttribute& from );

      virtual PRMType<GUM_SCALAR>* _type();
      virtual void _type( PRMType<GUM_SCALAR>* t );

      private:
      // ========================================================================
      /// @name Private members
      // ========================================================================
      /// @{

      /// The random variable type of this attribute
      PRMType<GUM_SCALAR>* __type;

      /// A pointer on the Potential of this attribute
      Potential<GUM_SCALAR>* __cpf;

      /// @}
    };


    extern template class PRMScalarAttribute<float>;
    extern template class PRMScalarAttribute<double>;


  } /* namespace prm */
}  // namespace gum

#include <agrum/PRM/elements/PRMScalarAttribute_tpl.h>

#endif /* GUM_SCALAR_ATTRIBUTE_H */
