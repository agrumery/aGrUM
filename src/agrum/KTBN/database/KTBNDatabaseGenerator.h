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


/** @file
 * @brief A database generator for k-order dynamic Bayesian networks.
 *
 * gum::learning::KTBNDatabaseGenerator is the k-DBN counterpart of
 * gum::learning::BNDatabaseGenerator. A plain Bayesian network has no notion of
 * time, so its generator draws i.i.d. realizations of a fixed set of variables
 * and writes them as a flat table. A k-DBN, on the contrary, describes a
 * stochastic @e process, so its database is a set of @b trajectories.
 *
 * It follows the @b same @b principle as BNDatabaseGenerator — forward
 * (ancestral) sampling: nodes drawn in topological order, each by an inverse-CDF
 * walk over its CPT, reusing a single gum::Instantiation and accumulating the
 * log2-likelihood — but it does @b not unroll the k-DBN. It keeps only the
 * @f$k@f$-slice @e template (via gum::KTBN::toBN(), which is small and
 * independent of the horizon) and slides it forward in time, reading each
 * parent's value from the trajectory under construction with the proper lag.
 *
 * To keep the memory footprint independent of the number of trajectories, the
 * generator @b never stores the whole database: it builds one trajectory at a
 * time in a small @f$O(T \times V)@f$ buffer and writes it straight to the CSV
 * file. drawSamples() therefore only returns the per-trajectory log2-likelihoods.
 *
 * The database is exported as @b one @b CSV @b file @b per @b trajectory: one
 * column per @e base variable and @f$T@f$ data rows, an atemporal variable
 * keeping the same value across all @f$T@f$ rows. Values may be exported as
 * modality indices or as labels, with a configurable rendering for discretized
 * variables.
 *
 * @code
 *  gum::KTBN< double >                            kdbn = ...;     // a k-DBN
 *  gum::learning::KTBNDatabaseGenerator< double > gen(kdbn);
 *  // 1000 trajectories of length 10 -> out_dir/traj1.csv ... out_dir/traj1000.csv
 *  std::vector< double > ll = gen.drawSamples(1000, 10, "out_dir", "traj");
 * @endcode
 *
 * @author Seth AGUILA & Anis KHACEF
 */

#ifndef GUM_KTBN_DATABASE_GENERATOR_H
#define GUM_KTBN_DATABASE_GENERATOR_H

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include <agrum/base/core/progressNotification.h>
#include <agrum/base/multidim/instantiation.h>
#include <agrum/KTBN/KTBN.h>

namespace gum {

  namespace learning {
    /**
     * @class KTBNDatabaseGenerator
     * @headerfile KTBNDatabaseGenerator.h <agrum/KTBN/database/KTBNDatabaseGenerator.h>
     * @brief Generates a database of trajectories from a k-DBN (one CSV per trajectory).
     * @ingroup bn_group
     *
     * @see gum::learning::BNDatabaseGenerator, the (static) Bayesian-network
     * counterpart. Same role and sampling principle, but it streams @e
     * trajectories from the k-DBN template without unrolling and without keeping
     * the whole database in memory.
     */
    template < GUM_Numeric GUM_SCALAR >
    class KTBNDatabaseGenerator: public ProgressNotifier {
      public:
      /// rendering of discretized variables when labels are requested
      enum class DiscretizedLabelMode : char { INTERVAL, MEDIAN, RANDOM };

      /// column order used for the exported CSV
      enum class VarOrderMode : char { RANDOM, TOPOLOGICAL, ANTI_TOPOLOGICAL };

      // #######################################################################
      /// @name Constructors / Destructors
      // #######################################################################
      /// @{

      /**
       * @brief Constructor.
       * @param kdbn The k-DBN to sample from (only its @f$k@f$-slice template is
       * copied, via toBN(); the k-DBN itself is not retained). The horizon is not
       * fixed here; it is passed to drawSamples().
       */
      explicit KTBNDatabaseGenerator(const KTBN< GUM_SCALAR >& kdbn);

      /// destructor
      ~KTBNDatabaseGenerator();

      /// @}
      // #######################################################################
      /// @name Accessors / Modifiers
      // #######################################################################
      /// @{

