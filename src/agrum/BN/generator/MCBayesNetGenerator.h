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
/** @file
 * @brief Class for generating bayesian networks.using MC algorithm
 * cf. [Ide and Cozman, 2002]
 *
 * @author Pierre-Henri WUILLEMIN and Ariele-Paolo MAESANO
 */

#ifndef GUM_MC_BAYES_NET_GENERATOR
#define GUM_MC_BAYES_NET_GENERATOR

#include <set>
#include <vector>
#include <iostream>
#include <fstream>

#include <sstream>

#include <dirent.h>
#include <sys/time.h>

#include <agrum/config.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/multidim/potential.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/generator/simpleCPTDisturber.h>
#include <agrum/core/hashTable.h>
#include <agrum/BN/generator/IBayesNetGenerator.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/BN/inference/lazyPropagation.h>

namespace gum {
  /**
   * @class MCBayesNetGenerator MCBayesNetGenerator.h
  <agrum/BN/generator/MCayesNetGenerator.h>
   * @brief Class for generating bayesian networks with Markov chains.
   * @ingroup bn_group
   *
   * This class randomly generates a bayesian network given 6 parameters:
   * the number of nodes, the maximum number of arcs the and of iterations the
  maximum modality.
   * @warning  Be Careful when entering the parameters, high Values may cause the
  density of the Bayesian Network to be too high
   * resulting in the failure of most of the inference Methods.\n
   *
   * \anchor probability_p_q
   *
   * This Generation method require the use of two probability parameters(p, q)
  defining the choice of processes that will provoke the change of state.
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
  template <typename GUM_SCALAR,
            template <class> class ICPTGenerator = SimpleCPTGenerator,
            template <class> class ICPTDisturber = SimpleCPTDisturber>
  class MCBayesNetGenerator : public IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>,
                              public ICPTDisturber<GUM_SCALAR> {

    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Constructor.
     * Use by default the SimpleCPTGenerator for generating the BNs CPT
     * and the SimpleCPTDisturber to tweak the CPT when the dimension of the table
     * changes.
     * @param nbrNodes The number of nodes in the generated BN.
     * @param maxArcs The maximum number of Arcs.
     * @param maxModality Each DRV has from 2 to maxModality modalities
     * @param iteration The number of iterations wanted to repeat the algorithm
     * @param p probability for the change of the state (see \ref probability_p_q
     * "use of p and q" )
     * @param q probability for the change of the state (see \ref probability_p_q
     * "use of p and q" )
     */
    MCBayesNetGenerator(Size nbrNodes, Size maxArcs, Idx maxModality = 2,
                        Size iteration = 5000, Idx p = 30, Idx q = 40);

    /**
    * Constructor.
    * Use by default the SimpleCPTGenerator for generating the BNs CPT
    * and the SimpleCPTDisturber to tweak the CPT when the dimension of the table
    * changes.
    * @param bayesNet the IBayesNet used as reference to fill the parameters
    * nbrNodes, maxArcs and maxModality
    * @param iteration The number of iterations wanted to repeat the algorithm
    * @param p probability for the change of the state (see \ref probability_p_q )
    * @param q probability for the change of the state (see \ref probability_p_q )
    */
    MCBayesNetGenerator(BayesNet<GUM_SCALAR> bayesNet, Size iteration = 5000,
                        Idx p = 30, Idx q = 40);

    /**
    * Destructor.
    */

    ~MCBayesNetGenerator();

    /// @}

    // ############################################################################
    /// @name BN generation methods
    // ############################################################################
    /// @{
    /**
     * Generates a random bayesian network.
     * @param bayesNet empty IBayesNet to generate.
     * @return null but modify inputed Bayesian Network
     */
    void generateBN(BayesNet<GUM_SCALAR> &bayesNet);

    /**
    * Change randomly the topology of a specific bayesian networks.
    * @param bayesNetinit IBayesNet to be modify
    * @param iteration The number of iterations wanted to repeat the algorithm
    * @return null but modify inputed Bayesian Network
    * @throws OperationNotAllow if the initial state of the IBayesNet is not
    * respecting the wanted conditions
    * if iteration = 0, it is assumed that the number of iteration wanted is the same
    * as the one specified in the constructor
    */
    void disturbBN(BayesNet<GUM_SCALAR> &bayesNetinit, Size iteration = 0);

    ///@}

