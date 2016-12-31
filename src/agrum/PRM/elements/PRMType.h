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
 * @brief Headers of Class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_RANDOM_VARIABLE_TYPE_H
#define GUM_RANDOM_VARIABLE_TYPE_H

#include <vector>

#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/variables/discreteVariable.h>
#include <agrum/variables/labelizedVariable.h>

#include <agrum/PRM/elements/PRMObject.h>
#include <agrum/PRM/utils_prm.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    class PRMFactory;

    /**
     * @class PRMType
     * @brief This is a decoration of the DiscreteVariable class.
     * The name of the DiscreteVariable will be the name of the type (i.e. of
     * the PRMObject).
     * As for all PRMObject, a type's name is unique in a given PRM, so equality
     * tests will be based on the types names.
     *
     * Since MultiDim use pointers to handle DiscreteVariables, it is necessary
     * to create a new instance of a type for each PRMAttribute.
     */
    template <typename GUM_SCALAR>
    class PRMType : public PRMObject {

      public:
      // ==========================================================================
      /// @name Friends of PRMType
      // ==========================================================================
      /// @{

      friend class PRMFactory<GUM_SCALAR>;

      /// @}
      // ==========================================================================
      /// @name Static methods for primitive types
      // ==========================================================================
      /// @{

      /// Returns a pointer on type boolean.
      static PRMType* boolean() {
        LabelizedVariable var( "boolean", "Boolean variable", 0 );
        var.addLabel( "false" );
        var.addLabel( "true" );
        return new PRMType( var );
      }

      /// @}
      // ==========================================================================
      /// @name Constructors & destructor
      // ==========================================================================
      /// @{

      /**
       * Default Constructor.
       * A copy is made of var.
       */
      PRMType( const DiscreteVariable& var );

      /**
       * Sub type constructor.
       * A copy is made of var.
       * @throw OperationNotAllowed Raised if label_map is invalid.
       */
      PRMType( PRMType&                super_type,
               const std::vector<Idx>& label_map,
               const DiscreteVariable& var );

      /**
       * Copy constructor.
       * The DiscreteVariable is copied.
       */
      PRMType( const PRMType& from );

      /**
       * Destructor.
       */
      virtual ~PRMType();

      /// @}
      // ==========================================================================
      /// @name Getters & setters
      // ==========================================================================
      /// @{

      /// Return a reference on the DiscreteVariable contained in this.
      DiscreteVariable& variable();

      /// Return a reference on the DiscreteVariable contained in this.
      const DiscreteVariable& variable() const;

      /// @}
      // ==========================================================================
      /// @name Operators
      // ==========================================================================
      /// @{

      /**
       * Indirection on the DiscreteVariable contained in this.
       */
      DiscreteVariable& operator*();

      /**
       * Constant indirection on the DiscreteVariable contained in this.
       */
      const DiscreteVariable& operator*() const;

      /**
       * Dereference the DiscreteVariable contained in this.
       */
      DiscreteVariable* operator->();

      /**
       * Constant dereference the DiscreteVariable contained in this.
       */
      DiscreteVariable const* operator->() const;

      /**
       * Equality operator.
       */
      bool operator==( const PRMObject& from ) const;

      /**
       * Difference operator.
       */
      bool operator!=( const PRMObject& from ) const;

      /// @}
      // ==========================================================================
      /// @name Getters & setters
      // ==========================================================================
      /// @{

      /**
       * Implementation of the pure virtual method of PRMObject.
       */
      virtual prm_type obj_type() const;

      /**
       * Returns the name of this object.
       */
      const std::string& name() const;

      /**
       * Returns true if this type is a sub-type.
       */
      bool isSubType() const;

      /**
       * Returns true if this is a subtype of super.
       * Note that two types that are equal are also subtypes,
       * if a == b then a.isSubTypeOf(b) == b.isSubTypeOf(a) == true.
       */
      bool isSubTypeOf( const PRMType& super ) const;

      /**
       * Returns true if this is a super type of t.
       */
      bool isSuperTypeOf( const PRMType& t ) const;

      /**
       * Returns the super type of this type.
       * @throw NotFound Raised if this type has no super type.
       */
      PRMType& superType();

      /**
       * Returns the super type of this type.
       * @throw NotFound Raised if this type has no super type.
       */
      const PRMType& superType() const;

      /**
       * @brief Changes the PRMType of this PRMType super.
       *
       * You can only change this PRMType super only if t and this->superType()
       * are
       * equal. Thus you should use this method only if you want to change the
       * DiscreteVariable pointer of this PRMType super.
       *
       * This is useful to maintain consistence between PRMAttribute's PRMType
       * and
       * their CPF.
       *
       * @param t The PRMType to replace this PRMType super.
       *
       * @throw OperationNotAllowed If this PRMType has no super.
       * @throw TypeError If t is not equal to this PRMType super.
       */
      void setSuper( PRMType& t );

      /**
       * Returns the vector in which the i-th element is the Idx of the super
       * type's label for the i-th label of this.
       * @throw NotFound Raised if this type has no super type.
       */
      const std::vector<Idx>& label_map() const;

      /// @}
      // ==========================================================================
      private:
      // ==========================================================================
      /// @name Private methods
      // ==========================================================================
      /// @{

      /**
       * Copy operator. Not implemented.
       */
      PRMType& operator=( const PRMType& from );

      /// @}
      // ==========================================================================
      /// @name Private members.
      // ==========================================================================
      /// @{

      /// Returns true if this is a valid type or subtype.
      bool __isValid() const;

      /// The discrete variable
      DiscreteVariable* __var;

      /// The super type of this, if any.
      PRMType* __superType;

      /// A vector in which the i-th element is the Idx of the super
      /// type's label for the i-th label of this.
      std::vector<Idx>* __label_map;

      /// @}
    };


    extern template class PRMType<float>;
    extern template class PRMType<double>;


  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/elements/PRMType_tpl.h>

#endif /* GUM_CLASS_H */