      /**
       * @brief Generates @p nbSamples independent trajectories, writing one CSV
       * file per trajectory into @p dirPath.
       *
       * File names are @p csvBaseName followed by the 1-based trajectory index
       * and @c ".csv" (e.g. @c "traj1.csv", @c "traj2.csv", …).
       * Each file has one column per base variable in the order chosen by
       * @p mode, and @p nbTimeSlices data rows.
       *
       * @param nbSamples    The number of trajectories to generate.
       * @param nbTimeSlices The horizon @f$T@f$ shared by every trajectory. Must be @f$\geq k@f$.
       * @param dirPath      Directory to write the CSV files into.
       * @param csvBaseName  Stem for each file name (index and @c .csv appended).
       * @param mode         The column order of the base variables.
       * @param useLabels    Render values as variable labels (else modality index).
       * @param csvSeparator Column separator (must not contain a newline).
       * @return The log2-likelihood of each generated trajectory (size @p nbSamples).
       * @throw OperationNotAllowed if @p nbTimeSlices is smaller than @f$k@f$.
       */
      std::vector< double > drawSamples(Size             nbSamples,
                                        Size             nbTimeSlices,
                                        std::string_view dirPath,
                                        std::string_view csvBaseName,
                                        VarOrderMode     mode         = VarOrderMode::RANDOM,
                                        bool             useLabels    = true,
                                        std::string      csvSeparator = ",");

      /**
       * @brief Like drawSamples(), but every trajectory may have its own horizon.
       *
       * @param nbTimeSlices One horizon per trajectory; its size is the number of
       *        trajectories. Every entry must be @f$\geq k@f$.
       * @param dirPath      Directory to write the CSV files into.
       * @param csvBaseName  Stem for each file name (index and @c .csv appended).
       * @param mode         The column order of the base variables.
       * @param useLabels    Render values as variable labels (else modality index).
       * @param csvSeparator Column separator (must not contain a newline).
       * @return The log2-likelihood of each generated trajectory.
       * @throw OperationNotAllowed if some entry of @p nbTimeSlices is smaller than @f$k@f$.
       */
      std::vector< double > drawSamples(const std::vector< Size >& nbTimeSlices,
                                        std::string_view           dirPath,
                                        std::string_view           csvBaseName,
                                        VarOrderMode               mode         = VarOrderMode::RANDOM,
                                        bool                       useLabels    = true,
                                        std::string                csvSeparator = ",");

      /// set discretized-label rendering to a uniform random draw in the interval
      /// (this is the default; each labelled export then differs)
      void setDiscretizedLabelModeRandom();
      /// set discretized-label rendering to the (deterministic) interval median
      void setDiscretizedLabelModeMedian();
      /// set discretized-label rendering to the interval label "[min,max["
      void setDiscretizedLabelModeInterval();


      /// returns the number of base variable columns
      Size nbVars() const;

      /// @}

      private:
      /// a parent of a template node, precompiled for fast sampling
      struct ParentRef {
        const DiscreteVariable* var;           ///< the parent variable (in _template_)
        Idx                     col;           ///< its base column index in _baseCols_
        int                     lag;           ///< time steps back: parentTime = nodeTime - lag
        bool                    isAtemporal;   ///< whether the parent is atemporal
      };

      /// a template node, precompiled for fast sampling
      struct NodeRef {
        const DiscreteVariable*     var;       ///< the node variable (in _template_)
        const Tensor< GUM_SCALAR >* cpt;       ///< its CPT (in _template_)
        int                         slice;     ///< its template slice (ATEMPORAL if static)
        Idx                         col;       ///< its base column index in _baseCols_
        std::vector< ParentRef >    parents;   ///< its parents (drives sampling and topology)
      };

      /// the @f$k@f$-slice template (a small copy, independent of the horizon)
      BayesNet< GUM_SCALAR > _template_;

      /// the order @f$k@f$ of the k-DBN
      Size _k_;

      /// number of base variable columns
      Size _nbVars_;

      /// col index -> base name (canonical column numbering)
      std::vector< std::string > _baseCols_;

