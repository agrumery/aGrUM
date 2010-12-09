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
 * @brief Class containing a schedule of operations to perform on multidims
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/core/debug.h>
#include <agrum/core/inline.h>


namespace gum {


  /// default constructor (construct an empty sequence)
  template <typename T_DATA>
  Schedule<T_DATA>::Schedule () {
    // for debugging purposes
    GUM_CONSTRUCTOR ( Schedule );
  }
    
  
  /// copy constructor
  template <typename T_DATA>  
  Schedule<T_DATA>::Schedule ( const Schedule<T_DATA>& from ) :
    __dag ( from.__dag ),
    __operation2node ( from.__operation2node ),
    __created_multidims ( from.__created_multidims ),
    __operations_with_wrong_parents ( from.__operations_with_wrong_parents ),
    __operations_available ( from.__operations_available ) {
    // for debugging purposes
    GUM_CONS_CPY ( Schedule );

    // we must now copy the operations of "from" into "this"
    const typename Property<ScheduleOperation<T_DATA>*>::onNodes&
      ops = from.__node2operation;
    for ( typename Property<ScheduleOperation<T_DATA>*>::onNodes::const_iterator
            iter = ops.begin(); iter != ops.end(); ++iter ) {
      __node2operation.insert ( iter.key (), (*iter)->newFactory() );
    }

    // update the set of operations involved with each multidim table
    const HashTable<MultiDimId,NodeSet*>& inv = from.__multidim2operations;
    for ( typename HashTable<MultiDimId,NodeSet*>::const_iterator
            iter = inv.begin(); iter != inv.end(); ++iter ) {
      __multidim2operations.insert ( iter.key(), new NodeSet ( **iter ) );
    }
  }


  /// destructor
  template <typename T_DATA>  
  Schedule<T_DATA>::~Schedule () {
    // for debugging purposes
    GUM_DESTRUCTOR ( Schedule );

    // remove all the operations that were stored
    for ( typename Property<ScheduleOperation<T_DATA>*>::onNodes::const_iterator
            iter = __node2operation.begin(); iter != __node2operation.end();
          ++iter ) {
      delete *iter;
    }

    // remove the sets of operations involved with each multidim table
    for ( typename HashTable<MultiDimId,NodeSet*>::const_iterator
            iter = __multidim2operations.begin();
          iter != __multidim2operations.end(); ++iter ) {
      delete *iter;
    }
  }


  /// copy operator
  template <typename T_DATA>  
  Schedule<T_DATA>& Schedule<T_DATA>::operator= ( const Schedule& from ) {
    // avoid self assignment
    if ( this != &from ) {
      // remove all the operations that were stored
      for ( typename Property<ScheduleOperation<T_DATA>*>::onNodes::const_iterator
              iter = __node2operation.begin(); iter != __node2operation.end();
            ++iter ) {
        delete *iter;
      }
      // remove the sets of operations involved with each multidim table
      for ( typename HashTable<MultiDimId,Set<NodeId>*>::const_iterator
              iter = __multidim2operations.begin();
            iter != __multidim2operations.end(); ++iter ) {
        delete *iter;
      }
 
      // fill all the data structures with the elements of from
      __dag = from.__dag;
      __operation2node = from.__operation2node;;
      __created_multidims = from.__created_multidims;
      __operations_with_wrong_parents = from.__operations_with_wrong_parents;
      __operations_available = from.__operations_available;

      __node2operation.clear ();
      const typename Property<ScheduleOperation<T_DATA>*>::onNodes&
        ops = from.__node2operation;
      for ( typename Property<ScheduleOperation<T_DATA>*>::onNodes::const_iterator
              iter = ops.begin(); iter != ops.end(); ++iter ) {
        __node2operation.insert ( iter.key (), (*iter)->newFactory() );
      }

      // update the set of operations involved with each multidim table
      __multidim2operations.clear ();
      const HashTable<MultiDimId,NodeSet*>& inv = from.__multidim2operations;
      for ( typename HashTable<MultiDimId,NodeSet*>::const_iterator
              iter = inv.begin(); iter != inv.end(); ++iter ) {
        __multidim2operations.insert ( iter.key(), new NodeSet ( **iter ) );
      }
    }

    return *this;
  }


