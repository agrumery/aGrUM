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
* @brief Base classes for oriented graphs
*
* @author Pierre-Henri WUILLEMIN
*/
#ifndef GUM_DIGRAPH_LISTENER_H
#define GUM_DIGRAPH_LISTENER_H

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
protected:
    DiGraph* _digraph;

    DiGraphListener(const DiGraphListener& d);
    DiGraphListener& operator=(const DiGraphListener& d);

public:
    /// @name Constructors / Destructors
    /// @{
    /** constructor of DiGraphListener
    * @param g the graph to be listened to
    */
    DiGraphListener(DiGraph* g);

    /// destructor
    ~DiGraphListener();
    /// @}

    /// signals to listen to
    /// @{
    virtual void whenNodeAdded( const void * ,gum::NodeId )=0;
    virtual void whenNodeDeleted( const void *,gum::NodeId )=0;
    virtual void whenArcAdded( const void *,gum::NodeId,gum::NodeId )=0;
    virtual void whenArcDeleted( const void *,gum::NodeId,gum::NodeId )=0;
    /// @}
};
} // namespace gum

#ifndef GUM_NO_INLINE
#include <agrum/graphs/diGraphListener.inl>
#endif //GUM_NOINLINE

#endif // GUM_DIGRAPH_LISTENER_H