      /// one representative variable per base column (same order as _baseCols_),
      /// pointing into _template_ so it outlives the source k-DBN. Label rendering only.
      std::vector< const DiscreteVariable* > _vars_;

      /// all template nodes in topological order (drives Phase 1, the bootstrap)
      std::vector< NodeRef > _nodes_;

      /// indices, in _nodes_, of the slice-(k-1) nodes (the transition kernel,
      /// drives Phase 2); already in topological order
      std::vector< Idx > _kernel_;

      /// a shared instantiation over all template variables, so we don't have to
      /// rebuild it for every draw
      Instantiation _inst_;

      /// rendering of discretized variables when labels are requested
      DiscretizedLabelMode _discretizedLabelMode_ = DiscretizedLabelMode::RANDOM;

      // ----- constructor helper -----

      /// decodes a template node name into (base, slice): "B[t]" with B a known
      /// temporal process -> (B, t); anything else (bare or bracket-named
      /// atemporal node) -> (name, ATEMPORAL).
      static std::pair< std::string, int >
          _decode_(const std::string& name, const std::unordered_set< std::string >& temporalSet);

      /// one-shot initialisation called by the constructor: fills the column index
      /// (_baseCols_, _nbVars_), the topological node/parent cache (_nodes_,
      /// _kernel_), the per-column representatives (_vars_), and the shared
      /// instantiation (_inst_). All cached pointers refer to _template_.
      void _build_(const KTBN< GUM_SCALAR >& kdbn);

      // ----- sampling -----

      /// inverse-CDF draw of @p var given the parents already set in _inst_;
      /// accumulates log2(P(drawn value)) into @p log2likelihood
      Idx _drawVar_(const DiscreteVariable&     var,
                    const Tensor< GUM_SCALAR >& cpt,
                    double&                     log2likelihood);

      // ----- export -----

      /// renders the label of modality @p idx of base column @p col
      /// (taking the discretized-label mode into account)
      std::string _label_(Idx col, Idx idx) const;

      /// writes one trajectory CSV (header + T rows) to @p csvFileURL. @p traj is
      /// the flat row-major buffer (T x _nbVars_) in canonical column order;
      /// @p colOrder gives the output column order.
      void _writeTrajectory_(std::string_view          csvFileURL,
                             const std::vector< Idx >& traj,
                             Size                      nbTimeSlices,
                             bool                      useLabels,
                             const std::string&        csvSeparator,
                             const std::vector< Idx >& colOrder) const;

      // ----- column order -----
      // each fills @p colOrder: output position -> canonical column index

      /// builds a uniformly random column order
      void setVarOrderRandomized(std::vector< Idx >& colOrder) const;

      /// builds a topological column order (transition-kernel projection)
      void setVarOrderTopological(std::vector< Idx >& colOrder) const;

      /// builds the reverse of setVarOrderTopological()
      void setVarOrderAntiTopological(std::vector< Idx >& colOrder) const;

      /// the single worker behind both drawSamples() overloads. Trajectory i's
      /// horizon is read from @p perTraj (when non-null) else from @p fixedLen;
      /// samples each trajectory and writes it straight to its own CSV file.
      std::vector< double > _drawSamples_(Size                       nbSamples,
                                          Size                       fixedLen,
                                          const std::vector< Size >* perTraj,
                                          std::string_view           dirPath,
                                          std::string_view           csvBaseName,
                                          VarOrderMode               mode,
                                          bool                       useLabels,
                                          const std::string&         csvSeparator);

      // forbidden copies / moves
      KTBNDatabaseGenerator(const KTBNDatabaseGenerator&)            = delete;
      KTBNDatabaseGenerator(KTBNDatabaseGenerator&&)                 = delete;
      KTBNDatabaseGenerator& operator=(const KTBNDatabaseGenerator&) = delete;
      KTBNDatabaseGenerator& operator=(KTBNDatabaseGenerator&&)      = delete;
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class KTBNDatabaseGenerator< double >;
#endif

  } /* namespace learning */
} /* namespace gum */

#include <agrum/KTBN/database/KTBNDatabaseGenerator_tpl.h>

#endif /* GUM_KTBN_DATABASE_GENERATOR_H */