  /// inserts an operation to be scheduled
  template <typename T_DATA>  
  NodeId Schedule<T_DATA>::insert ( const ScheduleOperation<T_DATA>& op ) {
    // create a copy of the operation
    ScheduleOperation<T_DATA>* operation = op.newFactory ();
    
    // create a new node for the operation in the DAG
    NodeId node_id = __dag.insertNode ();

    // assign the operation to the new node
    __node2operation.insert ( node_id, operation );
    __operation2node.insert ( operation->id(), node_id );

    // get the list of multidims passed in arguments and determine which ones
    // are abstract. If there are some abstract multidims, then the node we
    // just created must have parents. Try to add these arcs and, if some
    // parents have not been created yet, indicate it in the
    // __operations_with_wrong_parents list
    bool operation_available = true;
    const Sequence<const ScheduleMultiDim<T_DATA>*>&
      parents = operation->multiDimArgs ();
    for ( typename Sequence<const ScheduleMultiDim<T_DATA>*>::const_iterator
            iter = parents.begin(); iter != parents.end(); ++iter ) {
      if ( (*iter)->isAbstract () ) {
        // here we shall have a parent in the graph
        operation_available = false;
        MultiDimId multidim_id = (*iter)->id ();
        if ( __created_multidims.exists ( multidim_id ) ) {
          __dag.insertArc ( __created_multidims[multidim_id], node_id );
        }
        else {
          __operations_with_wrong_parents.insert ( node_id );
          break;
        }
      }
    }

    // if the operation is available to be processed, mark it as such
    if ( operation_available ) {
      __operations_available.insert ( node_id );
    }

    // now we shall find whether, upon executing the operation, new multidim
    // tables are created
    NodeSet *involved_ops;
    const Sequence<const ScheduleMultiDim<T_DATA>*>&
      created_tables = operation->multiDimResults ();
    for ( typename Sequence<const ScheduleMultiDim<T_DATA>*>::const_iterator
            iter = created_tables.begin(); iter != created_tables.end(); ++iter ) {
      MultiDimId table_id = (*iter)->id ();
      if ( (*iter)->isAbstract () ) {
        __created_multidims.insert ( table_id, node_id );
      }
      
      if ( ! __multidim2operations.exists ( table_id ) ) {
        involved_ops = __multidim2operations.insert ( table_id, new NodeSet );
      }
      else {
        involved_ops = __multidim2operations[table_id];
      }
      involved_ops->insert ( node_id );
    }

    // update __multidim2operations with the arguments passed to the newly
    // added operation
    for ( typename Sequence<const ScheduleMultiDim<T_DATA>*>::const_iterator
            iter = parents.begin(); iter != parents.end(); ++iter ) {
      MultiDimId table_id = (*iter)->id ();
      if ( ! __multidim2operations.exists ( table_id ) ) {
        involved_ops = __multidim2operations.insert ( table_id, new NodeSet );
      }
      else {
        involved_ops = __multidim2operations[table_id];
      }
      involved_ops->insert ( node_id );
    }
    
    return node_id;
  }

  
  /// updates the set of parents for the nodes whoses parents are not correct yet
  template <typename T_DATA>  
  void Schedule<T_DATA>::__updateWrongParents () const {
    // parse all the nodes whose parents sets are incorrect
    for ( typename NodeSet::const_iterator
            iter = __operations_with_wrong_parents.begin();
          iter != __operations_with_wrong_parents.end(); ++iter ) {
      // get the arguments passed to *iter and check that those that are abstract
      // multidims belong to the schedule
      const Sequence<const ScheduleMultiDim<T_DATA>*>& args =
        __node2operation[*iter]->multiDimArgs ();
      bool still_wrong = false;
      for ( typename Sequence<const ScheduleMultiDim<T_DATA>*>::const_iterator
              iter_args = args.begin(); iter_args != args.end(); ++iter_args ) {
        if ( (*iter_args)->isAbstract () &&
             ! __created_multidims.exists ( (*iter_args)->id() ) ) {
          still_wrong = true;
          break;
        }
      }

      // if the operation is not "still_wrong" anymore, then we should remove
      // it from __operations_with_wrong_parents and update its parents set
      // appropriately. In addition, if there is no parent, then we should
      // indicate that the operation is now available
      if ( ! still_wrong ) {
         unsigned int nb_parents = 0;
        for ( typename Sequence<const ScheduleMultiDim<T_DATA>*>::const_iterator
                iter_args = args.begin(); iter_args != args.end(); ++iter_args ) {
          if ( (*iter_args)->isAbstract () ) {
            __dag.insertArc ( __created_multidims[(*iter_args)->id()], *iter );
            ++nb_parents;
          }
        }

        // check that there is no parent
        if ( ! nb_parents ) {
          __operations_available.insert ( *iter );
        }
        
        __operations_with_wrong_parents.erase ( iter );
      }
    }
  }
  
  
  /** @brief adds a constraint indicating that an operation cannot be performed 
   * before another one */
  template <typename T_DATA>  
  INLINE void
  Schedule<T_DATA>::forceAfter ( NodeId op_to_force, NodeId op_before ) {
    // first, add the constraint into the graph
    __dag.insertArc ( op_before, op_to_force );

    // if op_to_force was available, it is not anymore
    __operations_available.erase ( op_to_force );
  }


