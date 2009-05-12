/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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

#ifndef GUM_DAG_GENERATOR_H
#define GUM_DAG_GENERATOR_H

#include <string>
#include <iostream>
#include <stdlib.h>
#include <time.h>

#include <agrum/core/gumExceptions.h>
#include <agrum/core/gumHashTable.h>
#include <agrum/BN/gumBayesNet.h>
#include <agrum/gumGraph.h>
#include <agrum/gumList.h>
#include <agrum/gumNode.h>
#include <agrum/graphs/gumGraphElements.h>

/**
 * This class generates a directed acyclic graph (aka DAG).
 *
 * The following parameter are allowed:
 *    - NBR_NODES:  The number of nodes (default value 20);
 *    - NBR_ROOT:   The number of root nodes (default value 1);
 *    - NBR_LEAF:   The number of leaf nodes (default value 5);
 *    - NBR_COMP:   The number of connex component (default value 1);
 *    - MIN_NBR_PARENT or MAX_NBR_PARENT: The minimal and maximal number of parents for each node (default value 1 and 3);
 *    - MIN_NBR_CHILD or MAX_NBR_CHILD:   The minimal and maximal number of childs for each node (default value 1 and 3);
 *
 * The parameters can be given at construction time, through a gumHashTable<string, string>,
 * or after construction through the setConfiguration() method.
 *
 * If a parameter is set to an erroneous value, an gumOperationNotAllowed will be raised.
 * If a parameter is missing, then a random value is chosen if possible, else
 * gumOperationNotAllowed is raised.
 *
 */

class gumDAGGenerator {

  public:
    /* ============================================================================== */
    /* ============================================================================== */
    /* ===                    CONSTRUCTORS AND DESTRUCTOR                         === */
    /* ============================================================================== */
    /* ============================================================================== */

    /**
     * @name Constructors and destructor
     */
    /// @{

    /**
     * Default constructor.
     *
     * All parameters are randomly generated.
    * @param maxNodes The maximum number of nodes allowed.
     * @param maxDomainSize The maximum domain size allowed.
     */
    gumDAGGenerator( gumSize maxNodes = 100, gumSize maxDomainSize = 4 );

    /**
     * Fast configuration constructor.
     *
     * @param config A hashtable which key's are a parameters and value's are those of
     * their respective parameter
     * value. The list of the parameters is given in the class documentation.
     */
    gumDAGGenerator( gumHashTable<std::string, std::string> config );

    /**
     * Destuctor.
     */
    ~gumDAGGenerator();

    /// @}

    /* ============================================================================== */
    /* ============================================================================== */
    /* ===                  PARAMETERS GETTERS AND SETTER                         === */
    /* ============================================================================== */
    /* ============================================================================== */

    /**
     * @name Parameters getters and setters
     */
    /// @{

    /**
     * See the class documentation for a list of the parameters.
     *
     * @return Returns the configuration hashtable.
     */
    const gumHashTable<std::string, std::string>& getConfiguration() const;

    /**
     * Set the configuration of this generator.
     *
     * A union is made with the current configuration. See the class documentation for
     * a list of the parameters.
     *
     * @param configuration A hastable which key's are parameters and values are those of
     * their respective parameter.
     */
    void setConfiguration( const gumHashTable<std::string, std::string>& configuration );

    /**
     * @return Returns the number of nodes.
     */
    gumSize getNbrNodes() const;

    /**
     * Set the number of nodes.
     */
    void setNbrNodes( gumSize n );

    /**
     * @return Returns the minimal number of parents of a node.
     */
    gumSize getMinParents() const;

    /**
     * Set the minimal number of parents added to a node.
     */
    void setMinParents( gumSize n );

    /**
     * @return Returns the maximal number of parents of a node.
     */
    gumSize getMaxParents() const;

    /**
     * Set the maximal number of parents added to a node.
     */
    void setMaxParents( gumSize n );

    /**
     * @return Returns the minimal number of children of a node.
     */
    gumSize getMinChildren() const;

    /**
     * Set the minimal number of children of a node.
     */
    void setMinChildren( gumSize n );

    /**
     * @return Returns the maximal number of children of a node.
     */
    gumSize getMaxChildren() const;

    /**
     * Set the maximal number of children of a node.
     */
    void setMaxChildren( gumSize n );

    /**
     * @return Returns the number of root nodes.
     */
    gumSize getNbrRootNodes() const;

    /**
     * Set the number of root nodes.
     */
    void setNbrRootNodes( gumSize n );

    /**
     * @return Returns the number of leaf nodes.
     */
    gumSize getNbrLeafNodes() const;

    /**
     * Set the number of leaf nodes.
     */
    void setNbrLeafNodes( gumSize n );

    /**
     * @return Returns the number of connex component.
     */
    gumSize getNbrConnexComponent() const;

    /**
     * Set the number of connex component.
     */
    void setNbrConnexComponent( gumSize n );

    /// @}

    /* ============================================================================== */
    /* ============================================================================== */
    /* ===                          GENERATION METHODS                            === */
    /* ============================================================================== */
    /* ============================================================================== */

    /**
     * @name Generation methods
     */
    /// @{

    // Check all parameters.
    // @throw gumOperationNotAllowed Raised if parameters are incorrect.
    void checkParameters() const;

    /**
     * Generates a bayesian network.
     *
     * @return Returns a randomly generated bayesian network.
     */
    gumDAG generateDAG() const;

    /// @}

    /* ============================================================================== */
    /* ============================================================================== */
    /* ===                           PROTECTED MEMBERS                            === */
    /* ============================================================================== */
    /* ============================================================================== */

  protected:
    /**
     * @name Protected members
     */
    /// @{

    /**
     * A gumHashTable containing the parameters.
     */
    gumHashTable<std::string, std::string> _param;

    /// @}

    /* ============================================================================== */
    /* ============================================================================== */
    /* ===                             PRIVATE MEMBERS                            === */
    /* ============================================================================== */
    /* ============================================================================== */
  private:

    // Static member listing all allowed parameters
    static const std::string __allowedParam[];

    // Static member giving the length of the previous array
    static const short __nbrAllowedParam = 8;

    /*
     * Generates nodes, and dispatch them in a set of root nodes, leaf nodes
     * and middle nodes.
     */
    void __generateNodes( gumDAG &dag,
                          gumNodeSet &rootList,
                          gumNodeSet &middleList,
                          gumNodeSet &leafList ) const;

    /*
     * Generates nodes, for a DAG with several connex component.
     */
    void __generateNodes( gumDAG &dag,
                          gumNodeSet &rootList,
                          gumNodeSet &middleList,
                          gumNodeSet &leafList ) const;

    /*
     * Generate arcs.
     */
    void __generateArcs( gumDAG &dag,
                         gumNodeSet &rootList,
                         gumNodeSet &middleList,
                         gumNodeSet &leafList ) const;

    /*
     * Generate arcs, for a DAG with several connex component.
     */
    void __generateArcs( gumDAG &dag,
                         gumNodeSet &rootList,
                         gumNodeSet &middleList,
                         gumNodeSet &leafList ) const;

    /*
     * Removes ancestors of aNode from candidates, and memorize them.
     * If restore is set at true, then it will restore the last memorized
     * nodes.
     */
    void __removeAncestors( const gumNodeId aNode,
                            const gumDAG &dag,
                            gumNodeSet& candidates,
                            bool restore = false ) const;

};

#ifndef GUM_NO_INLINE
#include "gumDAGGenerator.inl"
#endif

#endif  // GUM_DAG_GENERATOR_H

