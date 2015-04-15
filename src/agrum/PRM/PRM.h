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
 * @brief Headers of PRM.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_PRM_H
#define GUM_PRM_H

#include <string>

#include <agrum/core/hashTable.h>
#include <agrum/core/sequence.h>

#include <agrum/PRM/elements/PRMObject.h>
#include <agrum/PRM/elements/type.h>
#include <agrum/PRM/elements/class.h>
#include <agrum/PRM/elements/instance.h>
#include <agrum/PRM/elements/attribute.h>
#include <agrum/PRM/elements/aggregate.h>
#include <agrum/PRM/elements/referenceSlot.h>
#include <agrum/PRM/elements/slotChain.h>

#include <agrum/PRM/elements/system.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR> class System;
    template <typename GUM_SCALAR> class PRMFactory;
    /**
     * @class PRM PRM.h <agrum/PRM/PRM.h>
     * @brief This class represents a Probabilistic Relational System<GUM_SCALAR>.
     *
     * @ingroup prm_group
     */
    template <typename GUM_SCALAR> class PRM {
      public:
      friend class PRMFactory<GUM_SCALAR>;

      // ========================================================================
      /// @name Destructor.
      // ========================================================================
      /// @{

      /**
       * Default constructor.
       */
      PRM();

      /**
       * Destructor.
       */
      ~PRM();

      /// @}
      // ========================================================================
      /// @name Getters and setters.
      // ========================================================================
      /// @{

      /**
       * @param name The name of a possible Type<GUM_SCALAR> in this PRM.
       * @return Returns true if name names a Type<GUM_SCALAR> in this PRM.
       */
      bool isType(const std::string name) const;

      /**
       * @param name The name of a possible Class<GUM_SCALAR> in this PRM.
       * @return Returns true if name names a Class<GUM_SCALAR> in this PRM.
       */
      bool isClass(const std::string name) const;

      /**
       * @param name The name of a possible Interface<GUM_SCALAR> in this PRM.
       * @return Returns true if name names a Interface<GUM_SCALAR> in this PRM.
       */
      bool isInterface(const std::string name) const;

      /**
       * @param name The name of a possible System<GUM_SCALAR> in this PRM.
       * @return Returns true if name names a System<GUM_SCALAR> in this PRM.
       */
      bool isSystem(const std::string name) const;

      /**
       * Returns a constant reference on a Type<GUM_SCALAR> given it's name.
       * @throw NotFound Raised if no type is found with the given name.
       */
      Type<GUM_SCALAR> &type(const std::string &name);

      /**
       * Returns a constant reference on a Type<GUM_SCALAR> given it's name.
       * @throw NotFound Raised if no type is found with the given name.
       */
      const Type<GUM_SCALAR> &type(const std::string &name) const;

      /**
       * Returns the Set of all Type<GUM_SCALAR> in this PRM.
       */
      const Set<Type<GUM_SCALAR> *> &types() const;

      /**
       * Returns a constant reference on a Class<GUM_SCALAR> given it's name.
       * @throw NotFound Raised if no class is found with the given name.
       */
      Class<GUM_SCALAR> &getClass(const std::string &name);

      /**
       * Returns a constant reference on a Class<GUM_SCALAR> given it's name.
       * @throw NotFound Raised if no class is found with the given name.
       */
      const Class<GUM_SCALAR> &getClass(const std::string &name) const;

      /**
       * Returns the Set of all Class<GUM_SCALAR> in this PRM.
       */
      const Set<Class<GUM_SCALAR> *> &classes() const;

      /**
       * Returns a constant reference on a Class<GUM_SCALAR> given it's name.
       * @throw NotFound Raised if no class is found with the given name.
       */
      Interface<GUM_SCALAR> &interface(const std::string &name);

      /**
       * Returns a constant reference on a Class<GUM_SCALAR> given it's name.
       * @throw NotFound Raised if no class is found with the given name.
       */
      const Interface<GUM_SCALAR> &interface(const std::string &name) const;

      /**
       * Returns the Set of all Class<GUM_SCALAR> in this PRM.
       */
      const Set<Interface<GUM_SCALAR> *> &interfaces() const;

      /**
       * Returns a constant reference on a System<GUM_SCALAR> given it's name.
       * @throw NotFound Raised if no model is found with the given name.
       */
      System<GUM_SCALAR> &system(const std::string &name);

      /**
       * Returns a constant reference on a System<GUM_SCALAR> given it's name.
       * @throw NotFound Raised if no model is found with the given name.
       */
      const System<GUM_SCALAR> &system(const std::string &name) const;

      /**
       * Returns the Set of all Systems in this PRM.
       */
      const Set<System<GUM_SCALAR> *> &systems() const;

      /// @}
      private:
      // ========================================================================
      /// @name Private methods.
      // ========================================================================
      /// @{

      /**
       * Copy constructor. Not Implemented.
       */
      PRM(const PRM<GUM_SCALAR> &source);

      /**
       * Copy operator. Not Implemented.
       */
      PRM<GUM_SCALAR> &operator=(const PRM<GUM_SCALAR> &source);

      /// Add the built-in types in the PRM
      void __addBuiltInTypes();

      /// @}
      // ========================================================================
      /// @name Private members.
      // ========================================================================
      /// @{

      /// Mapping of all Class<GUM_SCALAR> given their name.
      HashTable<std::string, Class<GUM_SCALAR> *> __classMap;

      /// Set of all Class<GUM_SCALAR> in this PRM.
      Set<Class<GUM_SCALAR> *> __classes;

      /// Mapping of all Class<GUM_SCALAR> given their name.
      HashTable<std::string, Interface<GUM_SCALAR> *> __interfaceMap;

      /// Set of all Class<GUM_SCALAR> in this PRM.
      Set<Interface<GUM_SCALAR> *> __interfaces;

      /// Mapping of all Type<GUM_SCALAR> given their name.
      HashTable<std::string, Type<GUM_SCALAR> *> __typeMap;

      /// Set of all Type<GUM_SCALAR> in this PRM.
      Set<Type<GUM_SCALAR> *> __types;

      /// Mapping of all Systems given their name.
      HashTable<std::string, System<GUM_SCALAR> *> __systemMap;

      /// Set of all Systems in this PRM.
      Set<System<GUM_SCALAR> *> __systems;

      /// @}
    };
    extern template class Interface<double>;

  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/PRM.tcc>

#endif /* GUM_PRM_H */
