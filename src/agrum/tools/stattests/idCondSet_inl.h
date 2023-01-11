/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Template implementation of idSets
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/stattests/idCondSet.h>

namespace gum {

  namespace learning {


    /// default constructor
    INLINE IdCondSetIterator::IdCondSetIterator() { GUM_CONSTRUCTOR(IdCondSetIterator); }


    /// Constructor for a begin
    INLINE IdCondSetIterator::IdCondSetIterator(const IdCondSet& idset) : _seq_(&(idset.ids())) {
      GUM_CONSTRUCTOR(IdCondSetIterator);
    }


    /// Copy constructor.
    INLINE IdCondSetIterator::IdCondSetIterator(const IdCondSetIterator& from) :
        _seq_(from._seq_), _index_(from._index_) {
      GUM_CONS_CPY(IdCondSetIterator);
    }


    /// move constructor
    INLINE IdCondSetIterator::IdCondSetIterator(IdCondSetIterator&& from) :
        _seq_(from._seq_), _index_(from._index_) {
      GUM_CONS_MOV(IdCondSetIterator);
    }


    /// destructor
    INLINE IdCondSetIterator::~IdCondSetIterator() { GUM_DESTRUCTOR(IdCondSetIterator); }


    /// places the index to the end of the sequence
    INLINE void IdCondSetIterator::_gotoEnd_() {
      if (_seq_ != nullptr) _index_ = _seq_->size();
      else _index_ = std::size_t(0);
    }


    /// copy operator
    INLINE IdCondSetIterator& IdCondSetIterator::operator=(const IdCondSetIterator& from) {
      _seq_   = from._seq_;
      _index_ = from._index_;
      return *this;
    }


    /// move operator
    INLINE IdCondSetIterator& IdCondSetIterator::operator=(IdCondSetIterator&& from) {
      _seq_   = from._seq_;
      _index_ = from._index_;
      return *this;
    }


    /// Gives access to the content of the iterator.
    INLINE NodeId IdCondSetIterator::operator*() const { return _seq_->operator[](_index_); }


    /// Checks whether two iterators point toward different elements.
    INLINE bool IdCondSetIterator::operator!=(const IdCondSetIterator& from) const {
      return (_index_ != from._index_) || (_seq_ != from._seq_);
    }


    /// Checks whether two iterators point toward the same elements.
    INLINE bool IdCondSetIterator::operator==(const IdCondSetIterator& from) const {
      return !operator!=(from);
    }


    /// Makes the iterator point to the next element in the IdCondSet
    INLINE IdCondSetIterator& IdCondSetIterator::operator++() {
      ++_index_;
      return *this;
    }


    /// Makes the iterator point to i elements further in the IdCondSet
    INLINE IdCondSetIterator& IdCondSetIterator::operator+=(const std::size_t i) {
      _index_ += i;
      return *this;
    }


    /// Returns a new iterator pointing to i further elements in the IdCondSet
    INLINE IdCondSetIterator IdCondSetIterator::operator+(const std::size_t i) {
      IdCondSetIterator res(*this);
      res += i;
      return res;
    }


    /// Returns the position of the iterator in the IdCondSet
    INLINE std::size_t IdCondSetIterator::pos() const {
      if (_seq_ == nullptr)
        GUM_ERROR(UndefinedIteratorValue,
                  "The IdCondSet is empty, so its iterators have no position")
      if (_index_ >= _seq_->size())
        GUM_ERROR(UndefinedIteratorValue,
                  "the IdCondSet iterator has no position because it reached "
                  "the set's end.");
      return _index_;
    }


    /// ==========================================================================
    /// ==========================================================================


    /// default constructor
    INLINE IdCondSet::IdCondSet() : _end_safe_(*this) { GUM_CONSTRUCTOR(IdCondSet); }


    /// default constructor with no variable on the left side
    INLINE IdCondSet::IdCondSet(const std::vector< NodeId >& ids,
                                const bool                   rhs_ids,
                                const bool                   ordered_ids) :
        _end_safe_(*this) {
      _ids_.resize(ids.size());

      // if the rhs_ids should be considered as unordered, we sort them by
      // increasing order so that we can compare easily two different rhs_ids
      if (!ordered_ids) {
        std::vector< NodeId > vect(ids);
        std::sort(vect.begin(), vect.end());
        for (const auto id: vect)
          _ids_ << id;
      } else {
        for (const auto id: ids)
          _ids_ << id;
      }

      if (!rhs_ids) _nb_lhs_ids_ = _ids_.size();

      // update the end iterator
      _end_safe_._gotoEnd_();

      GUM_CONSTRUCTOR(IdCondSet);
    }


