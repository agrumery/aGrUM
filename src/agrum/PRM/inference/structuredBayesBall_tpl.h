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
 * @brief Inline implementation of StructuredBayesBall.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/PRM/inference/structuredBayesBall.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    StructuredBayesBall< GUM_SCALAR >::~StructuredBayesBall() {
      GUM_DESTRUCTOR(StructuredBayesBall);

      for (const auto& elt: _reqMap_)
        delete elt.second.first;
    }

    template < typename GUM_SCALAR >
    void StructuredBayesBall< GUM_SCALAR >::_clean_() {
      for (const auto& elt: _reqMap_)
        delete elt.second.first;

      _keyMap_.clear();
      _reqMap_.clear();
    }

    template < typename GUM_SCALAR >
    bool StructuredBayesBall< GUM_SCALAR >::_isHardEvidence_(const PRMInstance< GUM_SCALAR >* i,
                                                             NodeId                           n) {
      try {
        typename PRMInference< GUM_SCALAR >::Chain chain = std::make_pair(i, &(i->get(n)));

        if (_inf_->hasEvidence(chain)) {
          const Potential< GUM_SCALAR >* e = _inf_->evidence(i)[n];
          Instantiation                  inst(e);
          Size                           count = 0;

          for (inst.setFirst(); !inst.end(); inst.inc()) {
            if ((e->get(inst) == (GUM_SCALAR)1.0)) ++count;
            else if (e->get(inst) != (GUM_SCALAR)0.0) return false;
          }

          return (count == 1);
        }

        return false;
      } catch (NotFound const&) { return false; }
    }

    template < typename GUM_SCALAR >
    void StructuredBayesBall< GUM_SCALAR >::_compute_(const PRMInstance< GUM_SCALAR >* i,
                                                      NodeId                           n) {
      _clean_();
      /// Key = instance.PRMClassElement<GUM_DATA>
      /// pair = <upper mark, lower mark>
      StructuredBayesBall< GUM_SCALAR >::InstanceMap marks;
      _fromChild_(i, n, marks);
      _fillMaps_(marks);

      for (const auto& elt: marks)
        delete elt.second;
    }

    template < typename GUM_SCALAR >
    void StructuredBayesBall< GUM_SCALAR >::_fromChild_(const PRMInstance< GUM_SCALAR >* i,
                                                        NodeId                           n,
                                                        InstanceMap&                     marks) {
      if (!marks.exists(i)) { marks.insert(i, new StructuredBayesBall< GUM_SCALAR >::MarkMap()); }

      if (!marks[i]->exists(n)) { marks[i]->insert(n, std::pair< bool, bool >(false, false)); }

      // Sending message to parents
      switch (i->type().get(n).elt_type()) {
        case PRMClassElement< GUM_SCALAR >::prm_slotchain: {
          if (!_getMark_(marks, i, n).first) {
            _getMark_(marks, i, n).first = true;

            for (const auto inst: i->getInstances(n))
              _fromChild_(inst, inst->get(_getSC_(i, n).lastElt().safeName()).id(), marks);
          }

          if (!_getMark_(marks, i, n).second) {
            _getMark_(marks, i, n).second = true;

            for (const auto chi: i->type().containerDag().children(n))
              _fromParent_(i, chi, marks);
          }

          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_aggregate:
        case PRMClassElement< GUM_SCALAR >::prm_attribute: {
          if (!_getMark_(marks, i, n).first) {
            _getMark_(marks, i, n).first = true;

            if (!_isHardEvidence_(i, n))
              for (const auto par: i->type().containerDag().parents(n))
                _fromChild_(i, par, marks);
          }

          if (!_getMark_(marks, i, n).second) {
            _getMark_(marks, i, n).second = true;

            // In i.
            for (const auto chi: i->type().containerDag().children(n))
              _fromParent_(i, chi, marks);

            // Out of i.
            try {
              const auto& refs = i->getRefAttr(n);

              for (auto iter = refs.begin(); iter != refs.end(); ++iter)
                _fromParent_(iter->first, iter->first->type().get(iter->second).id(), marks);
            } catch (NotFound const&) {
              // Not an inverse sc
            }
          }

          break;
        }

        default: {
          // We shouldn't reach any other PRMClassElement<GUM_DATA> than
          // PRMAttribute
          // or
          // PRMSlotChain<GUM_SCALAR>.
          GUM_ERROR(FatalError, "This case is impossible.")
        }
      }
    }

    template < typename GUM_SCALAR >
    void StructuredBayesBall< GUM_SCALAR >::_fromParent_(const PRMInstance< GUM_SCALAR >* i,
                                                         NodeId                           n,
                                                         InstanceMap&                     marks) {
      if (!marks.exists(i)) { marks.insert(i, new StructuredBayesBall< GUM_SCALAR >::MarkMap()); }

      if (!marks[i]->exists(n)) { marks[i]->insert(n, std::pair< bool, bool >(false, false)); }

      // Concerns only PRMAttribute (because of the hard evidence)
      if ((_isHardEvidence_(i, n)) && (!_getMark_(marks, i, n).first)) {
        _getMark_(marks, i, n).first = true;

        for (const auto par: i->type().containerDag().parents(n))
          _fromChild_(i, par, marks);
      } else if (!_getMark_(marks, i, n).second) {
        _getMark_(marks, i, n).second = true;

        // In i.
        for (const auto chi: i->type().containerDag().children(n))
          _fromParent_(i, chi, marks);

        // Out of i.
        try {
          for (auto iter = i->getRefAttr(n).begin(); iter != i->getRefAttr(n).end(); ++iter)
            _fromParent_(iter->first, iter->first->type().get(iter->second).id(), marks);
        } catch (NotFound const&) {
          // Not an inverse sc
        }
      }
    }

    template < typename GUM_SCALAR >
    void StructuredBayesBall< GUM_SCALAR >::_fillMaps_(InstanceMap& marks) {
      // First find for each instance it's requisite nodes
      HashTable< const PRMInstance< GUM_SCALAR >*, Set< NodeId >* > req_map;

      for (const auto& elt: marks) {
        Set< NodeId >* req_set = new Set< NodeId >();

        for (const auto& elt2: *elt.second)
          if (elt2.second.first) req_set->insert(elt2.first);

        req_map.insert(elt.first, req_set);
      }

      // Remove all instances with 0 requisite nodes
      Set< const PRMInstance< GUM_SCALAR >* > to_remove;

      for (const auto& elt: req_map)
        if (elt.second->size() == 0) to_remove.insert(elt.first);

      for (const auto remo: to_remove) {
        delete req_map[remo];
        req_map.erase(remo);
      }

      // Fill  _reqMap_ and  _keyMap_
      for (const auto& elt: req_map) {
        std::string key = _buildHashKey_(elt.first, *elt.second);

        if (_reqMap_.exists(key)) {
          _keyMap_.insert(elt.first,
                          std::pair< std::string, Set< NodeId >* >(key, _reqMap_[key].first));
          _reqMap_[key].second += 1;
          delete elt.second;
          req_map[elt.first] = 0;
        } else {
          _reqMap_.insert(key, std::pair< Set< NodeId >*, Size >(elt.second, 1));
          _keyMap_.insert(elt.first, std::pair< std::string, Set< NodeId >* >(key, elt.second));
        }
      }
    }

    template < typename GUM_SCALAR >
    std::string
       StructuredBayesBall< GUM_SCALAR >::_buildHashKey_(const PRMInstance< GUM_SCALAR >* i,
                                                         Set< NodeId >& req_nodes) {
      std::stringstream sBuff;
      sBuff << i->type().name();

      for (const auto node: i->type().containerDag().nodes())
        if (req_nodes.exists(node)) sBuff << "-" << node;

      return sBuff.str();
    }

    template < typename GUM_SCALAR >
    INLINE StructuredBayesBall< GUM_SCALAR >::StructuredBayesBall(
       const PRMInference< GUM_SCALAR >& inference) :
        _inf_(&inference) {
      GUM_CONSTRUCTOR(StructuredBayesBall);
    }

    template < typename GUM_SCALAR >
    INLINE StructuredBayesBall< GUM_SCALAR >::StructuredBayesBall(
       const StructuredBayesBall< GUM_SCALAR >& source) :
        _inf_(0) {
      GUM_CONS_CPY(StructuredBayesBall);
      GUM_ERROR(FatalError, "Not allowed.")
    }

    template < typename GUM_SCALAR >
    INLINE StructuredBayesBall< GUM_SCALAR >& StructuredBayesBall< GUM_SCALAR >::operator=(
       const StructuredBayesBall< GUM_SCALAR >& source) {
      GUM_ERROR(FatalError, "Not allowed.")
    }

    template < typename GUM_SCALAR >
    INLINE const std::string&
                 StructuredBayesBall< GUM_SCALAR >::key(const PRMInstance< GUM_SCALAR >* i) const {
      return _keyMap_[i].first;
    }

    template < typename GUM_SCALAR >
    INLINE const std::string&
                 StructuredBayesBall< GUM_SCALAR >::key(const PRMInstance< GUM_SCALAR >& i) const {
      return _keyMap_[&i].first;
    }

    template < typename GUM_SCALAR >
    INLINE const Set< NodeId >&
       StructuredBayesBall< GUM_SCALAR >::requisiteNodes(const PRMInstance< GUM_SCALAR >* i) const {
      return *(_keyMap_[i].second);
    }

    template < typename GUM_SCALAR >
    INLINE const Set< NodeId >&
       StructuredBayesBall< GUM_SCALAR >::requisiteNodes(const PRMInstance< GUM_SCALAR >& i) const {
      return *(_keyMap_[&i].second);
    }

    template < typename GUM_SCALAR >
    INLINE Size StructuredBayesBall< GUM_SCALAR >::occurrence(const std::string& key) const {
      return _reqMap_[key].second;
    }

    template < typename GUM_SCALAR >
    INLINE float StructuredBayesBall< GUM_SCALAR >::liftRatio() const {
      return ((float)_reqMap_.size()) / ((float)_keyMap_.size());
    }

    template < typename GUM_SCALAR >
    INLINE bool
       StructuredBayesBall< GUM_SCALAR >::exists(const PRMInstance< GUM_SCALAR >* i) const {
      return _keyMap_.exists(i);
    }

    template < typename GUM_SCALAR >
    INLINE bool
       StructuredBayesBall< GUM_SCALAR >::exists(const PRMInstance< GUM_SCALAR >& i) const {
      return _keyMap_.exists(&i);
    }

    template < typename GUM_SCALAR >
    INLINE void StructuredBayesBall< GUM_SCALAR >::compute(const PRMInstance< GUM_SCALAR >* i,
                                                           NodeId                           n) {
      _compute_(i, n);
    }

    template < typename GUM_SCALAR >
    INLINE void StructuredBayesBall< GUM_SCALAR >::compute(const PRMInstance< GUM_SCALAR >& i,
                                                           NodeId                           n) {
      _compute_(&i, n);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMSlotChain< GUM_SCALAR >&
       StructuredBayesBall< GUM_SCALAR >::_getSC_(const PRMInstance< GUM_SCALAR >* i, NodeId n) {
      return static_cast< const PRMSlotChain< GUM_SCALAR >& >(i->type().get(n));
    }

    template < typename GUM_SCALAR >
    INLINE std::pair< bool, bool >&
           StructuredBayesBall< GUM_SCALAR >::_getMark_(InstanceMap&                     marks,
                                                    const PRMInstance< GUM_SCALAR >* i,
                                                    NodeId                           n) {
      return (*(marks[i]))[n];
    }

  } /* namespace prm */
} /* namespace gum */
