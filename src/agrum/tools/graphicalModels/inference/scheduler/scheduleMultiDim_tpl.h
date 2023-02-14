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
  template < typename TABLE >
  INLINE void ScheduleMultiDim< TABLE >::_removeTable_() {
    if (_table_contained_ && (_table_ != nullptr)) delete _table_;
    _table_ = nullptr;
  }


  /// constructs a ScheduleMultiDim containing an already built table
  template < typename TABLE >
  ScheduleMultiDim< TABLE >::ScheduleMultiDim(const TABLE& table, const bool copy, const Idx id) :
      IScheduleMultiDim(id) {
    if (copy) {
      // copy the table
      _table_           = new TABLE(table);
      _table_contained_ = true;
    } else {
      _table_           = const_cast< TABLE* >(&table);
      _table_contained_ = false;
    }

    _var_sequence_ = _table_->variablesSequence();
    _domain_size_  = _table_->domainSize();

    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleMultiDim);
  }


  /// constructs a ScheduleMultiDim containing an already built table
  template < typename TABLE >
  ScheduleMultiDim< TABLE >::ScheduleMultiDim(TABLE&& table, const Idx id) : IScheduleMultiDim(id) {
    // move the table
    _table_        = new TABLE(std::move(table));
    _var_sequence_ = _table_->variablesSequence();
    _domain_size_  = _table_->domainSize();

    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleMultiDim);
  }


  /// constructs a ScheduleMultiDim containing an already built table
  template < typename TABLE >
  ScheduleMultiDim< TABLE >::ScheduleMultiDim(const Sequence< const DiscreteVariable* >& vars,
                                              const Idx                                  id) :
      IScheduleMultiDim(id),
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


  /// copy constructor
  template < typename TABLE >
  ScheduleMultiDim< TABLE >::ScheduleMultiDim(const ScheduleMultiDim< TABLE >& from) :
      IScheduleMultiDim(from), _table_contained_(from._table_contained_),
      _var_sequence_(from._var_sequence_), _domain_size_(from._domain_size_) {
    if (from._table_ != nullptr) {
      if (from._table_contained_) {
        // copy the table
        _table_ = new TABLE(*(from._table_));
      } else {
        _table_ = from._table_;
      }
    }

    // for debugging purposes
    GUM_CONS_CPY(ScheduleMultiDim);
  }


  /// move constructor
  template < typename TABLE >
  ScheduleMultiDim< TABLE >::ScheduleMultiDim(ScheduleMultiDim< TABLE >&& from) :
      IScheduleMultiDim(from), _table_contained_(from._table_contained_),
      _var_sequence_(from._var_sequence_), _domain_size_(from._domain_size_) {
    if (from._table_ != nullptr) {
      _table_      = from._table_;
      from._table_ = nullptr;
    }

    // for debugging purposes
    GUM_CONS_MOV(ScheduleMultiDim);
  }


  /// virtual copy constructor
  template < typename TABLE >
  INLINE ScheduleMultiDim< TABLE >* ScheduleMultiDim< TABLE >::clone() const {
    return new ScheduleMultiDim< TABLE >(*this);
  }


  /// virtual copy constructor enabling to forcing a copy of the content
  template < typename TABLE >
  ScheduleMultiDim< TABLE >* ScheduleMultiDim< TABLE >::clone(bool force_copy) const {
    ScheduleMultiDim< TABLE >* new_sched;

    if (!force_copy) new_sched = new ScheduleMultiDim< TABLE >(*this);
    else {
      if (!isAbstract()) new_sched = new ScheduleMultiDim< TABLE >(*_table_, true, this->id());
      else {
        new_sched                    = new ScheduleMultiDim< TABLE >(_var_sequence_, this->id());
        new_sched->_table_contained_ = true;
      }
    }

    return new_sched;
  }


  /// destructor
  template < typename TABLE >
  ScheduleMultiDim< TABLE >::~ScheduleMultiDim() {
    _removeTable_();

    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleMultiDim);
  }


  /// copy operator
  template < typename TABLE >
  ScheduleMultiDim< TABLE >&
     ScheduleMultiDim< TABLE >::operator=(const ScheduleMultiDim< TABLE >& from) {
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
            try {
              _table_ = new TABLE(*(from._table_));
            } catch (...) {
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
          _table_           = from._table_;
          _table_contained_ = false;
        }
      } else {
        // here, we should make _table_ point to nullptr
        _removeTable_();
        _table_           = nullptr;
        _table_contained_ = from._table_contained_;
      }

      _var_sequence_ = from._var_sequence_;
      _domain_size_  = from._domain_size_;
      IScheduleMultiDim::operator=(from);
    }

    return *this;
  }


  /// move operator
  template < typename TABLE >
  ScheduleMultiDim< TABLE >&
     ScheduleMultiDim< TABLE >::operator=(ScheduleMultiDim< TABLE >&& from) {
    // avoid self assignment
    if (this != &from) {
      // assign the table of from
      _removeTable_();
      _table_           = from._table_;
      from._table_      = nullptr;
      _table_contained_ = from._table_contained_;

      _var_sequence_ = from._var_sequence_;
      _domain_size_  = from._domain_size_;
      IScheduleMultiDim::operator=(std::move(from));
    }

    return *this;
  }


  /// checks whether two ScheduleMultiDim have exactly the same ID
  template < typename TABLE >
  INLINE bool ScheduleMultiDim< TABLE >::operator==(const ScheduleMultiDim< TABLE >& m) const {
    return IScheduleMultiDim::operator==(m);
  }


  /// checks whether two ScheduleMultiDim have the same ID and type
  template < typename TABLE >
  bool ScheduleMultiDim< TABLE >::operator==(const IScheduleMultiDim& m) const {
    try {
      const ScheduleMultiDim< TABLE >& real_m = dynamic_cast< const ScheduleMultiDim< TABLE >& >(m);
      return ScheduleMultiDim< TABLE >::operator==(real_m);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleMultiDim have different IDs
  template < typename TABLE >
  INLINE bool ScheduleMultiDim< TABLE >::operator!=(const ScheduleMultiDim< TABLE >& m) const {
    return !ScheduleMultiDim< TABLE >::operator==(m);
  }


  /// checks whether two ScheduleMultiDim have different IDs or types
  template < typename TABLE >
  INLINE bool ScheduleMultiDim< TABLE >::operator!=(const IScheduleMultiDim& m) const {
    return !ScheduleMultiDim< TABLE >::operator==(m);
  }


  /// checks whether two ScheduleMultiDim have the same variables
  template < typename TABLE >
  INLINE bool
     ScheduleMultiDim< TABLE >::hasSameVariables(const ScheduleMultiDim< TABLE >& m) const {
    return ((_domain_size_ == m._domain_size_) && (_var_sequence_ == m._var_sequence_));
  }


  /// checks whether two ScheduleMultiDim have the same variables and types
  template < typename TABLE >
  bool ScheduleMultiDim< TABLE >::hasSameVariables(const IScheduleMultiDim& m) const {
    try {
      const ScheduleMultiDim< TABLE >& real_m = dynamic_cast< const ScheduleMultiDim< TABLE >& >(m);
      return ScheduleMultiDim< TABLE >::hasSameVariables(real_m);
    } catch (std::bad_cast&) { return false; }
  }


  /// checks whether two ScheduleMultiDim contain the same table
  template < typename TABLE >
  bool ScheduleMultiDim< TABLE >::hasSameContent(const ScheduleMultiDim< TABLE >& m) const {
    if (!hasSameVariables(m)) return false;

    if (_table_ == nullptr) return (m._table_ == nullptr);
    else {
      if (m._table_ == nullptr) return false;
      else { return (_table_ == m._table_) || (*_table_ == *(m._table_)); }
    }
  }


  /// checks whether two IScheduleMultiDim are related to the same table
  template < typename TABLE >
  bool ScheduleMultiDim< TABLE >::hasSameContent(const IScheduleMultiDim& m) const {
    try {
      const ScheduleMultiDim< TABLE >& real_m = dynamic_cast< const ScheduleMultiDim< TABLE >& >(m);
      return ScheduleMultiDim< TABLE >::hasSameContent(real_m);
    } catch (std::bad_cast&) { return false; }
  }


  /// returns the multiDimImplementation actually contained in the
  /// ScheduleMultiDim
  template < typename TABLE >
  INLINE const TABLE& ScheduleMultiDim< TABLE >::multiDim() const {
    if (_table_ == nullptr) {
      GUM_ERROR(NullElement,
                "the ScheduleMultiDim is abstract, so its table "
                   << "cannot be returned");
    }
    return *_table_;
  }


  /// returns whether the ScheduleMultiDim contains a real table
  template < typename TABLE >
  INLINE bool ScheduleMultiDim< TABLE >::isAbstract() const {
    return (_table_ == nullptr);
  }


  /// indicates whether the ScheduleMultiDim contains a table and possess it
  template < typename TABLE >
  INLINE bool ScheduleMultiDim< TABLE >::containsMultiDim() const {
    return _table_contained_ && (_table_ != nullptr);
  }


  /// if the ScheduleMultiDim is not abstract, make it abstract again
  template < typename TABLE >
  INLINE void ScheduleMultiDim< TABLE >::makeAbstract() {
    _removeTable_();
  }


  /// returns the contained table and make the ScheduleMultiDim abstract
  template < typename TABLE >
  TABLE* ScheduleMultiDim< TABLE >::exportMultiDim() {
    if (_table_ == nullptr) {
      GUM_ERROR(NullElement,
                "The ScheduleMultiDim being abstract, "
                   << "it is impossible to export its table");
    }
    if (!_table_contained_) {
      GUM_ERROR(OperationNotAllowed,
                "a ScheduleMultiDim cannot export a table it does not contain. "
                "Use method multiDim() instead.");
    }

    auto table = _table_;
    _table_    = nullptr;

    return table;
  }


  /// returns the set of variables involved in the multidim
  template < typename TABLE >
  INLINE const Sequence< const DiscreteVariable* >&
               ScheduleMultiDim< TABLE >::variablesSequence() const {
    return _var_sequence_;
  }


  /// returns the domain size of the multidim
  template < typename TABLE >
  INLINE Size ScheduleMultiDim< TABLE >::domainSize() const {
    return _domain_size_;
  }


  /// returns the sizeof of the elements stored into the ScheduleMultiDim
  template < typename TABLE >
  INLINE double ScheduleMultiDim< TABLE >::sizeOfContent() const {
    return double(sizeof(typename ElementType< TABLE >::value_type));
  }


  /// sets a new table inside the wrapper
  template < typename TABLE >
  void ScheduleMultiDim< TABLE >::setMultiDim(const TABLE& table, const bool copy) {
    if (copy) {
      // if the ScheduleMultiDim does not contain its table, then make
      // _table_ point to nullptr, so that we will allocate it
      TABLE* old_table = _table_;
      if (!_table_contained_) _table_ = nullptr;

      if (_table_ == nullptr) {
        try {
          _table_ = new TABLE(table);
        } catch (...) {
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
      _table_           = const_cast< TABLE* >(&table);
      _table_contained_ = false;
    }

    _var_sequence_ = _table_->variablesSequence();
    _domain_size_  = _table_->domainSize();
  }


  /// sets a new table inside the wrapper
  template < typename TABLE >
  void ScheduleMultiDim< TABLE >::setMultiDim(TABLE&& table) {
    // if the ScheduleMultiDim does not contain its table, then make
    // _table_ point to nullptr, so that we will allocate it
    TABLE* old_table = _table_;
    if (!_table_contained_) _table_ = nullptr;

    if (_table_ == nullptr) {
      try {
        _table_ = new TABLE(std::move(table));
      } catch (...) {
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
    _var_sequence_    = _table_->variablesSequence();
    _domain_size_     = _table_->domainSize();
  }


  /// displays the content of the multidim
  template < typename TABLE >
  std::string ScheduleMultiDim< TABLE >::toString() const {
    std::stringstream str;
    str << "<id: " << this->id() << ", table: ";
    if (_table_ == nullptr) str << "--";
    else str << _table_->content();
    str << ">";
    return str.str();
  }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
