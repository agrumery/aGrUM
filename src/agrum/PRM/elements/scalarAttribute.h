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
 * @brief Headers of gum::ScalarAttribute.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SCALAR_ATTRIBUTE_H
#define GUM_SCALAR_ATTRIBUTE_H

#include <agrum/PRM/elements/attribute.h>

namespace gum {
  namespace prm {

    /**
     * @class ScalarAttribute scalarAttribute.h
     *<agrum/PRM/elements/scalarAttribute.h>
     * @brief ScalarAttribute is a member of a Class in a PRM.
     *
     * A ScalarAttribute is defined by its name, its containing class, its type
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
     * @see PRM PRMFactory Class ClassElement Type<GUM_SCALAR> Potential
     * @ingroup prm_group
     */
    template <typename GUM_SCALAR>
    class ScalarAttribute : public Attribute<GUM_SCALAR> {

      public:
      // ========================================================================
      /// @name Constructor & destructor.
      // ========================================================================
      /// @{

      /**
       * @brief Constructor used by gum::Class.
       * This will create an Attribute with only one variable: type and with the
       * given
       * implementation.
       * @param name The name of this Attribute.
       * @param type The type of this Attribute, it is copied.
       * @param impl The MultiDimImplementation used by the internal Potential
       * of this Attribute.
       *             it will be deleted after the call of ~Attribute.
       */
      ScalarAttribute( const std::string& name,
                       const Type<GUM_SCALAR>& type,
                       MultiDimImplementation<GUM_SCALAR>* impl =
                           new MultiDimArray<GUM_SCALAR>() );

      /// Destructor.
      virtual ~ScalarAttribute();

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      virtual Attribute<GUM_SCALAR>*
      newFactory( const Class<GUM_SCALAR>& c ) const;
      virtual Attribute<GUM_SCALAR>*
      copy( Bijection<const DiscreteVariable*, const DiscreteVariable*> bij )
          const;

      virtual void copyCpf( const Bijection<const DiscreteVariable*,
                                            const DiscreteVariable*>& bif,
                            const Attribute<GUM_SCALAR>& source );

      /// See gum::ClassElement::elt_type().
      virtual typename ClassElement<GUM_SCALAR>::ClassElementType
      elt_type() const;

      /// See gum::ClassElement::type().
      virtual Type<GUM_SCALAR>& type();

      /// See gum::ClassElement::type().
      virtual const Type<GUM_SCALAR>& type() const;

      /// See gum::ClassElement::cpf().
      virtual const Potential<GUM_SCALAR>& cpf() const;

      /// See gum::ClassElement::_addParent().
      virtual void addParent( const ClassElement<GUM_SCALAR>& elt );

      /// See gum::ClassElement::_addChild().
      virtual void addChild( const ClassElement<GUM_SCALAR>& elt );

      virtual Attribute<GUM_SCALAR>* getCastDescendant() const;
      virtual void setAsCastDescendant( Attribute<GUM_SCALAR>* attr );
      virtual void becomeCastDescendant( Type<GUM_SCALAR>& subtype );

      /// Swap old_type with new_type in the ClassElement cpt.
      virtual void swap( const Type<GUM_SCALAR>& old_type,
                         const Type<GUM_SCALAR>& new_type );

      /// @}
      protected:
      /// Copy constructor. Don't use it.
      ScalarAttribute( const ScalarAttribute& source );

      /// Copy operator. Don't use it.
      ScalarAttribute& operator=( const ScalarAttribute& from );

      virtual Type<GUM_SCALAR>* _type();
      virtual void _type( Type<GUM_SCALAR>* t );

      private:
      // ========================================================================
      /// @name Private members
      // ========================================================================
      /// @{

      /// The random variable type of this attribute
      Type<GUM_SCALAR>* __type;

      /// A pointer on the Potential of this attribute
      Potential<GUM_SCALAR>* __cpf;

      /// @}
    };

#ifndef GUM_NO_FORWARD_DECLARATION
    extern template class ScalarAttribute<float>;
    extern template class ScalarAttribute<double>;
#endif // GUM_NO_FORWARD_DECLARATION

  } /* namespace prm */
}  // namespace gum

#include <agrum/PRM/elements/scalarAttribute.tcc>

#endif /* GUM_SCALAR_ATTRIBUTE_H */
