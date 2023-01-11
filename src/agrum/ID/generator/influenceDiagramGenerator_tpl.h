/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Source implementation of InfluenceDiagramGenerator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 *
 */
#include <agrum/tools/core/utils_random.h>
#include <agrum/ID/generator/influenceDiagramGenerator.h>

namespace gum {

  // Default constructor.
  // Use the SimpleCPTGenerator for generating the IDs CPT.
  template < typename GUM_SCALAR >
  InfluenceDiagramGenerator< GUM_SCALAR >::InfluenceDiagramGenerator() {
    GUM_CONSTRUCTOR(InfluenceDiagramGenerator)
    _cptGenerator_ = new SimpleCPTGenerator< GUM_SCALAR >();
    _utGenerator_  = new SimpleUTGenerator();
  }

  // Use this constructor if you want to use a different policy for generating
  // CPT than the default one.
  // The cptGenerator will be erased when the destructor is called.
  // @param cptGenerator The policy used to generate CPT.
  template < typename GUM_SCALAR >
  InfluenceDiagramGenerator< GUM_SCALAR >::InfluenceDiagramGenerator(
     ICPTGenerator< GUM_SCALAR >* cptGenerator) {
    GUM_CONSTRUCTOR(InfluenceDiagramGenerator)
    _cptGenerator_ = cptGenerator;
    _utGenerator_  = new SimpleUTGenerator();
  }

  // Use this constructor if you want to use a different policy for generating
  // UT than the default one.
  // The utGenerator will be erased when the destructor is called.
  // @param utGenerator The policy used to generate UT.
  template < typename GUM_SCALAR >
  InfluenceDiagramGenerator< GUM_SCALAR >::InfluenceDiagramGenerator(UTGenerator* utGenerator) {
    GUM_CONSTRUCTOR(InfluenceDiagramGenerator)
    _cptGenerator_ = new SimpleCPTGenerator< GUM_SCALAR >();
    _utGenerator_  = utGenerator;
  }

  // Use this constructor if you want to use a different policy for generating
  // both CPT & UT than the defaults ones.
  // The cptGenerator and utGenerator will be erased when the destructor is
  // called.
  // @param cptGenerator The policy used to generate CPT.
  // @param utGenerator The policy used to generate UT.
  template < typename GUM_SCALAR >
  InfluenceDiagramGenerator< GUM_SCALAR >::InfluenceDiagramGenerator(
     ICPTGenerator< GUM_SCALAR >* cptGenerator,
     UTGenerator*                 utGenerator) {
    GUM_CONSTRUCTOR(InfluenceDiagramGenerator)
    _cptGenerator_ = cptGenerator;
    _utGenerator_  = utGenerator;
  }

  // Destructor.
  template < typename GUM_SCALAR >
  InfluenceDiagramGenerator< GUM_SCALAR >::~InfluenceDiagramGenerator() {
    GUM_DESTRUCTOR(InfluenceDiagramGenerator)
    delete _cptGenerator_;
    delete _utGenerator_;
  }

  // Generates an influence diagram using floats.
  // @param nbrNodes The number of nodes in the generated ID.
  // @param arcdensity The probability of adding an arc between two nodes.
  // @param chanceNodeDensity The proportion of chance node
  // @param utilityNodeDensity The proportion of utility node
  // @param max_modality Each DRV has from 2 to max_modality modalities
  // @return A IDs randomly generated.
  template < typename GUM_SCALAR >
  InfluenceDiagram< GUM_SCALAR >*
     InfluenceDiagramGenerator< GUM_SCALAR >::generateID(Size       nbrNodes,
                                                         GUM_SCALAR arcDensity,
                                                         GUM_SCALAR chanceNodeDensity,
                                                         GUM_SCALAR utilityNodeDensity,
                                                         Size       max_modality) {
    auto influenceDiagram = new InfluenceDiagram< GUM_SCALAR >();
    // First we add nodes
    HashTable< Size, NodeId > map;
    std::stringstream         strBuff;
    Size                      nb_mod;

    for (Idx i = 0; i < nbrNodes; ++i) {
      strBuff << i;
      nb_mod = (max_modality == 2) ? 2 : 2 + randomValue(max_modality - 1);

      GUM_SCALAR cnd = chanceNodeDensity;
      GUM_SCALAR und = utilityNodeDensity;

      auto d = (GUM_SCALAR)randomProba();

      if (d < cnd)
        map.insert(i,
                   influenceDiagram->addChanceNode(LabelizedVariable(strBuff.str(), "", nb_mod)));
      else if (d < (cnd + und))
        map.insert(i, influenceDiagram->addUtilityNode(LabelizedVariable(strBuff.str(), "", 1)));
      else
        map.insert(i,
                   influenceDiagram->addDecisionNode(LabelizedVariable(strBuff.str(), "", nb_mod)));

      strBuff.str("");
    }

    // We add arcs
    GUM_SCALAR p = arcDensity;

    for (Size i = 0; i < nbrNodes; ++i)
      if (!influenceDiagram->isUtilityNode(map[i]))
        for (Size j = i + 1; j < nbrNodes; ++j)
          if (((GUM_SCALAR)randomProba()) < p) { influenceDiagram->addArc(map[i], map[j]); }

    // And fill the CPTs and UTs
    for (Size i = 0; i < nbrNodes; ++i)
      if (influenceDiagram->isChanceNode(map[i]))
        _cptGenerator_->generateCPT(
           influenceDiagram->cpt(map[i]).pos(influenceDiagram->variable(map[i])),
           influenceDiagram->cpt(map[i]));
      else if (influenceDiagram->isUtilityNode(map[i]))
        _utGenerator_->generateUT(
           influenceDiagram->utility(map[i]).pos(influenceDiagram->variable(map[i])),
           influenceDiagram->utility(map[i]));

    _checkTemporalOrder_(influenceDiagram);

    return influenceDiagram;
  }

  template < typename GUM_SCALAR >
  void InfluenceDiagramGenerator< GUM_SCALAR >::_checkTemporalOrder_(
     InfluenceDiagram< GUM_SCALAR >* infdiag) {
    if (!infdiag->decisionOrderExists()) {
      Sequence< NodeId > order = infdiag->topologicalOrder();

      auto orderIter = order.begin();

      while ((orderIter != order.end()) && (!infdiag->isDecisionNode(*orderIter)))
        ++orderIter;

      if (orderIter == order.end()) return;

      NodeId parentDecision = (*orderIter);

      ++orderIter;

      for (; orderIter != order.end(); ++orderIter)
        if (infdiag->isDecisionNode(*orderIter)) {
          infdiag->addArc(parentDecision, (*orderIter));
          parentDecision = (*orderIter);
        }
    }
  }

} /* namespace gum */
