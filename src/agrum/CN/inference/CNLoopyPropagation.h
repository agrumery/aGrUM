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
 * @brief Class implementing loopy-propagation with binary networks - L2U
 * algorithm.
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */

#ifndef __CN_LOOPY_PROPAGATION__H__
#define __CN_LOOPY_PROPAGATION__H__

#include <cstdlib>
#include <ctime>
#include <limits>

#include <agrum/CN/inference/inferenceEngine.h>

#define INF_ std::numeric_limits< GUM_SCALAR >::infinity()

namespace gum {
  namespace credal {

    /**
     * @class CNLoopyPropagation CNLoopyPropagation.h
     * <agrum/CN/CNLoopyPropagation.h>
     * @ingroup cn_group
     * @brief Class implementing loopy-propagation with binary networks - L2U
     * algorithm.
     * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
     * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6)
     */
    template < typename GUM_SCALAR >
    class CNLoopyPropagation: public InferenceEngine< GUM_SCALAR > {
      public:
      using msg   = std::vector< Potential< GUM_SCALAR >* >;
      using cArcP = const Arc*;

      /**
       * Inference type to be used by the algorithm.
       */
      enum class InferenceType : char {
        nodeToNeighbours, /**< Uses a node-set so we don't iterate on nodes that
                             can't send a new message. Should be the fastest
                             inference type. A step is going through the
                             node-set. */

        ordered, /**< Chooses an arc ordering and sends messages accordingly at
                    all
                    steps. Avoid it since it can give slightly worse results
                    than
                    other inference types. A step is going through all arcs. */

        randomOrder /**< Chooses a random arc ordering and sends messages
                       accordingly. A new order is set at each step. A step is
                       going
                       through all arcs. */
      };

      /// @name Public algorithm methods
      /// @{

      /** Starts the inference. */
      void makeInference();

      /// @}

      /// @name Getters and setters
      /// @{

      /**
       * %Set the inference type.
       * @param inft The choosen \c InferenceType.
       */
      void inferenceType(InferenceType inft);

      /**
       * Get the inference type.
       * @return The inference type.
       */
      InferenceType inferenceType();

      /// @}

      /// @name Post-inference methods
      /// @{

      /**
       * Erase all inference related data to perform another one. You need to
       * insert
       * evidence again if needed but modalities are kept. You can insert new
       * ones by
       * using the appropriate method which will delete the old ones.
       */
      void eraseAllEvidence();

      /**
       * @deprecated
       * Use saveMarginals() from InferenceEngine instead.
       * This one is easier to read but harder for scripts to parse.
       * @param path The path to the file to save marginals.
       */
      void saveInference(const std::string& path);

      /// @}

      /// @name Constructors / Destructors
      /// @{
      /**
       * Constructor.
       * @param cnet The CredalNet to be used with this algorithm.
       */
      explicit CNLoopyPropagation(const CredalNet< GUM_SCALAR >& cnet);
      /** Destructor. */
      virtual ~CNLoopyPropagation();
      /// @}

      protected:
      /// @name Protected initialization methods
      /// @{

      /** Topological forward propagation to initialize old marginals &
       * messages. */
      void initialize_();

      /// @}

      /// @name Protected algorithm methods
      /// @{
      /** Starts the inference with this inference type. */
      void makeInferenceNodeToNeighbours_();
      /** Starts the inference with this inference type. */
      void makeInferenceByOrderedArcs_();
      /** Starts the inference with this inference type. */
      void makeInferenceByRandomOrder_();

      /** Compute marginals from up-to-date messages. */
      void updateMarginals_();

      /**
       * Sends a message to one's parent, i.e. X is sending a message to a
       * demanding_parent.
       * @param X The constant node id of the node sending the message.
       * @param demanding_parent The constant node id of the node receiving the
       * message.
       */
      void msgL_(const NodeId X,
                 const NodeId demanding_parent);   // allways sent from X to demanding_X

