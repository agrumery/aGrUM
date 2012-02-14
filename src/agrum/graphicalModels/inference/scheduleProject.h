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

#include <agrum/config.h>

#include <agrum/core/set.h>
#include <agrum/core/sequence.h>
#include <agrum/multidim/discreteVariable.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/graphicalModels/inference/scheduleMultiDim.h>
#include <agrum/graphicalModels/inference/scheduleOperation.h>


namespace gum {


  template <typename T_DATA>

  class ScheduleProject : public ScheduleOperation<T_DATA> {
    public:
      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{

      /// default constructor
      /** @warning table and del_vars are passed by copy */
      ScheduleProject( const ScheduleMultiDim<T_DATA>& table,
                       const Set<const DiscreteVariable *>& del_vars,
                       MultiDimImplementation<T_DATA>*
                       ( *project )( const MultiDimImplementation<T_DATA>&,
                                     const Set<const DiscreteVariable *>& ) );

      /// copy constructor
      ScheduleProject( const ScheduleProject<T_DATA>& );

      /// virtual copy constructor: creates a clone of the operation
      virtual ScheduleProject<T_DATA>* newFactory() const;

      /// destructor
      virtual ~ScheduleProject();

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
      void execute();

      /** @brief returns an estimation of the number of elementary operations
      * needed to perform the ScheduleOperation */
      float nbOperations() const;

      /// returns the memory consumption used during the operation
      /** Actually, this function does not return a precise account of the memory
       * used by the ScheduleOperation but a rough estimate based on the sizes
       * of the tables involved in the operation.
       * @return a pair of memory consumption: the first one is the maximum
       * amount of memory used during the operation and the second one is the
       * amount of memory still used at the end of the function ( the memory used by
       * the resulting table ) */
      std::pair<long,long> memoryUsage() const;

      /// returns the scheduleMultidim resulting from the execution of the operation
      const ScheduleMultiDim<T_DATA>& result() const;

      /// returns the set of multidims passed in argument to the operation
      const Sequence<const ScheduleMultiDim<T_DATA>*>& multiDimArgs() const;

      /// returns the set of multidims that should be the result of the operation
      const Sequence<const ScheduleMultiDim<T_DATA>*>& multiDimResults() const;

      /// displays the content of the operation
      std::string toString() const;

      /// @}



    private:
      // the table to project
      ScheduleMultiDim<T_DATA> __table;

      // the set of variables that should be removed from the table
      Set<const DiscreteVariable *> __del_vars;

      /// the result of the operation
      ScheduleMultiDim<T_DATA>* __result;

      /// the set of ScheduleMultidims passed in arguments
      mutable Sequence<const ScheduleMultiDim<T_DATA>*>* __args;

      /// the set of ScheduleMultidims resulting from the operation
      mutable Sequence<const ScheduleMultiDim<T_DATA>*>* __results;

      /// the projection operator
      MultiDimImplementation<T_DATA>*
      ( *__project )( const MultiDimImplementation<T_DATA>&,
                      const Set<const DiscreteVariable *>& );

  };


} /* namespace gum */


// always include the template implementation
#include <agrum/graphicalModels/inference/scheduleProject.tcc>


#endif /* GUM_SCHEDULE_PROJECTION_H */

