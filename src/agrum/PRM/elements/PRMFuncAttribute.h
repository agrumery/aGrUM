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
 * @brief Headers of gum::PRMAttribute.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_FUNC_ATTRIBUTE_H
#define GUM_FUNC_ATTRIBUTE_H

#include <agrum/PRM/elements/PRMScalarAttribute.h>
#include <agrum/multidim/multiDimImplementation.h>

namespace gum {
  namespace prm {

    /**
     * @class PRMFuncAttribute funcAttribute.h
     *<agrum/PRM/elements/funcAttribute.h>
     * @brief This class overload gum::prm::PRMClassElement::_addParent() and
     *        gum::prm::PRMClassElement::_addChild with empty methods.
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
    template <typename GUM_SCALAR>
    class PRMFuncAttribute : public PRMScalarAttribute<GUM_SCALAR> {
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
      PRMFuncAttribute( const std::string&                  name,
                        const PRMType<GUM_SCALAR>&          type,
                        MultiDimImplementation<GUM_SCALAR>* impl =
                            new MultiDimArray<GUM_SCALAR>() );

      /// Destructor.
      virtual ~PRMFuncAttribute();

      /// See gum::PRMClassElement::_addParent().
      virtual void addParent( const PRMClassElement<GUM_SCALAR>& elt );

      /// See gum::PRMClassElement::_addChild().
      virtual void addChild( const PRMClassElement<GUM_SCALAR>& elt );

      protected:
      /// Copy constructor. Don't use it.
      PRMFuncAttribute( const PRMFuncAttribute& source );

      /// Copy operator. Don't use it.
      PRMFuncAttribute& operator=( const PRMFuncAttribute& from );

      virtual void _setCpf( Potential<GUM_SCALAR>* cpf );
    };


    extern template class PRMFuncAttribute<float>;
    extern template class PRMFuncAttribute<double>;


  } /* namespace prm */
}  // namespace gum

#include <agrum/PRM/elements/PRMFuncAttribute_tpl.h>

#endif /* GUM_FUNC_ATTRIBUTE_H */
