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


/** @file
 * @brief Base classes for mixed graph listeners
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_MIXEDGRAPH_LISTENER_H
#define GUM_MIXEDGRAPH_LISTENER_H

#include <agrum/base/graphs/mixedGraph.h>

namespace gum {
  /** @class MixedGraphListener
   * @brief Abstract Base class for all mixed Graph Listener
   *
   * \ingroup graph_group
   *
   *
   * This is the virtual base class for mixed graphs meta-listener.
   *
   * A MixedGraphListener will have to implement following pure methods :
   *  - void whenNodeAdded( const void * ,gum::NodeId )
   *  - void whenNodeDeleted( const void *,gum::NodeId )
   *  - void whenArcAdded( const void *,gum::NodeId,gum::NodeId )
   *  - void whenArcDeleted( const void *,gum::NodeId,gum::NodeId )
   *  - void whenEdgeAdded( const void *,gum::NodeId,gum::NodeId )
   *  - void whenEdgeDeleted( const void *,gum::NodeId,gum::NodeId )
   */
  class MixedGraphListener: public Listener {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param g the graph to listen to
     * @warning Note that, by the aGrUM's constructor parameter's rule, the fact
     * that g is passed as a pointer means that g is not copied, but only
     * referenced by the listener. */
    MixedGraphListener(MixedGraph* g);

    /// destructor
    ~MixedGraphListener();

    /// @}

    // ############################################################################
    /// @name signals to listen to
    // ############################################################################
    /// @{

    /// the action to take when a new node is inserted into the graph
    /** @param src the object that sent the signal
     * @param id the id of the new node inserted into the graph */
    virtual void whenNodeAdded(const void* src, NodeId id) = 0;

    /// the action to take when a node has just been removed from the graph
    /** @param src the object that sent the signal
     * @param id the id of the node has just been removed from the graph */
    virtual void whenNodeDeleted(const void* src, NodeId id) = 0;

    /// the action to take when a new arc is inserted into the graph
    /** @param src the object that sent the signal
     * @param from the id of tail of the new arc inserted into the graph
     * @param to the id of head of the new arc inserted into the graph */
    virtual void whenArcAdded(const void* src, NodeId from, NodeId to) = 0;

    /// the action to take when an arc has just been removed from the graph
    /** @param src the object that sent the signal
     * @param from the id of tail of the arc removed from the graph
     * @param to the id of head of the arc removed from the graph */
    virtual void whenArcDeleted(const void* src, NodeId from, NodeId to) = 0;

    /// the action to take when a new edge is inserted into the graph
    /** @param src the object that sent the signal
     * @param id1 the id of one node of the edge inserted into the graph
     * @param id2 the id of the other node of the edge inserted into the graph
     */
    virtual void whenEdgeAdded(const void* src, NodeId id1, NodeId id2) = 0;

    /// the action to take when an edge has just been removed from the graph
    /** @param src the object that sent the signal
     * @param from the id of one node of the edge removed from the graph
     * @param to the id of the other node of the edge removed from the graph */
    virtual void whenEdgeDeleted(const void* src, NodeId from, NodeId to) = 0;

    /// @}

    protected:
    /// the graph to listen to
    MixedGraph* graph_;

    private:
    /// copy constructor (for the moment, this operation is forbidden)
    /** @param d the MixedGraphListener to copy */
    MixedGraphListener(const MixedGraphListener& d);

    /// copy operator (for the moment, this operation is forbidden)
    /** @param d the MixedGraphListener to copy */
    MixedGraphListener& operator=(const MixedGraphListener& d);
  };

}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/base/graphs/parts/listeners/mixedGraphListener_inl.h>
#endif   // GU%_NO_INLINE

#endif   // GUM_MIXEDGRAPH_LISTENER_H
