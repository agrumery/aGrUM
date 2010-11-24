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
 * @brief a MultiDimDecorator Wrapper used for scheduling inferences
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_SCHEDULE_MULTI_DIM_H
#define GUM_SCHEDULE_MULTI_DIM_H


#include <string>
#include <agrum/core/inline.h>
#include <agrum/core/types.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/sequence.h>
#include <agrum/multidim/discreteVariable.h>
#include <agrum/multidim/multiDimDecorator.h>



namespace gum {
  

  template <typename T_DATA>
  class ScheduleMultiDim {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// constructs a ScheduleMultiDim containing an already built decorator
    ScheduleMultiDim ( const MultiDimDecorator<T_DATA>& );

    /// construct an (abstract) ScheduleMultiDim for a decorator yet to be built
    /** The ScheduleMultiDim created is abstract, i.e., it does not contain a
     * proper MultiDimDecorator yet. However, the variables of the latter need be
     * known to optimize inference processes
     * @warning the sequence of variables is copied into the wrapper. */
    ScheduleMultiDim ( const Sequence<const DiscreteVariable*>& vars );

    /// copy constructor
    ScheduleMultiDim ( const ScheduleMultiDim<T_DATA>& );

    /// destructor
    ~ScheduleMultiDim ();

    /// @}

    

    // ############################################################################
    /// @name Operators
    // ############################################################################

    /// @{

    /// copy operator
    ScheduleMultiDim<T_DATA>& operator= ( const ScheduleMultiDim<T_DATA>& );

    /// checks whether two ScheduleMultiDim are related to the same table
    INLINE bool operator== ( const ScheduleMultiDim<T_DATA>& );
    
    /// checks whether two ScheduleMultiDim are related to different tables
    INLINE bool operator!= ( const ScheduleMultiDim<T_DATA>& );
    
    /// @}

    

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// returns whether the ScheduleMultiDim contains a real multiDimDecorator
    /** @returns true if the ScheduleMultiDim is abstract, i.e., it is does not
     * actually contains a real MultiDimDecorator but rather a ID indicating that
     * the real multiDimDecorator is yet to be created as a result of an
     * operation on other multiDimDecorators. */
    INLINE bool isAbstract () const;

    /// returns the multiDimDecorator actually contained in the ScheduleMultiDim
    /** @throws NotFound exception is thrown if the multidim decorator does not
     * exist yet (because it has not been computed yet) */
    INLINE const MultiDimDecorator<T_DATA>& multiDim () const;
    
    /// returns the id of the ScheduleMultiDim
    INLINE Id id () const;

    /// returns the set of variables involved in the multidim
    INLINE const Sequence<const DiscreteVariable*>& variables () const;

    /// sets a new multiDimDecorator inside the wrapper
    INLINE void setMultiDim ( const MultiDimDecorator<T_DATA>& );
    
    /// displays the content of the multidim
    std::string toString () const;
    
    /// @}


  private:
    /// the unique Id of the ScheduleMultiDim
    Id __id;

    /// returns a new distinct ID for each abtract scheduleMultiDim
    static Id __newId ();

    /// returns a mapping from id to multidimdecorators
    static HashTable<Id,const MultiDimDecorator<T_DATA>*>& __id2multidims ();

    /// returns a table indicating how many ScheduleMultiDim have the same id
    static HashTable<Id,unsigned int>& __id2refs ();

    /// returns a table with the variables of the table corresponding to id
    static HashTable<Id,const Sequence<const DiscreteVariable*>*>& __id2vars ();
    
  };


} /* namespace gum */


// always include the template implementation
#include <agrum/multidim/scheduleMultiDim.tcc>


#endif /* GUM_SCHEDULE_MULTI_DIM_H */
