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


#include <limits>
#include <agrum/core/debug.h>


namespace gum {


  /// default constructor
  template<typename T_DATA>
  SchedulerBasic<T_DATA>::SchedulerBasic() :
      Scheduler<T_DATA> () {
    // for debugging purposes
    GUM_CONSTRUCTOR( SchedulerBasic );
  }


  /// copy constructor
  template<typename T_DATA>
  SchedulerBasic<T_DATA>::SchedulerBasic( const SchedulerBasic<T_DATA>& from ) :
      Scheduler<T_DATA> ( from ) {
    // for debugging purposes
    GUM_CONS_CPY( SchedulerBasic );
  }


  /// destructor
  template<typename T_DATA>
  SchedulerBasic<T_DATA>::~SchedulerBasic() {
    // for debugging purposes
    GUM_DESTRUCTOR( SchedulerBasic );
  }


  /// virtual constructor
  template<typename T_DATA>
  SchedulerBasic<T_DATA>* SchedulerBasic<T_DATA>::newFactory() const {
    return new SchedulerBasic<T_DATA> ( *this );
  }


  /// execute all the operations of a given schedule
  template<typename T_DATA>
  bool SchedulerBasic<T_DATA>::execute( Schedule<T_DATA>& schedule ) {
    const NodeSet& available = schedule.availableOperations();
    while ( ! available.empty() ) {
      for ( typename NodeSet::const_iterator iter = available.begin();
            iter != available.end(); ++iter ) {
        schedule.execute( *iter );
      }
    }

    return ( schedule.dag().size() == 0 );
  }


  /// execute only k operations of a given schedule (default k = 1)
  template<typename T_DATA>
  bool SchedulerBasic<T_DATA>::execute( Schedule<T_DATA>& schedule,
                                        unsigned int k ) {
    const NodeSet& available = schedule.availableOperations();
    while ( ! available.empty() && k ) {
      for ( typename NodeSet::const_iterator iter = available.begin();
            iter != available.end() && k; ++iter, --k ) {
        schedule.execute( *iter );
      }
    }

    return !k || ( schedule.dag().size() == 0 );
  }


  /** @bried returns an estimation of the number of elementary operations needed
   * to perform a given schedule */
  template<typename T_DATA>
  float
  SchedulerBasic<T_DATA>::nbOperations( const Schedule<T_DATA>& schedule ) const {
    NodeSet available = schedule.availableOperations();
    DAG dag = schedule.dag();
    float nb_operations = 0;

    while ( ! available.empty() ) {
      for ( typename NodeSet::const_iterator iter = available.begin();
            iter != available.end(); ++iter ) {
        NodeId id = *iter;
        nb_operations += schedule.nbOperations( id );
        const NodeSet& children = dag.children( id );
        for ( typename NodeSet::const_iterator iter_children = children.begin();
              iter_children != children.end(); ++iter_children ) {
          if ( dag.parents( *iter_children ).size() == 1 ) {
            available.insert( *iter_children );
          }
        }
        dag.eraseNode( id );
        available.erase( iter );
      }
    }

    return nb_operations;
  }


  /** @bried returns an estimation of the number of elementary operations needed
   * to perform the k first ScheduleOperations of a given schedule */
  template<typename T_DATA>
  float SchedulerBasic<T_DATA>::nbOperations
  ( const Schedule<T_DATA>& schedule, unsigned int k ) const {
    NodeSet available = schedule.availableOperations();
    DAG dag = schedule.dag();
    float nb_operations = 0;

    while ( ! available.empty() && k ) {
      for ( typename NodeSet::const_iterator iter = available.begin();
            iter != available.end() && k; ++iter, --k ) {
        NodeId id = *iter;
        nb_operations += schedule.nbOperations( id );
        const NodeSet& children = dag.children( id );
        for ( typename NodeSet::const_iterator iter_children = children.begin();
              iter_children != children.end(); ++iter_children ) {
          if ( dag.parents( *iter_children ).size() == 1 ) {
            available.insert( *iter_children );
          }
        }
        dag.eraseNode( id );
        available.erase( iter );
      }
    }

    return nb_operations;
  }


  /// returns the memory consumption used during the execution of a schedule
  template<typename T_DATA>
  std::pair<long,long>
  SchedulerBasic<T_DATA>::memoryUsage( const Schedule<T_DATA>& schedule ) const {
    NodeSet available = schedule.availableOperations();
    DAG dag = schedule.dag();
    long max_memory = 0;
    long current_memory = 0;

    while ( ! available.empty() ) {
      for ( typename NodeSet::const_iterator iter = available.begin();
            iter != available.end(); ++iter ) {
        NodeId id = *iter;

        std::pair<long,long> mem_op = schedule.memoryUsage( id );
        if (( std::numeric_limits<long>::max() -
              current_memory < mem_op.first ) ||
            ( std::numeric_limits<long>::max() -
              current_memory < mem_op.second ) ) {
          GUM_ERROR( OutOfBounds, "memory usage out of long int range" );
        }
        if ( current_memory + mem_op.first > max_memory )
          max_memory = current_memory + mem_op.first;
        current_memory += mem_op.second;

        const NodeSet& children = dag.children( id );
        for ( typename NodeSet::const_iterator iter_children = children.begin();
              iter_children != children.end(); ++iter_children ) {
          if ( dag.parents( *iter_children ).size() == 1 ) {
            available.insert( *iter_children );
          }
        }
        dag.eraseNode( id );
        available.erase( iter );
      }
    }

    return std::pair<long,long> ( max_memory,current_memory );
  }


  /** @brief returns the memory consumption used during the execution of the
   * k first ScheduleOperations of a given schedule */
  template<typename T_DATA>
  std::pair<long,long> SchedulerBasic<T_DATA>::memoryUsage
  ( const Schedule<T_DATA>& schedule, unsigned int k ) const {
    NodeSet available = schedule.availableOperations();
    DAG dag = schedule.dag();
    long max_memory = 0;
    long current_memory = 0;

    while ( ! available.empty() && k ) {
      for ( typename NodeSet::const_iterator iter = available.begin();
            iter != available.end() && k ; ++iter, --k ) {
        NodeId id = *iter;

        std::pair<long,long> mem_op = schedule.memoryUsage( id );
        if (( std::numeric_limits<long>::max() -
              current_memory < mem_op.first ) ||
            ( std::numeric_limits<long>::max() -
              current_memory < mem_op.second ) ) {
          GUM_ERROR( OutOfBounds, "memory usage out of long int range" );
        }
        if ( current_memory + mem_op.first > max_memory )
          max_memory = current_memory + mem_op.first;
        current_memory += mem_op.second;

        const NodeSet& children = dag.children( id );
        for ( typename NodeSet::const_iterator iter_children = children.begin();
              iter_children != children.end(); ++iter_children ) {
          if ( dag.parents( *iter_children ).size() == 1 ) {
            available.insert( *iter_children );
          }
        }
        dag.eraseNode( id );
        available.erase( iter );
      }
    }

    return std::pair<long,long> ( max_memory,current_memory );
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
