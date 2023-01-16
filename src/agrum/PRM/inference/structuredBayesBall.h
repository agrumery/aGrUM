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
 * @brief Headers of StructuredBayesBall.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <sstream>
#include <string>
#include <utility>


#include <agrum/PRM/inference/PRMInference.h>

#ifndef GUM_STRUCTURED_BB_H
#  define GUM_STRUCTURED_BB_H
namespace gum {
  namespace prm {

    /**
     * @class StructuredBayesBall structuredBayesBall.h
     * <agrum/PRM/structuredBayesBall.h>
     * @brief This class represent the BayesBall algorithm applied on PRMs.
     */
    template < typename GUM_SCALAR >
    class StructuredBayesBall {
      public:
      // ========================================================================
      /// @name Constructors & destructor.
      // ========================================================================
      /// @{

      /// Default Constructor.
      StructuredBayesBall(const PRMInference< GUM_SCALAR >& inference);

      /// Destructor.
      ~StructuredBayesBall();

      /// @}
      // ========================================================================
      /// @name Getters and Setters.
      // ========================================================================
      /// @{

      /// Returns a unique key w.r.t. d-separation for i.
      const std::string& key(const PRMInstance< GUM_SCALAR >* i) const;

      /// Returns a unique key w.r.t. d-separation for i.
      const std::string& key(const PRMInstance< GUM_SCALAR >& i) const;

      /// Returns the set of requisite nodes w.r.t. d-separation for i.
      const Set< NodeId >& requisiteNodes(const PRMInstance< GUM_SCALAR >* i) const;

      /// Returns the set of requisite nodes w.r.t. d-separation for i.
      const Set< NodeId >& requisiteNodes(const PRMInstance< GUM_SCALAR >& i) const;

      /// Returns the number of occurrence of the given key, which is the number
      /// of PRMInstance<GUM_SCALAR> sharing the same set of requisite nodes.
      Size occurrence(const std::string& key) const;

      /// Returns the ratio between the total number of instances and the number
      /// of instances with the same configuration.
      float liftRatio() const;

      /// Returns true if i has requisite nodes.
      bool exists(const PRMInstance< GUM_SCALAR >* i) const;

      /// Returns true if i has requisite nodes.
      bool exists(const PRMInstance< GUM_SCALAR >& i) const;

      /// @}

      /// Compute the set or requisite nodes for each required instance given
      /// the current set of observations.
      /// Discard previous computations.
      void compute(const PRMInstance< GUM_SCALAR >* i, NodeId n);

      /// Compute the set or requisite nodes for each required instance given
      /// the current set of observations.
      /// Discard previous computations.
      void compute(const PRMInstance< GUM_SCALAR >& i, NodeId n);

      /// Returns true if there is a hard evidence on i->get(n).
      bool _isHardEvidence_(const PRMInstance< GUM_SCALAR >* i, NodeId n);

      private:
      /// Copy constructor.
      StructuredBayesBall(const StructuredBayesBall& source);

      /// Copy operator.
      StructuredBayesBall& operator=(const StructuredBayesBall& source);

      /// Code alias
      using MarkMap     = HashTable< NodeId, std::pair< bool, bool > >;
      using InstanceMap = HashTable< const PRMInstance< GUM_SCALAR >*, MarkMap* >;

      /// Code alias
      std::pair< bool, bool >&
         _getMark_(InstanceMap& marks, const PRMInstance< GUM_SCALAR >* i, NodeId n);
      /// Code alias
      const PRMSlotChain< GUM_SCALAR >& _getSC_(const PRMInstance< GUM_SCALAR >* i, NodeId n);

      /// Cleans this before a new computation.
      void _clean_();

      /// The real compute method.
      void _compute_(const PRMInstance< GUM_SCALAR >* i, NodeId n);

      /// When the ball is received on i->get(n) from a child.
      void _fromChild_(const PRMInstance< GUM_SCALAR >* i, NodeId n, InstanceMap& marks);

      /// When the ball is receive on i->get(n) from a parent.
      void _fromParent_(const PRMInstance< GUM_SCALAR >* i, NodeId n, InstanceMap& marks);

      /// Fill  _keyMap_ and  _reqMap_.
      void _fillMaps_(InstanceMap& marks);

      /// Builds the HashKey for the given instance and requisite nodes set.
      std::string _buildHashKey_(const PRMInstance< GUM_SCALAR >* i, Set< NodeId >& req_nodes);

      /// The PRM at which  _model_ belongs.
      const PRMInference< GUM_SCALAR >* _inf_;

      /// Associate an PRMInstance<GUM_SCALAR> with a unique key w.r.t.
      /// d-separation and
      /// the
      /// set of requisite nodes deduced from d-separation analysis.
      HashTable< const PRMInstance< GUM_SCALAR >*, std::pair< std::string, Set< NodeId >* > >
         _keyMap_;

      /// Associate a Key with the set of requisite nodes associated with it.
      /// The Size value is the number of instance with the same key.
      HashTable< std::string, std::pair< Set< NodeId >*, Size > > _reqMap_;
    };


#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class StructuredBayesBall< double >;
#  endif


  } /* namespace prm */
} /* namespace gum */

#  include <agrum/PRM/inference/structuredBayesBall_tpl.h>

#endif /* GUM_STRUCTURED_BB_H */