  /** @brief adds a constraint indicating that an operation cannot be performed 
   * before another one */
  template <typename T_DATA>  
  INLINE void
  Schedule<T_DATA>::forceAfter ( const ScheduleOperation<T_DATA>& op_to_force,
                                 const ScheduleOperation<T_DATA>& op_before ) {
    forceAfter ( __operation2node[op_to_force.id ()],
                 __operation2node[op_before.id ()] );
  }
  

  /** @brief adds a constraint indicating that an operation cannot be performed 
   * before a set of operations */
  template <typename T_DATA>  
  void Schedule<T_DATA>::forceAfter ( NodeId op_to_force,
                                      const NodeSet& ops_before ) {
    for ( typename NodeSet::const_iterator iter = ops_before.begin();
          iter != ops_before.end(); ++iter ) {
      if ( *iter != op_to_force ) {
        forceAfter ( op_to_force, *iter );
      }
    }
  }


  /** @brief adds a constraint indicating that an operation cannot be performed 
   * before a set of operations */
  template <typename T_DATA>  
  void Schedule<T_DATA>::forceAfter
  ( const ScheduleOperation<T_DATA>& op_to_force,
    const Set<const ScheduleOperation<T_DATA>*>& ops_before ) {
    for ( typename Set<const ScheduleOperation<T_DATA>*>::const_iterator
            iter = ops_before.begin(); iter != ops_before.end(); ++iter ) {
      if ( **iter != op_to_force ) {
        forceAfter ( op_to_force, **iter );
      }
    }
  }

  
  /** @brief adds a constraint indicating that an operation must be performed 
   * before another one */
  template <typename T_DATA>  
  INLINE void
  Schedule<T_DATA>::forceBefore ( NodeId op_to_force, NodeId op_after ) {
    // first, add the constraint into the graph
    __dag.insertArc ( op_to_force, op_after );

    // if op_to_force was available, it is not anymore
    __operations_available.erase ( op_after );
  }


  /** @brief adds a constraint indicating that an operation must be performed 
   * before another one */
  template <typename T_DATA>  
  INLINE void
  Schedule<T_DATA>::forceBefore ( const ScheduleOperation<T_DATA>& op_to_force,
                                  const ScheduleOperation<T_DATA>& op_after ) {
    forceBefore ( __operation2node[op_to_force.id ()],
                  __operation2node[op_after.id ()] );
  }

  
  /** @brief adds a constraint indicating that an operation must be performed 
   * before a set of operations */
  template <typename T_DATA>  
  void
  Schedule<T_DATA>::forceBefore ( NodeId op_to_force, const NodeSet& ops_after ) {
    for ( typename NodeSet::const_iterator iter = ops_after.begin();
          iter != ops_after.end(); ++iter ) {
      if ( *iter != op_to_force ) {
        forceBefore ( op_to_force, *iter );
      }
    }
  }


