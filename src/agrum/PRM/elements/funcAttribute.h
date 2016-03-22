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
 * @brief Headers of gum::Attribute.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_FUNC_ATTRIBUTE_H
#define GUM_FUNC_ATTRIBUTE_H

#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/PRM/elements/scalarAttribute.h>

namespace gum {
  namespace prm {

    /**
     * @class FuncAttribute funcAttribute.h <agrum/PRM/elements/funcAttribute.h>
     * @brief This class overload gum::prm::ClassElement::_addParent() and
     *        gum::prm::ClassElement::_addChild with empty methods.
     *
     * This class should be used when dealing with functions such as Noisy-Or
     *which
     * require special method calls to add parents.
     * When dealing with such attributes, you should create the MultiDim
     *yourself and
     * populate it with the attribute's parents.
     *
     * See gum::prm::PRMFactory::addAttribute(Attribute*) for more details.
     */
    template <typename GUM_SCALAR>
    class FuncAttribute : public ScalarAttribute<GUM_SCALAR> {
      public:
      /**
       * @brief Constructor used by gum::Class.
       * This will create an FuncAttribute with only one variable: type and with
       *the given
       * implementation.
       *
       * @param name The name of this FuncAttribute.
       * @param type The type of this FuncAttribute, it is copied.
       * @param impl The MultiDimImplementation used by the internal Potential
       *of this FuncAttribute.
       *             it will be deleted after the call of ~FuncAttribute.
       */
      FuncAttribute( const std::string& name,
                     const Type<GUM_SCALAR>& type,
                     MultiDimImplementation<GUM_SCALAR>* impl =
                         new MultiDimArray<GUM_SCALAR>() );

      /// Destructor.
      virtual ~FuncAttribute();

      /// See gum::ClassElement::_addParent().
      virtual void addParent( const ClassElement<GUM_SCALAR>& elt );

      /// See gum::ClassElement::_addChild().
      virtual void addChild( const ClassElement<GUM_SCALAR>& elt );

      protected:
      /// Copy constructor. Don't use it.
      FuncAttribute( const FuncAttribute& source );

      /// Copy operator. Don't use it.
      FuncAttribute& operator=( const FuncAttribute& from );

      virtual void _setCpf( Potential<GUM_SCALAR>* cpf );
    };

#ifndef GUM_NO_FORWARD_DECLARATION
    extern template class FuncAttribute<float>;
    extern template class FuncAttribute<double>;
#endif // GUM_NO_FORWARD_DECLARATION

  } /* namespace prm */
}  // namespace gum

#include <agrum/PRM/elements/funcAttribute.tcc>

#endif /* GUM_FUNC_ATTRIBUTE_H */
