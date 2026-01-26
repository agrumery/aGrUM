/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


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

#include <agrum/base/core/bijection.h>
#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/variables/discreteVariable.h>

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
#  include <agrum/base/graphicalModels/variableNodeMap_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_VARIABLE_NODE_MAP_H*/
