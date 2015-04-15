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
/** @file
 * @brief Base classes for oriented graph listeners
 *
 * @author Pierre-Henri WUILLEMIN
 */
#ifndef GUM_DIGRAPH_LISTENER_H
#define GUM_DIGRAPH_LISTENER_H

#include <agrum/core/signal/listener.h>
#include <agrum/graphs/diGraph.h>

namespace gum {
  /** @class DiGraphListener
   * @brief Abstract Base class for all diGraph Listener
   *
   * \ingroup graph_group
   *
   *
   * This is the virtual base class for graphs meta-listener.
   *
   * A DiGraphListener will have to implement following pure methods :
   *  - void whenNodeAdded( const void * ,gum::NodeId )
   *  - void whenNodeDeleted( const void *,gum::NodeId )
   *  - void whenArcAdded( const void *,gum::NodeId,gum::NodeId )
   *  - void whenArcDeleted( const void *,gum::NodeId,gum::NodeId )
   */
  class DiGraphListener : public Listener {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param g the graph to listen to.
     * @warning Note that, by the aGrUM's constructor parameter's rule, the fact
     * that g is passed as a pointer means that g is not copied, but only
     * referenced by the listener. */
    DiGraphListener(const DiGraph *g);

    /// destructor
    ~DiGraphListener();

    /// @}

    // ############################################################################
    /// @name signals to listen to
    // ############################################################################
    /// @{

    /// the action to take when a new node is inserted into the graph
    /** @param src the object that sent the signal
     * @param id the id of the new node inserted into the graph */
    virtual void whenNodeAdded(const void *src, NodeId id) = 0;

    /// the action to take when a node has just been removed from the graph
    /** @param src the object that sent the signal
     * @param id the id of the node has just been removed from the graph */
    virtual void whenNodeDeleted(const void *src, NodeId id) = 0;

    /// the action to take when a new arc is inserted into the graph
    /** @param src the object that sent the signal
     * @param from the id of tail of the new arc inserted into the graph
     * @param to the id of head of the new arc inserted into the graph */
    virtual void whenArcAdded(const void *src, NodeId from, NodeId to) = 0;

    /// the action to take when an arc has just been removed from the graph
    /** @param src the object that sent the signal
     * @param from the id of tail of the arc removed from the graph
     * @param to the id of head of the arc removed from the graph */
    virtual void whenArcDeleted(const void *src, NodeId from, NodeId to) = 0;

    /// @}

    protected:
    /// the graph to listen to
    DiGraph *_graph;

    private:
    /// copy constructor (for the moment, this operation is forbidden)
    /** @param d the DiGraphListener to copy */
    DiGraphListener(const DiGraphListener &d);

    /// copy operator (for the moment, this operation is forbidden)
    /** @param d the DiGraphListener to copy */
    DiGraphListener &operator=(const DiGraphListener &d);
  };

} // namespace gum

#ifndef GUM_NO_INLINE
#include <agrum/graphs/listeners/diGraphListener.inl>
#endif // GUM_NOINLINE

#endif // GUM_DIGRAPH_LISTENER_H
