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
 * @author Lionel TORTI
 */
// ============================================================================
#include <string>
// ============================================================================
#include <agrum/core/hashTable.h>
#include <agrum/core/sequence.h>
// ============================================================================
#include <agrum/prm/PRMObject.h>
#include <agrum/prm/type.h>
#include <agrum/prm/class.h>
#include <agrum/prm/instance.h>
#include <agrum/prm/attribute.h>
#include <agrum/prm/aggregate.h>
#include <agrum/prm/referenceSlot.h>
#include <agrum/prm/slotChain.h>
#include <agrum/prm/system.h>
// ============================================================================
#ifndef GUM_PRM_H
#define GUM_PRM_H
namespace gum {
namespace prm {

class PRM;
class System;

/**
 * @class PRM PRM.h <agrum/prm/PRM.h>
 * @brief This class represents a Probabilistic Relational System.
 *
 * @ingroup prm_group
 */
class PRM {
  public:
    friend class PRMFactory;

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
     * @param name The name of a possible Type in this PRM.
     * @return Returns true if name names a Type in this PRM.
     */
    bool isType(const std::string name) const;

    /**
     * @param name The name of a possible Class in this PRM.
     * @return Returns true if name names a Class in this PRM.
     */
    bool isClass(const std::string name) const;

    /**
     * @param name The name of a possible Interface in this PRM.
     * @return Returns true if name names a Interface in this PRM.
     */
    bool isInterface(const std::string name) const;

    /**
     * @param name The name of a possible System in this PRM.
     * @return Returns true if name names a System in this PRM.
     */
    bool isSystem(const std::string name) const;

    /**
     * Returns a constant reference on a Type given it's name.
     * @throw NotFound Raised if no type is found with the given name.
     */
    Type& type(const std::string& name);

    /**
     * Returns a constant reference on a Type given it's name.
     * @throw NotFound Raised if no type is found with the given name.
     */
    const Type& type(const std::string& name) const;

    /**
     * Returns the Set of all Type in this PRM.
     */
    const Set<Type*>& types() const;

    /**
     * Returns a constant reference on a Class given it's name.
     * @throw NotFound Raised if no class is found with the given name.
     */
    Class& getClass(const std::string& name);

    /**
     * Returns a constant reference on a Class given it's name.
     * @throw NotFound Raised if no class is found with the given name.
     */
    const Class& getClass(const std::string& name) const;

    /**
     * Returns the Set of all Class in this PRM.
     */
    const Set<Class*>& classes() const;

    /**
     * Returns a constant reference on a Class given it's name.
     * @throw NotFound Raised if no class is found with the given name.
     */
    Interface& interface(const std::string& name);

    /**
     * Returns a constant reference on a Class given it's name.
     * @throw NotFound Raised if no class is found with the given name.
     */
    const Interface& interface(const std::string& name) const;

    /**
     * Returns the Set of all Class in this PRM.
     */
    const Set<Interface*>& interfaces() const;

    /**
     * Returns a constant reference on a System given it's name.
     * @throw NotFound Raised if no model is found with the given name.
     */
    System& system(const std::string& name);

    /**
     * Returns a constant reference on a System given it's name.
     * @throw NotFound Raised if no model is found with the given name.
     */
    const System& system(const std::string& name) const;

    /**
     * Returns the Set of all Systems in this PRM.
     */
    const Set<System*>& systems() const;

    /// @}
  private:
  // ========================================================================
  /// @name Private methods.
  // ========================================================================
    /// @{

    /**
     * Copy constructor. Not Implemented.
     */
    PRM(const PRM& source);

    /**
     * Copy operator. Not Implemented.
     */
    PRM& operator=(const PRM& source);

    /// Add the built-in types in the PRM
    void __addBuiltInTypes();

    /// @}
  // ========================================================================
  /// @name Private members.
  // ========================================================================
    /// @{

    /// Mapping of all Class given their name.
    HashTable<std::string, Class*> __classMap;

    /// Set of all Class in this PRM.
    Set<Class*> __classes;

    /// Mapping of all Class given their name.
    HashTable<std::string, Interface*> __interfaceMap;

    /// Set of all Class in this PRM.
    Set<Interface*> __interfaces;

    /// Mapping of all Type given their name.
    HashTable<std::string, Type*> __typeMap;

    /// Set of all Type in this PRM.
    Set<Type*> __types;

    /// Mapping of all Systems given their name.
    HashTable<std::string, System*> __systemMap;

    /// Set of all Systems in this PRM.
    Set<System*> __systems;

    /// @}
};

} /* namespace prm */
} /* namespace gum */
#ifndef GUM_NO_INLINE
#include <agrum/prm/PRM.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_PRM_H */
// ============================================================================
