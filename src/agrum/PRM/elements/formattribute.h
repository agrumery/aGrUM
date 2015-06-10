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
 * @brief Headers of gum::FormAttribute.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_FORM_ATTRIBUTE_H
#define GUM_FORM_ATTRIBUTE_H

#include <agrum/PRM/elements/attribute.h>

#include <agrum/PRM/elements/class.h>

namespace gum {
  namespace prm {

    /**
     * @class FormAttribute formattribute.h <agrum/PRM/elements/formattribute.h>
     * @brief FormAttribute is a member of a Class in a PRM.
     *
     * @see PRM PRMFactory Class ClassElement Type<GUM_SCALAR> Potential Attribute
     * @ingroup prm_group
     */
    template<typename GUM_SCALAR>
    class FormAttribute: public Attribute<GUM_SCALAR> {
      public:

        FormAttribute( const Class<GUM_SCALAR>& c,
                       const std::string & name,
                       const Type<GUM_SCALAR> & type,
                       MultiDimImplementation<std::string>* impl = new MultiDimArray<std::string>() );
  
        virtual ~FormAttribute();
        
        /// See gum::prm::Attribute.
        virtual Attribute<GUM_SCALAR>* newFactory(const Class<GUM_SCALAR>& c) const;

        /// See gum::prm::Attribute.
        virtual Attribute<GUM_SCALAR>* copy( Bijection<const DiscreteVariable*, const DiscreteVariable*> bij ) const;

        /// See gum::prm::Attribute.
        virtual void copyCpf( const Bijection<const DiscreteVariable*, const DiscreteVariable*>& bif,
                              const Attribute<GUM_SCALAR> & source);

        /// See gum::prm::Attribute.
        virtual typename ClassElement<GUM_SCALAR>::ClassElementType elt_type() const;

        /// See gum::prm::Attribute.
        virtual Type<GUM_SCALAR>& type();

        /// See gum::prm::Attribute.
        virtual const Type<GUM_SCALAR>& type() const;

        /// See gum::prm::Attribute.
        virtual const Potential<GUM_SCALAR>& cpf() const;

        /// See gum::prm::Attribute.
        virtual void addParent ( const ClassElement<GUM_SCALAR>& elt );

        /// See gum::prm::Attribute.
        virtual void addChild ( const ClassElement<GUM_SCALAR>& elt );

        /// See gum::prm::Attribute.
        virtual Attribute<GUM_SCALAR>* getCastDescendant() const;

        /// See gum::prm::Attribute.
        virtual void setAsCastDescendant ( Attribute<GUM_SCALAR>* attr );
        virtual void becomeCastDescendant( Type<GUM_SCALAR> &subtype );

        virtual MultiDimImplementation< std::string > & formulas();
        virtual const MultiDimImplementation< std::string > & formulas() const;

        /// Swap old_type with new_type in the ClassElement cpt.
        virtual void swap(const Type<GUM_SCALAR>& old_type, const Type<GUM_SCALAR>& new_type);

      protected:
        virtual Type<GUM_SCALAR>* _type();
        virtual void _type( Type<GUM_SCALAR>* t );

      private:
        FormAttribute( const FormAttribute& source);
        FormAttribute& operator=(const FormAttribute& source);

        /// The random variable type of this attribute
        Type<GUM_SCALAR>* __type;

        /// A pointer on the Potential of this attribute
        mutable Potential<GUM_SCALAR>* __cpf;
    
        /// A pointer on the Potential of this attribute
        MultiDimImplementation<std::string>* __formulas;

        /// A pointe toward the class of this attribute
        const Class<GUM_SCALAR> * __class;

        void __fillCpf() const;
    };

    extern template class FormAttribute<double>;

  } /* namespace prm */
} // namespace gum

#include <agrum/PRM/elements/formattribute.tcc>

#endif /* GUM_ATTRIBUTE_H */

