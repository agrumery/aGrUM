/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief Template implementation of BN/BayesNet.h class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Lionel TORTI
 */

#include <algorithm>
#include <limits>
#include <set>

#include <agrum/base/core/debug.h>
#include <agrum/base/multidim/aggregators/amplitude.h>
#include <agrum/base/multidim/aggregators/and.h>
#include <agrum/base/multidim/aggregators/count.h>
#include <agrum/base/multidim/aggregators/exists.h>
#include <agrum/base/multidim/aggregators/forall.h>
#include <agrum/base/multidim/aggregators/max.h>
#include <agrum/base/multidim/aggregators/median.h>
#include <agrum/base/multidim/aggregators/min.h>
#include <agrum/base/multidim/aggregators/or.h>
#include <agrum/base/multidim/aggregators/sum.h>
#include <agrum/base/multidim/ICIModels/multiDimLogit.h>
#include <agrum/base/multidim/ICIModels/multiDimNoisyAND.h>
#include <agrum/base/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/base/multidim/ICIModels/multiDimNoisyORNet.h>
#include <agrum/base/variables/allDiscreteVariables.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>

#include <agrum/base/core/utils_string.h>

namespace gum {
  template < typename GUM_SCALAR >
  NodeId build_node(gum::BayesNet< GUM_SCALAR >& bn,
                    const std::string&           node,
                    const std::string&           default_domain) {
    auto v = fastVariable< GUM_SCALAR >(node, default_domain);

    NodeId res;
    try {
      res = bn.idFromName(v->name());
    } catch (gum::NotFound const&) { res = bn.add(*v); }
    return res;
  }

  template < typename GUM_SCALAR >
  BayesNet< GUM_SCALAR > BayesNet< GUM_SCALAR >::fastPrototype(const std::string& dotlike,
                                                               Size               domainSize) {
    return fastPrototype(dotlike, "[" + std::to_string(domainSize) + "]");
  }

  template < typename GUM_SCALAR >
  BayesNet< GUM_SCALAR > BayesNet< GUM_SCALAR >::fastPrototype(const std::string& dotlike,
                                                               const std::string& domain) {
    gum::BayesNet< GUM_SCALAR > bn;

    for (const auto& chaine: split(remove_newline(dotlike), ";")) {
      NodeId lastId   = 0;
      bool   notfirst = false;
      for (const auto& souschaine: split(chaine, "->")) {
        bool forward = true;
        for (auto& node: split(souschaine, "<-")) {
          auto idVar = build_node(bn, node, domain);
          if (notfirst) {
            if (forward) {
              bn.addArc(lastId, idVar);
              forward = false;
            } else {
              bn.addArc(idVar, lastId);
            }
          } else {
            notfirst = true;
            forward  = false;
          }
          lastId = idVar;
        }
      }
    }
    bn.generateCPTs();
    bn.setProperty("name", "fastPrototype");
    return bn;
  }

  template < typename GUM_SCALAR >
  INLINE BayesNet< GUM_SCALAR >::BayesNet() : IBayesNet< GUM_SCALAR >() {
    GUM_CONSTRUCTOR(BayesNet)
  }

  template < typename GUM_SCALAR >
  INLINE BayesNet< GUM_SCALAR >::BayesNet(std::string name) : IBayesNet< GUM_SCALAR >(name) {
    GUM_CONSTRUCTOR(BayesNet)
  }

  template < typename GUM_SCALAR >
  BayesNet< GUM_SCALAR >::BayesNet(const BayesNet< GUM_SCALAR >& source) :
      IBayesNet< GUM_SCALAR >(source), _varMap_(source._varMap_) {
    GUM_CONS_CPY(BayesNet)

    _copyTensors_(source);
  }

  template < typename GUM_SCALAR >
  BayesNet< GUM_SCALAR >& BayesNet< GUM_SCALAR >::operator=(const BayesNet< GUM_SCALAR >& source) {
    if (this != &source) {
      IBayesNet< GUM_SCALAR >::operator=(source);
      _varMap_ = source._varMap_;

      _clearTensors_();
      _copyTensors_(source);
    }

    return *this;
  }