  /** @brief adds a constraint indicating that an operation must be performed 
   * before a set of operations */
  template <typename T_DATA>  
  void Schedule<T_DATA>::forceBefore
  ( const ScheduleOperation<T_DATA>& op_to_force,
    const Set<const ScheduleOperation<T_DATA>*>& ops_after ) {
    for ( typename Set<const ScheduleOperation<T_DATA>*>::const_iterator
            iter = ops_after.begin(); iter != ops_after.end(); ++iter ) {
      if ( **iter != op_to_force ) {
        forceBefore ( op_to_force, **iter );
      }
    }
  }

  
  /// returns the set of operations involving a given multidim table
  template <typename T_DATA>  
  INLINE const NodeSet&
  Schedule<T_DATA>::operationsInvolving
  ( const ScheduleMultiDim<T_DATA>& table) const {
    return *( __multidim2operations[table.id()] );
  }


  /// returns the set of operations involving a given multidim table
  template <typename T_DATA>  
  INLINE const NodeSet&
  Schedule<T_DATA>::operationsInvolving ( MultiDimId table_id) const {
    return *( __multidim2operations[table_id] );
  }
  
 
  /// returns a DAG indicating in which order the operations can be performed
  template <typename T_DATA>  
  INLINE const DAG& Schedule<T_DATA>::dag () const {
    // first update the set of parents of the nodes of the graph whose parents
    // were not set correctly
    __updateWrongParents ();
    return __dag;
  }
  

  /// returns the scheduleOperation corresponding to an id in the DAG
  template <typename T_DATA>  
  INLINE const ScheduleOperation<T_DATA>&
  Schedule<T_DATA>::operation ( NodeId node_id ) const {
    return *( __node2operation[ node_id ] );
  }
  

  /// returns the id associated to a given operation
  template <typename T_DATA>  
  INLINE NodeId
  Schedule<T_DATA>::nodeId ( const ScheduleOperation<T_DATA>& op ) const {
    return __operation2node[ op.id() ];
  }
  

  /// resturns the association between operations anf nodeIds
  template <typename T_DATA>  
  INLINE const typename Property<const ScheduleOperation<T_DATA>*>::onNodes&
  Schedule<T_DATA>::operations () const {
    return reinterpret_cast<typename
      Property<const ScheduleOperation<T_DATA>*>::onNodes&> ( __node2operation );
  }
  

  /// returns the set of ScheduleOperations that can be executed at once
  template <typename T_DATA>  
  INLINE const NodeSet& Schedule<T_DATA>::availableOperations () const {
    // update the set of parents
    __updateWrongParents ();
    return __operations_available;
  }

    
  /// executes a given operation
  template <typename T_DATA>
  void Schedule<T_DATA>::execute ( NodeId id ) {
    // update the parents of the sets of nodes which were not correct
    // !!! it is important to execute the following method before the execution
    // of operation id: this guarantees that the children of id with exactly
    // one parent (i.e., id) are now available to be processed
    __updateWrongParents ();

    // before executing an operation, check that the operation is available
    if ( __dag.parents ( id ).size() != 0 ) {
      GUM_ERROR ( OperationNotAllowed, "the operation cannot be executed yet" );
    }
    
    // actually execute the operation
    __node2operation[id]->execute ();

    // now update the availability of the children of id: a child is available
    // if and only if it has only one parent
    const NodeSet& children = __dag.children ( id );
    for ( NodeSet::const_iterator iter = children.begin ();
          iter != children.end (); ++iter ) {
      NodeId child = *iter;
      if ( __dag.parents ( child ).size() == 1 ) {
        __operations_available.insert ( child );
      }
    }

    // remove the operation's node and its adjacent arcs
    __dag.eraseChildren ( id );
    __dag.eraseNode ( id );
    delete __node2operation[id];
    __operation2node.erase ( __node2operation[id]->id() );
    __node2operation.erase ( id );
    __operations_available.erase ( id );
  }


  /// executes a given operation
  template <typename T_DATA>
  INLINE void Schedule<T_DATA>::execute ( const ScheduleOperation<T_DATA>& op ) {
    execute ( __operation2node[op.id()] );
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */


