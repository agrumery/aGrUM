
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief Headers of the SearchStrategy class and child
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SEARCHSTRATEGY_H
#define GUM_SEARCHSTRATEGY_H

#include <list>
#include <ostream>
#include <utility>
#include <vector>

#include <agrum/core/math/math.h>
#include <agrum/core/bijection.h>
#include <agrum/core/sequence.h>
#include <agrum/core/set.h>

#include <agrum/graphs/diGraph.h>

#include <agrum/graphs/algorithms/triangulations/partialOrderedTriangulation.h>

#include <agrum/PRM/gspan/edgeGrowth.h>
#include <agrum/PRM/gspan/interfaceGraph.h>
#include <agrum/PRM/gspan/pattern.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    class GSpan;

    namespace gspan {
      template < typename GUM_SCALAR >
      class DFSTree;

      // clang_format off
      /**
       * @class SearchStrategy
       * @headerfile searchStrategy.h <agrum/PRM/gspan/searchStrategy.h>
       *
       * This is an abstract class used to tune search strategies in the gspan
       * algorithm. Since GSpan uses a DFS to expand the search tree, this class
       * works as a stack regarding adding and removing informations about the
       * growths.
       */
      // clang_format on
      template < typename GUM_SCALAR >
      class SearchStrategy {
        public:
        // =========================================================================
        /// @name Constructor and destructor.
        // ==========================================================================
        /// @{

        /// Default constructor.
        SearchStrategy< GUM_SCALAR >();

        /// Copy constructor.
        SearchStrategy< GUM_SCALAR >(const SearchStrategy< GUM_SCALAR >& from);

        /// Destructor.
        virtual ~SearchStrategy< GUM_SCALAR >();

        /// Copy operator.
        SearchStrategy< GUM_SCALAR >&
           operator=(const SearchStrategy< GUM_SCALAR >& from);

        /// @}
        // =========================================================================
        /// @name Search methods.
        // ==========================================================================
        /// @{

        void setTree(DFSTree< GUM_SCALAR >* tree);

        virtual bool accept_root(const Pattern* r) = 0;

        virtual bool accept_growth(const Pattern*                  parent,
                                   const Pattern*                  child,
                                   const EdgeGrowth< GUM_SCALAR >& growth) = 0;

        virtual bool operator()(LabelData* i, LabelData* j) = 0;
        virtual bool operator()(Pattern* i, Pattern* j) = 0;
        /// @}

        protected:
        DFSTree< GUM_SCALAR >* _tree;
        double                 _computeCost(const Pattern& p);
      };

      /**
       * @class FrequenceSearch
       * @headerfile DFSTree.h <agrum/PRM/gspan/DFSTree.h>
       *
       * This is class is an implementation of a simple serach strategy for the
       *gspan
       * algorithm: it accept a growth if its frequency is above a user defined
       *value.
       */
      template < typename GUM_SCALAR >
      class FrequenceSearch : public SearchStrategy< GUM_SCALAR > {
        public:
        // =========================================================================
        /// @name Constructor and destructor.
        // ==========================================================================
        /// @{

        /// Default constructor.
        explicit FrequenceSearch(Size freq);

        /// Copy constructor.
        FrequenceSearch(const FrequenceSearch& from);

        /// Destructor.
        virtual ~FrequenceSearch();

        /// Copy operator.
        FrequenceSearch& operator=(const FrequenceSearch& from);

        /// @}
        // =========================================================================
        /// @name Search methods.
        // ==========================================================================
        /// @{

        virtual bool accept_root(const Pattern* r);

        virtual bool accept_growth(const Pattern*                  parent,
                                   const Pattern*                  child,
                                   const EdgeGrowth< GUM_SCALAR >& growth);

        virtual bool operator()(LabelData* i, LabelData* j);
        virtual bool operator()(Pattern* i, Pattern* j);
        /// @}

        private:
        Size __freq;
      };

      /**
       * @class StrictSearch
       * @headerfile DFSTree.h <agrum/PRM/gspan/DFSTree.h>
       *
       * This is class is an implementation of a strict strategy for the GSpan
       * algorithm. This will force early cuts in the DFSTree and should help
       * not spending much time searching for new patterns.
       *
       * A new growth is accepted if it is at least better than its predecessor.
       */
      template < typename GUM_SCALAR >
      class StrictSearch : public SearchStrategy< GUM_SCALAR > {
        public:
        // =========================================================================
        /// @name Constructor and destructor.
        // ==========================================================================
        /// @{

        /// Default constructor.
        explicit StrictSearch(Size freq = 2);

        /// Copy constructor.
        StrictSearch(const StrictSearch& from);

        /// Destructor.
        virtual ~StrictSearch();

        /// Copy operator.
        StrictSearch& operator=(const StrictSearch& from);

        /// @}
        // =========================================================================
        /// @name Search methods.
        // ==========================================================================
        /// @{

        virtual bool accept_root(const Pattern* r);

        virtual bool accept_growth(const Pattern*                  parent,
                                   const Pattern*                  child,
                                   const EdgeGrowth< GUM_SCALAR >& growth);

        virtual bool operator()(LabelData* i, LabelData* j);
        virtual bool operator()(Pattern* i, Pattern* j);
        /// @}

        private:
        Size   __freq;
        double __inner_cost(const Pattern* p);
        double __outer_cost(const Pattern* p);
        void   __compute_costs(const Pattern* p);
        HashTable< const Pattern*, std::pair< double, double > > __map;
        /// Private structure to represent data about a pattern.
        struct PData {
          /// A yet to be triangulated undigraph
          UndiGraph graph;
          /// The pattern's variables modalities
          NodeProperty< Size > mod;
          /// A bijection to easily keep track  between graph and attributes,
          /// its of
          /// the
          /// form instance_name DOT attr_name
          Bijection< NodeId, std::string > node2attr;
          /// Bijection between graph's nodes and their corresponding
          /// DiscreteVariable, for
          /// inference purpose
          Bijection< NodeId, const DiscreteVariable* > vars;
          /// Returns the set of inner nodes
          NodeSet inners;
          /// Returns the set of outputs nodes given all the matches of pattern
          NodeSet outputs;
        };
        std::string __dot;
        std::string __str(const PRMInstance< GUM_SCALAR >*  i,
                          const PRMAttribute< GUM_SCALAR >* a) const;
        std::string __str(const PRMInstance< GUM_SCALAR >*  i,
                          const PRMAttribute< GUM_SCALAR >& a) const;
        std::string __str(const PRMInstance< GUM_SCALAR >*  i,
                          const PRMSlotChain< GUM_SCALAR >& a) const;
        void        __buildPatternGraph(
                  typename StrictSearch< GUM_SCALAR >::PData&   data,
                  Set< Potential< GUM_SCALAR >* >&              pool,
                  const Sequence< PRMInstance< GUM_SCALAR >* >& match);
        std::pair< Size, Size >
           __elimination_cost(typename StrictSearch< GUM_SCALAR >::PData& data,
                              Set< Potential< GUM_SCALAR >* >&            pool);
      };

      /**
       * @class TreeWidthSearch
       * @headerfile DFSTree.h <agrum/PRM/gspan/DFSTree.h>
       *
       * A growth is accepted if and only if the new growth has a tree width
       *less
       *large
       * or equal than its father.
       */
      template < typename GUM_SCALAR >
      class TreeWidthSearch : public SearchStrategy< GUM_SCALAR > {
        public:
        // =========================================================================
        /// @name Constructor and destructor.
        // ==========================================================================
        /// @{

        /// Default constructor.
        TreeWidthSearch();

        /// Copy constructor.
        TreeWidthSearch(const TreeWidthSearch& from);

        /// Destructor.
        virtual ~TreeWidthSearch();

        /// Copy operator.
        TreeWidthSearch& operator=(const TreeWidthSearch& from);

        /// @}
        // =========================================================================
        /// @name Search methods.
        // ==========================================================================
        /// @{

        double cost(const Pattern& p);

        virtual bool accept_root(const Pattern* r);

        virtual bool accept_growth(const Pattern*                  parent,
                                   const Pattern*                  child,
                                   const EdgeGrowth< GUM_SCALAR >& growth);

        virtual bool operator()(LabelData* i, LabelData* j);
        virtual bool operator()(Pattern* i, Pattern* j);
        /// @}

        private:
        HashTable< const Pattern*, double > __map;
      };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      extern template class SearchStrategy< double >;
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      extern template class FrequenceSearch< double >;
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      extern template class StrictSearch< double >;
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      extern template class TreeWidthSearch< double >;
#      endif
#    endif
#  endif
#endif


    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/gspan/searchStrategy_tpl.h>

#endif /* GUM_DFS_TREE_H */
