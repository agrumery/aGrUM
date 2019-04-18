
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
    template < typename GUM_SCALAR >
    class PRMFormAttribute : public PRMAttribute< GUM_SCALAR > {
      public:
      PRMFormAttribute(const PRMClass< GUM_SCALAR >&          c,
                       const std::string&                     name,
                       const PRMType&                         type,
                       MultiDimImplementation< std::string >* impl =
                          new MultiDimArray< std::string >());

      virtual ~PRMFormAttribute();

      /// See gum::prm::PRMAttribute.
      virtual PRMAttribute< GUM_SCALAR >*
         newFactory(const PRMClass< GUM_SCALAR >& c) const;

      /// See gum::prm::PRMAttribute.
      virtual PRMAttribute< GUM_SCALAR >* copy(
         Bijection< const DiscreteVariable*, const DiscreteVariable* > bij) const;

      /// See gum::prm::PRMAttribute.
      virtual void copyCpf(
         const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bif,
         const PRMAttribute< GUM_SCALAR >& source);

      /// See gum::prm::PRMAttribute.
      virtual typename PRMClassElement< GUM_SCALAR >::ClassElementType
         elt_type() const;

      /// See gum::prm::PRMAttribute.
      virtual PRMType& type();

      /// See gum::prm::PRMAttribute.
      virtual const PRMType& type() const;

      /// See gum::prm::PRMAttribute.
      virtual const Potential< GUM_SCALAR >& cpf() const;

      /// See gum::prm::PRMAttribute.
      virtual void addParent(const PRMClassElement< GUM_SCALAR >& elt);

      /// See gum::prm::PRMAttribute.
      virtual void addChild(const PRMClassElement< GUM_SCALAR >& elt);

      /// See gum::prm::PRMAttribute.
      virtual PRMAttribute< GUM_SCALAR >* getCastDescendant() const;

      /// See gum::prm::PRMAttribute.
      virtual void setAsCastDescendant(PRMAttribute< GUM_SCALAR >* attr);
      virtual void becomeCastDescendant(PRMType& subtype);

      virtual MultiDimImplementation< std::string >&       formulas();
      virtual const MultiDimImplementation< std::string >& formulas() const;

      /// Swap old_type with new_type in the PRMClassElement cpt.
      virtual void swap(const PRMType& old_type, const PRMType& new_type);

      protected:
      virtual PRMType* _type();
      virtual void     _type(PRMType* t);

      private:
      PRMFormAttribute(const PRMFormAttribute& source);
      PRMFormAttribute& operator=(const PRMFormAttribute& source);

      /// The random variable type of this attribute
      PRMType* __type;

      /// A pointer on the Potential of this attribute
      mutable Potential< GUM_SCALAR >* __cpf;

      /// A pointer on the Potential of this attribute
      MultiDimImplementation< std::string >* __formulas;

      /// A pointe toward the class of this attribute
      const PRMClass< GUM_SCALAR >* __class;

      void __fillCpf() const;
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMFormAttribute< double >;
#endif


  } /* namespace prm */
}   // namespace gum

#include <agrum/PRM/elements/PRMFormAttribute_tpl.h>

#endif /* GUM_ATTRIBUTE_H */
