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
 * @brief a Projection operator class used for scheduling inferences
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SCHEDULE_PROJECT_H
#define GUM_SCHEDULE_PROJECT_H


#include <string>
#include <agrum/core/inline.h>
#include <agrum/core/set.h>
#include <agrum/core/sequence.h>
#include <agrum/multidim/discreteVariable.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/scheduleMultiDim.h>
#include <agrum/multidim/scheduleOperation.h>


namespace gum {

  
  template <typename T_DATA>
  class ScheduleProject : public ScheduleOperation<T_DATA> {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning del_vars is passed by copy */
    ScheduleProject ( const ScheduleMultiDim<T_DATA>* table,
                      const Set<const DiscreteVariable *>& del_vars,
                      MultiDimImplementation<T_DATA>*
                      (*project) ( const MultiDimImplementation<T_DATA>&,
                                   const Set<const DiscreteVariable *>& ) );
    
    /// copy constructor
    ScheduleProject ( const ScheduleProject<T_DATA>& );

    /// destructor
    virtual ~ScheduleProject ();

    /// @}

    
    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    ScheduleProject<T_DATA>& operator= ( const ScheduleProject<T_DATA>& );

    /// operator ==
    /** Two operations are identical if and only if they have the same
     * arguments and their types are identical (combine, project, etc) */
    bool operator== ( const ScheduleOperation<T_DATA>& ) const;

    /// operator !=
    /** Two operations are identical if and only if they have the same
     * arguments and their types are identical (combine, project, etc) */
    bool operator!= ( const ScheduleOperation<T_DATA>& ) const;

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
    Sequence<const ScheduleMultiDim<T_DATA>*> multiDimArgs () const;

    /// displays the content of the operation
    std::string toString () const;
    
    /// @}

    
    
  private:
    // the table to project
    const ScheduleMultiDim<T_DATA>* __table;

    // the set of variables that should be removed from the table
    Set<const DiscreteVariable *> __del_vars;
    
    /// the result of the operation
    ScheduleMultiDim<T_DATA>* __result;

    /// the projection operator
    MultiDimImplementation<T_DATA>*
    (*__project) ( const MultiDimImplementation<T_DATA>&,
                   const Set<const DiscreteVariable *>& );

  };


} /* namespace gum */


// always include the template implementation
#include <agrum/multidim/scheduleProject.tcc>


#endif /* GUM_SCHEDULE_PROJECTION_H */

