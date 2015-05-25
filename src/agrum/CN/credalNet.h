/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
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

#ifndef __CREDAL_NET__H__
#define __CREDAL_NET__H__

/**
 * @file
 * @brief Class representing Credal Networks
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 */

#include <vector>
#include <iostream>
#include <unistd.h>
//#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <cstdlib>

#include <utility> /// c++11 stuff, like declval ( decltype from prototype without a default constructor )

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/io/BIF/BIFWriter.h>
#include <agrum/core/exceptions.h>

#include <agrum/core/math/rational.h> // custom decimal to rational
#include <agrum/core/math/pow.h> // custom pow functions with integers, faster implementation

#include <agrum/CN/LrsWrapper.h>

#include <agrum/core/OMPThreads.h>

namespace gum {
  namespace credal {

    /**
     * @class CredalNet CredalNet.h <agrum/CN/CredalNet.h>
     * @brief Class template representing a Credal Network.
     * @ingroup cn_group
     * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
     * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
     */
    template <typename GUM_SCALAR> class CredalNet {
      public:
      /** @brief NodeType to speed-up computations in some algorithms */
      enum class NodeType : char { Precise, Credal, Vacuous, Indic };

      /// @name Constructors / Destructors
      /// @{

      /**
       * Constructor used to create a CredalNet step by step, i.e. node by node, arc
       * by arc, manually filling potentials.
       */
      CredalNet();

      /**
       * Constructor for interval defined credal network which takes 2 BayesNet file
       *path.
       * One can also provide a single BayesNet in order to perturb it's probability
       *distributions into credal sets according to another BayesNet containing the
       *number of cases, for each node, of each parent instantiation met during
       *learning, i.e. \f$ p(X = 0 \mid pa(X) = j) = N_{pa(X) = j} \f$.
       *
       * @param src_min_num The path to a BayesNet which contains lower
       *probabilities.
       * @param src_max_den The ( optional ) path to a BayesNet which contains upper
       *probabilities.
       */
      CredalNet(const std::string &src_min_num, const std::string &src_max_den = "");

      /**
       * Constructor for interval defined credal network which takes 2 BayesNet.
       * One can also provide a single BayesNet in order to perturb it's probability
       *distributions into credal sets according to another BayesNet containing the
       *number of cases, for each node, of each parent instantiation met during
       *learning, i.e. \f$ p(X = 0 \mid pa(X) = j) = N_{pa(X) = j} \f$.
       *
       * @param src_min_num The BayesNet which contains lower probabilities.
       * @param src_max_den The ( optional ) BayesNet which contains upper
       *probabilities.
       */
      CredalNet(const BayesNet<GUM_SCALAR> &src_min_num,
                const BayesNet<GUM_SCALAR> &src_max_den = BayesNet<GUM_SCALAR>());

      /**
       * Destructor.
       */
      ~CredalNet();

      /// @}

      /// @name Credal network creation
      /// @{

      /**
       * @brief Adds a discrete node into the network
       * @param name The name of the discrete variable to be added
       * @param card The cardinality of the variable
       * @return The \c NodeId of the variable in the network
       */
      NodeId addNode(const std::string &name, const unsigned long &card);

      /**
       * @brief Adds an arc between two nodes.
       * @param tail The \c NodeId of the tail node
       * @param head The \c NodeId of the head node
       */
      void addArc(const NodeId &tail, const NodeId &head);

      /**
       * @brief %Set the vertices of the credal sets ( all of the conditionals ) of a
       *given node
       * @param id The \c NodeId of the node
       * @param cpt The vertices of every credal set ( for each instantiation of the
       *parents )
       *
       * @warning : Does not change the \c BayesNet (s) associated to this credal net
       *!
       *
       * First dimension is instantiation position ( from 0 to K - 1 ).
       * Second is the credal set vertice index
       * Third is the vertex
       */
      void setCPTs(const NodeId &id,
                   const std::vector<std::vector<std::vector<GUM_SCALAR>>> &cpt);

      /**
       * @brief %Set the vertices of one credal set of a given node ( any
       *instantiation index )
       * @param id The \c NodeId of the node
       * @param entry The index of the instantiation ( from 0 to K - 1 ) excluding
       *the given node ( only the parents are used to compute the index of the credal
       *set )
       * @param cpt The vertices of every credal set ( for each instantiation of the
       *parents )
       *
       * Use this with either \c LRSWrapper or \c LpInterface to get the vertices of
       *a credal set represented by linear constraints.
       * @warning : Does not change the \c BayesNet (s) associated to this credal net
       *!
       */
      void setCPT(const NodeId &id, unsigned long int &entry,
                  const std::vector<std::vector<GUM_SCALAR>> &cpt);

      /**
       * @brief %Set the vertices of one credal set of a given node ( any
       *instantiation )
       * @param id The \c NodeId of the node
       * @param ins The \c Instantiation ( only the parents matter to find the credal
       *set index )
       * @param cpt The vertices of every credal set ( for each instantiation of the
       *parents )
       *
       * Use this with either \c LRSWrapper or \c LpInterface to get the vertices of
       *a credal set represented by linear constraints.
       * @warning : Does not change the \c BayesNet (s) associated to this credal net
       *!
       *
       * @note we forget the master ref of \c ins to check variable order in the
       *instantiation ( to get index ), therefor we pass it by value
       */
      void setCPT(const NodeId &id, Instantiation ins,
                  const std::vector<std::vector<GUM_SCALAR>> &cpt);

      /**
       * @brief %Set the interval constraints of the credal sets of a given node (
       *all instantiations )
       * @param id The \c NodeId of the node
       * @param lower The lower value for each probability in correct order
       * @param upper The upper value for each probability in correct order
       *
       * You need to call intervalToCredal when done filling all constraints.
       *
       * @warning : DOES change the \c BayesNet (s) associated to this credal net !
       * @note we forget the master ref of \c ins to check variable order in the
       *instantiation ( to get index ), therefor we pass it by value
       */
      void fillConstraints(const NodeId &id, const std::vector<GUM_SCALAR> &lower,
                           const std::vector<GUM_SCALAR> &upper);

      /**
       * @brief %Set the interval constraints of a credal set of a given node ( from
       *an instantiation index )
       * @param id The \c NodeId of the node
       * @param entry The index of the instantiation excluding the given node ( only
       *the parents are used to compute the index of the credal set )
       * @param lower The lower value for each probability in correct order
       * @param upper The upper value for each probability in correct order
       *
       * You need to call intervalToCredal when done filling all constraints.
       *
       * @warning : DOES change the \c BayesNet (s) associated to this credal net !
       * @note we forget the master ref of \c ins to check variable order in the
       *instantiation ( to get index ), therefor we pass it by value
       */
      void fillConstraint(const NodeId &id, unsigned long int &entry,
                          const std::vector<GUM_SCALAR> &lower,
                          const std::vector<GUM_SCALAR> &upper);

      /**
       * @brief %Set the interval constraints of a credal sets of a given node ( from
       *an instantiation )
       * @param id The \c NodeId of the node
       * @param ins The \c Instantiation
       * @param lower The lower value for each probability in correct order
       * @param upper The upper value for each probability in correct order
       *
       * You need to call intervalToCredal when done filling all constraints.
       *
       * @warning : DOES change the \c BayesNet (s) associated to this credal net !
       * @note we forget the master ref of \c ins to check variable order in the
       *instantiation ( to get index ), therefor we pass it by value
       */
      void fillConstraint(const NodeId &id, Instantiation ins,
                          const std::vector<GUM_SCALAR> &lower,
                          const std::vector<GUM_SCALAR> &upper);

      /**
       * @brief %Get an \c Instantiation from a node id, usefull to fill the
       * constraints of the network
       * @param id The \c NodeId we want an instantiation from
       * @return The instantiation
       */
      Instantiation instantiation(const NodeId &id);

      /**
       * @brief %Get the cardinality of a node
       * @param id The \c NodeId of the node
       * @return The cardinality of the node
       */
      Size domainSize(const NodeId &id);

      /// @}

      /// @name Public manipulation methods
      /// @{

      /**
       * Perturbates the BayesNet provided as input for this CredalNet by generating
       *intervals instead of point probabilities and then computes each vertex of
       *each credal set.
       *
       * The perturbations are done according to the number of cases met for each
       *node and each of it's parent instantiation, i.e. \f$ \epsilon =
       *\beta^{ln(N_{pa(X) = j} + 1)} \f$ is the imprecision introduced which leads
       *to \f$ \underline{p}(X = i \mid pa(X) = j) = (1 - \epsilon) p(X = i \mid
       *pa(X) = j) \f$ and \f$ \overline{p}(X = i \mid pa(X) = j) = \underline{p}(X =
       *i \mid pa(X) = j) + \epsilon \f$.
       * Use this method when using a single BayesNet storing counts of events with
       *\c oneNet set to \c TRUE or when using two BayesNet, one with lower
       *probabilities and one with upper probabilities, with \c oneNet set to \c
       *FALSE.
       *
       * @param beta The beta used to perturbate the network. \f$ 0 \leq \beta \leq 1
       *\f$.
       * @param oneNet Boolean used as a flag. %Set to \c TRUE if one BayesNet if
       *provided with counts, to \c FALSE if two BayesNet are provided; one with
       *probabilities (the lower net) and one with denominators over the first
       *modalities (the upper net).
       * @param keepZeroes Boolean used as a flag as whether or not - respectively \c
       *TRUE or \c FALSE - we keep zeroes as zeroes. Default is \c FALSE, i.e. zeroes
       *are not kept.
       */
      void bnToCredal(const GUM_SCALAR beta, const bool oneNet,
                      const bool keepZeroes = false);

      /**
       * @deprecated Use intervalToCredal ( lrsWrapper with no input / output files
       *needed ).
       *
       * Computes the vertices of each credal set according to their interval
       *definition (uses lrs).
       *
       * Use this method when using a single BayesNet storing counts of events.
       */
      void intervalToCredalWithFiles();

      /**
       * Computes the vertices of each credal set according to their interval
       *definition (uses lrs).
       *
       * Use this method when using two BayesNet, one with lower probabilities and
       *one with upper probabilities.
       */
      void intervalToCredal();

      /**
       * Normalize counts of a BayesNet storing counts of each events such that no
       *probability is 0.
       *
       * Use this method when using a single BayesNet storing counts of events.
       * Lagrange normalization. This call is irreversible and modify counts stored
       *by \c __src_bn.
       *
       * Doest not performs computations of the parameters but keeps normalized
       *counts of events only. Call \c idmLearning to compute the probabilities (with
       *any parameter value).
       */
      void lagrangeNormalization();

      /**
       * Learns parameters from a BayesNet storing counts of events.
       *
       * Use this method when using a single BayesNet storing counts of events.
       * IDM model if \c s > 0, standard point probability if \c s = 0 (default value
       *if none precised).
       * @param s The IDM parameter.
       * @param keepZeroes Boolean used as a flag as whether or not - respectively \c
       *TRUE or \c FALSE - we keep zeroes as zeroes. Default is \c FALSE, i.e. zeroes
       *are not kept.
       */
      void idmLearning(const unsigned int s = 0, const bool keepZeroes = false);

      /**
       * Approximate binarization. Each bit has a lower and upper probability which
       *is the lowest - resp. highest - over all vertices of the credal set.
       * Enlarge the orignal credal sets and may induce huge imprecision.
       *
       * @warning Enlarge the orignal credal sets and therefor induce huge
       *imprecision by propagation. Not recommanded, use MCSampling or something else
       *instead.
       */
      void approximatedBinarization();

      /// @}

      // other utility member methods
      // PH void saveCNet ( const std::string &cn_path ) const;
      // PH void loadCNet ( const std::string &src_cn_path );

      /**
       * If this CredalNet was built over a perturbed BayesNet, one can save the
       *intervals as two BayesNet.
       *
       * @param min_path The path to save the BayesNet which contains the lower
       *probabilities of each node X, i.e. \f$ \underline{p}(X = i \mid pa(X) = j)
       *\f$.
       * @param max_path The path to save the BayesNet which contains the upper
       *probabilities of each node X, i.e. \f$ \overline{p}(X = i \mid pa(X) = j)
       *\f$.
       */
      void saveBNsMinMax(const std::string &min_path,
                         const std::string &max_path) const;

      // PH void vacants ( int &result ) const;
      // PH void notVacants ( int &result ) const;
      // PH void averageVertices ( double &result ) const;

      /**
       * @return Returns the string representation of this CredalNet, i.e. it's CPTs
       * (which also represent arcs).
       */
      std::string toString() const;
      // PH void toPNG ( const std::string &png_path ) const;

      /**
       * Used with binary networks to speed-up L2U inference.
       *
       * Store the lower and upper probabilities of each node X over the "true"
       *modality, i.e. respectively \f$ \underline{p}(X = 1 \mid pa(X) = j) \f$ and
       *\f$ \overline{p}(X = 1 \mid pa(X) = j) \f$.
       */
      void computeCPTMinMax(); // REDO THIS IN PRIVATE !!!

      /// @name Getters and setters
      /// @{
      /**
       * @return Returns a constant reference to the original BayesNet (used as a
       * DAG, it's CPTs does not matter).
       */
      const BayesNet<GUM_SCALAR> &src_bn() const;

      /**
       * @return Returs a constant reference to the actual BayesNet (used as a DAG,
       * it's CPTs does not matter).
       */
      const BayesNet<GUM_SCALAR> &current_bn() const;

      /**
       * @return Returns a constant reference to the ( up-to-date ) CredalNet CPTs.
       */
      const NodeProperty<std::vector<std::vector<std::vector<GUM_SCALAR>>>> &
      credalNet_currentCpt() const;

      /**
       * @return Returns a constant reference to the ( up-to-date ) CredalNet CPTs.
       */
      const NodeProperty<std::vector<std::vector<std::vector<GUM_SCALAR>>>> &
      credalNet_srcCpt() const;

      /**
       * @param id The constant reference to the choosen NodeId
       * @return Returns the type of the choosen node in the ( up-to-date ) CredalNet
       * __current_bn if any, __src_bn otherwise.
       */
      NodeType currentNodeType(const NodeId &id) const;

      /**
       * @param id The constant reference to the choosen NodeId
       * @return Returns the type of the choosen node in the ( up-to-date ) CredalNet
       * in __src_bn.
       */
      NodeType nodeType(const NodeId &id) const;

      /**
       * @return Returns a constant reference to the lowest perturbation of the
       * BayesNet provided as input for this CredalNet.
       */
      const double &epsilonMin() const;

      /**
       * @return Returns a constant reference to the highest perturbation of the
       * BayesNet provided as input for this CredalNet.
       */
      const double &epsilonMax() const;

      /**
       * @return Returns a constant reference to the average perturbation of the
       * BayesNet provided as input for this CredalNet.
       */
      const double &epsilonMean() const;

      /**
       * @return Returns \c TRUE if this CredalNet is separately and interval
       * specified, \c FALSE otherwise.
       */
      const bool isSeparatelySpecified() const;

      /**
       * @return Returns \c TRUE if this CredalNet has called computeCPTMinMax() to
       * speed-up inference with binary networks and L2U. This needs to be reworked
       * as it is too easy to forget to call it and it can't be called within the
       * inference engine (constness).
       */
      const bool hasComputedCPTMinMax() const;

      /**
       * Used with binary networks to speed-up L2U inference.
       *
       * @return Returns a constant reference to the lower probabilities of each node
       *X over the "true" modality, i.e. \f$ \underline{p}(X = 1 \mid pa(X) = j) \f$.
       */
      const std::vector<std::vector<GUM_SCALAR>> &get_CPT_min() const;

      /**
       * Used with binary networks to speed-up L2U inference.
       *
       * @return Returns a constant reference to the upper probabilities of each node
       *X over the "true" modality, i.e. \f$ \overline{p}(X = 1 \mid pa(X) = j) \f$.
       */
      const std::vector<std::vector<GUM_SCALAR>> &get_CPT_max() const;

      // PH const std::vector< std::vector< NodeId > > & var_bits() const;

      /// @}

      protected:
      private:
      /** @brief 1e6 by default, used by __fracC as precision. */
      double __precisionC; // = 1e6;
      /** @brief 5 by default, used by __fracC as number of decimals. */
      double __deltaC; // = 5;

      /** @brief The lowest perturbation of the BayesNet provided as input for this
       * CredalNet. */
      double __epsilonMin;
      /** @brief The highest perturbation of the BayesNet provided as input for this
       * CredalNet. */
      double __epsilonMax;
      /** @brief The average perturbation of the BayesNet provided as input for this
       * CredalNet. */
      double __epsilonMoy;

      /** @brief Value under which a decimal number is considered to be zero when
       * computing redundant vertices. */
      double __epsRedund; //= 1e-6;

      /** @brief Value under which a decimal number is considered to be zero when
       * using __farey. */
      double __epsF; // = 1e-6;
      /** @brief Highest possible denominator allowed when using __farey. A value too
       * high may lead to lrs being unable to find vertices. */
      double __denMax; // = 1e6; // beware LRS

      /** @brief Precision used by __frac. */
      int __precision; // = 1e6; // beware LRS

      /** @brief \c TRUE if this CredalNet is separately and interval specified, \c
       * FALSE otherwise. */
      bool __separatelySpecified;

      /** @brief Original BayesNet (used as a DAG). Is never modified. */
      BayesNet<GUM_SCALAR> __src_bn;

      /** @brief BayesNet used to store lower probabilities. */
      BayesNet<GUM_SCALAR> __src_bn_min;
      /** @brief BayesNet used to store upper probabilities. */
      BayesNet<GUM_SCALAR> __src_bn_max;

      /** @brief Up-to-date BayesNet (used as a DAG). */
      BayesNet<GUM_SCALAR> *__current_bn; // = nullptr;

      /** @brief This CredalNet original CPTs. */
      NodeProperty<std::vector<std::vector<std::vector<GUM_SCALAR>>>>
          __credalNet_src_cpt;

      /** @brief This CredalNet up-to-date CPTs. */
      NodeProperty<std::vector<std::vector<std::vector<GUM_SCALAR>>>> *
          __credalNet_current_cpt; // =  nullptr;

      /** @deprecated @brief Corresponding bits of each variable. */
      NodeProperty<std::vector<NodeId>> __var_bits;

      /** @brief The NodeType of each node from the ORIGINAL network. */
      NodeProperty<NodeType> __original_nodeType;
      /** @brief The NodeType of each node from the up-to-date network. */
      NodeProperty<NodeType> *__current_nodeType; // = nullptr;

      /** @brief Used by L2U, to know if lower and upper probabilities over the
       * second modality has been stored in order to speed-up the algorithm. */
      bool __hasComputedCPTMinMax;
      /**
       * @brief Used with binary networks to speed-up L2U inference. Store the lower
       * probabilities of each node X over the "true" modality, i.e. \f$
       * \underline{p}(X = 1 \mid pa(X) = j) \f$.
       */
      typename std::vector<std::vector<GUM_SCALAR>> __binCptMin;

      /**
       * @brief Used with binary networks to speed-up L2U inference. Store the upper
       * probabilities of each node X over the "true" modality, i.e. \f$
       * \overline{p}(X = 1 \mid pa(X) = j) \f$.
       */
      typename std::vector<std::vector<GUM_SCALAR>> __binCptMax;

      /** @brief %Set the NodeType of each node */
      void __sort_varType();

      /**
       * @deprecated
       * @param var_cpt The reference to a node CPT which may need a Decision Node.
       * @return Returns the cardinality of the Decision Node.
       */
      int __find_dNode_card(
          const std::vector<std::vector<std::vector<GUM_SCALAR>>> &var_cpt) const;

      /**
       * Computes the vertices of each credal set according to their interval
       *definition (does not use lrs). Only works with credal sets defined such that
       *when one modality reach it's upper probability, all others are at their
       *lowest.
       *
       * Called by bnToCredal and idmLearning.
       */
      void __intervalToCredal();

      /** @brief Initialize private constant variables after the Constructor has been
       * called */
      void __initParams();

      /** @brief Initialize private BayesNet variables after the Constructor has been
       * called */
      void __initCNNets(const std::string &src_min_num,
                        const std::string &src_max_den);

      /** @brief Initialize private BayesNet variables after the Constructor has been
       * called */
      void __initCNNets(const BayesNet<GUM_SCALAR> &src_min_num,
                        const BayesNet<GUM_SCALAR> &src_max_den);

      /**
       * @deprecated
       * @warning May be useless since the BayesNet copy constructor seems to now
       *work well (parent order is preserved).
       *
       * Copy the up-to-date BayesNet associated with this CredalNet. Since all we
       *care about is the DAG, only arcs are copied. Because the order with which
       *arcs are created is important, the function iterates over the CPTs variables
       *to be sure parent order stays the same from a net to it's copy.
       *
       * @param bn_dest The reference to the new copy
       */
      void __bnCopy(BayesNet<GUM_SCALAR> &bn_dest);

      // void __H2Vcdd ( const std::vector< std::vector< GUM_SCALAR > > & h_rep,
      // std::vector< std::vector< GUM_SCALAR > > & v_rep ) const;
      /**
       * @deprecated one should use the LrsWrapper class
       * Computes the V-representation of a credal set, i.e. it's vertices, from it's
       *H-representation, i.e. the hyper-plan inequalities. Uses lrs.
       *
       * @param h_rep A constant reference to the H-representation of a credal set.
       * @param v_rep A reference to the V-representation of the same credal set.
       */
      void __H2Vlrs(const std::vector<std::vector<GUM_SCALAR>> &h_rep,
                    std::vector<std::vector<GUM_SCALAR>> &v_rep) const;

    }; // CredalNet

    extern template class CredalNet<float>;
    extern template class CredalNet<double>;
  } // namespace credal
} // namespace gum

#include <agrum/CN/credalNet.tcc>

#endif // __CREDAL_NET__H__
