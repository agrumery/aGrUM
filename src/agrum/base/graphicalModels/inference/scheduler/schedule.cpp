/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Class to include at least once this header
 *
 * @author Christophe GONZALES(@AMU) & Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/base/graphicalModels/inference/scheduler/schedule.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/graphicalModels/inference/scheduler/schedule_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  // the version number of the schedules
  std::atomic< Idx > Schedule::_overall_version_number_ = Idx(0);

  /// returns the graph that would obtain when no operation is performed
  DAG Schedule::_fullDAG_() const {
    // if no operation has been performed yet, return _dag_
    if (_dag_.sizeNodes() == _node2op_.size()) return _dag_;

    // now, we should reconstruct the graph. First, we add all the nodes
    DAG dag(_node2op_.size(), true, 2 * _node2op_.size(), true);   // estimated default size
    for (auto iter = _node2op_.cbegin(); iter != _node2op_.cend(); ++iter) {
      dag.addNodes(iter.first());
    }

    // add the arcs toward node or from node if another node deletes one of its
    // arguments
    for (auto iter = _node2op_.cbegin(); iter != _node2op_.cend(); ++iter) {
      const NodeId node = iter.first();
      const auto*  op   = iter.second();

      for (const auto arg: op->args()) {
        // get the arguments used by node/op that are not sources and add arcs
        // from the operations/nodes that created them to the current node.

        if (const auto arg_location = _multidim_location_[arg].first; arg_location != nullptr) {
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
            if (other_node != node) { dag.addArc(other_node, node); }
          }
        }
      }
    }

    return dag;
  }

  /// a function to copy the content of a schedule into another one
  void Schedule::_copy_(const Schedule& from) {
    // copy the dag-related structures
    _dag_            = from._dag_;
    _newId_          = from._newId_;
    _version_number_ = from._version_number_;

    // here, we create the mapping from all the ScheduleMultiDims contained into
    // from to those contained into this
    HashTable< const IScheduleMultiDim*, const IScheduleMultiDim* > multidim_from2this(
        from._multidim2id_.size());

    // we copy the source multidims, i.e., those that are not the result of any
    // operation, and we store the mapping from these IScheduleMultiDim contained
    // into from to those contained into this
    for (const auto& [first, second]: from._multidim_location_) {
      if (second.first == nullptr) {   // here, this is a source
        const IScheduleMultiDim* new_multidim
            = from._emplaced_multidims_.exists(first) ? first : first->clone();
        multidim_from2this.insert(first, new_multidim);
        _multidim_location_.insert(new_multidim, second);
        _multidim2id_.insert(new_multidim, new_multidim->id());
        _multidim2nodes_.insert(new_multidim, NodeSet());
      }
    }
    _emplaced_multidims_ = from._emplaced_multidims_;


    // get a topological order of from's full dag, i.e. the dag corresponding to
    // all the operations of that schedule, as if those had not been executed.
    // This is the order in which we will reconstruct all the operations. This
    // will allow us to construct the ScheduleMultiDims in the right order.
    const Sequence< NodeId > nodes_sequence = (from._dag_.sizeNodes() == from._node2op_.size())
                                                ? from._dag_.topologicalOrder()
                                                : from._fullDAG_().topologicalOrder();

    // we can now create all the operations contained in this in the same
    // topological order as from: this enables to create all the ScheduleMultiDims
    // needed by a ScheduleOperator before creating the ScheduleOperator itself
    for (const auto node: nodes_sequence) {
      // get the "from" operation and clone it
      const ScheduleOperator* from_op = from._node2op_.second(node);
      ScheduleOperator*       new_op  = from_op->clone();
      _node2op_.insert(node, new_op);

      // we get the ScheduleMultiDims passed as parameters to from_op, and we
      // compute the corresponding ScheduleMultiDims in the current schedule,
      // and we update accordingly the parameters of new_op. In addition,
      // we store the information that the ScheduleMultiDims are used by our
      // new operation
      const Sequence< const IScheduleMultiDim* > from_args = from_op->args();
      Sequence< const IScheduleMultiDim* >       new_args(from_args.size());
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

      // we get the ScheduleMultiDims resulting from from_op, and we compute their
      // mapping with those resulting from new_op. We also store the location and
      // id of the new results
      const Sequence< const IScheduleMultiDim* > from_res = from_op->results();
      const Sequence< const IScheduleMultiDim* > new_res  = new_op->results();
      for (Idx i = Idx(0), end = from_res.size(); i < end; ++i) {
        multidim_from2this.insert(from_res[i], new_res[i]);
        _multidim_location_.insert(new_res[i], std::pair< ScheduleOperator*, Idx >(new_op, i));
        _multidim2id_.insert(new_res[i], new_res[i]->id());
        _multidim2nodes_.insert(new_res[i], NodeSet());
      }
    }
  }

  /// a function to delete from memory the allocated content of a schedule
  void Schedule::_destroy_() {
    // remove all the operations of the schedule
    for (auto iter = _node2op_.begin(), end = _node2op_.end(); iter != end; ++iter) {
      delete iter.second();
    }

    // remove all the source ScheduleMultiDims
    for (const auto& [first, second]: _multidim_location_) {
      if ((second.first == nullptr) && !_emplaced_multidims_.exists(first)) {
        const IScheduleMultiDim* multidim = first;
        delete multidim;
      }
    }
  }

  /// empty the schedule, i.e., remove its content
  void Schedule::clear() {
    // remove all the allocated objects
    _destroy_();

    // clear all the data structures
    _dag_.clear();
    _newId_ = NodeId(0);
    _node2op_.clear();
    _multidim_location_.clear();
    _multidim2id_.clear();
    _emplaced_multidims_.clear();
    _multidim2nodes_.clear();
    _deleted_multidim2node_.clear();

    // indicate that the version of the schedule has changed
    _version_number_ = _newVersionNumber_();
  }

  /// default constructor (construct an empty set of operations)
  Schedule::Schedule(const Size nb_ops) :
      _dag_(nb_ops, true, 2 * nb_ops, true), _node2op_(nb_ops), _multidim_location_(2 * nb_ops),
      _multidim2id_(2 * nb_ops), _emplaced_multidims_(2 * nb_ops), _multidim2nodes_(2 * nb_ops),
      _deleted_multidim2node_(2 * nb_ops),
      _version_number_(_newVersionNumber_()) {   // for debugging purposes
    GUM_CONSTRUCTOR(Schedule)
  }

  /// copy constructor
  Schedule::Schedule(const Schedule& from) : _version_number_(from._version_number_) {
    // really perform the copy
    _copy_(from);

    // for debugging purposes
    GUM_CONS_CPY(Schedule)
  }

  /// move constructor
  Schedule::Schedule(Schedule&& from) :
      _dag_(std::move(from._dag_)), _newId_(from._newId_), _node2op_(std::move(from._node2op_)),
      _multidim_location_(std::move(from._multidim_location_)),
      _multidim2id_(std::move(from._multidim2id_)),
      _emplaced_multidims_(std::move(from._emplaced_multidims_)),
      _multidim2nodes_(std::move(from._multidim2nodes_)),
      _deleted_multidim2node_(std::move(from._deleted_multidim2node_)),
      _version_number_(from._version_number_) {
    // empty properly from
    from._newId_ = NodeId(0);
    from._dag_.clear();
    from._node2op_.clear();
    from._multidim_location_.clear();
    from._multidim2id_.clear();
    from._emplaced_multidims_.clear();
    from._multidim2nodes_.clear();
    from._deleted_multidim2node_.clear();

    // for debugging purposes
    GUM_CONS_MOV(Schedule)
  }

  /// destructor
  Schedule::~Schedule() {
    // really destroy all the allocated objects contained into the schedule
    _destroy_();

    // for debugging purposes
    GUM_DESTRUCTOR(Schedule)
  }

  /// copy operator
  Schedule& Schedule::operator=(const Schedule& from) {
    // avoid self assignment
    if (this != &from) {
      // remove all the operations that are currently stored
      clear();

      // perform the copy
      _copy_(from);
    }

    return *this;
  }

  /// move operator
  Schedule& Schedule::operator=(Schedule&& from) {
    // avoid self assignment
    if (this != &from) {
      // remove all the operations that are currently stored
      clear();

      // move from
      _newId_                 = from._newId_;
      _dag_                   = std::move(from._dag_);
      _node2op_               = std::move(from._node2op_);
      _multidim_location_     = std::move(from._multidim_location_);
      _multidim2id_           = std::move(from._multidim2id_);
      _emplaced_multidims_    = std::move(from._emplaced_multidims_);
      _multidim2nodes_        = std::move(from._multidim2nodes_);
      _deleted_multidim2node_ = std::move(from._deleted_multidim2node_);
      _version_number_        = from._version_number_;

      // empty properly from
      from._newId_ = NodeId(0);
      from._dag_.clear();
      from._node2op_.clear();
      from._multidim_location_.clear();
      from._multidim2id_.clear();
      from._emplaced_multidims_.clear();
      from._multidim2nodes_.clear();
      from._deleted_multidim2node_.clear();
    }

    return *this;
  }

  /// operator ==
  bool Schedule::operator==(const Schedule& from) const {
    // check if both schedules have the same graph and the same set of
    // operation ids
    if ((_dag_ != from._dag_) || (_node2op_.size() != from._node2op_.size())) return false;
    for (auto iter = _node2op_.cbegin(); iter != _node2op_.cend(); ++iter)
      if (!from._node2op_.existsFirst(iter.first())) return false;

    // map "this"'s operations and source multidims to those of "from"
    HashTable< const ScheduleOperator*, const ScheduleOperator* >   this_op2from(_node2op_.size());
    Bijection< const IScheduleMultiDim*, const IScheduleMultiDim* > this_multidim2from(
        _multidim2nodes_.size());

    // get a topological order of the full graph. We will use it to compare the
    // parameters of the operations. This order enforces that the parameters are
    // already known before we examine them in the operations.
    // check if all the operations have the same type and the same parameters
    for (const Sequence< NodeId > order = (_dag_.sizeNodes() == _node2op_.size())
                                            ? _dag_.topologicalOrder()
                                            : _fullDAG_().topologicalOrder();
         const auto               node: order) {
      // get the operations corresponding to node
      const ScheduleOperator* this_op = _node2op_.second(node);
      const ScheduleOperator* from_op = from._node2op_.second(node);

      // check if they perform the same operations
      if (!this_op->isSameOperator(*from_op)) return false;

      // check that the content of their parameters are the same
      const Sequence< const IScheduleMultiDim* > this_args = this_op->args();
      const Sequence< const IScheduleMultiDim* > from_args = from_op->args();
      if (this_args.size() != from_args.size()) return false;

      for (Idx i = 0, end = this_args.size(); i < end; ++i) {
        const auto& [first, second]           = _multidim_location_[this_args[i]];
        const auto& [from_first, from_second] = from._multidim_location_[from_args[i]];

        if (first != nullptr) {
          // here, this's and from's locations are the same if they correspond
          // to the same operation, with the same argument's index
          if ((from_first == nullptr) || (second != from_second)
              || (from_first != this_op2from[first]))
            return false;
        } else {
          // here, this_args[i] and from_args[i] should both be source multidims
          if (from_first != nullptr) return false;

          // here, we know that they are truly source multidims. If we have
          // already examined these multidims, we have already put the mapping
          // between this's and from's multidim into Bijection this_multidim2from
          bool found = false;
          if (this_multidim2from.existsFirst(this_args[i])) {
            if (this_multidim2from.second(this_args[i]) != from_args[i]) return false;
            else found = true;
          }

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
    // identical. Hence, both schedules should be considered equal
    return true;
  }

  /// inserts an already constructed ScheduleMultiDim
  const IScheduleMultiDim* Schedule::insertScheduleMultiDim(const IScheduleMultiDim& multidim) {
    // check that the ScheduleMultiDim neither already belongs to the schedule
    // nor contains an abstract table: since it is a source multidim, it will
    // never be computed by the schedule. Hence if it is abstract, it will not
    // be possible to execute the schedule
    if (_multidim2id_.existsSecond(multidim.id())) {
      GUM_ERROR(DuplicateScheduleMultiDim,
                "A ScheduleMultiDim with Id " << multidim.id() << " already exists in the schedule")
    }
    if (multidim.isAbstract()) {
      GUM_ERROR(AbstractScheduleMultiDim,
                "It is impossible to insert an abstract ScheduleMultiDim " << "into a Schedule")
    }

    // now, everything is ok, so we should insert a copy of the ScheduleMultiDim
    // into the schedule
    const IScheduleMultiDim* new_multidim = multidim.clone();
    _multidim2nodes_.insert(new_multidim, NodeSet());
    _multidim_location_.insert(new_multidim, std::pair< ScheduleOperator*, Idx >(nullptr, Idx(0)));
    _multidim2id_.insert(new_multidim, new_multidim->id());

    // indicate that the schedule has been modified
    ++_version_number_;

    return new_multidim;
  }

  /// inserts an already constructed ScheduleMultiDim
  void Schedule::emplaceScheduleMultiDim(const IScheduleMultiDim& multidim) {
    // check that the ScheduleMultiDim neither already belongs to the schedule
    // nor contains an abstract table: since it is a source multidim, it will
    // never be computed by the schedule. Hence, if it is abstract, it will not
    // be possible to execute the schedule
    if (_multidim2id_.existsSecond(multidim.id())) {
      GUM_ERROR(DuplicateScheduleMultiDim,
                "A ScheduleMultiDim with Id " << multidim.id() << " already exists in the schedule")
    }
    if (multidim.isAbstract()) {
      GUM_ERROR(AbstractScheduleMultiDim,
                "It is impossible to insert an abstract ScheduleMultiDim " << "into a Schedule")
    }

    // now, everything is ok, so we should insert the ScheduleMultiDim
    // into the schedule
    _multidim2nodes_.insert(&multidim, NodeSet());
    _multidim_location_.insert(&multidim, std::pair< ScheduleOperator*, Idx >(nullptr, Idx(0)));
    _multidim2id_.insert(&multidim, multidim.id());
    _emplaced_multidims_.insert(&multidim);

    // indicate that the schedule has been modified
    ++_version_number_;
  }

  /// returns the adjective corresponding to a parameter index (1st, 2nd, etc.)
  std::string Schedule::_paramString_(Idx i) {
    if (i == 0) return "1st";
    else if (i == 1) return "2nd";
    else if (i == 2) return "3rd";

    std::stringstream str;
    str << (i + 1) << "th";
    return str.str();
  }

  /// inserts an operation into the schedule
  const ScheduleOperator& Schedule::insertOperation(const ScheduleOperator& op,
                                                    const bool are_results_persistent) {
    // check that the parameters of the operation already belong to the schedule.
    // to do so, it is sufficient to check that their ids belong to the schedule
    const Sequence< const IScheduleMultiDim* >& op_args = op.args();
    for (Idx i = Idx(0), end = op_args.size(); i < end; ++i) {
      if (!_multidim2id_.existsSecond(op_args[i]->id())) {
        GUM_ERROR(UnknownScheduleMultiDim,
                  "Schedule::insertOperation: the "
                      << _paramString_(i + 1) << " (id: " << op_args[i]->id()
                      << ") operation's argument does not already belong to"
                      << " the schedule")
      }
    }

    // if another operation deletes some parameters used by the new operation, we
    // should check that this operation has not been executed yet. In this case,
    // we will enforce that the new operation is performed before the deletion.
    // In addition, we should also check that the new operation does not delete
    // its parameters (else those would be deleted twice)
    for (Idx i = Idx(0), end = op_args.size(); i < end; ++i) {
      const IScheduleMultiDim* arg = op_args[i];
      if (_deleted_multidim2node_.existsFirst(arg)
          && (_node2op_.second(_deleted_multidim2node_.second(arg))->isExecuted()
              || op.implyDeletion())) {
        GUM_ERROR(OperationNotAllowed,
                  "Schedule::insertOperation: The operation deletes its "
                      << _paramString_(i + 1) << " argument, already deleted by another operation.")
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
                          << " deleted its " << _paramString_(i + 1)
                          << " argument, which is used by another operation"
                          << " not executed yet.")
          }
        }
      }
    }

    // here, we know that we can safely add the new operation into the schedule

    // create a copy of the operation. Remember that the parameters of
    // ScheduleOperations are pointers to ScheduleMultiDim. Therefore, we need
    // to map the pointers of op_args to pointers of the corresponding
    // ScheduleMultiDim in our new operation
    ScheduleOperator*                    new_op = op.clone();
    Sequence< const IScheduleMultiDim* > new_args(op_args.size());
    for (Idx i = Idx(0), end = op_args.size(); i < end; ++i) {
      try {
        new_args << _multidim2id_.first(op_args[i]->id());
      } catch (NotFound const&) {
        // deallocate everything we have allocated
        delete new_op;

        GUM_ERROR(UnknownScheduleMultiDim,
                  "the " << _paramString_(i + 1) << " argument of the operation is not known by"
                         << " the schedule")
      }
    }
    new_op->updateArgs(new_args);
    new_op->makeResultsPersistent(are_results_persistent);

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
    const Sequence< const IScheduleMultiDim* > new_results = new_op->results();
    for (Idx i = Idx(0), end = new_results.size(); i < end; ++i) {
      _multidim_location_.insert(new_results[i], std::pair< ScheduleOperator*, Idx >(new_op, i));
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
        if (other_node != new_node) { _dag_.addArc(new_node, other_node); }
      }

      // if Parameter arg has been created by another operation, then new_node
      // should be a child of this node
      if (const auto arg_location = _multidim_location_[arg].first; arg_location != nullptr) {
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
          if ((other_node != new_node) && !_dag_.existsArc(other_node, new_node)
              && !_node2op_.second(other_node)->isExecuted()) {
            _dag_.addArc(other_node, new_node);
          }
        }
      }
    }

    // indicate that the schedule has been modified
    ++_version_number_;

    return *new_op;
  }

  /// returns the set of ScheduleOperations that can be executed at once
  NodeSet Schedule::availableOperations() const {
    NodeSet available_nodes;
    for (const auto node: _dag_) {
      if (!this->operation(node).isExecuted()) {
        bool all_parents_executed = true;
        for (const auto par: _dag_.parents(node)) {
          if (!this->operation(par).isExecuted()) {
            all_parents_executed = false;
            break;
          }
        }
        if (all_parents_executed) available_nodes.insert(node);
      }
    }
    return available_nodes;
  }

  /// updates the DAG after a given operation has been executed
  void Schedule::updateAfterExecution(const NodeId           exec_node,
                                      std::vector< NodeId >& new_available_nodes,
                                      const bool             check) {
    if (check) {
      // check that the node exists
      if (!_dag_.existsNode(exec_node)) {
        GUM_ERROR(UnknownScheduleOperation,
                  "the schedule cannot be updated because Operation of Id "
                      << exec_node << " that has been executed does not belong to its DAG.")
      }

      // before performing the update, check that the operation was available
      // to do so, it is sufficient to check it had no parent
      if (!_dag_.parents(exec_node).empty()) {
        GUM_ERROR(UnavailableScheduleOperation,
                  "the schedule cannot be updated because Operation of Id "
                      << exec_node << " is not available yet and should not have been executed.")
      }

      // check that the operation has really been executed
      if (!_node2op_.second(exec_node)->isExecuted()) {
        GUM_ERROR(UnexecutedScheduleOperation,
                  "the schedule cannot be updated because Operation of Id "
                      << exec_node << " has not been executed yet.")
      }
    }

    // If the children of exec_node have just one remaining parent (i.e., this
    // is exec_node), then these children are new available operations
    new_available_nodes.clear();
    for (const auto child_node: _dag_.children(exec_node)) {
      if (_dag_.parents(child_node).size() == 1) {   // only exec_node is a parent
        new_available_nodes.push_back(child_node);
      }
    }

    // remove node_exec
    _dag_.eraseNode(exec_node);

    _version_number_ = _newVersionNumber_();
  }

}   // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
