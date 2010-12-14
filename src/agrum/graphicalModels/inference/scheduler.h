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
/** @file
 * @brief the common interface of all the schedulers 
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SCHEDULER_H
#define GUM_SCHEDULER_H


#include <agrum/graphicalModels/inference/schedule.h>


namespace gum {


  template<typename T_DATA>
  class Scheduler {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    Scheduler ();

    /// copy constructor
    Scheduler ( const Scheduler<T_DATA>& );

    /// destructor
    virtual ~Scheduler ();

    /// virtual constructor
    /** @return a new fresh Scheduler */
    virtual Scheduler<T_DATA>* newFactory () const = 0;

    /// @}

    
    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// execute all the operations of a given schedule
    /** @return a Boolean indicating whether all the schedule was performed */
    virtual bool execute ( Schedule<T_DATA>& ) = 0;

    /// execute only k operations of a given schedule (default k = 1)
    /** If there are fewer than k operations in the schedule, then all those
     * operations are performed
     * @return a Boolean indicating whether the k operations (or all the
     * operations of the schedule) were performed */
    virtual bool execute ( Schedule<T_DATA>&, unsigned int k ) = 0;
    
    /// @}

  };


} /* namespace gum */


// always include the template implementation
#include <agrum/graphicalModels/inference/scheduler.tcc>


#endif /* GUM_SCHEDULER_H */
