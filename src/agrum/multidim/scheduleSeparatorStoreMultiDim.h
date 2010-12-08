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
 * @brief an operator used by scheduled inferences to store tables into separators
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SCHEDULE_SEPARATOR_STORE_MULTI_DIM_H
#define GUM_SCHEDULE_SEPARATOR_STORE_MULTI_DIM_H


#include <string>
#include <agrum/core/inline.h>
#include <agrum/core/sequence.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/scheduleMultiDim.h>
#include <agrum/multidim/scheduleOperation.h>


namespace gum {


  template <typename T_DATA>
  class ScheduleSeparatorStoreMultiDim : public ScheduleOperation<T_DATA> {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    ScheduleSeparatorStoreMultiDim
    ( const ScheduleMultiDim<T_DATA>* table,
      typename Property<Set<const MultiDimImplementation<T_DATA>*> >::onArcs&
      separator_tables,
      Arc separator );
    
    /// copy constructor
    ScheduleSeparatorStoreMultiDim
    ( const ScheduleSeparatorStoreMultiDim<T_DATA>& );

    /// virtual copy constructor: creates a clone of the operation
    virtual ScheduleSeparatorStoreMultiDim<T_DATA>* newFactory () const;

    /// destructor
    virtual ~ScheduleSeparatorStoreMultiDim ();

    /// @}

    
    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    ScheduleSeparatorStoreMultiDim<T_DATA>& operator=
    ( const ScheduleSeparatorStoreMultiDim<T_DATA>& );

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

    /// returns the set of multidims passed in argument to the operation
    Sequence<const ScheduleMultiDim<T_DATA>*> multiDimArgs () const;

    /// displays the content of the operation
    std::string toString () const;
    
    /// @}

    
    
  private:
    // the table to store into the clique
    const ScheduleMultiDim<T_DATA>* __table;

    // a mapping assigning to each clique a set of tables
    typename Property<Set<const MultiDimImplementation<T_DATA>*> >::onArcs*
    __tableSet;
    
    // the separator into which the table will be stored
    Arc __separator;

  };
  
  
} /* namespace gum */


// always include the template implementation
#include <agrum/multidim/scheduleSeparatorStoreMultiDim.tcc>


#endif /* GUM_SCHEDULE_SEPARATOR_STORE_MULTI_DIM_H */
