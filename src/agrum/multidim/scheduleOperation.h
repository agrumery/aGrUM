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
 * @brief the base operation class used to schedule inferences
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_SCHEDULE_OPERATION_H
#define GUM_SCHEDULE_OPERATION_H


#include <string>
#include <agrum/core/inline.h>
#include <agrum/core/types.h>
#include <agrum/core/sequence.h>
#include <agrum/multidim/scheduleMultiDim.h>


namespace gum {  


  template <typename T_DATA>
  class ScheduleOperation {
  public:
    /// the currently supported types of operations
    enum Type {
      /// combine 2 ScheduleMultiDims 
      GUM_COMBINE_MULTIDIM,
      /// project a ScheduleMultiDim over a subset of its variables
      GUM_PROJECT_MULTIDIM,
      /// delete a MultiDim from memory
      GUM_DELETE_MULTIDIM,
      /// store a Multidim into a clique
      GUM_CLIQUE_STORE_MULTIDIM,
      /// store a multidim into a separator
      GUM_SEPARATOR_STORE_MULTIDIM
    };

    
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// virtual copy constructor: creates a clone of the operation
    virtual ScheduleOperation<T_DATA>* newFactory () const = 0;

    /// destructor
    virtual ~ScheduleOperation ();

    /// @}

    
    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// operator ==
    /** Two operations are identical if and only if they have the same
     * arguments and their types are identical (combine, project, etc) */
    virtual bool operator== ( const ScheduleOperation<T_DATA>& ) const = 0;

    /// operator !=
    /** Two operations are identical if and only if they have the same
     * arguments and their types are identical (combine, project, etc) */
    virtual bool operator!= ( const ScheduleOperation<T_DATA>& ) const = 0;

    /// @}
    

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// returns the name of the operation
    INLINE Type type () const;

    /// returns the set of multidims passed in argument to the operation
    virtual Sequence<const ScheduleMultiDim<T_DATA>*> multiDimArgs () const = 0;
    
    /// executes the operation
    virtual void execute () = 0;

    /// displays the content of the operation
    virtual std::string toString () const = 0;

    /// @}

    
  protected:
    /// default constructor
    ScheduleOperation ( Type t );

    
  private:
    /// the name of the operation to perform
    const Type __type;
    

    /// prevent copy constructor
    ScheduleOperation ( const ScheduleOperation<T_DATA>& );

    /// prevent copy operator
    ScheduleOperation<T_DATA>& operator= ( const ScheduleOperation<T_DATA>& );

  };
  

} /* namespace gum */


// always include the template implementation
#include <agrum/multidim/scheduleOperation.tcc>


#endif /* GUM_SCHEDULE_OPERATION_H */

