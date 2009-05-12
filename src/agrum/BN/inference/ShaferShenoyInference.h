/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Implementation of the Shafer-Shenoy alogorithm for inference in
 * Bayesian Networks.
 */
#ifndef GUM_SHAFER_SHENOY_INFERENCE_H
#define GUM_SHAFER_SHENOY_INFERENCE_H

#include <vector>
#include <utility>
#include <iostream>
#include <string>
#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/core/debug.h>
#include <agrum/graphs/defaultTriangulation.h>


namespace gum {


  /**
   * @class ShaferShenoyInference
   * @brief This class implements the Shafer-Shenoy alogorithm for inference
   * in Bayesian Networks.
   * @ingroup bn_group
   *
   * The class used for the triangulation is DefaultTriangulation.
   * @todo Make it generic to Triangulation.
   * @todo Gestion of multiple connected components
   * @todo generalize joint law of two variables into n variables
   *
   * @warning : this class doesn't seem to infer bayesian network with multiple
   *           connected components
   */
  template<typename T_DATA>
  class ShaferShenoyInference: public BayesNetInference<T_DATA> {
  public:
    /**
     * Default constructor.
     *
     * @param bayesNet The Bayesian Network used for the inference.
     */
    ShaferShenoyInference( const BayesNet<T_DATA>& bayesNet );

    /**
     * Destructor.
     */
    virtual ~ShaferShenoyInference();

    /**
     * Makes the inference.
     */
    virtual void makeInference();

    /**
     * Insert new evidence in the graph.
     * @warning if an evidence already w.r.t. a given node and a new
     * evidence w.r.t. this node is onserted, the old evidence is removed.
     */
    virtual void insertEvidence( const List<const Potential<T_DATA>*>& pot_list );

    /**
     * Remove a given evidence from the graph.
     */
    virtual void eraseEvidence( const Potential<T_DATA>* );

    /**
     * Remove all evidence from the graph.
     */
    virtual void eraseAllEvidence();


  protected:
    /**
     * Returns the probability of the variable.
     *
     * @param id The variable's id.
     * @param marginal the potential to fill
     * @throw ElementNotFound Raised if no variable matches id.
     */
    virtual void _fillMarginal( Id id ,Potential<T_DATA>& marginal );


  private:
    /*****************************************************************************
     *                              USEFUL ALIAS                                 *
     ****************************************************************************/
    // Alias for the neighbours list
    typedef EdgeSet __neighbourList;

    // Alias for separators
    typedef NodeSet __Separator;

    /*****************************************************************************
     *                           PRIVATE MEMBERS                                 *
     ****************************************************************************/
    // The triangulation algorithm.
    DefaultTriangulation *__triangulation;

    /*****************************************************************************
     *                       USEFUL COMPUTATION MEMBERS                          *
     ****************************************************************************/
    // Mapping of the nodes with the clique used to put their CPT
    Property<NodeId>::onNodes __node2CliqueMap;

    // Lit of the evidences in a clique
    typename Property< List<const Potential<T_DATA>*>*>::onNodes __clique_evidences;

    // Mapping of an arc and the message which transited from pair.first to
    // pair.second
    typename Property<Potential<T_DATA>*>::onArcs __messagesMap;

    // Flags on cliques to collect and diffuse in every connexe composent
    Property< bool>::onNodes __collected_cliques;

    // Mapping between the variable's id and their domain size
    Property< unsigned int>::onNodes __modalitiesMap;

    // Mapping of the cliques with the tables associated with them
    typename Property< Potential<T_DATA>*>::onNodes __cliqueMap;

    // @return Returns the number of neighbours of a given clique
    Size __getNbrneighbours( Id cliqueId );

    // @return Returns the list of neighbours of a given clique
    const EdgeSet& __getneighbours( Id cliqueId );

    // @return Returns a separator given two adjacent cliques
    const __Separator& __getSeparator( Id clique_1, Id clique_2 );

    // @return Returns the clique in which the node's cpt must be stored
    Id __getClique( const std::vector<Id> &eliminationOrder, Id id );


    /*****************************************************************************
     *                          INFERENCE SUB METHODS                            *
     ****************************************************************************/
    // Builds the cliques tables
    // Uses __getCliquesTable to initialize the cliques table, and multiply
    // the tables with the adequate CPT.
    void __buildCliquesTables();

    // Add the evidences to the clique's table
    void __addEvidences();

    // Starting collect w.r.t. a clique
    void __collectFromClique( Id source );

    // Collecting phase of the inference
    bool __collect( Id source, Id current );

    // Starting diffusion w.r.t. a clique
    void __diffuseFromClique( Id source );

    // Diffusing phase of the inference
    void __diffuse( Id source, Id current, bool recompute );

    // Create and saves the message from key.first to key.second in the
    // __messagesMap.
    void __sendMessage( Id tail, Id head );

    // Return true if the message from source to dest exists
    bool __messageExists( Id source, Id dest );

    // Removes all diffused message sent by cliqueId
    void __removeDiffusedMessages( Id cliqueId );
  };


} /* namespace gum */


#include <agrum/BN/inference/ShaferShenoyInference.tcc>


#endif /* GUM_SHAFER_SHENOY_INFERENCE_H */