      /**
       * Used by msgL_. Compute the final message for the given parent's message
       * and
       * likelihood (children's messages), numerators & denominators.
       * @param msg_l_min The reference to the current lower value of the
       * message to
       * be sent.
       * @param msg_l_max The reference to the current upper value of the
       * message to
       * be sent.
       * @param lx The lower and upper likelihood.
       * @param num_min The reference to the previously computed lower
       * numerator.
       * @param num_max The reference to the previously computed upper
       * numerator.
       * @param den_min The reference to the previously computed lower
       * denominator.
       * @param den_max The reference to the previously computed upper
       * denominator.
       */
      void compute_ext_(GUM_SCALAR&                msg_l_min,
                        GUM_SCALAR&                msg_l_max,
                        std::vector< GUM_SCALAR >& lx,
                        GUM_SCALAR&                num_min,
                        GUM_SCALAR&                num_max,
                        GUM_SCALAR&                den_min,
                        GUM_SCALAR&                den_max);

      /**
       * Used by msgL_. Compute the numerators & denominators for the given
       * parent's
       * message and likelihood (children's messages). Marginalisation.
       * @param combi_msg_p The parent's choosen message.
       * @param id The constant id of the node sending the message.
       * @param msg_l_min The reference to the current lower value of the
       * message to
       * be sent.
       * @param msg_l_max The reference to the current upper value of the
       * message to
       * be sent.
       * @param lx The lower and upper likelihood.
       * @param pos The position of the parent node to receive the message in
       * the CPT
       * of the one sending the message ( first parent, second ... ).
       */
      void compute_ext_(std::vector< std::vector< GUM_SCALAR > >& combi_msg_p,
                        const NodeId&                             id,
                        GUM_SCALAR&                               msg_l_min,
                        GUM_SCALAR&                               msg_l_max,
                        std::vector< GUM_SCALAR >&                lx,
                        const Idx&                                pos);

      /**
       * Used by msgL_. Enumerate parent's messages.
       * @param msgs_p All the messages from the parents which will be
       * enumerated.
       * @param id The constant id of the node sending the message.
       * @param msg_l_min The reference to the current lower value of the
       * message to
       * be sent.
       * @param msg_l_max The reference to the current upper value of the
       * message to
       * be sent.
       * @param lx The lower and upper likelihood.
       * @param pos The position of the parent node to receive the message in
       * the CPT
       * of the one sending the message ( first parent, second ... ).
       */
      void enum_combi_(std::vector< std::vector< std::vector< GUM_SCALAR > > >& msgs_p,
                       const NodeId&                                            id,
                       GUM_SCALAR&                                              msg_l_min,
                       GUM_SCALAR&                                              msg_l_max,
                       std::vector< GUM_SCALAR >&                               lx,
                       const Idx&                                               pos);

      /**
       * Sends a message to one's child, i.e. X is sending a message to a
       * demanding_child.
       * @param X The constant node id of the node sending the message.
       * @param demanding_child The constant node id of the node receiving the
       * message.
       */
      void msgP_(const NodeId X, const NodeId demanding_child);

      /**
       * Used by msgP_. Enumerate parent's messages.
       * @param msgs_p All the messages from the parents which will be
       * enumerated.
       * @param id The constant id of the node sending the message.
       * @param msg_p_min The reference to the current lower value of the
       * message to
       * be sent.
       * @param msg_p_max The reference to the current upper value of the
       * message to
       * be sent.
       */
      void enum_combi_(std::vector< std::vector< std::vector< GUM_SCALAR > > >& msgs_p,
                       const NodeId&                                            id,
                       GUM_SCALAR&                                              msg_p_min,
                       GUM_SCALAR&                                              msg_p_max);

