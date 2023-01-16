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
 * @brief Headers of gum::PRMAttribute.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_FUNC_ATTRIBUTE_H
#define GUM_FUNC_ATTRIBUTE_H

#include <agrum/PRM/elements/PRMType.h>

#include <agrum/PRM/elements/PRMScalarAttribute.h>

namespace gum {
  namespace prm {

    /**
     * @class PRMFuncAttribute funcAttribute.h
     *<agrum/PRM/elements/funcAttribute.h>
     * @brief This class overload gum::prm::PRMClassElement::addParent_() and
     *        gum::prm::PRMClassElement::addChild_ with empty methods.
     *
     * This class should be used when dealing with functions such as Noisy-Or
     *which
     * require special method calls to add parents.
     * When dealing with such attributes, you should create the MultiDim
     *yourself and
     * populate it with the attribute's parents.
     *
     * See gum::prm::PRMFactory::addAttribute(PRMAttribute*) for more details.
     */
    template < typename GUM_SCALAR >
    class PRMFuncAttribute: public PRMScalarAttribute< GUM_SCALAR > {
      public:
      /**
       * @brief Constructor used by gum::Class.
       * This will create an PRMFuncAttribute with only one variable: type and
       *with
       *the given
       * implementation.
       *
       * @param name The name of this PRMFuncAttribute.
       * @param type The type of this PRMFuncAttribute, it is copied.
       * @param impl The MultiDimImplementation used by the internal Potential
       *of this PRMFuncAttribute.
       *             it will be deleted after the call of ~PRMFuncAttribute.
       */
      PRMFuncAttribute(const std::string&                    name,
                       const PRMType&                        type,
                       MultiDimImplementation< GUM_SCALAR >* impl
                       = new MultiDimArray< GUM_SCALAR >());

      /// Destructor.
      virtual ~PRMFuncAttribute();

      /// See gum::PRMClassElement::addParent_().
      virtual void addParent(const PRMClassElement< GUM_SCALAR >& elt);

      /// See gum::PRMClassElement::addChild_().
      virtual void addChild(const PRMClassElement< GUM_SCALAR >& elt);

      protected:
      /// Copy constructor. Don't use it.
      PRMFuncAttribute(const PRMFuncAttribute< GUM_SCALAR >& source);

      /// Copy operator. Don't use it.
      PRMFuncAttribute< GUM_SCALAR >& operator=(const PRMFuncAttribute< GUM_SCALAR >& from);

      virtual void setCpf_(Potential< GUM_SCALAR >* cpf);
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMFuncAttribute< double >;
#endif


  } /* namespace prm */
}   // namespace gum

#include <agrum/PRM/elements/PRMFuncAttribute_tpl.h>

#endif /* GUM_FUNC_ATTRIBUTE_H */
