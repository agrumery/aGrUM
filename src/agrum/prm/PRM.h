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
#include <agrum/prm/model.h>
// ============================================================================
#ifndef GUM_PRM_H
#define GUM_PRM_H
namespace gum {
namespace prm {

class PRM;
class Model;

/**
 * @class PRM PRM.h <agrum/prm/PRM.h>
 * @brief This class represents a Probabilistic Relational Model.
 *
 */
class PRM {
  public:
    friend class PRMFactory;

  // ========================================================================
  /// @name Destructor.
  // ========================================================================
    /// @{

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
     * Returns a constant reference on a Type given it's name.
     * @throw NotFound Raised if no type is found with the given name.
     */
    Type& getType(const std::string& name);

    /**
     * Returns a constant reference on a Type given it's name.
     * @throw NotFound Raised if no type is found with the given name.
     */
    const Type& getType(const std::string& name) const;

    /**
     * Returns the Sequence of all Type in this PRM.
     */
    const Sequence<Type*>& types() const;

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
     * Returns the Sequence of all Class in this PRM.
     */
    const Sequence<Class*>& classes() const;

    /**
     * Returns a constant reference on a Class given it's name.
     * @throw NotFound Raised if no class is found with the given name.
     */
    Class& getInterface(const std::string& name);

    /**
     * Returns a constant reference on a Class given it's name.
     * @throw NotFound Raised if no class is found with the given name.
     */
    const Class& getInterface(const std::string& name) const;

    /**
     * Returns the Sequence of all Class in this PRM.
     */
    const Sequence<Class*>& interfaces() const;

    /**
     * Returns a constant reference on a Model given it's name.
     * @throw NotFound Raised if no model is found with the given name.
     */
    Model& getModel(const std::string& name);

    /**
     * Returns a constant reference on a Model given it's name.
     * @throw NotFound Raised if no model is found with the given name.
     */
    const Model& getModel(const std::string& name) const;

    /**
     * Returns the Sequence of all Models in this PRM.
     */
    const Sequence<Model*>& models() const;

    /// @}
  private:
  // ========================================================================
  /// @name Private methods.
  // ========================================================================
    /// @{

    /**
     * Default constructor.
     */
    PRM();

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

    /// Sequence of all Class in this PRM.
    Sequence<Class*> __classes;

    /// Mapping of all Class given their name.
    HashTable<std::string, Class*> __interfaceMap;

    /// Sequence of all Class in this PRM.
    Sequence<Class*> __interfaces;

    /// Mapping of all Type given their name.
    HashTable<std::string, Type*> __typeMap;

    /// Sequence of all Type in this PRM.
    Sequence<Type*> __types;

    /// Mapping of all Models given their name.
    HashTable<std::string, Model*> __modelMap;

    /// Sequence of all Models in this PRM.
    Sequence<Model*> __models;

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