    /// default constructor with one variable on the left side
    INLINE IdCondSet::IdCondSet(NodeId                       var1,
                                const std::vector< NodeId >& rhs_ids,
                                const bool                   ordered_rhs_ids) :
        _nb_lhs_ids_(std::size_t(1)),
        _end_safe_(*this) {
      _ids_.resize(rhs_ids.size() + std::size_t(1));
      _ids_ << var1;

      // if the rhs_ids should be considered as unordered, we sort them by
      // increasing order so that we can compare easily two different rhs_ids
      if (!ordered_rhs_ids) {
        std::vector< NodeId > vect(rhs_ids);
        std::sort(vect.begin(), vect.end());
        for (const auto id: vect)
          _ids_ << id;
      } else {
        for (const auto id: rhs_ids)
          _ids_ << id;
      }

      // update the end iterator
      _end_safe_._gotoEnd_();

      GUM_CONSTRUCTOR(IdCondSet);
    }


    /// default constructor with two variables on the left side
    INLINE IdCondSet::IdCondSet(NodeId                       var1,
                                NodeId                       var2,
                                const std::vector< NodeId >& rhs_ids,
                                const bool                   ordered_lhs_vars,
                                const bool                   ordered_rhs_ids) :
        _nb_lhs_ids_(std::size_t(2)),
        _end_safe_(*this) {
      _ids_.resize(rhs_ids.size() + std::size_t(2));

      // if the variables on the left side are unordered, sort them by
      // increasing order
      if (!ordered_lhs_vars && (var1 > var2)) std::swap(var1, var2);
      _ids_ << var1;
      _ids_ << var2;

      // if the rhs_ids should be considered as unordered, we sort them by
      // increasing order so that we can compare easily two different rhs_ids
      if (!ordered_rhs_ids) {
        std::vector< NodeId > vect(rhs_ids);
        std::sort(vect.begin(), vect.end());
        for (const auto id: vect)
          _ids_ << id;
      } else {
        for (const auto id: rhs_ids)
          _ids_ << id;
      }

      // update the end iterator
      _end_safe_._gotoEnd_();

      GUM_CONSTRUCTOR(IdCondSet);
    }


    /// default constructor with three variables on the left side
    INLINE IdCondSet::IdCondSet(NodeId                       var1,
                                NodeId                       var2,
                                NodeId                       var3,
                                const std::vector< NodeId >& rhs_ids,
                                const bool                   ordered_lhs_vars,
                                const bool                   ordered_rhs_ids) :
        _nb_lhs_ids_(std::size_t(3)),
        _end_safe_(*this) {
      _ids_.resize(rhs_ids.size() + std::size_t(3));

      // if the variables on the left side are unordered, sort them by
      // increasing order
      if (!ordered_lhs_vars) {
        if (var1 > var2) std::swap(var1, var2);
        if (var1 > var3) std::swap(var1, var3);
        if (var2 > var3) std::swap(var2, var3);
      }
      _ids_ << var1;
      _ids_ << var2;
      _ids_ << var3;

      // if the rhs_ids should be considered as unordered, we sort them by
      // increasing order so that we can compare easily two different rhs_ids
      if (!ordered_rhs_ids) {
        std::vector< NodeId > vect(rhs_ids);
        std::sort(vect.begin(), vect.end());
        for (const auto id: vect)
          _ids_ << id;
      } else {
        for (const auto id: rhs_ids)
          _ids_ << id;
      }

      // update the end iterator
      _end_safe_._gotoEnd_();

      GUM_CONSTRUCTOR(IdCondSet);
    }


    /// copy constructor
    INLINE IdCondSet::IdCondSet(const IdCondSet& from) :
        _ids_(from._ids_), _nb_lhs_ids_(from._nb_lhs_ids_), _end_safe_(*this) {
      _end_safe_._gotoEnd_();
      GUM_CONS_CPY(IdCondSet);
    }


