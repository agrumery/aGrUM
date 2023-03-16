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


/** @file
 * @brief Class for generating Bayesian networks.using MC algorithm
 * cf. [Ide and Cozman, 2002]
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Ariele-Paolo MAESANO
 */

#ifndef GUM_MC_BAYES_NET_GENERATOR
#define GUM_MC_BAYES_NET_GENERATOR

#define NB_INIT_ITERATIONS 5000

#include <agrum/agrum.h>

#include <fstream>
#include <iostream>
#include <set>
#include <vector>

#include <sstream>

#ifdef HAVE_DIRENT_H
#  include <dirent.h>
#else
#  include <agrum/tools/core/mvsc/dirent.h>
#endif

#include <agrum/BN/generator/IBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTDisturber.h>
#include <agrum/BN/inference/lazyPropagation.h>

namespace gum {
  /**
   * @class MCBayesNetGenerator
   * @headerfile MCBayesNetGenerator.h
   * <agrum/BN/generator/MCayesNetGenerator.h>
   * @brief Class for generating Bayesian networks with Markov chains.
   * @ingroup bn_generator
   *
   * This class randomly generates a Bayesian network given 6 parameters: the
   * number of nodes, the maximum number of arcs the and of iterations the
   * maximum modality.
   * @warning  Be Careful when entering the parameters, high Values may cause
   * the density of the Bayesian network to be too high resulting in the
   * failure of most of the inference Methods.
   *
   * \anchor probability_p_q
   *
   * This Generation method require the use of two probability parameters(p, q)
   * defining the choice of processes that will provoke the change of state.
   * You can see in the graph below how the probabilities are used.
   *
   * \dot
  digraph {
  size=4
  bgcolor="transparent"
  //subgraph cluster_cond{
  if[shape=none, color=white ]
  //}

  subgraph cluster_state{ color=invis;

  polytree
  multiconnected
  }

  subgraph cluster_function {
    color=invis;
  AorR1[shape=box,label="AorR", color=white]
  AR1[shape=box,label="AR", color=white]
  jump1[shape=box,label="Jump", color=white]
  AorR2[shape=box,label="AorR", color=white]
  jump2[shape=box,label="jump", color=white]
  }

  //subgraph cluster_verdict{
  reject[shape=none, color=white]
  accept[shape=none, color=white]
  //}

  //subgraph cluster_cond2{
  if2[label="if",shape=none, color=white]
  //}

  subgraph cluster_intstate{ color=invis;


  polytree2[label="polytree", color=white]
  multiconnected2[label="multiconnected", color=white]
  }
  subgraph cluster_finstate{ color=invis;


  polytree3[label="polytree", color=white]
  multiconnected3[label="multiconnected", color=white]
  }
  or[shape=none, color=white]

  if -> polytree
  if -> multiconnected

  polytree -> AorR1[label="p"]
  polytree -> AR1[label="q"]
  polytree -> jump1[label="1-p-q"]

  multiconnected -> AorR2[label="p+q"]
  multiconnected -> jump2[label="1-p-q"]

  AorR1 -> multiconnected3[style=dotted]
  AR1 -> polytree3[style=dotted]
  jump1 -> multiconnected3[style=dotted]

  AorR2 -> if2
  //jump2 -> polytree3[style=dotted]

  if2 -> polytree2
  if2 ->  multiconnected2
  polytree2 -> reject[label="p/(p+q)"]
  polytree2 -> accept[label="q/(p+q)"]

  accept -> polytree3[style=dotted]
  multiconnected2 -> multiconnected3[style=dotted]

  jump2 -> or

  or -> polytree3[style=dotted]
  or -> multiconnected3[style=dotted]

  }
   \enddot
   */
  template < typename GUM_SCALAR,
             template < typename > class ICPTGenerator = SimpleCPTGenerator,
             template < typename > class ICPTDisturber = SimpleCPTDisturber >
  class MCBayesNetGenerator:
      public IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >,
      public ICPTDisturber< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Constructor.
     * Use by default the SimpleCPTGenerator for generating the BNs CPT
     * and the SimpleCPTDisturber to tweak the CPT when the dimension of the
     * table
     * changes.
     * @param nbrNodes The number of nodes in the generated BN.
     * @param maxArcs The maximum number of Arcs.
     * @param maxModality Each DRV has from 2 to maxModality modalities
     * @param iteration The number of iterations wanted to repeat the algorithm
     * @param p probability for the change of the state (see \ref
     * probability_p_q
     * "use of p and q" )
     * @param q probability for the change of the state (see \ref
     * probability_p_q
     * "use of p and q" )
     */
    MCBayesNetGenerator(Size nbrNodes,
                        Size maxArcs,
                        Idx  maxModality = 2,
                        Size iteration   = NB_INIT_ITERATIONS,
                        Idx  p           = 30,
                        Idx  q           = 40);

