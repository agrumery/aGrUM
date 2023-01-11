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
 * @brief This file contains abstract class definitions influence diagrams
 *        inference classes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#ifndef GUM_INFLUENCE_DIAGRAM_INFERENCE_H
#define GUM_INFLUENCE_DIAGRAM_INFERENCE_H

#include <agrum/ID/influenceDiagram.h>
#include <agrum/tools/graphicalModels/inference/graphicalModelInference.h>

namespace gum {
  /**
   * @class InfluenceDiagramInference InfluenceDiagramInference.h
   *<agrum/ID/inference/influenceDiagramInference.h>
   * @brief Abstract base class for inference engines in influence diagrams.
   * @ingroup id_group
   *
   */
  template < typename GUM_SCALAR >
  class InfluenceDiagramInference: public GraphicalModelInference< GUM_SCALAR > {
    public:
    /**
     * Default constructor
     */
    explicit InfluenceDiagramInference(const InfluenceDiagram< GUM_SCALAR >* infDiag);

    /**
     * Destructor.
     */
    virtual ~InfluenceDiagramInference();

    /// assigns a new influence diagram to the inference engine
    /** Assigns a new influence diagram to the o,fere,ce e,go,e engine and sends
     * messages to the descendants of ShaferShenoyLIMIDInference to inform them
     * that the ID has changed.
     * @warning note that, by aGrUM's rule, the influence Diagram is not copied
     * into the inference engine but only referenced. */
    virtual void setInfluenceDiagram(const InfluenceDiagram< GUM_SCALAR >* infdiag);

    /// Returns a constant reference over the IBayesNet referenced by this class
    /** @throws UndefinedElement is raised if no Bayes net has been assigned to
     * the inference. */
    virtual const InfluenceDiagram< GUM_SCALAR >& influenceDiagram() const final;

    virtual gum::Potential< GUM_SCALAR > optimalDecision(NodeId decisionId)           = 0;
    virtual gum::Potential< GUM_SCALAR > optimalDecision(const std::string& nodename) = 0;

    virtual const Potential< GUM_SCALAR >& posterior(NodeId node)                 = 0;
    virtual const Potential< GUM_SCALAR >& posterior(const std::string& nodename) = 0;

    virtual const Potential< GUM_SCALAR >& posteriorUtility(NodeId node)                 = 0;
    virtual const Potential< GUM_SCALAR >& posteriorUtility(const std::string& nodename) = 0;

    virtual std::pair< GUM_SCALAR, GUM_SCALAR > meanVar(NodeId node)                 = 0;
    virtual std::pair< GUM_SCALAR, GUM_SCALAR > meanVar(const std::string& nodename) = 0;

    virtual std::pair< GUM_SCALAR, GUM_SCALAR > MEU() = 0;

    private:
    /// assigns a influence diagram during the inference engine construction
    void _setIDDuringConstruction_(const InfluenceDiagram< GUM_SCALAR >* infdiag);
  };

} /* namespace gum */

#include <agrum/ID/inference/tools/influenceDiagramInference_tpl.h>

#endif /* GUM_INFLUENCE_DIAGRAM_INFERENCE_H */
