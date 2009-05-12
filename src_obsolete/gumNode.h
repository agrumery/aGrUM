/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
 *   test $Id: $                                                           *
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

/** @file
 * @brief Base class for every kind of node
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file provides the gumNode class, which is the basis for manipulating any
 * node in any graph (this is actually what chance nodes, decision nodes,
 * utility nodes, etc, have in common). Consequently, it contains very few
 * informations: just an ID and a label/description. In aGrUM, each node is uniquely
 * identified by its ID, hence IDs are the very core of the nodes. The label is
 * just a convenience for easy debugging. It can be also used when displaying
 * the content of a graph. In any case, when manipulating nodes, developers should
 * rely on IDs, not on labels. Note that once a node has been created, you cannot
 * change by yourself its ID (this would be a mess if you forgot to update its
 * adjacent edges). As a result, operator= is not public.
 *
 * @par Usage example:
 * @code
 * // create a node whose ID is 10 and whose full name is "node 10"
 * gumNode node1 (10, "node 10");
 *
 * // get the name and ID of the node
 * string label1 = node1.label ();
 * gumId id1 = node1.getID ();
 *
 * // display the content of the node in a console
 * cerr << node1;
 *
 * // check whether two nodes have the same ID's
 * if (node1 == gumNode (10, "node")) cerr << "same IDs" << endl;
 * @endcode
 */


#ifndef GUM_NODE_H
#define GUM_NODE_H

#include <string>
#include <iostream>

#include <agrum/gum_utils.h>

#include <agrum/gumId.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// prepare nodes to be accessed by their smallest container, i.e., the gumNodeList

template <typename Node> class gumNodeList;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */







/* ============================================================================== */
/* ===                              GENERIC NODES                             === */
/* ============================================================================== */
/** @class gumNode
 * @brief The base class for all nodes.
 *
 * This class is used as a basis for manipulating any node in any graph (this is
 * actually what chance nodes, decision nodes, utility nodes, etc, have in common).
 * Consequently, it contains very few informations: just an ID and a
 * label/description. In aGrUM, each node is uniquely identified by its ID, hence
 * IDs are the very core of the nodes. The label is just a convenience for easy
 * debugging. It can be also used when displaying the content of a graph. In any
 * case, when manipulating nodes, developers should rely on IDs, not on labels.
 * Note that once a node has been created, you cannot change by yourself its ID
 * (this would be a mess if you forgot to update its adjacent edges). As a result,
 * operator= is not public.
 * @par Example:
 * @code
 * // create a node whose ID is 10 and whose full name is "node 10"
 * gumNode node1 (10, "node 10");
 *
 * // get the name and ID of the node
 * string label1 = node1.label ();
 * gumId id1 = node1.getID ();
 *
 * // display the content of the node in a console
 * cerr << node1;
 *
 * // check whether two nodes have the same ID's
 * if (node1 == gumNode (10, "node")) cerr << "same IDs" << endl;
 * @endcode */
/* ============================================================================== */

class gumNode  {
  public:
    // ##############################################################################
    /// @name Constructors / Destructors
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// basic constructor
    /** the node stores copies of the label and the ID passed in arguments. Note that
     * the label is just provided for debugging and friendly-displaying. The core
     * of a node is its ID.
     * @param id the identifier assigned to the node
     * @param aLabel the name of the node (useful for easy debugging) */
    // ==============================================================================
    gumNode( const gumId& id, const std::string& aLabel );

    // ==============================================================================
    /// copy constructor: creates a perfect node copy, including that of the ID
    /** @warning after the copy, the IDs of \e from and of the current
     * node are identical. */
    // ==============================================================================
    gumNode( const gumNode& from );

    // ==============================================================================
    /// Destructor
    // ==============================================================================
    virtual ~gumNode();

    /// @}



    // ##############################################################################
    /// @name Accessors / Modifiers
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// changes the label of a node
    /** @param theValue the new label of the node. */
    // ==============================================================================
    void setLabel( const std::string& theValue );

    // ==============================================================================
    /// gets the label of a node
    // ==============================================================================
    const std::string& label() const ;

    // ==============================================================================
    /// gets the id of the node
    // ==============================================================================
    const gumId& getID() const ;

    /// @}



    // ##############################################################################
    /// @name Operators
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// checks if two nodes are equal (i.e., have the same IDs)
    /** The test is not performed on labels: those are needed for user-friendly
     * displays and debugging, but the core of the nodes is their ID. Hence
     * two nodes are considered equal if they have the same identifier.
     * @warning this test is meaningful when comparing two nodes that belong to the
     * same graph. But when the nodes belong to different graphs, the result may
     * be meaningless (unless similar nodes have the same ID in both graphs). */
    // ==============================================================================
    const bool operator== ( const gumNode& from ) const ;

    // ==============================================================================
    /// checks if two nodes are different (i.e., have different IDs)
    /** The test is not performed on labels: those are needed for user-friendly
     * displays and debugging, but the core of the nodes is their ID. Hence
     * two nodes are considered different if they have different identifiers
     * @warning this test is meaningful when comparing two nodes that belong to the
     * same graph. But when the nodes belong to different graphs, the result may
     * be meaningless (unless similar nodes have the same ID in both graphs). */
    // ==============================================================================
    const bool operator!= ( const gumNode& from ) const ;

    /// @}



  private:
    /// the identifier of the node
    gumId __ID;

    /// the name of the node (used for display and debugging)
    std::string __label;

    /// graphs can decide to change the identifiers of nodes

    template <typename Node> friend class gumNodeList;

    // ==============================================================================
    /// method for changing the id of a node
    /** @param new_id the new ID of the node. */
    // ==============================================================================
    void setID( const gumId& new_id ) ;

    // ==============================================================================
    /// copy operator: creates a perfect node copy, including that of the ID
    /** @warning after the copy, even the IDs of \e from and of the current
     * node will be identical. */
    // ==============================================================================
    gumNode& operator= ( const gumNode& from );

};




// ================================================================================
/// to friendly display the content of a gumNode
// ================================================================================
std::ostream& operator<< ( std::ostream&, const gumNode& );





/* ============================================================================== */
/* ============================================================================== */
/* ===                       GENERIC NODES IMPLEMENTATION                     === */
/* ============================================================================== */
/* ============================================================================== */

#ifndef GUM_NO_INLINE
#include "gumNode.inl"
#endif /* GUM_NO_INLINE */


#endif /* GUM_NODE_H */
