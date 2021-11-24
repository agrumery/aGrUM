/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/** @file
 * @brief Class containing a schedule of operations to perform
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

namespace gum {

  /// returns the allocator used by the schedule
  template < template < typename > class ALLOC >
  INLINE typename Schedule< ALLOC >::allocator_type
     Schedule< ALLOC >::get_allocator() const {
    return *this;
  }


  /// returns the graph that would obtain when no operation is performed
  template < template < typename > class ALLOC >
  DAG Schedule< ALLOC >::_fullDAG_() const {
    // if no operation has been performed yet, return _dag_
    if (_dag_.sizeNodes() == _node2op_.size())
      return _dag_;

    // now, we should reconstruct the graph. First, we add all the nodes
    DAG dag;
    for (auto iter=_node2op_.cbegin(); iter != _node2op_.cend(); ++iter) {
      dag.addNodes(iter.first());
    }

    // add the arcs toward node or from node if another node deletes one of its
    // arguments
    for (auto iter=_node2op_.cbegin(); iter != _node2op_.cend(); ++iter) {
      const NodeId node = iter.first();
      const auto   op   = iter.second();

      for (const auto arg: op->args()) {
        // get the arguments used by node/op that are not sources and add arcs
        // from the operations/nodes that created them to the current node.
        const auto arg_location = _multidim_location_[arg].first;
        if (arg_location != nullptr) {
          const NodeId arg_node = _node2op_.first(arg_location);
          dag.addArc(arg_node, node);
        }

        // If there exists another operation that deletes Parameter arg, then we
        // should add an arc from node to this operation (as node/op should be
        // executed before the argument is deleted).
        if (_deleted_multidim2node_.existsFirst(arg)) {
          const NodeId other_node = _deleted_multidim2node_.second(arg);
          if (other_node != node) { dag.addArc(node, other_node); }
        }

        // if node/op deletes its parameters, then we should add arcs from all
        // the operations that use arg to node: actually, these nodes use arg
        // but do not delete it, hence they should be executed before node/op
        if (op->implyDeletion()) {
          for (const auto other_node: _multidim2nodes_[arg]) {
            if (other_node != node) {
              dag.addArc(other_node, node);
            }
          }
        }
      }
    }

    return dag;
  }


  /// a function to copy the content of a schedule into another one
  template < template < typename > class ALLOC >
  void Schedule< ALLOC >::_copy_(
     const Schedule< ALLOC >& from,
     const typename Schedule< ALLOC >::allocator_type& alloc) {
    // copy the dag-related structures
    _dag_   = from._dag_;
    _newId_ = from._newId_;

    // here, we create the mapping from all the ScheduleMultiDims contained into
    // from to those contained into this
    HashTable< const IScheduleMultiDim< ALLOC >*,
               const IScheduleMultiDim< ALLOC >* >
       multidim_from2this (from._multidim2id_.size());

    // we copy the source multidims, i.e., those that are not the result of any
    // operation, and we store the mapping from these IScheduleMultiDim contained
    // into from to those contained into this
    for (const auto& source : from._multidim_location_) {
      if (source.second.first == nullptr) { // here, this is a source
        IScheduleMultiDim< ALLOC >* new_multidim = source.first->clone(alloc);
        multidim_from2this.insert(source.first, new_multidim);
        _multidim_location_.insert(new_multidim, source.second);
        _multidim2id_.insert(new_multidim, new_multidim->id());
        _multidim2nodes_.insert(new_multidim, NodeSet());
      }
    }


    // get a topological order of from's full dag, i.e. the dag corresponding to
    // all the operations of that schedule, as if those had not been executed.
    // This is the order in which we will reconstruct all the operations. This
    // will allow us to construct the ScheduleMultiDims in the right order.
    const Sequence< NodeId > nodes_sequence =
       (from._dag_.sizeNodes() == from._node2op_.size())
          ? from._dag_.topologicalOrder() : from._fullDAG_().topologicalOrder();

    // we can now create all the operations contained in this in the same
    // topological order as from: this enables to create all the ScheduleMultiDims
    // needed by a ScheduleOperation before creating the ScheduleOperation itself
    for (const auto node : nodes_sequence) {
      // get the "from" operation and clone it
      const ScheduleOperation< ALLOC >* from_op = from._node2op_.second(node);
      ScheduleOperation< ALLOC >* new_op = from_op->clone(alloc);
      _node2op_.insert(node, new_op);

      // we get the ScheduleMultiDims passed as parameters to from_op and we
      // compute the corresponding ScheduleMultiDims in the current schedule,
      // and we update accordingly the parameters of new_op. In addition,
      // we store the information that the ScheduleMultiDims are used by our
      // new operation
      const Sequence< const IScheduleMultiDim< ALLOC >* >
         from_args = from_op->args();
      Sequence< const IScheduleMultiDim< ALLOC >* > new_args (from_args.size());
      for (auto from_arg: from_args) {
        const auto& new_arg = multidim_from2this[from_arg];
        new_args << new_arg;
        _multidim2nodes_[new_arg].insert(node);
      }
      new_op->updateArgs(new_args);

      // if the operation deletes its parameters, then keep this information
      if (new_op->implyDeletion()) {
        for (auto new_arg: new_args) {
          _deleted_multidim2node_.insert(new_arg, node);
        }
      }

      // we get the ScheduleMultiDims resulting from from_op and we compute their
      // mapping with those resulting from new_op. We also store the location and
      // id of the new results
      const Sequence< const IScheduleMultiDim< ALLOC >* >
         from_res = from_op->results();
      const Sequence< const IScheduleMultiDim< ALLOC >* >
         new_res = new_op->results();
      for (Idx i = Idx(0), end = from_res.size(); i < end; ++i) {
        multidim_from2this.insert(from_res[i], new_res[i]);
        _multidim_location_.insert(
           new_res[i],
           std::pair<ScheduleOperation< ALLOC >*, Idx >(new_op, i));
        _multidim2id_.insert(new_res[i], new_res[i]->id());
        _multidim2nodes_.insert(new_res[i], NodeSet());
      }
    }
  }


  /// a function to delete from memory the allocated content of a schedule
  template < template < typename > class ALLOC >
  void Schedule< ALLOC >::_destroy_() {
    // remove all the operations of the schedule
    ALLOC< ScheduleOperation< ALLOC > > op_alloc(this->get_allocator());
    for (auto iter = _node2op_.begin(), end = _node2op_.end();
         iter != end; ++iter) {
      iter.second()->~ScheduleOperation< ALLOC >();
      op_alloc.deallocate(iter.second(), 1);
    }

    // remove all the source ScheduleMultiDims
    ALLOC< IScheduleMultiDim< ALLOC > > multidim_alloc(this->get_allocator());
    for (auto& source: _multidim_location_) {
      if (source.second.first == nullptr) {
        IScheduleMultiDim< ALLOC >* multidim =
           const_cast< IScheduleMultiDim< ALLOC >* > (source.first);
        multidim->~IScheduleMultiDim< ALLOC >();
        multidim_alloc.deallocate(multidim, 1);
      }
    }
  }


  /// empty the schedule, i.e., remove its content
  template < template < typename > class ALLOC >
  void Schedule< ALLOC >::clear() {
    // remove all the allocated objects
    _destroy_();

    // clear all the data structures
    _dag_.clear();
    _newId_ = NodeId(0);
    _node2op_.clear();
    _multidim_location_.clear();
    _multidim2id_.clear();
    _multidim2nodes_.clear();
    _deleted_multidim2node_.clear();
  }


  /// default constructor (construct an empty set of operations)
  template < template < typename > class ALLOC >
  Schedule< ALLOC >::Schedule(
     const typename Schedule< ALLOC >::allocator_type& alloc):
      ALLOC< Idx >(alloc) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Schedule);
  }


  /// copy constructor with a given allocator
  template < template < typename > class ALLOC >
  Schedule< ALLOC >::Schedule(
     const Schedule< ALLOC >&                          from,
     const typename Schedule< ALLOC >::allocator_type& alloc) {
    // really perform the copy
    _copy_(from, alloc);

    // for debugging purposes
    GUM_CONS_CPY(Schedule);
   }


  /// copy constructor
  template < template < typename > class ALLOC >
  INLINE Schedule< ALLOC >::Schedule(const Schedule< ALLOC >& from) :
     Schedule< ALLOC >(from, from.get_allocator()) {}


  /// move constructor with a given allocator
  template < template < typename > class ALLOC >
  Schedule< ALLOC >::Schedule(
     Schedule< ALLOC >&& from,
     const typename Schedule< ALLOC >::allocator_type& alloc) :
      _dag_(std::move(from._dag_)),
      _newId_(from._newId_),
      _node2op_(std::move(from._node2op_)),
      _multidim_location_(std::move(from._multidim_location_)),
      _multidim2id_(std::move(from._multidim2id_)),
      _multidim2nodes_(std::move(from._multidim2nodes_)),
      _deleted_multidim2node_(std::move(from._deleted_multidim2node_)) {
    // empty properly from
    from._newId_ = NodeId(0);
    from._dag_.clear();
    from._node2op_.clear();
    from._multidim_location_.clear();
    from._multidim2id_.clear();
    from._multidim2nodes_.clear();
    from._deleted_multidim2node_.clear();

    // for debugging purposes
    GUM_CONS_MOV(Schedule);
  }


  /// move constructor
  template < template < typename > class ALLOC >
  INLINE Schedule< ALLOC >::Schedule(Schedule< ALLOC >&& from) :
      Schedule< ALLOC >(std::move(from), from.get_allocator()) {}


  /// virtual copy constructor with a given allocator
  template < template < typename > class ALLOC >
  Schedule< ALLOC >* Schedule< ALLOC >::clone(
     const typename Schedule< ALLOC >::allocator_type& alloc)
     const {
    ALLOC< Schedule< ALLOC > > allocator(alloc);
    Schedule< ALLOC >* new_sched = allocator.allocate(1);
    try {
      new ((void*)new_sched) Schedule< ALLOC >(*this, alloc);
    } catch (...) {
      allocator.deallocate(new_sched, 1);
      throw;
    }

    return new_sched;
  }


  /// virtual copy constructor with a given allocator
  template < template < typename > class ALLOC >
  INLINE Schedule< ALLOC >* Schedule< ALLOC >::clone() const {
    return clone(this->get_allocator());
  }


  /// destructor
  template < template < typename > class ALLOC >
  Schedule< ALLOC >::~Schedule() {
    // really destroy all the allocated objects contained into the schedule
    _destroy_();

    // for debugging purposes
    GUM_DESTRUCTOR(Schedule);
  }


  /// copy operator
  template < template < typename > class ALLOC >
  Schedule< ALLOC >& Schedule< ALLOC >::operator=(const Schedule< ALLOC >& from) {
    // avoid self assignment
    if (this != &from) {
      // remove all the operations that are currently stored
      clear();

      // perform the copy
      _copy_(from, get_allocator());
    }

    return *this;
  }


  /// move operator
  template < template < typename > class ALLOC >
  Schedule< ALLOC >& Schedule< ALLOC >::operator=(Schedule< ALLOC >&& from) {
    // avoid self assignment
    if (this != &from) {
      // remove all the operations that are currently stored
      clear();

      // move from
      _newId_                 = from._new_Id_;
      _dag_                   = std::move(from._dag_);
      _node2op_               = std::move(from._node2op_);
      _multidim_location_     = std::move(from._multidim_location_);
      _multidim2id_           = std::move(from._multidim2id_);
      _multidim2nodes_        = std::move(from._multidim2nodes_);
      _deleted_multidim2node_ = std::move(from._deleted_multidim2node_);

      // empty properly from
      from._new_Id_ = NodeId(0);
      from._dag_.clear();
      from._node2op_.clear();
      from._multidim_location_.clear();
      from._multidim2id_.clear();
      from._multidim2nodes_.clear();
      from._deleted_multidim2node_.clear();
    }

    return *this;
  }


  /// operator ==
  template < template < typename > class ALLOC >
  bool Schedule< ALLOC >::operator==(const Schedule< ALLOC >& from) const {
    // check if both schedules have the same graph and the same set of
    // operation ids
    if ((_dag_ != from._dag_) || (_node2op_.size() != from._node2op_.size()))
      return false;
    for (auto iter = _node2op_.cbegin(); iter != _node2op_.cend(); ++iter)
      if (! from._node2op_.existsFirst(iter.first()))
        return false;

    // map "this"'s operations and source multidims to those of "from"
    HashTable< const ScheduleOperation< ALLOC >*,
               const ScheduleOperation< ALLOC >* >
       this_op2from(_node2op_.size());
    Bijection< const IScheduleMultiDim< ALLOC >*,
               const IScheduleMultiDim< ALLOC >* >
       this_multidim2from(_multidim2nodes_.size());

    // get a topological order of the full graph. We will use it to compare the
    // parameters of the operations. This order enforces that the parameters are
    // already known before we examine them in the operations.
    const Sequence< NodeId > order =
       (_dag_.sizeNodes() == _node2op_.size())
        ? _dag_.topologicalOrder() : _fullDAG_().topologicalOrder();

    // check if all the operations have the same type and the same parameters
    for (const auto node : order) {
      // get the operations corresponding to node
      const ScheduleOperation< ALLOC >* this_op = _node2op_.second(node);
      const ScheduleOperation< ALLOC >* from_op = from._node2op_.second(node);

      // check if they perform the same operations
      if (!this_op->isSameOperation(*from_op))
        return false;

      // check that the content of their parameters are the same
      const Sequence< const IScheduleMultiDim< ALLOC >* >
         this_args = this_op->args();
      const Sequence< const IScheduleMultiDim< ALLOC >* >
         from_args = from_op->args();
      if (this_args.size() != from_args.size())
        return false;

      for(Idx i = 0, end = this_args.size(); i < end; ++i) {
        const auto& this_location = _multidim_location_[this_args[i]];
        const auto& from_location = from._multidim_location_[from_args[i]];

        if (this_location.first != nullptr) {
          // here, this's and from's locations are the same if they correspond
          // to the same operation, with the same argument's index
          if ((from_location.first == nullptr) ||
              (this_location.second != from_location.second) ||
              (from_location.first != this_op2from[this_location.first]))
            return false;
        }
        else {
          // here, this_args[i] and from_args[i] should both be source multidims
          if (from_location.first != nullptr) return false;

          // here, we know that they are truly source multidims. If we have
          // already examined these multidims, we have already put the mapping
          // between this's and from's multidim into Bijection this_multidim2from
          bool found = false;
          try {
            if (this_multidim2from.second(this_args[i]) != from_args[i])
              return false;
            else
              found = true;
          } catch (NotFound&) {}

          if (!found) {
            // here, this is the first time we encounter both multidims. Hence,
            // we should check that their content are equal and store the two
            // ScheduleMultiDim into Structure this_multidim2from
            if (!this_args[i]->hasSameVariables(*from_args[i])
                || !this_args[i]->hasSameContent(*from_args[i]))
              return false;

            this_multidim2from.insert(this_args[i], from_args[i]);
          }
        }
      }

      // here, both operations have been identified as identical.
      // we should therefore store them into mapping table this_op2from
      this_op2from.insert(this_op, from_op);
    }

    // here, source multidims and operations, including their parameters, are
    // identical. Hence both schedules should be considered equal
    return true;
  }


  /// operator !=
  template < template < typename > class ALLOC >
  INLINE bool Schedule< ALLOC >::operator!=(const Schedule< ALLOC >& from) const {
    return !operator==(from);
  }


  /// inserts a new table as a source multidim
  template < template < typename > class ALLOC >
  template < typename TABLE >
  const IScheduleMultiDim< ALLOC >*
     Schedule< ALLOC >::insertTable(const TABLE& table,
                                    const bool   copy,
                                    const Idx    id) {
    // if the schedule already contains the id, throw an error
    if (_multidim2id_.existsSecond(id)) {
      GUM_ERROR(DuplicateScheduleMultiDim,
                "A table with Id "
                   << id << " already exists in the schedule");
    }

    // allocate the new table within the schedule
    ALLOC< ScheduleMultiDim< TABLE, ALLOC > > alloc = get_allocator();
    ScheduleMultiDim< TABLE, ALLOC >* new_multidim = alloc.allocate(1);
    try {
      new ((void*)new_multidim)
         ScheduleMultiDim< TABLE, ALLOC >(table, copy, id, get_allocator());
    } catch (...) {
      alloc.deallocate(new_multidim, 1);
      throw;
    }

    // keep track that this is a source multidim
    _multidim2nodes_.insert(new_multidim, NodeSet());
    _multidim_location_.insert(
       new_multidim,
       std::pair< ScheduleOperation< ALLOC >*, Idx >(nullptr, Idx(0)));
    _multidim2id_.insert(new_multidim, new_multidim->id());

    return new_multidim;
  }


  /// inserts an already constructed ScheduleMultiDim
  template < template < typename > class ALLOC >
  const IScheduleMultiDim< ALLOC >*
     Schedule< ALLOC >::insertScheduleMultiDim(
     const IScheduleMultiDim< ALLOC>& multidim) {
    // check that the ScheduleMultiDim neither already belongs to the schedule
    // nor contains an abstract table: since it is a source multidim, it will
    // never be computed by the schedule. Hence if it is abstract, it will not
    // be possible to execute the schedule
    if (_multidim2id_.existsSecond(multidim.id())) {
       GUM_ERROR(DuplicateScheduleMultiDim,
                "A ScheduleMultiDim with Id "
                   << multidim.id() << " already exists in the schedule");
    }
    if (multidim.isAbstract()) {
      GUM_ERROR(AbstractScheduleMultiDim,
                "It is impossible to insert an abstract ScheduleMultiDim "
                   << "into a Schedule");
    }

    // now, everything is ok, so we should insert a copy of the ScheduleMultiDim
    // into the schedule
    IScheduleMultiDim< ALLOC >* new_multidim = multidim.clone(get_allocator());
    _multidim2nodes_.insert(new_multidim, NodeSet());
    _multidim_location_.insert(
       new_multidim,
       std::pair< ScheduleOperation< ALLOC >*, Idx >(nullptr, Idx(0)));
    _multidim2id_.insert(new_multidim, new_multidim->id());

    return new_multidim;
  }


  /// returns the adjective corresponding to a parameter index (1st, 2nd, etc.)
  template < template < typename > class ALLOC >
  std::string Schedule< ALLOC >::_paramString_(Idx i) const {
    if (i == 0) return "1st";
    else if (i == 1) return "2nd";
    else if (i == 2) return "3rd";

    std::stringstream str;
    str << (i+1) << "th";
    return str.str();
  }


  /// inserts an operation into the schedule
  template < template < typename > class ALLOC >
  const ScheduleOperation< ALLOC >&
     Schedule< ALLOC >::insertOperation(const ScheduleOperation< ALLOC >& op) {
    // check that the parameters of the operation already belong to the schedule.
    // to do so, it is sufficient to check that their ids belong to the schedule
    const Sequence< const IScheduleMultiDim< ALLOC >* >& op_args = op.args();
    for (Idx i = Idx(0), end = op_args.size(); i < end; ++i) {
      if (!_multidim2id_.existsSecond(op_args[i]->id())) {
        GUM_ERROR(UnknownScheduleMultiDim,
                  "Schedule::insertOperation: the "
                     << _paramString_(i+1) << " (id: " << op_args[i]->id()
                     << " operation's argument does not already belong to"
                     << " the schedule");
      }
    }

    // if another operation deletes some parameters used by the new operation, we
    // should check that this operation has not been executed yet. In this case,
    // we will enforce that the new operation is performed before the deletion.
    // In addition, we should also check that the new operation does not delete
    // its parameters (else those would be deleted twice)
    for (Idx i = Idx(0), end = op_args.size(); i < end; ++i) {
      const IScheduleMultiDim< ALLOC >* arg = op_args[i];
      if (_deleted_multidim2node_.existsFirst(arg) &&
          (_node2op_.second(_deleted_multidim2node_.second(arg))->isExecuted() ||
           op.implyDeletion())) {
        GUM_ERROR(OperationNotAllowed,
                "Schedule::insertOperation: The operation deletes its "
                     << _paramString_(i+1)
                     << " argument, already deleted by another operation.");
      }
    }

    // finally, if the new operation deletes its parameters and has already
    // been executed, we should check that none of its parameters are used
    // by any other operation not performed yet
    if (op.implyDeletion() && op.isExecuted()) {
      for (Idx i = Idx(0), end = op_args.size(); i < end; ++i) {
        const auto arg = _multidim2id_.first(op_args[i]->id());
        for (const auto using_node: _multidim2nodes_[arg]) {
          if (!_node2op_.second(using_node)->isExecuted()) {
            GUM_ERROR(OperationNotAllowed,
                      "Schedule::insertOperation: the operation has"
                         << " deleted its "
                         << _paramString_(i+1)
                         << " argument, which is used by another operation"
                         << " not executed yet.");
          }
        }
      }
    }

    // here, we know that we can safely add the new operation into the schedule

    // create a copy of the operation. Remember that the parameters of
    // ScheduleOperations are pointers to ScheduleMultiDim. Therefore, we need
    // to map the pointers of op_args to pointers of the corresponding
    // ScheduleMultiDim in our new operation
    ScheduleOperation< ALLOC >* new_op = op.clone();
    Sequence< const IScheduleMultiDim< ALLOC >* > new_args(op_args.size());
    for (Idx i = Idx(0), end = op_args.size(); i < end; ++i) {
      try {
        new_args << _multidim2id_.first(op_args[i]->id());
      } catch (NotFound&) {
        // deallocate everything we have allocated
        ALLOC< ScheduleOperation< ALLOC > > alloc(this->get_allocator());
        new_op->~ScheduleOperation< ALLOC >();
        alloc.deallocate(new_op, 1);

        GUM_ERROR(UnknownScheduleMultiDim,
                  "the " << _paramString_(i+1)
                         << " argument of the operation is not known by"
                         << " the schedule");
      }
    }
    new_op->updateArgs(new_args);

    // everything is ok, so we should add the operation to the data structures
    const NodeId new_node = ++_newId_;
    _dag_.addNodeWithId(new_node);
    _node2op_.insert(new_node, new_op);

    // keep track that the new_node uses its arguments
    for (const auto new_arg: new_args) {
      _multidim2nodes_[new_arg].insert(new_node);
    }

    // if the operation deletes its arguments, keep track of this information.
    if (new_op->implyDeletion()) {
      for (const auto new_arg: new_args) {
        _deleted_multidim2node_.insert(new_arg, new_node);
      }
    }

    // keep track of the locations and ids of new_op's results
    const Sequence< const IScheduleMultiDim< ALLOC >* > new_results =
       new_op->results();
    for (Idx i = Idx(0), end = new_results.size(); i < end; ++i) {
      _multidim_location_.insert(
         new_results[i], std::pair< ScheduleOperation< ALLOC >*, Idx >(new_op, i));
      _multidim2id_.insert(new_results[i], new_results[i]->id());
      _multidim2nodes_.insert(new_results[i], NodeSet());
    }


    // now we should try to update Graph _dag_.
    // if the new operation has already been executed, we should remove it from
    // _dag_ and no other update is needed
    if (new_op->isExecuted()) {
      _dag_.eraseNode(new_node);
      return *new_op;
    }


    // Here, the new operation has not been executed yet, so we should update the
    // graph and the set of available operations (operations that delete their
    // arguments could have been available but can now become unavailable due to
    // the fact that the new operation uses these arguments)
    for (const auto arg: new_args) {
      // If there exists another operation that deletes Parameter arg, then we
      // should add an arc from new_node to this operation (as new_op should be
      // executed before the argument is deleted).
      if (_deleted_multidim2node_.existsFirst(arg)) {
        const NodeId other_node = _deleted_multidim2node_.second(arg);
        if (other_node != new_node) {
          _dag_.addArc(new_node, other_node);
        }
      }

      // if Parameter arg has been created by another operation, then new_node
      // should be a child of this node
      const auto arg_location = _multidim_location_[arg].first;
      if (arg_location != nullptr) {
        const NodeId parent_node = _node2op_.first(arg_location);
        if ((parent_node != new_node) && !arg_location->isExecuted()) {
          _dag_.addArc(parent_node, new_node);
        }
      }

      // if new_op deletes its parameters, then we should add arcs from all
      // the operations that use Parameter arg to new_node: actually, these nodes
      // use the parameter but do not delete it, hence they should be executed
      // before new_op
      if (new_op->implyDeletion()) {
        for (const auto other_node: _multidim2nodes_[arg]) {
          if ((other_node != new_node)
              && !_dag_.existsArc(other_node, new_node)
              && !_node2op_.second(other_node)->isExecuted()) {
            _dag_.addArc(other_node, new_node);
          }
        }
      }
    }

    return *new_op;
  }


  /// emplace a new schedule binary combination operator
  template < template < typename > class ALLOC >
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  const ScheduleOperation< ALLOC >& Schedule< ALLOC >::emplaceBinaryCombination(
        const ScheduleMultiDim< TABLE1, ALLOC >& table1,
        const ScheduleMultiDim< TABLE2, ALLOC >& table2,
        TABLE_RES (*combine)( const TABLE1&, const TABLE2&)) {
    // note that the insertOperation will check that table1 and table2
    // already belong to the schedule
    return insertOperation(
      ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES, ALLOC >
         (table1, table2, combine, get_allocator()));
  }


  /// emplace a new schedule projection operator
  template < template < typename > class ALLOC >
  template < typename TABLE >
  const ScheduleOperation< ALLOC >& Schedule< ALLOC >::emplaceProjection(
          const ScheduleMultiDim< TABLE, ALLOC >& table,
          const Set< const DiscreteVariable* >&   del_vars,
          TABLE (*project)(const TABLE&, const Set< const DiscreteVariable* >&)) {
    // note that the insertOperation will check that table already belongs
    // to the schedule
    return insertOperation(
       ScheduleProjection< TABLE, ALLOC >
          (table, del_vars, project, get_allocator()));
  }


  /// emplace a new schedule deletion operation
  template < template < typename > class ALLOC >
  template < typename TABLE >
  const ScheduleOperation< ALLOC >&
     Schedule< ALLOC >::emplaceDeletion(const ScheduleMultiDim< TABLE, ALLOC >& table) {
    // note that the insertOperation will check that table already belongs
    // to the schedule
    return insertOperation(
       ScheduleDeletion< TABLE, ALLOC >(table, get_allocator()));
  }


  /// emplace a new schedule storage operation
  template < template < typename > class ALLOC >
  template < typename TABLE,
             template < typename, typename... > class CONTAINER,
             typename... CONTAINER_PARAMS >
  const ScheduleOperation< ALLOC >&
     Schedule< ALLOC >::emplaceStorage(
        const IScheduleMultiDim< ALLOC >& table,
        CONTAINER<TABLE, CONTAINER_PARAMS...>& container) {
    // note that the insertOperation will check that table already belongs
    // to the schedule
    return insertOperation(
       ScheduleStorage< TABLE, CONTAINER, ALLOC, CONTAINER_PARAMS... >(
          table, container, get_allocator()));
  }


  /// returns a DAG indicating in which order the operations can be performed
  template < template < typename > class ALLOC >
  INLINE const DAG& Schedule< ALLOC >::dag() const {
    return _dag_;
  }


  /// returns the scheduleOperation corresponding to an id in the DAG
  template < template < typename > class ALLOC >
  INLINE const ScheduleOperation< ALLOC >&
  Schedule< ALLOC >::operation(const NodeId node_id) const {
    return *(_node2op_.second(node_id));
  }


  /// returns the id associated to a given operation
  template < template < typename > class ALLOC >
  INLINE NodeId Schedule< ALLOC >::nodeId(
     const ScheduleOperation< ALLOC >& op) const {
    return _node2op_.first(const_cast< ScheduleOperation< ALLOC >* >(&op));
  }


  /// returns the set of ScheduleOperations that can be executed at once
  template < template < typename > class ALLOC >
  INLINE NodeSet Schedule< ALLOC >::availableOperations() const {
    NodeSet available_nodes;
    for (const auto node: _dag_) {
      if (_dag_.parents(node).empty()) {
        available_nodes.insert(node);
      }
    }
    return available_nodes;
  }


  /// updates the DAG after a given operation has been executed
  template < template < typename > class ALLOC >
  void Schedule< ALLOC >::updateAfterExecution(
     const NodeId exec_node, std::vector<NodeId>& new_available_nodes,
     const bool check) {
    if (check) {
      // check that the node exists
      if (!_dag_.existsNode(exec_node)) {
        GUM_ERROR(UnknownScheduleOperation,
                  "the schedule cannot be updated because Operation of Id "
                     << exec_node
                     << " that has been executed does not belong to its DAG.");
      }

      // before performing the update, check that the operation was available
      // to do so, it is sufficient to check it had no parent
      if (!_dag_.parents(exec_node).empty()) {
        GUM_ERROR(UnavailableScheduleOperation,
                  "the schedule cannot be updated because Operation of Id "
                     << exec_node
                     << " is not available yet and should not have been executed.");
      }

      // check that the operation has really been executed
      if (!_node2op_.second(exec_node)->isExecuted()) {
        GUM_ERROR(UnexecutedScheduleOperation,
                  "the schedule cannot be updated because Operation of Id "
                     << exec_node
                     << " has not been executed yet.");
      }
    }

    // If the children of exec_node have just one remaining parent (i.e., this
    // is exec_node), then these children are new available operations
    new_available_nodes.clear();
    for (const auto child_node: _dag_.children(exec_node)) {
      if (_dag_.parents(child_node).size() == 1) { // only exec_node is a parent
        new_available_nodes.push_back(child_node);
      }
    }

    // remove node_exec
    _dag_.eraseNode(exec_node);
  }


  /// updates the DAG after a given operation has been executed
  template < template < typename > class ALLOC >
  INLINE void
  Schedule< ALLOC >::updateAfterExecution(
        const ScheduleOperation< ALLOC >& op,
        std::vector<NodeId>& new_available_nodes,
        const bool check) {
    /// check that the node belongs to the schedule
    if (check) {
      if (!_node2op_.existsSecond(const_cast<ScheduleOperation< ALLOC >*>(&op))) {
        GUM_ERROR(UnknownScheduleOperation,
                  "the schedule cannot be updated because Operation "
                     << op.toString()
                     << " that has been executed does not belong to its DAG.");
      }
    }

    updateAfterExecution(
       _node2op_.first(const_cast< ScheduleOperation< ALLOC >* >(&op)),
       new_available_nodes,
       check);
  }


  /// returns the ScheduleMultiDim corresponding to a given id
  template < template < typename > class ALLOC >
  INLINE const IScheduleMultiDim< ALLOC >*
     Schedule< ALLOC >::scheduleMultiDim(const NodeId id) const {
    return _multidim2id_.first(id);
  }


  /// returns the id of a given IScheduleMultiDim
  template < template < typename > class ALLOC >
  INLINE NodeId
     Schedule< ALLOC >::scheduleMultiDimId(const IScheduleMultiDim< ALLOC >* multidim) const {
    return _multidim2id_.second(multidim);
  }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