    // ############################################################################
    /// @name Getters and Setters
    // ############################################################################
    /// @{
    ///@name Getters

    /**
     * Return a constant reference to the number of iteration imposed on the Markov
     * Chain BayesNetGenerator.
     */
    Size iteration() const;
    /**
    * Return a constant reference to the probabilité p imposed on the Markov Chain
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
    * Modifies the value of the number of iterations impose on the BayesNetGenerator
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
    Size _iteration;
    Idx _p, _q;
    bool _disturbing;
    BayesNet<GUM_SCALAR> _bayesNettemp;
    HashTable<NodeId, Potential<GUM_SCALAR> *> _hashMarginal;

    /**
     * The function that verify if graph is a polytree.
     **/
    bool __isPolytree();
    /**
     * The function that verify if node i and j are connected.
     **/
    bool __connect(const NodeId i, const NodeId j);
    /**
     * The function that verify if there is a oriented path from node i to node j.
     **/
    bool __directedPath(const NodeId i, const NodeId j);
    /**
     * The function that will insert an arc between node i to node j, but only if
     *there isn't any cycle created.
     **/
    void __insertArc(NodeId i, NodeId j);
    /**
     * The function that will remove the arc between node i and node j. If the
     *boolean parameter mustbeconnex is true, the function will assert that the graph
     *remain connected and will restore the arc otherwise.
     **/
    void __eraseArc(NodeId i, NodeId j, bool mustbeconnex = true);

    /**
    * In the case that the graph is a polytree, the function will, according to the
    *probability p and q, choose which change of state must occure( AorR or AR or
    *jump) then will assert that the imposed constraint are respected and if not,
    *will return to the previous topology.
    **/

    void __PMMx_poly();
    /**
    * In the case that the graph is a multiconnected graph, the function will,
    *according to the probability p and q, choose which change of state must occure(
    *AorR or jump) then will assert that the imposed constraint are respected and if
    *not, will return to the previous topology.
    **/
    void __PMMx_multi();
    /**
     * In the case that the graph is a polytree, the function will add a ramdom arc
     *by the use of the function __insertArc if the arc does not exist allready.
     **/

    void __jump_poly();

    /**
     * In the case that the graph is a multiconnect graph, the function will choose
     *randomly two nodes and will remove the arc between them by the use of the
     *function __insertArc if the arc exists.
     **/

    void __jump_multi();

    /**
     * The function will add or remove a random arc in the graph using the functions
     *__insertArc and __removeArc.
     **/
    void __AorR();
    /**
    * The function will remove and add a random arc changing the topology of the
    *graph but asserting its connectivity.
    **/
    void __AR();
    /**
     * The boolean function that will assert the respect of the constraint.
     **/
    virtual bool __checkConditions();

    void __createDAG(Size BNSize, Size iniRoot);

    std::vector<Idx> *__createPartDAG(Size BNSize, Size iniRoot);

    /**
     * The internal function used by the previous __connect. It asserts the existence
     *of an unoriented path between node i and node j avoiding passing through nodes
     *listed in excluded.
     **/

    bool __connect(const NodeId i, const NodeId j, NodeSet &excluded);

    /**
     * The internal function used by the previous __directedPath. It asserts the
     *existence of an oriented path between node i and node j avoiding passing
     *through nodes listed in excluded.
     **/
    bool __directedPath(const NodeId tail, const NodeId head, NodeSet &excluded);

    /**
     * The function that randomly choose two nodes of the graph.
     **/

    void __chooseNodes(NodeId &i, NodeId &j);

    /**
     * The function that randomly choose two neighbours nodes of the graph.
     **/
    void __chooseCloseNodes(NodeId &i, NodeId &j);

    /**
     * The function that randomly change the simple tree into a polytree.
     **/

    void __transformPoly(Idx nbiter);

    /**
     * The function that randomly generate a simple tree.
     **/
    void __createTree(Size BNSize);

    /**
     * The internal function used by __createTree that randomly generate a simple
     *tree.
     * n : id number for node label
     **/
    NodeId __createPartTree(Size BNSize, Idx &n);
  };

  extern template class MCBayesNetGenerator<float>;
  extern template class MCBayesNetGenerator<double>;
} /*namespace gum*/

#include <agrum/BN/generator/MCBayesNetGenerator.tcc>
#endif // MCBAYESNETGENERATOR
