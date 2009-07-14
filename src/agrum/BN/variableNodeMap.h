/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
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
 * @brief Header of class VariableNodeMap.
 *
 * @author Lionel Torti and Pierre-Henri Wuillemin
 */
// ============================================================================
#ifndef GUM_VARIABLENODEMAP_H
#define GUM_VARIABLENODEMAP_H
// ============================================================================
#include <agrum/core/hashTable.h>
#include <agrum/core/exceptions.h>
// ============================================================================
#include <agrum/graphs/graphElements.h>
// ============================================================================
#include <agrum/multidim/discreteVariable.h>
// ============================================================================
namespace gum {
/**
 * @class VariableNodeMap
 * @brief Container used to map discrete variables with nodes.
 */
class VariableNodeMap {
  public:
  // ===========================================================================
  /// @name Constructors / Destructors
  // ===========================================================================
    /// @{

    /// Default constructor
    VariableNodeMap();

    /// Copy constructor
    /// Proceed a deep copy: all variables are copied but keep the same node id.
    VariableNodeMap( const VariableNodeMap& source );

    /// Destructor
    ~VariableNodeMap();

    // @}
  // ===========================================================================
  /// @name Getters and setters.
  // ===========================================================================
    /// @{

    /**
     * Returns a discrete variable given it's node id.
     * @throws NotFound Raised if no nodes matches id.
     */
    const DiscreteVariable& get(NodeId id) const;

    /**
     * Returns a node id given it's variable.
     * @throws NotFound Raised if no nodes matches var.
     */
    virtual NodeId get(const DiscreteVariable& var) const;

    /**
     * Return true if id matches a node.
     */
    virtual bool exists(NodeId id) const;

    /**
     * Return true if var matches a node.
     */
    virtual bool exists(const DiscreteVariable& var) const;

    // @}
  // ===========================================================================
  /// @name Operators.
  // ===========================================================================
    /// @{

    /**
     * Returns a discrete variable given it's node id.
     * @throws NotFound Raised if no nodes matches id.
     */
    virtual const DiscreteVariable& operator[](NodeId id) const;

    /**
     * Returns a node id given it's variable.
     * @throws NotFound Raised if no nodes matches var.
     */
    virtual NodeId operator[](const DiscreteVariable& var) const;

    /**
     * Returns a constant reference over a variabe given it's node id.
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const DiscreteVariable& variable(NodeId id) const = 0;

    /**
     * Return id node from discrete var pointer.
     * @throw NotFound If no variable matches var.
     */
    virtual NodeId nodeId(const DiscreteVariable &var) const = 0;

    /**
     * Copy operator.
     */
    virtual VariableNodeMap& operator=(const VariableNodeMap& source);

    // @}

  protected:

  // ===========================================================================
  /// @name Protected methods.
  // ===========================================================================
    /// @{

    /// Maps id with var. Var is added by copy.
    /// @warning If the map already exist it will delete the precedent var!
    /// @return Returns id (usefull in some case);
    NodeId _set(NodeId id, const DiscreteVariable& var);

    /// Removes a var and it's id of this mapping. The pointer is deleted.
    void _erase(NodeId id);

    /// Removes a var and it's id of this mapping. The pointer is deleted.
    void _erase(const DiscreteVariable& var);

    /// @}

  private:

    /// Mapping between the node's NodeIds and the variables.
    HashTable<NodeId, DiscreteVariable*> __nodes2vars;

    /// Mapping between in the other sense for fast access.
    HashTable<DiscreteVariable*, NodeId> __vars2nodes;
};
} /* namespace gum */
// ============================================================================
#ifndef GUM_NO_INLINE
#include <agrum/BN/variableNodeMap.inl>
#endif /* GUM_NO_INLINE */
// ============================================================================
#endif /* GUM_VARIABLENODEMAP_H*/
// ============================================================================
