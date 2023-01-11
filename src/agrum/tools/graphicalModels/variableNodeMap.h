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
 * @brief Header of class VariableNodeMap.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_VARIABLE_NODE_MAP_H
#define GUM_VARIABLE_NODE_MAP_H

#include <iostream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/tools/core/bijection.h>
#include <agrum/tools/graphs/graphElements.h>
#include <agrum/tools/variables/discreteVariable.h>

namespace gum {

  /**
   * @class VariableNodeMap
   * @brief Container used to map discrete variables with nodes.
   * @warning VariableNodeMap ensures that every name of variable is unique in
   * the container.
   */
  class VariableNodeMap {
    public:
    /// @name Constructors / Destructors

    /// @{

    /// Default constructor
    VariableNodeMap();

    /// Copy constructor
    /// Proceed a deep copy: all variables are copied but keep the same node id.
    VariableNodeMap(const VariableNodeMap& source);

    /// Destructor
    ~VariableNodeMap();

    /// Copy operator.
    VariableNodeMap& operator=(const VariableNodeMap& source);

    /// @}

    // ===========================================================================
    /// @name Getters and setters.
    // ===========================================================================
    /// @{

    /// Returns a discrete variable given it's node id.
    /** @throws NotFound Raised if no nodes matches id. */
    const DiscreteVariable& get(NodeId id) const;

    /// Returns a node id given it's variable.
    /** @throws NotFound Raised if no nodes matches var. */
    NodeId get(const DiscreteVariable& var) const;

    /// Return true if id matches a node.
    bool exists(NodeId id) const;

    /// Return true if var matches a node.
    bool exists(const DiscreteVariable& var) const;

    /// Maps id with var.
    /**
     * @warning Var is added by copy.
     * @throws DuplicateLabel if this name already exists
     * @throws DuplicateElement if this id already exists
     * @return Returns id (useful in some case); */
    NodeId insert(NodeId id, const DiscreteVariable& var);

    /// Removes a var and it's id of this mapping. The pointer is deleted.
    /** @throws NotFound Raised if no nodes matches id. */
    void erase(NodeId id);

    /// Removes a var and it's id of this mapping. The pointer is deleted.
    /** @throws NotFound Raised if no nodes matches id. */
    void erase(const DiscreteVariable& var);

    /// we allow the user to change the name of a variable
    /** @throws DuplicateLabel if this name already exists
     * @throws NotFound Raised if no nodes matches id. */
    void changeName(NodeId id, const std::string& new_name);

    /// removes all the associations
    void clear();

    /// give the size
    Size size() const;

    /// friendly displays the content of the VariableNodeMap
    std::string toString() const;

    /**
     * Returns the name of a variable given its id.
     * @param id The variable's id.
     * @return id's name.
     * @throw NotFound Raised if no variable matches id.
     */
    const std::string& name(NodeId id) const;

    /**
     * Returns the name of a variable.
     * @param var The variable.
     * @return var's name.
     * @throw NotFound Raised if var is not in this VariableNodeMap.
     */
    const std::string& name(const DiscreteVariable& var) const;

    /// @}

    // ===========================================================================
    /// @name Operators.
    // ===========================================================================
    /// @{

    /// Returns a discrete variable given it's node id.
    /** @throws NotFound Raised if no nodes matches id. */
    const DiscreteVariable& operator[](NodeId id) const;

    /// Returns a node id given it's variable.
    /** @throws NotFound Raised if no nodes matches var. */
    NodeId operator[](const DiscreteVariable& var) const;

    /// @}

    // ===========================================================================
    /// @name Accessor by name
    // ===========================================================================
    /// @{

    /// @throw NotFound if no such name exists in the graph.
    NodeId                  idFromName(const std::string& name) const;
    const DiscreteVariable& variableFromName(const std::string& name) const;

    /// @}

    private:
    /// effectively do the copy (for copy constructor or operator=)
    void _copy_(const VariableNodeMap& source);

    /// Bijection between the node's NodeIds and the variables.
    Bijection< NodeId, const DiscreteVariable* > _nodes2vars_;

    /// HashTable for easely find an id from a name
    Bijection< std::string, NodeId > _names2nodes_;
  };

  /// for friendly displaying the content of clique graphs

  std::ostream& operator<<(std::ostream&, const VariableNodeMap&);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphicalModels/variableNodeMap_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_VARIABLE_NODE_MAP_H*/