  template < typename GUM_SCALAR >
  BayesNet< GUM_SCALAR >::~BayesNet() {
    GUM_DESTRUCTOR(BayesNet)
    for (const auto& p: _probaMap_) {
      delete p.second;
    }
  }

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable& BayesNet< GUM_SCALAR >::variable(NodeId id) const {
    return _varMap_.get(id);
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNet< GUM_SCALAR >::changeVariableName(NodeId id, const std::string& new_name) {
    _varMap_.changeName(id, new_name);
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNet< GUM_SCALAR >::changeVariableLabel(NodeId             id,
                                                          const std::string& old_label,
                                                          const std::string& new_label) {
    if (variable(id).varType() != VarType::LABELIZED)
      GUM_ERROR(NotFound, "Variable " << id << " is not a LabelizedVariable.")

    LabelizedVariable const* var
        = dynamic_cast< LabelizedVariable* >(const_cast< DiscreteVariable* >(&variable(id)));

    var->changeLabel(var->posLabel(old_label), new_label);
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::nodeId(const DiscreteVariable& var) const {
    return _varMap_.get(var);
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::add(const DiscreteVariable& var) {
    auto ptr = new MultiDimArray< GUM_SCALAR >();
    try {
      return add(var, ptr);
    } catch (Exception const&) {
      delete ptr;
      throw;
    }
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::add(const std::string& fast_description,
                                            unsigned int       default_nbrmod) {
    auto v = fastVariable< GUM_SCALAR >(fast_description, default_nbrmod);
    if (v->domainSize() < 2) GUM_ERROR(OperationNotAllowed, v->name() << " has a domain size <2")
    return add(*v);
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::add(const DiscreteVariable&               var,
                                            MultiDimImplementation< GUM_SCALAR >* aContent) {
    NodeId proposedId = dag().nextNodeId();

    return add(var, aContent, proposedId);
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::add(const DiscreteVariable& var, NodeId id) {
    auto ptr = new MultiDimArray< GUM_SCALAR >();

    try {
      return add(var, ptr, id);
    } catch (Exception const&) {
      delete ptr;
      throw;
    }
  }

  template < typename GUM_SCALAR >
  NodeId BayesNet< GUM_SCALAR >::add(const DiscreteVariable&               var,
                                     MultiDimImplementation< GUM_SCALAR >* aContent,
                                     NodeId                                id) {
    _varMap_.insert(id, var);
    this->dag_.addNodeWithId(id);

    auto cpt = new Tensor< GUM_SCALAR >(aContent);
    (*cpt) << variable(id);
    _probaMap_.insert(id, cpt);
    return id;
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::idFromName(const std::string& name) const {
    return _varMap_.idFromName(name);
  }

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable&
      BayesNet< GUM_SCALAR >::variableFromName(const std::string& name) const {
    return _varMap_.variableFromName(name);
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >& BayesNet< GUM_SCALAR >::cpt(NodeId varId) const {
    return *(_probaMap_[varId]);
  }

  template < typename GUM_SCALAR >
  INLINE const VariableNodeMap& BayesNet< GUM_SCALAR >::variableNodeMap() const {
    return _varMap_;
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNet< GUM_SCALAR >::erase(const DiscreteVariable& var) {
    erase(_varMap_.get(var));
  }

  template < typename GUM_SCALAR >
  void BayesNet< GUM_SCALAR >::erase(NodeId varId) {
    if (_varMap_.exists(varId)) {
      // Reduce the variable child's CPT
      for (const NodeSet& children = this->children(varId); const auto c: children) {
        _probaMap_[c]->erase(variable(varId));
      }

      delete _probaMap_[varId];

      _probaMap_.erase(varId);
      _varMap_.erase(varId);
      this->dag_.eraseNode(varId);
    }
  }

  template < typename GUM_SCALAR >
  void BayesNet< GUM_SCALAR >::clear() {
    if (!this->empty()) {
      auto l = this->nodes();
      for (const auto no: l) {
        this->erase(no);
      }
    }
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNet< GUM_SCALAR >::addArc(NodeId tail, NodeId head) {
    if (this->dag_.existsArc(tail, head)) {
      GUM_ERROR(DuplicateElement, "The arc (" << tail << "," << head << ") already exists.")
    }

    this->dag_.addArc(tail, head);
    // Add parent in the child's CPT
    (*(_probaMap_[head])) << variable(tail);
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNet< GUM_SCALAR >::addArc(const std::string& tail, const std::string& head) {
    try {
      addArc(this->idFromName(tail), this->idFromName(head));
    } catch (DuplicateElement const&) {
      GUM_ERROR(DuplicateElement, "The arc " << tail << "->" << head << " already exists.")
    }
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNet< GUM_SCALAR >::eraseArc(const Arc& arc) {
    if (_varMap_.exists(arc.tail()) && _varMap_.exists(arc.head())) {
      NodeId head = arc.head();
      NodeId tail = arc.tail();
      this->dag_.eraseArc(arc);
      // Remove parent from child's CPT
      (*(_probaMap_[head])) >> variable(tail);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNet< GUM_SCALAR >::eraseArc(NodeId tail, NodeId head) {
    eraseArc(Arc(tail, head));
  }

  template < typename GUM_SCALAR >
  void BayesNet< GUM_SCALAR >::reverseArc(const Arc& arc) {
    // check that the arc exists
    if (!_varMap_.exists(arc.tail()) || !_varMap_.exists(arc.head()) || !dag().existsArc(arc)) {
      GUM_ERROR(InvalidArc, "a non-existing arc cannot be reversed")
    }

    NodeId tail = arc.tail();
    NodeId head = arc.head();

    // check that the reversal does not induce a cycle
    try {
      DAG d = dag();
      d.eraseArc(arc);
      d.addArc(head, tail);
    } catch (Exception const&) {
      GUM_ERROR(InvalidArc, "this arc reversal would induce a directed cycle")
    }

    // with the same notations as Shachter (1986), "evaluating influence
    // diagrams", p.878, we shall first compute the product of probabilities:
    // pi_j^old (x_j | x_c^old(j) ) * pi_i^old (x_i | x_c^old(i) )
    Tensor< GUM_SCALAR > prod{cpt(tail) * cpt(head)};

    // modify the topology of the graph: add to tail all the parents of head
    // and add to head all the parents of tail
    beginTopologyTransformation();
    NodeSet new_parents;
    for (const auto node: this->parents(tail))
      new_parents.insert(node);
    for (const auto node: this->parents(head))
      new_parents.insert(node);
    // remove arc (head, tail)
    eraseArc(arc);

    // add the necessary arcs to the tail
    for (const auto p: new_parents) {
      if ((p != tail) && !dag().existsArc(p, tail)) { addArc(p, tail); }
    }

    addArc(head, tail);
    // add the necessary arcs to the head
    new_parents.erase(tail);

    for (const auto p: new_parents) {
      if ((p != head) && !dag().existsArc(p, head)) { addArc(p, head); }
    }

    endTopologyTransformation();

    // update the conditional distributions of head and tail
    gum::VariableSet del_vars;
    del_vars << &(variable(tail));
    Tensor< GUM_SCALAR > new_cpt_head = prod.sumOut(del_vars).putFirst(&variable(head));

    auto& cpt_head = const_cast< Tensor< GUM_SCALAR >& >(cpt(head));
    cpt_head       = std::move(new_cpt_head);

    Tensor< GUM_SCALAR > new_cpt_tail{(prod / cpt_head).putFirst(&variable(tail))};
    auto&                cpt_tail = const_cast< Tensor< GUM_SCALAR >& >(cpt(tail));
    cpt_tail                      = std::move(new_cpt_tail);
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNet< GUM_SCALAR >::reverseArc(NodeId tail, NodeId head) {
    reverseArc(Arc(tail, head));
  }

  //==============================================
  // Aggregators
  //=============================================
  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addAMPLITUDE(const DiscreteVariable& var) {
    return add(var, new aggregator::Amplitude< GUM_SCALAR >());
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addAND(const DiscreteVariable& var) {
    if (var.domainSize() > 2) GUM_ERROR(SizeError, "an AND has to be boolean")

    return add(var, new aggregator::And< GUM_SCALAR >());
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addCOUNT(const DiscreteVariable& var, Idx value) {
    return add(var, new aggregator::Count< GUM_SCALAR >(value));
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addEXISTS(const DiscreteVariable& var, Idx value) {
    if (var.domainSize() > 2) GUM_ERROR(SizeError, "an EXISTS has to be boolean")

    return add(var, new aggregator::Exists< GUM_SCALAR >(value));
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addFORALL(const DiscreteVariable& var, Idx value) {
    if (var.domainSize() > 2) GUM_ERROR(SizeError, "an EXISTS has to be boolean")

    return add(var, new aggregator::Forall< GUM_SCALAR >(value));
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addMAX(const DiscreteVariable& var) {
    return add(var, new aggregator::Max< GUM_SCALAR >());
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addMEDIAN(const DiscreteVariable& var) {
    return add(var, new aggregator::Median< GUM_SCALAR >());
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addMIN(const DiscreteVariable& var) {
    return add(var, new aggregator::Min< GUM_SCALAR >());
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addOR(const DiscreteVariable& var) {
    if (var.domainSize() > 2) GUM_ERROR(SizeError, "an OR has to be boolean")

    return add(var, new aggregator::Or< GUM_SCALAR >());
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addSUM(const DiscreteVariable& var) {
    return add(var, new aggregator::Sum< GUM_SCALAR >());
  }

  //================================
  // ICIModels
  //================================
  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addNoisyOR(const DiscreteVariable& var,
                                                   GUM_SCALAR              external_weight) {
    return addNoisyORCompound(var, external_weight);
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addNoisyORCompound(const DiscreteVariable& var,
                                                           GUM_SCALAR external_weight) {
    return add(var, new MultiDimNoisyORCompound< GUM_SCALAR >(external_weight));
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addNoisyORNet(const DiscreteVariable& var,
                                                      GUM_SCALAR              external_weight) {
    return add(var, new MultiDimNoisyORNet< GUM_SCALAR >(external_weight));
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addNoisyAND(const DiscreteVariable& var,
                                                    GUM_SCALAR              external_weight) {
    return add(var, new MultiDimNoisyAND< GUM_SCALAR >(external_weight));
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addLogit(const DiscreteVariable& var,
                                                 GUM_SCALAR              external_weight) {
    return add(var, new MultiDimLogit< GUM_SCALAR >(external_weight));
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addNoisyOR(const DiscreteVariable& var,
                                                   GUM_SCALAR              external_weight,
                                                   NodeId                  id) {
    return addNoisyORCompound(var, external_weight, id);
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addNoisyAND(const DiscreteVariable& var,
                                                    GUM_SCALAR              external_weight,
                                                    NodeId                  id) {
    return add(var, new MultiDimNoisyAND< GUM_SCALAR >(external_weight), id);
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addLogit(const DiscreteVariable& var,
                                                 GUM_SCALAR              external_weight,
                                                 NodeId                  id) {
    return add(var, new MultiDimLogit< GUM_SCALAR >(external_weight), id);
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addNoisyORCompound(const DiscreteVariable& var,
                                                           GUM_SCALAR              external_weight,
                                                           NodeId                  id) {
    return add(var, new MultiDimNoisyORCompound< GUM_SCALAR >(external_weight), id);
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNet< GUM_SCALAR >::addNoisyORNet(const DiscreteVariable& var,
                                                      GUM_SCALAR              external_weight,
                                                      NodeId                  id) {
    return add(var, new MultiDimNoisyORNet< GUM_SCALAR >(external_weight), id);
  }

  template < typename GUM_SCALAR >
  void BayesNet< GUM_SCALAR >::addWeightedArc(NodeId tail, NodeId head, GUM_SCALAR causalWeight) {
    auto* CImodel = dynamic_cast< const MultiDimICIModel< GUM_SCALAR >* >(cpt(head).content());

    if (CImodel != 0) {
      // or is OK
      addArc(tail, head);

      CImodel->causalWeight(variable(tail), causalWeight);
    } else {
      GUM_ERROR(InvalidArc,
                "Head variable (" << variable(head).name() << ") is not a CIModel variable !")
    }
  }

  template < typename GUM_SCALAR >
  INLINE std::ostream& operator<<(std::ostream& output, const BayesNet< GUM_SCALAR >& bn) {
    output << bn.toString();
    return output;
  }

  /// begin Multiple Change for all CPTs
  template < typename GUM_SCALAR >
  void BayesNet< GUM_SCALAR >::beginTopologyTransformation() {
    for (const auto node: nodes())
      _probaMap_[node]->beginMultipleChanges();
  }

  /// end Multiple Change for all CPTs
  template < typename GUM_SCALAR >
  void BayesNet< GUM_SCALAR >::endTopologyTransformation() {
    for (const auto node: nodes())
      _probaMap_[node]->endMultipleChanges();
  }

  /// clear all tensors
  template < typename GUM_SCALAR >
  void BayesNet< GUM_SCALAR >::_clearTensors_() {
    // Removing previous tensors
    for (const auto& elt: _probaMap_) {
      delete elt.second;
    }

    _probaMap_.clear();
  }

  /// copy of tensors from a BN to another, using names of vars as ref.
  template < typename GUM_SCALAR >
  void BayesNet< GUM_SCALAR >::_copyTensors_(const BayesNet< GUM_SCALAR >& source) {
    // Copying tensors

    for (const auto& src: source._probaMap_) {
      // First we build the node's CPT
      auto copy_array = new Tensor< GUM_SCALAR >();
      copy_array->beginMultipleChanges();
      for (gum::Idx i = 0; i < src.second->nbrDim(); i++) {
        (*copy_array) << variableFromName(src.second->variable(i).name());
      }
      copy_array->endMultipleChanges();
      copy_array->copyFrom(*(src.second));

      // We add the CPT to the CPT hashmap
      _probaMap_.insert(src.first, copy_array);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNet< GUM_SCALAR >::generateCPTs() const {
    for (const auto node: nodes())
      generateCPT(node);
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNet< GUM_SCALAR >::generateCPT(NodeId node) const {
    SimpleCPTGenerator< GUM_SCALAR > generator;

    generator.generateCPT(cpt(node).pos(variable(node)), cpt(node));
  }

  template < typename GUM_SCALAR >
  void BayesNet< GUM_SCALAR >::changeTensor(NodeId id, Tensor< GUM_SCALAR >* newPot) {
    if (cpt(id).nbrDim() != newPot->nbrDim()) {
      GUM_ERROR(OperationNotAllowed,
                "cannot exchange tensors with different "
                "dimensions for variable with id "
                    << id)
    }

    for (Idx i = 0; i < cpt(id).nbrDim(); i++) {
      if (&cpt(id).variable(i) != &(newPot->variable(i))) {
        GUM_ERROR(OperationNotAllowed,
                  "cannot exchange tensors because, for variable with id " << id << ", dimension "
                                                                           << i << " differs. ")
      }
    }

    _unsafeChangeTensor_(id, newPot);
  }

  template < typename GUM_SCALAR >
  void BayesNet< GUM_SCALAR >::_unsafeChangeTensor_(NodeId id, Tensor< GUM_SCALAR >* newPot) {
    delete _probaMap_[id];
    _probaMap_[id] = newPot;
  }

  template < typename GUM_SCALAR >
  void BayesNet< GUM_SCALAR >::changeTensor(const std::string& name, Tensor< GUM_SCALAR >* newPot) {
    changeTensor(idFromName(name), newPot);
  }

  template < typename GUM_SCALAR >
  BayesNet< GUM_SCALAR >
      BayesNet< GUM_SCALAR >::contextualize(const gum::Instantiation& observations,
                                            const gum::Instantiation& interventions) const {
    Instantiation all;
    for (gum::Idx i = 0; i < observations.nbrDim(); i++) {
      if (interventions.contains(observations.variable(i))) {
        GUM_ERROR(ArgumentError,
                  "Cannot have both an observation and an intervention on the same variable")
      }
      all.add(observations.variable(i));
    }
    for (gum::Idx i = 0; i < observations.nbrDim(); i++) {
      all.add(interventions.variable(i));
    }
    all.setVals(observations);
    all.setVals(interventions);

    NodeSet cpt_changed;

    gum::BayesNet< GUM_SCALAR > bn(*this);

    bn.beginTopologyTransformation();
    for (gum::Idx i = 0; i < observations.nbrDim(); i++) {
      const std::string& nam = observations.variable(i).name();
      const gum::NodeId  nod = this->idFromName(nam);
      for (gum::NodeId child: this->children(nod)) {
        bn.eraseArc(bn.idFromName(nam), bn.idFromName(this->variable(child).name()));
      }
    }
    for (gum::Idx i = 0; i < interventions.nbrDim(); i++) {
      const std::string& nam = interventions.variable(i).name();
      const gum::NodeId  nod = this->idFromName(nam);
      for (gum::NodeId child: this->children(nod)) {
        bn.eraseArc(bn.idFromName(nam), bn.idFromName(this->variable(child).name()));
      }
      for (gum::NodeId par: this->parents(nod)) {
        const auto v1 = bn.idFromName(this->variable(par).name());
        const auto v2 = bn.idFromName(nam);
        if (bn.existsArc(v1, v2)) bn.eraseArc(v1, v2);
      }
      cpt_changed.insert(bn.idFromName(nam));
      bn.cpt(bn.idFromName(nam))
          .fillWith(Tensor< GUM_SCALAR >::deterministicTensor(bn.variable(bn.idFromName(nam)),
                                                              interventions.val(i)));
    }
    bn.endTopologyTransformation();

    for (gum::Idx i = 0; i < all.nbrDim(); i++) {
      const gum::NodeId nod = this->idFromName(all.variable(i).name());
      for (gum::NodeId child: this->children(nod)) {
        if (!cpt_changed.contains(child)) {
          cpt_changed.insert(child);
          bn.cpt(bn.idFromName(this->variable(child).name()))
              .fillWith(this->cpt(child).extract(all));
        }
      }
    }

    return bn;
  }
} /* namespace gum */
