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

    template<typename GUM_SCALAR>
    class ScalarAttribute: public Attribute<GUM_SCALAR> {

      public:
        // ========================================================================
        /// @name Constructor & destructor.
        // ========================================================================
        /// @{

        /**
         * @brief Constructor used by gum::Class.
         * This will create an Attribute with only one variable: type and with the given
         * implementation.
         * @param name The name of this Attribute.
         * @param type The type of this Attribute, it is copied.
         * @param impl The MultiDimImplementation used by the internal Potential of this Attribute.
         *             it will be deleted after the call of ~Attribute.
         */
        ScalarAttribute ( const std::string& name,
                          const Type<GUM_SCALAR>& type,
                          MultiDimImplementation<GUM_SCALAR>* impl = new MultiDimArray<GUM_SCALAR>() );

        /**
         * @brief Constructor used by gum::Instance.
         * This will create an Attribute with a ready Potential, however it will check the existence
         * of type in cpf and raise an exception if it is not found.
         * @param type The type of this attribute, it will be deleted after a call to ~Attribute.
         * @param name The name of this Attribute.
         * @param cpf The Potential of this Attribute, it will be deleted after the call of
         *            ~Attribute.
         * @param delete_type If true, the type is deleted with this instance.
         */
        // ScalarAttribute ( const std::string& name,
        //                   Type<GUM_SCALAR>* type,
        //                   Potential<GUM_SCALAR>* cpf,
        //                   bool delete_type );


        /// Destructor.
        virtual ~ScalarAttribute();

        /// @}
        // ========================================================================
        /// @name Getters & setters
        // ========================================================================
        /// @{

        virtual Attribute<GUM_SCALAR>* newFactory(const Class<GUM_SCALAR>& c) const;
        virtual Attribute<GUM_SCALAR>* copy( Bijection<const DiscreteVariable*, const DiscreteVariable*> bij) const;

        virtual void copyCpf( const Bijection<const DiscreteVariable*, const DiscreteVariable*>& bif,
                              const Attribute<GUM_SCALAR> & source);

        /// See gum::ClassElement::elt_type().
        virtual typename ClassElement<GUM_SCALAR>::ClassElementType elt_type() const;

        /// See gum::ClassElement::type().
        virtual Type<GUM_SCALAR>& type();

        /// See gum::ClassElement::type().
        virtual const Type<GUM_SCALAR>& type() const;

        /// See gum::ClassElement::cpf().
        virtual const Potential<GUM_SCALAR>& cpf() const;

        /// See gum::ClassElement::_addParent().
        virtual void addParent ( const ClassElement<GUM_SCALAR>& elt );

        /// See gum::ClassElement::_addChild().
        virtual void addChild ( const ClassElement<GUM_SCALAR>& elt );

        virtual Attribute<GUM_SCALAR>* getCastDescendant() const;
        virtual void setAsCastDescendant ( Attribute<GUM_SCALAR>* attr );
        virtual void becomeCastDescendant( Type<GUM_SCALAR> &subtype );

        /// Swap old_type with new_type in the ClassElement cpt.
        virtual void swap(const Type<GUM_SCALAR>& old_type, const Type<GUM_SCALAR>& new_type);

        /// @}
      protected:

        /// Copy constructor. Don't use it.
        ScalarAttribute ( const ScalarAttribute& source );

        /// Copy operator. Don't use it.
        ScalarAttribute& operator= ( const ScalarAttribute& from );

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

    extern template class ScalarAttribute<double>;

  } /* namespace prm */
} // namespace gum

#include <agrum/PRM/elements/scalarattribute.tcc>

#endif /* GUM_SCALAR_ATTRIBUTE_H */

