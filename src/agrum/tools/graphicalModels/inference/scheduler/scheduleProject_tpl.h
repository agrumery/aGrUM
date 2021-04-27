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
 * @brief a Projection operator class used for scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <limits>

namespace gum {

  /// default constructor
  template < typename GUM_SCALAR >
  ScheduleProject< GUM_SCALAR >::ScheduleProject(
     const ScheduleMultiDim< GUM_SCALAR >& table,
     const Set< const DiscreteVariable* >& del_vars,
     MultiDimImplementation< GUM_SCALAR >* (*project)(
        const MultiDimImplementation< GUM_SCALAR >&,
        const Set< const DiscreteVariable* >&)) :
      ScheduleOperation< GUM_SCALAR >(
         ScheduleOperation< GUM_SCALAR >::Type::PROJECT_MULTIDIM),
      table__(table), del_vars__(del_vars), args__(0), results__(0),
      project__(project) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleProject);

    // compute the variables that shall belong to the result of the projection
    Sequence< const DiscreteVariable* > vars = table__.variablesSequence();

    for (const auto var: del_vars)
      vars.erase(var);

    // create the scheduleMultiDim that should result from the combination of
    // table1 and table2
    result__ = new ScheduleMultiDim< GUM_SCALAR >(vars);
  }

  /// copy constructor
  template < typename GUM_SCALAR >
  ScheduleProject< GUM_SCALAR >::ScheduleProject(
     const ScheduleProject< GUM_SCALAR >& f) :
      ScheduleOperation< GUM_SCALAR >(f),
      table__(f.table__), del_vars__(f.del_vars__),
      result__(new ScheduleMultiDim< GUM_SCALAR >(*(f.result__))), args__(0),
      results__(0), project__(f.project__) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleProject);
  }

  /// virtual copy constructor: creates a clone of the operation
  template < typename GUM_SCALAR >
  ScheduleProject< GUM_SCALAR >*
     ScheduleProject< GUM_SCALAR >::newFactory() const {
    return new ScheduleProject< GUM_SCALAR >(*this);
  }

  /// destructor
  template < typename GUM_SCALAR >
  ScheduleProject< GUM_SCALAR >::~ScheduleProject() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleProject);
    delete result__;

    if (args__) delete args__;

    if (results__) delete results__;
  }

  /// copy operator
  template < typename GUM_SCALAR >
  ScheduleProject< GUM_SCALAR >& ScheduleProject< GUM_SCALAR >::operator=(
     const ScheduleProject< GUM_SCALAR >& from) {
    // avoid self assignment
    if (this != &from) {
      ScheduleOperation< GUM_SCALAR >::operator=(from);
      table__                                  = from.table__;
      del_vars__                               = from.del_vars__;
      *result__                                = *(from.result__);
      project__                                = from.project__;

      // update args__ and results__ if they were already created
      if (args__) {
        args__->clear();
        args__->insert(&table__);
      }

      if (results__) {
        results__->clear();
        results__->insert(result__);
      }
    }

    return *this;
  }

  /// operator ==
  template < typename GUM_SCALAR >
  bool ScheduleProject< GUM_SCALAR >::operator==(
     const ScheduleOperation< GUM_SCALAR >& op) const {
    if (this->type() != op.type()) return false;

    const ScheduleProject< GUM_SCALAR >& real_op
       = static_cast< const ScheduleProject< GUM_SCALAR >& >(op);
    return ((table__ == real_op.table__) && (del_vars__ == real_op.del_vars__)
            && (project__ == real_op.project__));
  }

  /// operator !=
  template < typename GUM_SCALAR >
  bool ScheduleProject< GUM_SCALAR >::operator!=(
     const ScheduleOperation< GUM_SCALAR >& op) const {
    if (this->type() != op.type()) return true;

    const ScheduleProject< GUM_SCALAR >& real_op
       = static_cast< const ScheduleProject< GUM_SCALAR >& >(op);
    return ((table__ != real_op.table__) || (del_vars__ != real_op.del_vars__)
            || (project__ != real_op.project__));
  }

  /// executes the operation
  template < typename GUM_SCALAR >
  void ScheduleProject< GUM_SCALAR >::execute() {
    if (result__->isAbstract()) {
      const MultiDimImplementation< GUM_SCALAR >& t   = table__.multiDim();
      MultiDimImplementation< GUM_SCALAR >*       res = project__(t, del_vars__);
      result__->setMultiDim(*res);
    }
  }

  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template < typename GUM_SCALAR >
  INLINE float ScheduleProject< GUM_SCALAR >::nbOperations() const {
    return float(table__.domainSize());
  }

  /// returns the memory consumption used during the operation
  template < typename GUM_SCALAR >
  std::pair< long, long > ScheduleProject< GUM_SCALAR >::memoryUsage() const {
    long                                       size = 1;
    const Sequence< const DiscreteVariable* >& seq  = table__.variablesSequence();

    for (const auto var: seq)
      if (!del_vars__.contains(var)) {
        if (std::numeric_limits< long >::max() / (long)var->domainSize() < size) {
          GUM_ERROR(OutOfBounds, "memory usage out of long int range");
        }

        size *= long(var->domainSize());
      }

    return std::pair< long, long >(size, size);
  }

  /// returns the scheduleMultidim resulting from the execution of the operation
  template < typename GUM_SCALAR >
  INLINE const ScheduleMultiDim< GUM_SCALAR >&
               ScheduleProject< GUM_SCALAR >::result() const {
    return *result__;
  }

  /// returns the set of multidims passed in argument to the operation
  template < typename GUM_SCALAR >
  INLINE const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >&
               ScheduleProject< GUM_SCALAR >::multiDimArgs() const {
    if (!args__) {
      args__ = new Sequence< const ScheduleMultiDim< GUM_SCALAR >* >;
      args__->insert(&table__);
    }

    return *args__;
  }

  /// returns the set of multidims that should be the result of the operation
  template < typename GUM_SCALAR >
  INLINE const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >&
               ScheduleProject< GUM_SCALAR >::multiDimResults() const {
    if (!results__) {
      results__ = new Sequence< const ScheduleMultiDim< GUM_SCALAR >* >;
      results__->insert(result__);
    }

    return *results__;
  }

  /// displays the content of the operation
  template < typename GUM_SCALAR >
  std::string ScheduleProject< GUM_SCALAR >::toString() const {
    return result__->toString() + " = project ( " + table__.toString() + " , "
         + del_vars__.toString() + " )";
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