    /**
     * Constructor.
     * Use by default the SimpleCPTGenerator for generating the BNs CPT
     * and the SimpleCPTDisturber to tweak the CPT when the dimension of the
     * table
     * changes.
     * @param bayesNet the IBayesNet used as reference to fill the parameters
     * nbrNodes, maxArcs and maxModality
     * @param iteration The number of iterations wanted to repeat the algorithm
     * @param p probability for the change of the state (see \ref probability_p_q
     * )
     * @param q probability for the change of the state (see \ref probability_p_q
     * )
     */
    explicit MCBayesNetGenerator(BayesNet< GUM_SCALAR > bayesNet,
                                 Size                   iteration = NB_INIT_ITERATIONS,
                                 Idx                    p         = 30,
                                 Idx                    q         = 40);

    /**
     * Destructor.
     */

    ~MCBayesNetGenerator() override;

    /// @}

    // ############################################################################
    /// @name BN generation methods
    // ############################################################################
    /// @{
    /**
     * Generates a random Bayesian network.
     * @param bayesNet empty IBayesNet to generate.
     * @return null but modify inputed Bayesian network
     */
    void generateBN(BayesNet< GUM_SCALAR >& bayesNet) override;

    /**
     * Change randomly the topology of a specific Bayesian networks.
     * @param bayesNetinit IBayesNet to be modify
     * @param iteration The number of iterations wanted to repeat the algorithm
     * @return null but modify inputed Bayesian network
     * @throws OperationNotAllow if the initial state of the IBayesNet is not
     * respecting the wanted conditions
     * if iteration = 0, it is assumed that the number of iteration wanted is the
     * same
     * as the one specified in the constructor

    void disturbBN(BayesNet< GUM_SCALAR >& bayesNetinit, Size iteration = 0);
    */

    ///@}

    // ############################################################################
    /// @name Getters and Setters
    // ############################################################################
    /// @{
    ///@name Getters

    /**
     * Return a constant reference to the number of iteration imposed on the
     * Markov
     * Chain BayesNetGenerator.
     */
    Size iteration() const;
    /**
     * Return a constant reference to the probabilité p imposed on the Markov
     * Chain
     * BayesNetGenerator.
     */
    Idx p() const;
    /**
     * Return a constant reference to the probabilité imposed on the Markov Chain
     * BayesNetGenerator.
     */
    Idx q() const;

    ///@name Setters

    /**
     * Modifies the value of the number of iterations impose on the
     * BayesNetGenerator
     */
    void setIteration(Size iteration);
    /**
     * Modifies the value of the probability p imposed on the BayesNetGenerator
     */
    void setP(Idx p);
    /**
     * Modifies the value of the probability q imposed on the BayesNetGenerator
     */
    void setQ(Idx q);

    /// @}
    protected:
    Size iteration_;
    Idx  p_, q_;
    /*
       bool                                          disturbing_;
    BayesNet< GUM_SCALAR >                        bayesNettemp_;
    HashTable< NodeId, Potential< GUM_SCALAR >* > hashMarginal_;*/

