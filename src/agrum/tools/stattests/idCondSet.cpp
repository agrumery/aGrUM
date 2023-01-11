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


/**
 * @file
 * @brief Implementation of idSets
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/stattests/idCondSet.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/stattests/idCondSet_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// returns the idSet at the right hand side of the conditioning bar
    IdCondSet IdCondSet::conditionalIdCondSet() const {
      IdCondSet         set;
      const std::size_t size = _ids_.size();
      for (std::size_t i = _nb_lhs_ids_; i < size; ++i)
        set._ids_ << _ids_[i];
      set._end_safe_._gotoEnd_();
      return set;
    }


    /// erase a node in the idset
    void IdCondSet::erase(const NodeId id) {
      // search for id in Sequence  _ids_
      const std::size_t size = _ids_.size();
      std::size_t       pos  = std::size_t(0);
      for (; pos < size; ++pos) {
        if (_ids_[pos] == id) break;
      }

      // if we found the id, remove it
      if (pos < size) {
        _ids_.erase(SequenceIteratorSafe< NodeId >(_ids_, pos));
        if (pos < _nb_lhs_ids_) --_nb_lhs_ids_;
        _end_safe_._gotoEnd_();
      }
    }


    /// returns the content of the set as a string
    std::string IdCondSet::toString() const {
      std::stringstream str;

      str << '{';
      bool deja = false;

      for (std::size_t i = std::size_t(0); i < _nb_lhs_ids_; ++i) {
        if (deja) str << " , ";
        else deja = true;
        str << _ids_[i];
      }

      deja = false;
      for (auto iter = _ids_.begin() + _nb_lhs_ids_; iter != _ids_.end(); ++iter) {
        if (deja) str << " , ";
        else {
          deja = true;
          str << " | ";
        }
        str << *iter;
      }

      str << '}';

      return str.str();
    }

    std::pair< NodeSet, NodeSet > IdCondSet::toNodeSets() const {
      gum::NodeSet left;
      gum::NodeSet right;

      for (auto i = std::size_t(0); i < _ids_.size(); ++i)
        if (i < _nb_lhs_ids_) left.insert(_ids_[i]);
        else right.insert(_ids_[i]);
      return {left, right};
    }


    /// indicates whether the IdCondSet contains the IdCondSet passed in argument
    bool IdCondSet::contains(const IdCondSet& set) const {
      if (set._ids_.size() > _ids_.size()) return false;
      for (const auto node: set._ids_) {
        if (!_ids_.exists(node)) return false;
      }
      return true;
    }


    // the display operator
    std::ostream& operator<<(std::ostream& stream, const IdCondSet& idset) {
      return stream << idset.toString();
    }

  } /* namespace learning */


  // Returns the value of a key as a Size.
  Size HashFunc< learning::IdCondSet >::castToSize(const learning::IdCondSet& key) {
    Size                      h    = Size(key.nbLHSIds());
    const Sequence< NodeId >& vect = key.ids();
    const std::size_t         size = vect.size();

    std::size_t i = std::size_t(0);
    while (i < size) {
      const Size id = Size(vect[i]);
      ++i;
      h += Size(i) * id;
    }

    return h;
  }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
