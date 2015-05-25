/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
/**
 * @file
 * @brief Inline implementation of StructuredBayesBall.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/inference/structuredBayesBall.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    StructuredBayesBall<GUM_SCALAR>::~StructuredBayesBall() {
      GUM_DESTRUCTOR(StructuredBayesBall);

      for (const auto &elt : __reqMap)
        delete elt.second.first;
    }

    template <typename GUM_SCALAR> void StructuredBayesBall<GUM_SCALAR>::__clean() {
      for (const auto &elt : __reqMap)
        delete elt.second.first;

      __keyMap.clear();
      __reqMap.clear();
    }

    template <typename GUM_SCALAR>
    bool
    StructuredBayesBall<GUM_SCALAR>::__isHardEvidence(const Instance<GUM_SCALAR> *i,
                                                      NodeId n) {
      try {
        typename PRMInference<GUM_SCALAR>::Chain chain =
            std::make_pair(i, &(i->get(n)));

        if (__inf->hasEvidence(chain)) {
          const Potential<GUM_SCALAR> *e = __inf->evidence(i)[n];
          Instantiation inst(e);
          Size count = 0;

          for (inst.setFirst(); not inst.end(); inst.inc()) {
            if ((e->get(inst) == (GUM_SCALAR)1.0))
              ++count;
            else if (e->get(inst) != (GUM_SCALAR)0.0)
              return false;
          }

          return (count == 1);
        }

        return false;
      } catch (NotFound &) {
        return false;
      }
    }

    template <typename GUM_SCALAR>
    void StructuredBayesBall<GUM_SCALAR>::__compute(const Instance<GUM_SCALAR> *i,
                                                    NodeId n) {
      __clean();
      /// Key = instance.ClassElement<GUM_DATA>
      /// pair = <upper mark, lower mark>
      StructuredBayesBall<GUM_SCALAR>::InstanceMap marks;
      __fromChild(i, n, marks);
      __fillMaps(marks);

      for (const auto &elt : marks)
        delete elt.second;
    }

    template <typename GUM_SCALAR>
    void StructuredBayesBall<GUM_SCALAR>::__fromChild(const Instance<GUM_SCALAR> *i,
                                                      NodeId n, InstanceMap &marks) {
      // std::stringstream sBuff;
      // sBuff << i->name() << "." << i->type().get(n).safeName();
      // std::string in = "in __fromChild ";
      // GUM_TRACE(in + sBuff.str());
      if (not marks.exists(i)) {
        marks.insert(i, new StructuredBayesBall<GUM_SCALAR>::MarkMap());
      }

      if (not marks[i]->exists(n)) {
        marks[i]->insert(n, std::pair<bool, bool>(false, false));
      }

      // Sending message to parents
      switch (i->type().get(n).elt_type()) {
        case ClassElement<GUM_SCALAR>::prm_slotchain: {
          if (not __getMark(marks, i, n).first) {
            __getMark(marks, i, n).first = true;

            for (const auto inst : i->getInstances(n))
              __fromChild(inst, inst->get(__getSC(i, n).lastElt().safeName()).id(),
                          marks);
          }

          if (not __getMark(marks, i, n).second) {
            __getMark(marks, i, n).second = true;

            for (const auto chi : i->type().dag().children(n))
              __fromParent(i, chi, marks);
          }

          break;
        }

        case ClassElement<GUM_SCALAR>::prm_aggregate:
        case ClassElement<GUM_SCALAR>::prm_attribute: {
          if (not __getMark(marks, i, n).first) {
            __getMark(marks, i, n).first = true;

            if (not __isHardEvidence(i, n))
              for (const auto par : i->type().dag().parents(n))
                __fromChild(i, par, marks);
          }

          if (not __getMark(marks, i, n).second) {
            __getMark(marks, i, n).second = true;

            // In i.
            for (const auto chi : i->type().dag().children(n))
              __fromParent(i, chi, marks);

            // Out of i.
            try {
              const auto &refs = i->getRefAttr(n);

              for (auto iter = refs.begin(); iter != refs.end(); ++iter)
                __fromParent(iter->first, iter->first->type().get(iter->second).id(),
                             marks);
            } catch (NotFound &) {
              // Not an inverse sc
            }
          }

          break;
        }

        default: {
          // We shouldn't reach any other ClassElement<GUM_DATA> than Attribute or
          // SlotChain<GUM_SCALAR>.
          GUM_ERROR(FatalError, "This case is impossible.");
        }
      }
    }

    template <typename GUM_SCALAR>
    void StructuredBayesBall<GUM_SCALAR>::__fromParent(const Instance<GUM_SCALAR> *i,
                                                       NodeId n,
                                                       InstanceMap &marks) {
      // std::stringstream sBuff;
      // sBuff << i->name() << "." << i->type().get(n).safeName();
      // std::string in = "in __fromParent ";
      // GUM_TRACE(in + sBuff.str());
      if (not marks.exists(i)) {
        marks.insert(i, new StructuredBayesBall<GUM_SCALAR>::MarkMap());
      }

      if (not marks[i]->exists(n)) {
        marks[i]->insert(n, std::pair<bool, bool>(false, false));
      }

      // Concerns only Attribute (because of the hard evidence)
      if ((__isHardEvidence(i, n)) and (not __getMark(marks, i, n).first)) {
        __getMark(marks, i, n).first = true;

        for (const auto par : i->type().dag().parents(n))
          __fromChild(i, par, marks);
      } else if (not __getMark(marks, i, n).second) {
        __getMark(marks, i, n).second = true;

        // In i.
        for (const auto chi : i->type().dag().children(n))
          __fromParent(i, chi, marks);

        // Out of i.
        try {
          for (auto iter = i->getRefAttr(n).begin(); iter != i->getRefAttr(n).end();
               ++iter)
            __fromParent(iter->first, iter->first->type().get(iter->second).id(),
                         marks);
        } catch (NotFound &) {
          // Not an inverse sc
        }
      }

      // std::string out = "out of __fromParent ";
      // GUM_TRACE(out + sBuff.str());
    }

    template <typename GUM_SCALAR>
    void StructuredBayesBall<GUM_SCALAR>::__fillMaps(InstanceMap &marks) {
      // First find for each instance it's requisite nodes
      HashTable<const Instance<GUM_SCALAR> *, Set<NodeId> *> req_map;

      for (const auto &elt : marks) {
        Set<NodeId> *req_set = new Set<NodeId>();

        for (const auto &elt2 : *elt.second)
          if (elt2.second.first)
            req_set->insert(elt2.first);

        req_map.insert(elt.first, req_set);
      }

      // Remove all instances with 0 requisite nodes
      Set<const Instance<GUM_SCALAR> *> to_remove;

      for (const auto &elt : req_map)
        if (elt.second->size() == 0)
          to_remove.insert(elt.first);

      for (const auto remo : to_remove) {
        delete req_map[remo];
        req_map.erase(remo);
      }

      // Fill __reqMap and __keyMap
      for (const auto &elt : req_map) {
        std::string key = __buildHashKey(elt.first, *elt.second);

        if (__reqMap.exists(key)) {
          __keyMap.insert(elt.first, std::pair<std::string, Set<NodeId> *>(
                                         key, __reqMap[key].first));
          __reqMap[key].second += 1;
          delete elt.second;
          req_map[elt.first] = 0;
        } else {
          __reqMap.insert(key, std::pair<Set<NodeId> *, Size>(elt.second, 1));
          __keyMap.insert(elt.first,
                          std::pair<std::string, Set<NodeId> *>(key, elt.second));
        }
      }
    }

    template <typename GUM_SCALAR>
    std::string
    StructuredBayesBall<GUM_SCALAR>::__buildHashKey(const Instance<GUM_SCALAR> *i,
                                                    Set<NodeId> &req_nodes) {
      std::stringstream sBuff;
      sBuff << i->type().name();

      for (const auto node : i->type().dag().nodes())
        if (req_nodes.exists(node))
          sBuff << "-" << node;

      return sBuff.str();
    }

    template <typename GUM_SCALAR>
    INLINE StructuredBayesBall<GUM_SCALAR>::StructuredBayesBall(
        const PRMInference<GUM_SCALAR> &inference)
        : __inf(&inference) {
      GUM_CONSTRUCTOR(StructuredBayesBall);
    }

    template <typename GUM_SCALAR>
    INLINE StructuredBayesBall<GUM_SCALAR>::StructuredBayesBall(
        const StructuredBayesBall<GUM_SCALAR> &source)
        : __inf(0) {
      GUM_CONS_CPY(StructuredBayesBall);
      GUM_ERROR(FatalError, "Not allowed.");
    }

    template <typename GUM_SCALAR>
    INLINE StructuredBayesBall<GUM_SCALAR> &StructuredBayesBall<GUM_SCALAR>::
    operator=(const StructuredBayesBall<GUM_SCALAR> &source) {
      GUM_ERROR(FatalError, "Not allowed.");
    }

    template <typename GUM_SCALAR>
    INLINE const std::string &
    StructuredBayesBall<GUM_SCALAR>::key(const Instance<GUM_SCALAR> *i) const {
      return __keyMap[i].first;
    }

    template <typename GUM_SCALAR>
    INLINE const std::string &
    StructuredBayesBall<GUM_SCALAR>::key(const Instance<GUM_SCALAR> &i) const {
      return __keyMap[&i].first;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<NodeId> &StructuredBayesBall<GUM_SCALAR>::requisiteNodes(
        const Instance<GUM_SCALAR> *i) const {
      return *(__keyMap[i].second);
    }

    template <typename GUM_SCALAR>
    INLINE const Set<NodeId> &StructuredBayesBall<GUM_SCALAR>::requisiteNodes(
        const Instance<GUM_SCALAR> &i) const {
      return *(__keyMap[&i].second);
    }

    template <typename GUM_SCALAR>
    INLINE Size
        StructuredBayesBall<GUM_SCALAR>::occurrence(const std::string &key) const {
      return __reqMap[key].second;
    }

    template <typename GUM_SCALAR>
    INLINE float StructuredBayesBall<GUM_SCALAR>::liftRatio() const {
      return ((float)__reqMap.size()) / ((float)__keyMap.size());
    }

    template <typename GUM_SCALAR>
    INLINE bool
    StructuredBayesBall<GUM_SCALAR>::exists(const Instance<GUM_SCALAR> *i) const {
      return __keyMap.exists(i);
    }

    template <typename GUM_SCALAR>
    INLINE bool
    StructuredBayesBall<GUM_SCALAR>::exists(const Instance<GUM_SCALAR> &i) const {
      return __keyMap.exists(&i);
    }

    template <typename GUM_SCALAR>
    INLINE void
    StructuredBayesBall<GUM_SCALAR>::compute(const Instance<GUM_SCALAR> *i,
                                             NodeId n) {
      __compute(i, n);
    }

    template <typename GUM_SCALAR>
    INLINE void
    StructuredBayesBall<GUM_SCALAR>::compute(const Instance<GUM_SCALAR> &i,
                                             NodeId n) {
      __compute(&i, n);
    }

    template <typename GUM_SCALAR>
    INLINE const SlotChain<GUM_SCALAR> &
    StructuredBayesBall<GUM_SCALAR>::__getSC(const Instance<GUM_SCALAR> *i,
                                             NodeId n) {
      return static_cast<const SlotChain<GUM_SCALAR> &>(i->type().get(n));
    }

    template <typename GUM_SCALAR>
    INLINE std::pair<bool, bool> &StructuredBayesBall<GUM_SCALAR>::__getMark(
        InstanceMap &marks, const Instance<GUM_SCALAR> *i, NodeId n) {
      return (*(marks[i]))[n];
    }

  } /* namespace prm */
} /* namespace gum */
