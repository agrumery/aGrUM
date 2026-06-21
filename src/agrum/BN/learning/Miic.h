/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief The Miic algorithm
 *
 * The Miic class implements the MIIC algorithm based on
 * https://doi.org/10.1371/journal.pcbi.1005662.
 * It starts by eliminating edges that correspond to independent variables to
 * build the skeleton of the graph, and then directs the remaining edges to get
 * an essential graph. Latent variables can be detected using bi-directed arcs.
 *
 * Miic allows the option of adding constraints on the skeleton construction
 * such as: a maximum number of parents, mandatory arcs, forbidden arcs or an
 * order between the variables.
 *
 * @author Quentin FALCAND and Pierre-Henri WUILLEMIN(_at_LIP6) and Maria Virginia
 * RUIZ CUEVAS
 */
#ifndef GUM_LEARNING_MIIC_H
#define GUM_LEARNING_MIIC_H

#include <agrum/base/core/heap.h>
#include <agrum/BN/learning/ConstraintBasedLearning.h>
#include <agrum/BN/learning/correctedMutualInformation.h>

namespace gum {

  namespace learning {

    using CondThreePoints      = std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >;
    using CondRanking          = std::pair< CondThreePoints*, double >;
    using Ranking              = std::pair< ThreePoints*, double >;
    using ProbabilisticRanking = std::tuple< ThreePoints*, double, double, double >;

    class GreaterPairOn2nd {
      public:
      bool operator()(const CondRanking& e1, const CondRanking& e2) const;
    };

    class GreaterAbsPairOn2nd {
      public:
      bool operator()(const Ranking& e1, const Ranking& e2) const;
    };

    class GreaterTupleOnLast {
      public:
      bool operator()(const ProbabilisticRanking& e1, const ProbabilisticRanking& e2) const;
    };

    /**
     * @class Miic
     * @brief The MIIC learning algorithm
     *
     * Implements learnSkeleton() and learnMixedStructure() from the
     * ConstraintBasedLearning interface using the MIIC algorithm:
     * https://doi.org/10.1371/journal.pcbi.1005662
     *
     * @ingroup learning_group
     */
    class Miic: public ConstraintBasedLearning {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      Miic();
      explicit Miic(int maxLog);
      Miic(const Miic& from);
      Miic(Miic&& from);
      ~Miic() override;

      /// @}

      Miic& operator=(const Miic& from);
      Miic& operator=(Miic&& from);

      // ##########################################################################
      /// @name Scorer injection
      // ##########################################################################
      /// @{

      void setMutualInformation(CorrectedMutualInformation& mi);

      /// @}

      // ##########################################################################
      /// @name ConstraintBasedLearning interface
      // ##########################################################################
      /// @{

      MixedGraph learnSkeleton(MixedGraph graph) override;

      MixedGraph learnMixedStructure(MixedGraph graph) override;

      /// @}

      protected:
      // ##########################################################################
      /// @name MIIC skeleton learning phases
      // ##########################################################################
      /// @{

      void initiation_(CorrectedMutualInformation& mutualInformation,
                       MixedGraph&                 graph,
                       HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet,
                       Heap< CondRanking, GreaterPairOn2nd >&                           rank);

      void iteration_(CorrectedMutualInformation& mutualInformation,
                      MixedGraph&                 graph,
                      HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet,
                      Heap< CondRanking, GreaterPairOn2nd >&                           rank);

      /// @}

      // ##########################################################################
      /// @name MIIC orientation phase
      // ##########################################################################
      /// @{

      void orientationMiic_(
          CorrectedMutualInformation&                                            mutualInformation,
          MixedGraph&                                                            graph,
          const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet);

      void findBestContributor_(NodeId                                 x,
                                NodeId                                 y,
                                const std::vector< NodeId >&           ui,
                                const MixedGraph&                      graph,
                                CorrectedMutualInformation&            mutualInformation,
                                Heap< CondRanking, GreaterPairOn2nd >& rank);

      std::vector< ProbabilisticRanking > unshieldedTriplesMiic_(
          const MixedGraph&                                                      graph,
          CorrectedMutualInformation&                                            mutualInformation,
          const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet,
          HashTable< std::pair< NodeId, NodeId >, char >&                        marks);

      std::vector< ProbabilisticRanking >
          updateProbaTriples_(const MixedGraph&                   graph,
                              std::vector< ProbabilisticRanking > probaTriples);

      /// @}

      private:
      CorrectedMutualInformation* mi_{nullptr};
      ArcProperty< double >       _arcProbas_;

      void _orientingVstructureMiic_(MixedGraph&                                     graph,
                                     HashTable< std::pair< NodeId, NodeId >, char >& marks,
                                     NodeId                                          x,
                                     NodeId                                          y,
                                     NodeId                                          z,
                                     double                                          p1,
                                     double                                          p2);

      void _propagatingOrientationMiic_(MixedGraph&                                     graph,
                                        HashTable< std::pair< NodeId, NodeId >, char >& marks,
                                        NodeId                                          x,
                                        NodeId                                          y,
                                        NodeId                                          z,
                                        double                                          p1,
                                        double                                          p2);

      bool _isNotLatentCouple_(NodeId x, NodeId y);
    };

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_MIIC_H */
