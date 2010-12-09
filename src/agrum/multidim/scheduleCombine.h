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
 * @brief a Combination operator class used for scheduling inferences
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_SCHEDULE_COMBINE_H
#define GUM_SCHEDULE_COMBINE_H


#include <string>
#include <agrum/core/inline.h>
#include <agrum/core/sequence.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/scheduleMultiDim.h>
#include <agrum/multidim/scheduleOperation.h>


namespace gum {
    

  template <typename T_DATA>
  class ScheduleCombine : public ScheduleOperation<T_DATA> {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    ScheduleCombine ( const ScheduleMultiDim<T_DATA>* table1,
                      const ScheduleMultiDim<T_DATA>* table2,
                      MultiDimImplementation<T_DATA>*
                      (*combine) ( const MultiDimImplementation<T_DATA>&,
                                   const MultiDimImplementation<T_DATA>& ) );
    
    /// copy constructor
    ScheduleCombine ( const ScheduleCombine<T_DATA>& );

    /// virtual copy constructor: creates a clone of the operation
    virtual ScheduleCombine<T_DATA>* newFactory () const;

    /// destructor
    ~ScheduleCombine ();

    /// @}
    


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    ScheduleCombine<T_DATA>& operator= ( const ScheduleCombine<T_DATA>& );

    /// operator ==
    /** Two operations are identical if and only if they have the same
     * arguments and their types are identical (combine, project, etc) */
    INLINE bool operator== ( const ScheduleOperation<T_DATA>& ) const;

    /// operator !=
    /** Two operations are identical if and only if they have the same
     * arguments and their types are identical (combine, project, etc) */
    INLINE bool operator!= ( const ScheduleOperation<T_DATA>& ) const;

    /// @}
    

 
    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// executes the operation
    void execute ();

    /// returns the scheduleMultidim resulting from the execution of the operation
    INLINE const ScheduleMultiDim<T_DATA>& result () const;

    /// returns the set of multidims passed in argument to the operation
    const Sequence<const ScheduleMultiDim<T_DATA>*>& multiDimArgs () const;

    /// returns the set of multidims that should be the result of the operation
    const Sequence<const ScheduleMultiDim<T_DATA>*>& multiDimResults () const;
    
    /// displays the content of the operation
    std::string toString () const;
    
    /// @}
    

  private:
    /// the first table to combine
    const ScheduleMultiDim<T_DATA>* __table1;

    /// the second table to combine with
    const ScheduleMultiDim<T_DATA>* __table2;

    /// the result of the operation
    /** the result is allocated and deallocated by ScheduleCombine */
    ScheduleMultiDim<T_DATA>* __result;

    /// the set of ScheduleMultidims passed in arguments 
    mutable Sequence<const ScheduleMultiDim<T_DATA>*>* __args;

    /// the set of ScheduleMultidims resulting from the operation
    mutable Sequence<const ScheduleMultiDim<T_DATA>*>* __results;
    
    /// the function actually used to perform the combination
    MultiDimImplementation<T_DATA>*
    (*__combine) ( const MultiDimImplementation<T_DATA>&,
                   const MultiDimImplementation<T_DATA>& );
  };


} /* namespace gum */


// always include the template implementation
#include <agrum/multidim/scheduleCombine.tcc>


#endif /* GUM_SCHEDULE_COMBINE_H */
