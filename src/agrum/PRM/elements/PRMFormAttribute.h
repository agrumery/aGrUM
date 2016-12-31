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
 * @brief Headers of gum::PRMFormAttribute.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_FORM_ATTRIBUTE_H
#define GUM_FORM_ATTRIBUTE_H

#include <agrum/PRM/elements/PRMAttribute.h>

#include <agrum/PRM/elements/PRMClass.h>

namespace gum {
  namespace prm {

    /**
     * @class PRMFormAttribute formAttribute.h
     *<agrum/PRM/elements/formAttribute.h>
     * @brief PRMFormAttribute is a member of a Class in a PRM.
     *
     * @see PRM PRMFactory Class PRMClassElement PRMType<GUM_SCALAR> Potential
     *PRMAttribute
     * @ingroup prm_group
     */
    template <typename GUM_SCALAR>
    class PRMFormAttribute : public PRMAttribute<GUM_SCALAR> {
      public:
      PRMFormAttribute( const PRMClass<GUM_SCALAR>&          c,
                        const std::string&                   name,
                        const PRMType<GUM_SCALAR>&           type,
                        MultiDimImplementation<std::string>* impl =
                            new MultiDimArray<std::string>() );

      virtual ~PRMFormAttribute();

      /// See gum::prm::PRMAttribute.
      virtual PRMAttribute<GUM_SCALAR>*
      newFactory( const PRMClass<GUM_SCALAR>& c ) const;

      /// See gum::prm::PRMAttribute.
      virtual PRMAttribute<GUM_SCALAR>* copy(
          Bijection<const DiscreteVariable*, const DiscreteVariable*> bij ) const;

      /// See gum::prm::PRMAttribute.
      virtual void copyCpf(
          const Bijection<const DiscreteVariable*, const DiscreteVariable*>& bif,
          const PRMAttribute<GUM_SCALAR>& source );

      /// See gum::prm::PRMAttribute.
      virtual typename PRMClassElement<GUM_SCALAR>::ClassElementType
      elt_type() const;

      /// See gum::prm::PRMAttribute.
      virtual PRMType<GUM_SCALAR>& type();

      /// See gum::prm::PRMAttribute.
      virtual const PRMType<GUM_SCALAR>& type() const;

      /// See gum::prm::PRMAttribute.
      virtual const Potential<GUM_SCALAR>& cpf() const;

      /// See gum::prm::PRMAttribute.
      virtual void addParent( const PRMClassElement<GUM_SCALAR>& elt );

      /// See gum::prm::PRMAttribute.
      virtual void addChild( const PRMClassElement<GUM_SCALAR>& elt );

      /// See gum::prm::PRMAttribute.
      virtual PRMAttribute<GUM_SCALAR>* getCastDescendant() const;

      /// See gum::prm::PRMAttribute.
      virtual void setAsCastDescendant( PRMAttribute<GUM_SCALAR>* attr );
      virtual void becomeCastDescendant( PRMType<GUM_SCALAR>& subtype );

      virtual MultiDimImplementation<std::string>&       formulas();
      virtual const MultiDimImplementation<std::string>& formulas() const;

      /// Swap old_type with new_type in the PRMClassElement cpt.
      virtual void swap( const PRMType<GUM_SCALAR>& old_type,
                         const PRMType<GUM_SCALAR>& new_type );

      protected:
      virtual PRMType<GUM_SCALAR>* _type();
      virtual void _type( PRMType<GUM_SCALAR>* t );

      private:
      PRMFormAttribute( const PRMFormAttribute& source );
      PRMFormAttribute& operator=( const PRMFormAttribute& source );

      /// The random variable type of this attribute
      PRMType<GUM_SCALAR>* __type;

      /// A pointer on the Potential of this attribute
      mutable Potential<GUM_SCALAR>* __cpf;

      /// A pointer on the Potential of this attribute
      MultiDimImplementation<std::string>* __formulas;

      /// A pointe toward the class of this attribute
      const PRMClass<GUM_SCALAR>* __class;

      void __fillCpf() const;
    };


    extern template class PRMFormAttribute<float>;
    extern template class PRMFormAttribute<double>;


  } /* namespace prm */
}  // namespace gum

#include <agrum/PRM/elements/PRMFormAttribute_tpl.h>

#endif /* GUM_ATTRIBUTE_H */