    /// move constructor
    INLINE IdCondSet::IdCondSet(IdCondSet&& from) :
        _ids_(std::move(from._ids_)), _nb_lhs_ids_(from._nb_lhs_ids_), _end_safe_(*this) {
      _end_safe_._gotoEnd_();
      GUM_CONS_MOV(IdCondSet);
    }


    /// virtual copy constructor
    INLINE IdCondSet* IdCondSet::clone() const { return new IdCondSet(*this); }


    /// destructor
    INLINE IdCondSet::~IdCondSet() { GUM_DESTRUCTOR(IdCondSet); }


    /// copy operator
    INLINE IdCondSet& IdCondSet::operator=(const IdCondSet& from) {
      if (this != &from) {
        _ids_        = from._ids_;
        _nb_lhs_ids_ = from._nb_lhs_ids_;
        _end_safe_._gotoEnd_();
      }
      return *this;
    }


    /// move operator
    INLINE IdCondSet& IdCondSet::operator=(IdCondSet&& from) {
      if (this != &from) {
        _ids_        = std::move(from._ids_);
        _nb_lhs_ids_ = from._nb_lhs_ids_;
        _end_safe_._gotoEnd_();
      }
      return *this;
    }


    /// returns the id stored at a given index
    INLINE NodeId IdCondSet::operator[](const std::size_t index) const {
      return _ids_.atPos(index);
    }


    /// returns true if both sets are equal
    INLINE bool IdCondSet::operator==(const IdCondSet& from) const {
      if (_nb_lhs_ids_ != from._nb_lhs_ids_) return false;

      const std::size_t size = _ids_.size();

      if (size != from._ids_.size()) return false;

      for (std::size_t i = std::size_t(0); i < size; ++i) {
        if (_ids_[i] != from._ids_[i]) return false;
      }

      return true;
    }


    /// returns true if the sets differ
    INLINE bool IdCondSet::operator!=(const IdCondSet& from) const { return !operator==(from); }


    /// Returns a safe begin iterator.
    INLINE typename IdCondSet::iterator_safe IdCondSet::beginSafe() const {
      return IdCondSetIterator(*this);
    }


    /// Returns the safe end iterator.
    INLINE const typename IdCondSet::iterator_safe& IdCondSet::endSafe() const {
      return _end_safe_;
    }


    /// Returns an unsafe begin iterator.
    INLINE typename IdCondSet::iterator IdCondSet::begin() const {
      return IdCondSetIterator(*this);
    }


    /// Returns the unsafe end iterator.
    INLINE const typename IdCondSet::iterator& IdCondSet::end() const { return _end_safe_; }


    /// returns the set of ids contained in the object
    INLINE const Sequence< NodeId >& IdCondSet::ids() const { return _ids_; }


    /// returns the number of left hand side ids
    INLINE std::size_t IdCondSet::nbLHSIds() const { return _nb_lhs_ids_; }


    /// returns the number of right hand side ids
    INLINE std::size_t IdCondSet::nbRHSIds() const { return _ids_.size() - _nb_lhs_ids_; }


    /// removes all the nodes from the IdCondSet
    INLINE void IdCondSet::clear() {
      _ids_.clear();
      _nb_lhs_ids_ = std::size_t(0);
      _end_safe_._gotoEnd_();
    }


    /// returns the number of variables (both left and right hand side)
    INLINE std::size_t IdCondSet::size() const { return _ids_.size(); }


    /// returns the position of a given node in the IdCondSet
    INLINE std::size_t IdCondSet::pos(const NodeId id) const { return _ids_.pos(id); }


    /// indicates whether a given id is contained in the IdCondSet
    INLINE bool IdCondSet::exists(const NodeId id) const { return _ids_.exists(id); }


    /// indicates whether the idset contains a non-empty conditioning set
    INLINE bool IdCondSet::hasConditioningSet() const { return _nb_lhs_ids_ != _ids_.size(); }


    /// indicates whether the IdCondSet contains some nodes or not
    INLINE bool IdCondSet::empty() const { return _ids_.empty(); }

  } /* namespace learning */


  // the hash function for idSets
  INLINE Size HashFunc< learning::IdCondSet >::operator()(const learning::IdCondSet& key) const {
    return (castToSize(key) * HashFuncConst::gold) & this->hash_mask_;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
