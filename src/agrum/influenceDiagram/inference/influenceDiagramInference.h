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
 * @brief This file contains abstract class definitions influence diagrams
 *        inference classes.
 *
 * @author Jean-Christophe Magnan.
 */
// ============================================================================
#ifndef GUM_INFLUENCE_DIAGRAM_INFERENCE_H
#define GUM_INFLUENCE_DIAGRAM_INFERENCE_H
// ============================================================================
#include <agrum/core/exceptions.h>
// ============================================================================
#include <agrum/influenceDiagram/influenceDiagram.h>
// ============================================================================

namespace gum {
/**
 * @class InfluenceDiagramInference influenceDiagramInference.h <agrum/influenceDiagram/inference/influenceDiagramInference.h>
 * @brief Abstract class for making inference in influence diagrams.
 * @ingroup id_group
 *
 */
template <typename T_DATA>

class InfluenceDiagramInference {
public:
    /**
     * Default constructor
     */
    InfluenceDiagramInference( const InfluenceDiagram<T_DATA>& infDiag );

    /**
     * Destructor.
     */
    virtual ~InfluenceDiagramInference();

    /**
     * Makes the inference
     */
    virtual void makeInference() = 0;

    /**
     * Insert new evidence in the graph.
     * @warning if an evidence already w.r.t. a given node and a new
     * evidence w.r.t. this node is onserted, the old evidence is removed.
     * @throw OperationNotAllowed Raised if an evidence is over more than one variable.
     */
    virtual void insertEvidence( const List<const Potential<T_DATA>*>& evidenceList ) = 0;

    /**
     * Remove a given evidence from the graph.
     */
    virtual void eraseEvidence( const Potential<T_DATA>* evidence ) = 0;

    /**
     * Remove all evidence from the graph.
     */
    virtual void eraseAllEvidence() = 0;

    /**
     * Returns a constant reference over the InfluenceDiagram on which this class work.
     */
    const InfluenceDiagram<T_DATA>& getInfluenceDiagram() const;

    /**
     * Returns maximum expected utility obtained from inference
     * @throw OperationNotAllowed if no inference have yet been made
     */
    virtual T_DATA getMEU() = 0;

    /**
     * Returns best choice for decision variable given in parameter ( based upon MEU criteria )
     * @param decisionId the id of the decision variable
     * @throw OperationNotAllowed if no inference have yet been made
     * @throw InvalidNode if node given in parmaeter is not a decision node
     */
    virtual const Idx getBestDecisionChoice( NodeId decisionId ) = 0;

protected:

    /**
     * The Bayes net we wish to perform inference on.
     */
    const InfluenceDiagram<T_DATA>& __infDiag;
};

// ============================================================================
} /* namespace gum */

// ============================================================================
#include <agrum/influenceDiagram/inference/influenceDiagramInference.tcc>
// ============================================================================
#endif /* GUM_INFUENCE_DIAGRAM_INFERENCE_H */
// ============================================================================
// kate: indent-mode cstyle; space-indent on; indent-width 0;  replace-tabs on;
