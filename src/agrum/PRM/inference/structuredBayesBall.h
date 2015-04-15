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
 * @brief Headers of StructuredBayesBall.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <utility>
#include <string>
#include <sstream>

#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/inference/PRMInference.h>

#ifndef GUM_STRUCTURED_BB_H
#define GUM_STRUCTURED_BB_H
namespace gum {
  namespace prm {

    /**
     * @class StructuredBayesBall structuredBayesBall.h
     * <agrum/PRM/structuredBayesBall.h>
     * @brief This class represent the BayesBall algorithm applied on PRMs.
     */
    template <typename GUM_SCALAR> class StructuredBayesBall {
      public:
      // ========================================================================
      /// @name Constructors & destructor.
      // ========================================================================
      /// @{

      /// Default Constructor.
      StructuredBayesBall(const PRMInference<GUM_SCALAR> &inference);

      /// Destructor.
      ~StructuredBayesBall();

      /// @}
      // ========================================================================
      /// @name Getters and Setters.
      // ========================================================================
      /// @{

      /// Returns a unique key w.r.t. d-separation for i.
      const std::string &key(const Instance<GUM_SCALAR> *i) const;

      /// Returns a unique key w.r.t. d-separation for i.
      const std::string &key(const Instance<GUM_SCALAR> &i) const;

      /// Returns the set of requisite nodes w.r.t. d-separation for i.
      const Set<NodeId> &requisiteNodes(const Instance<GUM_SCALAR> *i) const;

      /// Returns the set of requisite nodes w.r.t. d-separation for i.
      const Set<NodeId> &requisiteNodes(const Instance<GUM_SCALAR> &i) const;

      /// Returns the number of occurrence of the given key, which is the number
      /// of Instance<GUM_SCALAR> sharing the same set of requisite nodes.
      Size occurrence(const std::string &key) const;

      /// Returns the ratio between the total number of instances and the number
      /// of instances with the same configuration.
      float liftRatio() const;

      /// Returns true if i has requisite nodes.
      bool exists(const Instance<GUM_SCALAR> *i) const;

      /// Returns true if i has requisite nodes.
      bool exists(const Instance<GUM_SCALAR> &i) const;

      /// @}

      /// Compute the set or requisite nodes for each required instance given
      /// the current set of observations.
      /// Discard previous computations.
      void compute(const Instance<GUM_SCALAR> *i, NodeId n);

      /// Compute the set or requisite nodes for each required instance given
      /// the current set of observations.
      /// Discard previous computations.
      void compute(const Instance<GUM_SCALAR> &i, NodeId n);

      /// Returns true if there is a hard evidence on i->get(n).
      bool __isHardEvidence(const Instance<GUM_SCALAR> *i, NodeId n);

      private:
      /// Copy constructor.
      StructuredBayesBall(const StructuredBayesBall &source);

      /// Copy operator.
      StructuredBayesBall &operator=(const StructuredBayesBall &source);

      /// Code alias
      typedef HashTable<NodeId, std::pair<bool, bool>> MarkMap;
      /// Code alias
      typedef HashTable<const Instance<GUM_SCALAR> *, MarkMap *> InstanceMap;
      /// Code alias
      std::pair<bool, bool> &__getMark(InstanceMap &marks,
                                       const Instance<GUM_SCALAR> *i, NodeId n);
      /// Code alias
      const SlotChain<GUM_SCALAR> &__getSC(const Instance<GUM_SCALAR> *i, NodeId n);

      /// Cleans this before a new computation.
      void __clean();

      /// The real compute method.
      void __compute(const Instance<GUM_SCALAR> *i, NodeId n);

      /// When the ball is received on i->get(n) from a child.
      void __fromChild(const Instance<GUM_SCALAR> *i, NodeId n, InstanceMap &marks);

      /// When the ball is receive on i->get(n) from a parent.
      void __fromParent(const Instance<GUM_SCALAR> *i, NodeId n, InstanceMap &marks);

      /// Fill __keyMap and __reqMap.
      void __fillMaps(InstanceMap &marks);

      /// Builds the HashKey for the given instance and requisite nodes set.
      std::string __buildHashKey(const Instance<GUM_SCALAR> *i,
                                 Set<NodeId> &req_nodes);

      /// The PRM at which __model belongs.
      const PRMInference<GUM_SCALAR> *__inf;

      /// Associate an Instance<GUM_SCALAR> with a unique key w.r.t. d-separation and
      /// the
      /// set of requisite nodes deduced from d-separation analysis.
      HashTable<const Instance<GUM_SCALAR> *, std::pair<std::string, Set<NodeId> *>>
          __keyMap;

      /// Associate a Key with the set of requisite nodes associated with it.
      /// The Size value is the number of instance with the same key.
      HashTable<std::string, std::pair<Set<NodeId> *, Size>> __reqMap;
    };

    extern template class StructuredBayesBall<double>;
  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/inference/structuredBayesBall.tcc>

#endif /* GUM_STRUCTURED_BB_H */