    /**
     * The function that verify if graph is a polytree.
     **/
    bool _isPolytree_();
    /**
     * The function that verify if node i and j are connected.
     **/
    bool _connect_(NodeId i, NodeId j);
    /**
     * The function that verify if there is a oriented path from node i to node
     *j.
     **/
    bool _directedPath_(NodeId tail, NodeId head);
    /**
     * The function that will insert an arc between node i to node j, but only
     *if
     *there isn't any cycle created.
     **/
    void _insertArc_(NodeId i, NodeId j);
    /**
     * The function that will remove the arc between node i and node j. If the
     *boolean parameter mustbeconnex is true, the function will assert that the
     *graph
     *remain connected and will restore the arc otherwise.
     **/
    void _eraseArc_(NodeId i, NodeId j, bool mustbeconnex = true);

    /**
     * In the case that the graph is a polytree, the function will, according to
     *the
     *probability p and q, choose which change of state must occure( AorR or AR
     *or
     *jump) then will assert that the imposed constraint are respected and if
     *not,
     *will return to the previous topology.
     **/

    void _PMMx_poly_();
    /**
     * In the case that the graph is a multiconnected graph, the function will,
     *according to the probability p and q, choose which change of state must
     *occure(
     *AorR or jump) then will assert that the imposed constraint are respected
     *and if
     *not, will return to the previous topology.
     **/
    void _PMMx_multi_();
    /**
     * In the case that the graph is a polytree, the function will add a ramdom
     *arc
     *by the use of the function  _insertArc_ if the arc does not exist allready.
     **/

    void _jump_poly_();

    /**
     * In the case that the graph is a multiconnect graph, the function will
     *choose
     *randomly two nodes and will remove the arc between them by the use of the
     *function  _insertArc_ if the arc exists.
     **/

    void _jump_multi_();

    /**
     * The function will add or remove a random arc in the graph using the
     *functions
     * _insertArc_ and  _removeArc_.
     **/
    void _AorR_();
    /**
     * The function will remove and add a random arc changing the topology of the
     *graph but asserting its connectivity.
     **/
    void _AR_();
    /**
     * The boolean function that will assert the respect of the constraint.
     **/
    virtual bool _checkConditions_();

    // NOT USED ? void  _createDAG_( Size BNSize, Size iniRoot );

    // NOT USED ? std::vector<Idx>*  _createPartDAG_( Size BNSize, Size iniRoot
    // );

    /**
     * The internal function used by the previous  _is_connected_. It asserts the
     *existence
     *of an unoriented path between node i and node j avoiding passing through
     *nodes
     *listed in excluded.
     **/

    bool _is_connected_(const NodeId i, const NodeId j, NodeSet& excluded);

    /**
     * The internal function used by the previous  _directedPath_. It asserts the
     *existence of an oriented path between node i and node j avoiding passing
     *through nodes listed in excluded.
     **/
    bool _directedPath_(NodeId tail, NodeId head, NodeSet& excluded);

    /**
     * The function that randomly choose two nodes of the graph.
     **/

    void _chooseNodes_(NodeId& i, NodeId& j);

    /**
     * The function that randomly choose two neighbours nodes of the graph.
     **/
    void _chooseCloseNodes_(NodeId& i, NodeId& j);

    /**
     * The function that randomly change the simple tree into a polytree.
     **/

    void _transformPoly_(Idx nbiter);

    /**
     * The function that randomly generate a simple tree.
     **/
    void _createTree_(Size BNSize);

    /**
     * The internal function used by  _createTree_ that randomly generate a
     *simple
     *tree.
     * n : id number for node label
     **/
    NodeId _createPartTree_(Size BNSize, Idx& n);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MCBayesNetGenerator< double >;
#endif

} /*namespace gum*/

#include <agrum/BN/generator/MCBayesNetGenerator_tpl.h>
#endif   // MCBAYESNETGENERATOR
