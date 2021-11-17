/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) et Christophe GONZALES(@AMU)
 *  info_at_agrum_dot_org
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
 * @brief a Wrapper for multi-dimensional tables used for scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <sstream>
#  include <string>

#  include <agrum/tools/graphicalModels/inference/scheduler/scheduleMultiDim.h>


namespace gum {


  /// remove the table if it is contained in th ScheduleMultiDim
  template < typename TABLE, template < typename > class ALLOC >
  void ScheduleMultiDim< TABLE, ALLOC >::_removeTable_() {
    if (_table_contained_ && (_table_ != nullptr)) {
      ALLOC< TABLE > allocator(this->get_allocator());
      _table_->~TABLE();
      allocator.deallocate(_table_, 1);
    }
    _table_ = nullptr;
  }


  /// constructs a ScheduleMultiDim containing an already built table
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleMultiDim< TABLE, ALLOC >::ScheduleMultiDim(
     const TABLE&                                                     table,
     const bool                                                       copy,
     const Idx                                                        id,
     const typename ScheduleMultiDim< TABLE, ALLOC >::allocator_type& alloc) :
      IScheduleMultiDim< ALLOC >(id, alloc) {
    if (copy) {
      // copy the table
      ALLOC< TABLE > allocator(alloc);
      _table_ = allocator.allocate(1);
      try {
        new ((void*)_table_) TABLE(table);
      } catch (...) {
        allocator.deallocate(_table_, 1);
        throw;
      }
      _table_contained_ = true;
    } else {
      _table_ = const_cast< TABLE* >(&table);
      _table_contained_ = false;
    }

    _var_sequence_ = _table_->variablesSequence();
    _domain_size_ = _table_->domainSize();

    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleMultiDim);
  }


  /// constructs a ScheduleMultiDim containing an already built table
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleMultiDim< TABLE, ALLOC >::ScheduleMultiDim(
     TABLE&&                                                          table,
     const Idx                                                        id,
     const typename ScheduleMultiDim< TABLE, ALLOC >::allocator_type& alloc) :
      IScheduleMultiDim< ALLOC >(id, alloc) {
    // move the table
    ALLOC< TABLE > allocator(alloc);
    _table_ = allocator.allocate(1);
    try {
      new ((void*)_table_) TABLE(std::move(table));
    } catch (...) {
      allocator.deallocate(_table_, 1);
      throw;
    }
    _table_contained_ = true;
    _var_sequence_ = _table_->variablesSequence();
    _domain_size_ = _table_->domainSize();

    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleMultiDim);
  }


  /// constructs a ScheduleMultiDim containing an already built table
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleMultiDim< TABLE, ALLOC >::ScheduleMultiDim(
     const Sequence< const DiscreteVariable* >&                       vars,
     const Idx                                                        id,
     const typename ScheduleMultiDim< TABLE, ALLOC >::allocator_type& alloc) :
      IScheduleMultiDim< ALLOC >(id, alloc),
      _var_sequence_(vars) {
    if (!_var_sequence_.empty()) {
      // compute and store the domain size
      _domain_size_ = Size(1);
      for (const auto var: vars)
        _domain_size_ *= var->domainSize();
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleMultiDim);
  }


  /// copy constructor with a given allocator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleMultiDim< TABLE, ALLOC >::ScheduleMultiDim(
     const ScheduleMultiDim< TABLE, ALLOC >&                          from,
     const typename ScheduleMultiDim< TABLE, ALLOC >::allocator_type& alloc) :
      IScheduleMultiDim< ALLOC >(from, alloc),
      _var_sequence_(from._var_sequence_), _domain_size_(from._domain_size_) {
    if (from._table_ != nullptr) {
      if (from._table_contained_) {
        // copy the table
        ALLOC< TABLE > allocator(alloc);
        _table_ = allocator.allocate(1);
        try {
          new ((void*)_table_) TABLE(*(from._table_));
        } catch (...) {
          allocator.deallocate(_table_, 1);
          throw;
        }
        _table_contained_ = true;
      } else {
        _table_ = from._table_;
        _table_contained_ = false;
      }
    }

    // for debugging purposes
    GUM_CONS_CPY(ScheduleMultiDim);
  }


  /// copy constructor
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleMultiDim< TABLE, ALLOC >::ScheduleMultiDim(
     const ScheduleMultiDim< TABLE, ALLOC >& from) :
      ScheduleMultiDim< TABLE, ALLOC >(from, from.get_allocator()) {}


  /// move constructor with a given allocator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleMultiDim< TABLE, ALLOC >::ScheduleMultiDim(
     ScheduleMultiDim< TABLE, ALLOC >&&                               from,
     const typename ScheduleMultiDim< TABLE, ALLOC >::allocator_type& alloc) :
      IScheduleMultiDim< ALLOC >(from, alloc),
      _var_sequence_(from._var_sequence_), _domain_size_(from._domain_size_) {
    if (from._table_ != nullptr) {
      _table_ = from._table_;
      from._table_ = nullptr;
      _table_contained_ = from._table_contained_;
    }

    // for debugging purposes
    GUM_CONS_MOV(ScheduleMultiDim);
  }


  /// move constructor
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleMultiDim< TABLE, ALLOC >::ScheduleMultiDim(
     ScheduleMultiDim< TABLE, ALLOC >&& from) :
      ScheduleMultiDim< TABLE, ALLOC >(std::move(from), from.get_allocator()) {}


  /// virtual copy constructor with a given allocator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleMultiDim< TABLE, ALLOC >* ScheduleMultiDim< TABLE, ALLOC >::clone(
     const typename ScheduleMultiDim< TABLE, ALLOC >::allocator_type& alloc)
     const {
    ALLOC< ScheduleMultiDim< TABLE, ALLOC > > allocator(alloc);
    ScheduleMultiDim< TABLE, ALLOC >*         new_sched = allocator.allocate(1);
    try {
      new ((void*)new_sched) ScheduleMultiDim< TABLE, ALLOC >(*this, alloc);
    } catch (...) {
      allocator.deallocate(new_sched, 1);
      throw;
    }

    return new_sched;
  }


  /// virtual copy constructor
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleMultiDim< TABLE, ALLOC >*
     ScheduleMultiDim< TABLE, ALLOC >::clone() const {
    return clone(this->get_allocator());
  }


  /// destructor
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleMultiDim< TABLE, ALLOC >::~ScheduleMultiDim() {
    _removeTable_();

    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleMultiDim);
  }


  /// copy operator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleMultiDim< TABLE, ALLOC >& ScheduleMultiDim< TABLE, ALLOC >::operator=(
     const ScheduleMultiDim< TABLE, ALLOC >& from) {
    // avoid self assignment
    if (this != &from) {
      // assign the table of from
      if (from._table_ != nullptr) {
        if (from._table_contained_) {
          // if the ScheduleMultiDim does not contain its table, then make
          // _table_ point to nullptr, so that we will allocate it
          TABLE* old_table = _table_;
          if (!_table_contained_) _table_ = nullptr;

          if (_table_ == nullptr) {
            ALLOC< TABLE > allocator(this->get_allocator());
            try {
              _table_ = allocator.allocate(1);
            } catch (...) {
              _table_ = old_table;
              throw;
            }

            try {
              new ((void*)_table_) TABLE(*(from._table_));
            } catch (...) {
              allocator.deallocate(_table_, 1);
              _table_ = old_table;
              throw;
            }
          } else {
            // here, _table_ is not null and the ScheduleMultiDim contains
            // it. So we should copy from's table into it.
            *_table_ = *(from._table_);
          }
          _table_contained_ = true;
        } else {
          _removeTable_();
          _table_ = from._table_;
          _table_contained_ = false;
        }
      } else {
        // here, we should make _table_ point to nullptr
        _removeTable_();
        _table_ = nullptr;
        _table_contained_ = from._table_contained_;
      }

      _var_sequence_ = from._var_sequence_;
      _domain_size_ = from._domain_size_;
      IScheduleMultiDim< ALLOC >::operator=(from);
    }

    return *this;
  }


  /// move operator
  template < typename TABLE, template < typename > class ALLOC >
  ScheduleMultiDim< TABLE, ALLOC >& ScheduleMultiDim< TABLE, ALLOC >::operator=(
     ScheduleMultiDim< TABLE, ALLOC >&& from) {
    // avoid self assignment
    if (this != &from) {
      // assign the table of from
      _removeTable_();
      _table_ = from._table_;
      from._table_ = nullptr;
      _table_contained_ = from._table_contained_;

      _var_sequence_ = from._var_sequence_;
      _domain_size_ = from._domain_size_;
      IScheduleMultiDim< ALLOC >::operator=(std::move(from));
    }

    return *this;
  }


  /// checks whether two ScheduleMultiDim have exactly the same ID
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleMultiDim< TABLE, ALLOC >::operator==(
     const ScheduleMultiDim< TABLE, ALLOC >& m) const {
    return IScheduleMultiDim< ALLOC >::operator==(m);
  }


  /// checks whether two ScheduleMultiDim have the same ID and type
  template < typename TABLE, template < typename > class ALLOC >
  bool ScheduleMultiDim< TABLE, ALLOC >::operator==(
     const IScheduleMultiDim< ALLOC >& m) const {
    try {
      const ScheduleMultiDim< TABLE, ALLOC >& real_m =
         dynamic_cast< const ScheduleMultiDim< TABLE, ALLOC >& >(m);
      return ScheduleMultiDim< TABLE, ALLOC >::operator==(real_m);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleMultiDim have different IDs
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleMultiDim< TABLE, ALLOC >::operator!=(
     const ScheduleMultiDim< TABLE, ALLOC >& m) const {
    return !ScheduleMultiDim< TABLE, ALLOC >::operator==(m);
  }


  /// checks whether two ScheduleMultiDim have different IDs or types
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleMultiDim< TABLE, ALLOC >::operator!=(
     const IScheduleMultiDim< ALLOC >& m) const {
    return !ScheduleMultiDim< TABLE, ALLOC >::operator==(m);
  }


  /// checks whether two ScheduleMultiDim have the same variables
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleMultiDim< TABLE, ALLOC >::hasSameVariables(
     const ScheduleMultiDim< TABLE, ALLOC >& m) const {
    return ((_domain_size_ == m._domain_size_) &&
            (_var_sequence_ == m._var_sequence_));
  }


  /// checks whether two ScheduleMultiDim have the same variables and types
  template < typename TABLE, template < typename > class ALLOC >
  bool ScheduleMultiDim< TABLE, ALLOC >::hasSameVariables(
     const IScheduleMultiDim< ALLOC >& m) const {
    try {
      const ScheduleMultiDim< TABLE, ALLOC >& real_m =
         dynamic_cast< const ScheduleMultiDim< TABLE, ALLOC >& >(m);
      return ScheduleMultiDim< TABLE, ALLOC >::hasSameVariables(real_m);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleMultiDim contain the same table
  template < typename TABLE, template < typename > class ALLOC >
  bool ScheduleMultiDim< TABLE, ALLOC >::hasSameContent(
     const ScheduleMultiDim< TABLE, ALLOC >& m) const {
    if (!hasSameVariables(m))
      return false;

    if (_table_ == nullptr)
      return (m._table_ == nullptr);
    else {
      if (m._table_ == nullptr)
        return false;
      else {
        return (_table_ == m._table_) || (*_table_ == *(m._table_));
      }
    }
  }


  /// checks whether two IScheduleMultiDim are related to the same table
  template < typename TABLE, template < typename > class ALLOC >
  bool ScheduleMultiDim< TABLE, ALLOC >::hasSameContent(
     const IScheduleMultiDim< ALLOC >& m) const {
    try {
      const ScheduleMultiDim< TABLE, ALLOC >& real_m =
         dynamic_cast< const ScheduleMultiDim< TABLE, ALLOC >& >(m);
      return ScheduleMultiDim< TABLE, ALLOC >::hasSameContent(real_m);
    } catch (std::bad_cast&) { return false; }
  }


  /// returns the multiDimImplementation actually contained in the
  /// ScheduleMultiDim
  template < typename TABLE, template < typename > class ALLOC >
  INLINE const TABLE& ScheduleMultiDim< TABLE, ALLOC >::multiDim() const {
    if (_table_ == nullptr) {
      GUM_ERROR(NotFound,
                "the ScheduleMultiDim is abstract, so its table "
                   << "cannot be returned");
    }
    return *_table_;
  }


  /// returns whether the ScheduleMultiDim contains a real table
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleMultiDim< TABLE, ALLOC >::isAbstract() const {
    return (_table_ == nullptr);
  }


  /// indicates whether the ScheduleMultiDim contains a table and possess it
  template < typename TABLE, template < typename > class ALLOC >
  INLINE bool ScheduleMultiDim< TABLE, ALLOC >::containsMultiDim() const {
    return _table_contained_ && (_table_ != nullptr);
  }


  /// if the ScheduleMultiDim is not abstract, make it abstract again
  template < typename TABLE, template < typename > class ALLOC >
  INLINE void ScheduleMultiDim< TABLE, ALLOC >::makeAbstract() {
    _removeTable_();
  }


  /// returns the contained table and make the ScheduleMultiDim abstract
  template < typename TABLE, template < typename > class ALLOC >
  TABLE ScheduleMultiDim< TABLE, ALLOC >::exportMultiDim() {
    if (_table_ == nullptr) {
      GUM_ERROR(NullElement,
                "The ScheduleMultiDim being abstract, "
                   << "it is impossible to export its table");
    }
    if (!_table_contained_) {
      GUM_ERROR(OperationNotAllowed,
                "The ScheduleMultiDim only references its table, it "
                   << "does not contain it. So the latter cannot be exported");
    }

    // save the table
    TABLE table = std::move(*_table_);
    makeAbstract();
    return table;
  }


  /// returns the set of variables involved in the multidim
  template < typename TABLE, template < typename > class ALLOC >
  INLINE const Sequence< const DiscreteVariable* >&
               ScheduleMultiDim< TABLE, ALLOC >::variablesSequence() const {
    return _var_sequence_;
  }


  /// returns the domain size of the multidim
  template < typename TABLE, template < typename > class ALLOC >
  INLINE Size ScheduleMultiDim< TABLE, ALLOC >::domainSize() const {
    return _domain_size_;
  }


  /// returns the sizeof of the elements stored into the ScheduleMultiDim
  template < typename TABLE, template < typename > class ALLOC >
  INLINE double ScheduleMultiDim< TABLE, ALLOC >::sizeOfContent() const {
    return double(sizeof(typename ElementType<TABLE>::value_type));
  }


  /// sets a new table inside the wrapper
  template < typename TABLE, template < typename > class ALLOC >
  void ScheduleMultiDim< TABLE, ALLOC >::setMultiDim(const TABLE& table,
                                                     const bool   copy) {
    if (copy) {
      // if the ScheduleMultiDim does not contain its table, then make
      // _table_ point to nullptr, so that we will allocate it
      TABLE* old_table = _table_;
      if (!_table_contained_) _table_ = nullptr;

      if (_table_ == nullptr) {
        ALLOC< TABLE > allocator(this->get_allocator());
        try {
          _table_ = allocator.allocate(1);
        } catch (...) {
          _table_ = old_table;
          throw;
        }

        try {
          new ((void*)_table_) TABLE(table);
        } catch (...) {
          allocator.deallocate(_table_, 1);
          _table_ = old_table;
          throw;
        }
      } else {
        // here, _table_ is not null and the ScheduleMultiDim contains
        // it. So we should copy table into it.
        try {
          *_table_ = table;
        } catch (...) {
          _table_ = old_table;
          throw;
        }
      }
      _table_contained_ = true;
    } else {
      _removeTable_();
      _table_ = const_cast< TABLE* >(&table);
      _table_contained_ = false;
    }

    _var_sequence_ = _table_->variablesSequence();
    _domain_size_ = _table_->domainSize();
  }


  /// sets a new table inside the wrapper
  template < typename TABLE, template < typename > class ALLOC >
  void ScheduleMultiDim< TABLE, ALLOC >::setMultiDim(TABLE&& table) {
    // if the ScheduleMultiDim does not contain its table, then make
    // _table_ point to nullptr, so that we will allocate it
    TABLE* old_table = _table_;
    if (!_table_contained_) _table_ = nullptr;

    if (_table_ == nullptr) {
      ALLOC< TABLE > allocator(this->get_allocator());
      try {
        _table_ = allocator.allocate(1);
      } catch (...) {
        _table_ = old_table;
        throw;
      }

      try {
        new ((void*)_table_) TABLE(std::move(table));
      } catch (...) {
        allocator.deallocate(_table_, 1);
        _table_ = old_table;
        throw;
      }
    } else {
      // here, _table_ is not null and the ScheduleMultiDim contains
      // it. Si we should copy table into it.
      try {
        *_table_ = std::move(table);
      } catch (...) {
        _table_ = old_table;
        throw;
      }
    }
    _table_contained_ = true;
    _var_sequence_ = _table_->variablesSequence();
    _domain_size_ = _table_->domainSize();
  }


  /// displays the content of the multidim
  template < typename TABLE, template < typename > class ALLOC >
  std::string ScheduleMultiDim< TABLE, ALLOC >::toString() const {
    std::stringstream str;
    str << "<id: " << this->id() << ", table: ";
    if (_table_ == nullptr)
      str << "--";
    else
      str << _table_->content();
    str << ">";
    return str.str();
  }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
