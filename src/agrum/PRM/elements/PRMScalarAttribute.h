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
 * @brief Headers of gum::PRMScalarAttribute.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_SCALAR_ATTRIBUTE_H
#define GUM_SCALAR_ATTRIBUTE_H

#include <agrum/agrum.h>
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
     * @see PRM PRMFactory Class PRMClassElement PRMType Potential
     * @ingroup prm_group
     */
    template < typename GUM_SCALAR >
    class PRMScalarAttribute: public PRMAttribute< GUM_SCALAR > {
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
      PRMScalarAttribute(const std::string&                    name,
                         const PRMType&                        type,
                         MultiDimImplementation< GUM_SCALAR >* impl
                         = new MultiDimArray< GUM_SCALAR >());

      /// Destructor.
      virtual ~PRMScalarAttribute();

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      virtual PRMAttribute< GUM_SCALAR >* newFactory(const PRMClass< GUM_SCALAR >& c) const;
      virtual PRMAttribute< GUM_SCALAR >*
         copy(Bijection< const DiscreteVariable*, const DiscreteVariable* > bij) const;

      virtual void copyCpf(const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bif,
                           const PRMAttribute< GUM_SCALAR >& source);

      /// See gum::PRMClassElement::elt_type().
      virtual typename PRMClassElement< GUM_SCALAR >::ClassElementType elt_type() const;

      /// See gum::PRMClassElement::type().
      virtual PRMType& type();

      /// See gum::PRMClassElement::type().
      virtual const PRMType& type() const;

      /// See gum::PRMClassElement::cpf().
      virtual const Potential< GUM_SCALAR >& cpf() const;

      /// See gum::PRMClassElement::addParent_().
      virtual void addParent(const PRMClassElement< GUM_SCALAR >& elt);

      /// See gum::PRMClassElement::addChild_().
      virtual void addChild(const PRMClassElement< GUM_SCALAR >& elt);

      virtual PRMAttribute< GUM_SCALAR >* getCastDescendant() const;
      virtual void                        setAsCastDescendant(PRMAttribute< GUM_SCALAR >* attr);
      virtual void                        becomeCastDescendant(PRMType& subtype);

      /// Swap old_type with new_type in the PRMClassElement cpt.
      virtual void swap(const PRMType& old_type, const PRMType& new_type);

      /// @}
      protected:
      /// Copy constructor. Don't use it.
      PRMScalarAttribute(const PRMScalarAttribute& source);

      /// Copy operator. Don't use it.
      PRMScalarAttribute& operator=(const PRMScalarAttribute& from);

      virtual PRMType* type_();
      virtual void     type_(PRMType* t);

      private:
      // ========================================================================
      /// @name Private members
      // ========================================================================
      /// @{

      /// The random variable type of this attribute
      PRMType* _type_;

      /// A pointer on the Potential of this attribute
      Potential< GUM_SCALAR >* _cpf_;

      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMScalarAttribute< double >;
#endif


  } /* namespace prm */
}   // namespace gum

#include <agrum/PRM/elements/PRMScalarAttribute_tpl.h>

#endif /* GUM_SCALAR_ATTRIBUTE_H */
