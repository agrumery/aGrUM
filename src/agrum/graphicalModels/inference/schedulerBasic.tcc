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
 * @brief a scheduler that executes any available operation (chosen aribtrarily)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/core/debug.h>


namespace gum {


  /// default constructor
  template<typename T_DATA>
  SchedulerBasic<T_DATA>::SchedulerBasic () :
    Scheduler<T_DATA> () {
    // for debugging purposes
    GUM_CONSTRUCTOR ( SchedulerBasic );
  }


  /// copy constructor
  template<typename T_DATA>
  SchedulerBasic<T_DATA>::SchedulerBasic ( const SchedulerBasic<T_DATA>& from ) :
  Scheduler<T_DATA> ( from ) {
    // for debugging purposes
    GUM_CONS_CPY ( SchedulerBasic );
  }

  
  /// destructor
  template<typename T_DATA>
  SchedulerBasic<T_DATA>::~SchedulerBasic () {
    // for debugging purposes
    GUM_DESTRUCTOR ( SchedulerBasic );
  }

  
  /// virtual constructor
  template<typename T_DATA>
  SchedulerBasic<T_DATA>* SchedulerBasic<T_DATA>::newFactory () const {
    return new SchedulerBasic<T_DATA> ( *this );
  }

  
  /// execute all the operations of a given schedule
  template<typename T_DATA>
  bool SchedulerBasic<T_DATA>::execute ( Schedule<T_DATA>& schedule ) {
    const NodeSet& available = schedule.availableOperations ();
    while ( ! available.empty () ) {
      for ( NodeSet::const_iterator iter = available.begin();
            iter != available.end (); ++iter ) {
        schedule.execute ( *iter );
      }
    }

    return ( schedule.dag().size() == 0);
  }

  
  /// execute only k operations of a given schedule (default k = 1)
  template<typename T_DATA>
  bool SchedulerBasic<T_DATA>::execute ( Schedule<T_DATA>& schedule,
                                         unsigned int k ) {
    const NodeSet& available = schedule.availableOperations ();
    while ( ! available.empty () && k ) {
      for ( NodeSet::const_iterator iter = available.begin();
            iter != available.end () && k; ++iter, --k ) {
        schedule.execute ( *iter );
      }
    }

    return !k || ( schedule.dag().size() == 0);
  }
     

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
