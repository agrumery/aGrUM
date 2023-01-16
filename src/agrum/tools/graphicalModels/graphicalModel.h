/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Class representing probabilistic DAG model
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#ifndef GUM_GRAPHICALMODEL_H
#define GUM_GRAPHICALMODEL_H
#include <agrum/agrum.h>
#include <agrum/tools/multidim/instantiation.h>

#include <agrum/tools/graphs/DAG.h>

#include <agrum/tools/graphicalModels/variableNodeMap.h>

namespace gum {

  /**
   * @class GraphicalModel
   * @headerfile GraphicalModel.h <agrum/tools/graphicalModels/GraphicalModel.h>
   * Virtual base class for probabilistic graphical models
   *
   */
  class GraphicalModel {
    public:
    /// @name Constructors / Destructors
    /// @{

    /**
     * Default constructor.
     */
    GraphicalModel();

    /**
     * Destructor.
     */
    virtual ~GraphicalModel();

    /**
     * Copy constructor. Do nothing.
     */
    GraphicalModel(const GraphicalModel& source);

    /// @}
    /// @name Getter and setters
    /// @{

    /**
     * Return the value of the property name of this GraphicalModel.
     * @throw NotFound Raised if no name property is found.
     */
    const std::string& property(const std::string& name) const;

    /**
     * Return the value of the property name of this GraphicalModel.
     * return byDefault if the property name is not found
     */
    const std::string& propertyWithDefault(const std::string& name,
                                           const std::string& byDefault) const;

    /**
     * Add or change a property of this GraphicalModel.
     */
    void setProperty(const std::string& name, const std::string& value);

    /// @}
    /// @name Variable manipulation methods.
    /// @{

    /**
     * Returns a constant reference to the VariableNodeMap of this Graphical Model
     */
    virtual const VariableNodeMap& variableNodeMap() const = 0;

    /**
     * Returns the number of variables in this Directed Graphical Model.
     */
    virtual Size size() const = 0;

    /**
     * Return true if this graphical model is empty.
     */
    virtual bool empty() const;

    /**
     * Return true if this node exists in this graphical model.
     */
    virtual bool exists(NodeId node) const = 0;
    bool         exists(const std::string& name) const { return exists(idFromName(name)); };

    /**
     * transform a vector of NodeId in a vector of names
     * @return the vector of names
     */
    std::vector< std::string > names(const std::vector< NodeId >& ids) const;

    /**
     * transform a NodeSet in a vector of names
     * @return the vector of names
     */
    std::vector< std::string > names(const NodeSet& ids) const;

    /**
     * transform a vector of names into a vector of nodeId
     * @return the vector of names
     */
    std::vector< NodeId > ids(const std::vector< std::string >& names) const;

    /**
     * transform a vector of names into a NodeSet
     * @return NodeSet
     */
    NodeSet nodeset(const std::vector< std::string >& names) const;

    virtual const NodeGraphPart& nodes() const = 0;

    /// Get an instantiation over all the variables of the model
    Instantiation completeInstantiation() const;

    /**
     * Returns a constant reference over a variable given it's node id.
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const DiscreteVariable& variable(NodeId id) const = 0;

    /**
     * Return id node src discrete var pointer.
     * @throw NotFound If no variable matches var.
     */
    virtual NodeId nodeId(const DiscreteVariable& var) const = 0;

    /// Getter by name
    /// @throw NotFound if no such name exists in the graph.
    virtual NodeId idFromName(const std::string& name) const = 0;

    /// Getter by name
    /// @throw NotFound if no such name exists in the graph.
    virtual const DiscreteVariable& variableFromName(const std::string& name) const = 0;

    /// @}

    double log10DomainSize() const;

    /** check if node X and node Y are independent given nodes Z
     */
    virtual bool isIndependent(NodeId X, NodeId Y, const NodeSet& Z) const = 0;

    /** check if nodes X and nodes Y are independent given nodes Z
     */
    virtual bool isIndependent(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const = 0;


    /**
     * send to the stream the space complexity with 3 parametrs
     * @param s the stream
     * @param dSize the log10domainSize
     * @param dim  the dimension
     * @param usedMem the memory needed for the params
     */
    static void spaceCplxToStream(std::stringstream& s, double dSize, int dim, Size usedMem);

    protected:
    /**
     * Private copy operator.
     */
    GraphicalModel& operator=(const GraphicalModel& source);

    private:
    /// The properties of this Directed Graphical Model.
    HashTable< std::string, std::string > _propertiesMap_;

    /// Return the properties of this Directed Graphical Model
    const HashTable< std::string, std::string >& _properties_() const;
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphicalModels/graphicalModel_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_GRAPHICALMODEL_H */