      /**
       * Used by msgP_. Marginalisation.
       * @param combi_msg_p The parent's choosen message.
       * @param id The constant id of the node sending the message.
       * @param msg_p_min The reference to the current lower value of the
       * message to
       * be sent.
       * @param msg_p_max The reference to the current upper value of the
       * message to
       * be sent.
       */
      void compute_ext_(std::vector< std::vector< GUM_SCALAR > >& combi_msg_p,
                        const NodeId&                             id,
                        GUM_SCALAR&                               msg_p_min,
                        GUM_SCALAR&                               msg_p_max);

      /** Get the last messages from one's parents and children. */
      void refreshLMsPIs_(bool refreshIndic = false);

      /**
       * Compute epsilon.
       * @return Epsilon.
       */
      GUM_SCALAR calculateEpsilon_();

      /// @}

      /// @name Post-inference protected methods
      /// @{

      /** Since the network is binary, expectations can be computed from the
       * final
       * marginals which give us the credal set vertices. */
      void computeExpectations_();

      /** @brief Only update indicatrices variables at the end of computations (
       * calls msgP_ ). */
      void updateIndicatrices_();

      /// @}

      /** Used to keep track of which node needs to update it's information
       * coming
       * from it's parents. */
      NodeProperty< bool > update_p_;
      /** Used to keep track of which node needs to update it's information
       * coming
       * from it's children. */
      NodeProperty< bool > update_l_;

      /** The current node-set to iterate through at this current step. */
      NodeSet active_nodes_set;
      /** The next node-set, i.e. the nodes that will send messages at the next
       * step.
       */
      NodeSet next_active_nodes_set;

      /** Used to keep track of one's messages sent to it's parents. */
      NodeProperty< NodeSet* > msg_l_sent_;

      /** "Lower" information \f$ \Lambda \f$ coming from one's children. */
      ArcProperty< GUM_SCALAR > ArcsL_min_;
      /** "Lower" information \f$ \pi \f$ coming from one's parent. */
      ArcProperty< GUM_SCALAR > ArcsP_min_;
      /** "Lower" node information \f$ \Lambda \f$ obtained by combinaison of
       * children messages. */
      NodeProperty< GUM_SCALAR > NodesL_min_;
      /** "Lower" node information \f$ \pi \f$ obtained by combinaison of
       * parent's
       * messages. */
      NodeProperty< GUM_SCALAR > NodesP_min_;

      /** "Upper" information \f$ \Lambda \f$ coming from one's children. */
      ArcProperty< GUM_SCALAR > ArcsL_max_;
      /** "Upper" information \f$ \pi \f$ coming from one's parent. */
      ArcProperty< GUM_SCALAR > ArcsP_max_;
      /** "Upper" node information \f$ \Lambda \f$ obtained by combinaison of
       * children messages. */
      NodeProperty< GUM_SCALAR > NodesL_max_;
      /** "Upper" node information \f$ \pi \f$ obtained by combinaison of
       * parent's
       * messages. */
      NodeProperty< GUM_SCALAR > NodesP_max_;

      /** \c TRUE if inference has already been performed, \c FALSE otherwise.
       */
      bool InferenceUpToDate_;

      private:
      /** To easily access InferenceEngine< GUM_SCALAR > methods. */
      using _infE_ = InferenceEngine< GUM_SCALAR >;

      /** The choosen inference type. nodeToNeighbours by Default. */
      InferenceType _inferenceType_;

      /** A pointer to the CredalNet to be used. */
      const CredalNet< GUM_SCALAR >* _cn_;

      /** A pointer to it's IBayesNet used as a DAG. */
      const IBayesNet< GUM_SCALAR >* _bnet_;

      // cnfunc getCN = &infE::getCN;
      public:
      virtual void insertEvidenceFile(const std::string& path) {
        InferenceEngine< GUM_SCALAR >::insertEvidenceFile(path);
      };
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class CNLoopyPropagation< double >;
#endif
  }   // namespace credal
}   // namespace gum

#include <agrum/CN/inference/CNLoopyPropagation_tpl.h>

#endif
