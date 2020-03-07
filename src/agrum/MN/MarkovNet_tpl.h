
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief Template implementation of BN/MarkovNet.h class.
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */

#include <limits>
#include <set>

#include <agrum/MN/MarkovNet.h>

#include <agrum/variables/rangeVariable.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/variables/discretizedVariable.h>

#include <agrum/multidim/aggregators/amplitude.h>
#include <agrum/multidim/aggregators/and.h>
#include <agrum/multidim/aggregators/count.h>
#include <agrum/multidim/aggregators/exists.h>
#include <agrum/multidim/aggregators/forall.h>
#include <agrum/multidim/aggregators/max.h>
#include <agrum/multidim/aggregators/median.h>
#include <agrum/multidim/aggregators/min.h>
#include <agrum/multidim/aggregators/or.h>

#include <agrum/multidim/ICIModels/multiDimNoisyAND.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORNet.h>

#include <agrum/multidim/ICIModels/multiDimLogit.h>

#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/core/utils_string.h>

namespace gum {
  template<typename GUM_SCALAR>
  NodeId build_node(gum::MarkovNet<GUM_SCALAR> &bn,
                    std::string node,
                    gum::Size default_domain_size) {
    std::string name = node;
    auto ds = default_domain_size;
    long range_min = 0;
    long range_max = long(ds) - 1;
    std::vector<std::string> labels;
    std::vector<GUM_SCALAR> ticks;

    if (*(node.rbegin()) == ']') {
      auto posBrack = node.find('[');
      if (posBrack != std::string::npos) {
        name = node.substr(0, posBrack);
        const auto &s_args = node.substr(posBrack + 1, node.size() - posBrack - 2);
        const auto &args = split(s_args, ",");
        if (args.size() == 0) {   // n[]
          GUM_ERROR(InvalidArgument, "Empty range for variable " << node)
        } else if (args.size() == 1) {   // n[4]
          ds = static_cast< Size >(std::stoi(args[0]));
          range_min = 0;
          range_max = long(ds) - 1;
        } else if (args.size() == 2) {   // n[5,10]
          range_min = std::stol(args[0]);
          range_max = std::stol(args[1]);
          if (1 + range_max - range_min < 2) {
            GUM_ERROR(InvalidArgument, "Invalid range for variable " << node);
          }
          ds = static_cast< Size >(1 + range_max - range_min);
        } else {   // n[3.14,5,10,12]
          for (const auto &tick: args) {
            ticks.push_back(static_cast< GUM_SCALAR >(std::atof(tick.c_str())));
          }
          ds = static_cast< Size >(args.size() - 1);
        }
      }
    } else if (*(node.rbegin()) == '}') {   // node like "n{one|two|three}"
      auto posBrack = node.find('{');
      if (posBrack != std::string::npos) {
        name = node.substr(0, posBrack);
        labels = split(node.substr(posBrack + 1, node.size() - posBrack - 2), "|");
        if (labels.size() < 2) {
          GUM_ERROR(InvalidArgument, "Not enough labels in node " << node);
        }
        if (!hasUniqueElts(labels)) {
          GUM_ERROR(InvalidArgument, "Duplicate labels in node " << node);
        }
        ds = static_cast< Size >(labels.size());
      }
    }

    if (ds == 0) {
      GUM_ERROR(InvalidArgument, "No value for variable " << name << ".");
    } else if (ds == 1) {
      GUM_ERROR(InvalidArgument,
                "Only one value for variable " << name
                                               << " (2 at least are needed).");
    }

    // now we add the node in the BN
    NodeId idVar;
    try {
      idVar = bn.idFromName(name);
    } catch (gum::NotFound &) {
      if (!labels.empty()) {
        idVar = bn.add(LabelizedVariable(name, name, labels));
      } else if (!ticks.empty()) {
        idVar = bn.add(DiscretizedVariable<GUM_SCALAR>(name, name, ticks));
      } else {
        idVar = bn.add(RangeVariable(name, name, range_min, range_max));
      }
    }

    return idVar;
  }

  template<typename GUM_SCALAR>
  MarkovNet<GUM_SCALAR>
  MarkovNet<GUM_SCALAR>::fastPrototype(const std::string &dotlike,
                                       Size domainSize) {
    GUM_ERROR(FatalError, "Not Implemented Yet");
    /*
    gum::MarkovNet< GUM_SCALAR > mn;


    for (const auto& chaine: split(dotlike, ";")) {
      NodeId lastId = 0;
      bool   notfirst = false;
      for (const auto& souschaine: split(chaine, "->")) {
        bool forward = true;
        for (const auto& node: split(souschaine, "<-")) {
          auto idVar = build_node(mn, node, domainSize);
          if (notfirst) {
            if (forward) {
              mn.addArc(lastId, idVar);
              forward = false;
            } else {
              mn.addArc(idVar, lastId);
            }
          } else {
            notfirst = true;
            forward = false;
          }
          lastId = idVar;
        }
      }
    }
    mn.generateFactors();
    mn.setProperty("name", "fastPrototype");
    return mn;
     */
  }

  template<typename GUM_SCALAR>
  INLINE MarkovNet<GUM_SCALAR>::MarkovNet() : IMarkovNet<GUM_SCALAR>() {
    GUM_CONSTRUCTOR(MarkovNet);
  }

  template<typename GUM_SCALAR>
  INLINE MarkovNet<GUM_SCALAR>::MarkovNet(std::string name) :
      IMarkovNet<GUM_SCALAR>(name) {
    GUM_CONSTRUCTOR(MarkovNet);
  }

  template<typename GUM_SCALAR>
  MarkovNet<GUM_SCALAR>::MarkovNet(const MarkovNet<GUM_SCALAR> &source) :
      IMarkovNet<GUM_SCALAR>(source), __varMap(source.__varMap) {
    GUM_CONS_CPY(MarkovNet);

    __copyFactors(source);
  }

  template<typename GUM_SCALAR>
  MarkovNet<GUM_SCALAR> &
  MarkovNet<GUM_SCALAR>::operator=(const MarkovNet<GUM_SCALAR> &source) {
    if (this != &source) {
      IMarkovNet<GUM_SCALAR>::operator=(source);
      __varMap = source.__varMap;

      __clearFactors();
      __copyFactors(source);
    }

    return *this;
  }

  template<typename GUM_SCALAR>
  MarkovNet<GUM_SCALAR>::~MarkovNet() {
    GUM_DESTRUCTOR(MarkovNet);
  }

  template<typename GUM_SCALAR>
  INLINE const DiscreteVariable &
  MarkovNet<GUM_SCALAR>::variable(NodeId id) const {
    return __varMap.get(id);
  }

  template<typename GUM_SCALAR>
  INLINE void
  MarkovNet<GUM_SCALAR>::changeVariableName(NodeId id,
                                            const std::string &new_name) {
    __varMap.changeName(id, new_name);
  }

  template<typename GUM_SCALAR>
  INLINE void MarkovNet<GUM_SCALAR>::changeVariableLabel(
      NodeId id, const std::string &old_label, const std::string &new_label) {
    if (variable(id).varType() != VarType::Labelized) {
      GUM_ERROR(NotFound, "Variable " << id << " is not a LabelizedVariable.");
    }
    LabelizedVariable *var = dynamic_cast< LabelizedVariable * >(
        const_cast< DiscreteVariable * >(&variable(id)));

    var->changeLabel(var->posLabel(old_label), new_label);
  }

  template<typename GUM_SCALAR>
  INLINE NodeId MarkovNet<GUM_SCALAR>::nodeId(const DiscreteVariable &var) const {
    return __varMap.get(var);
  }

  template<typename GUM_SCALAR>
  const Potential<GUM_SCALAR> &MarkovNet<GUM_SCALAR>::factor(const NodeSet &varIds) const {
    GUM_ERROR(FatalError, "Not Implemented Yet");
  }


  template<typename GUM_SCALAR>
  INLINE NodeId MarkovNet<GUM_SCALAR>::add(const std::string &name,
                                           unsigned int nbrmod) {
    if (nbrmod < 2) {
      GUM_ERROR(OperationNotAllowed,
                "Variable " << name << "needs more than " << nbrmod
                            << " modalities");
    }

    RangeVariable v(name, name, 0, nbrmod - 1);
    return add(v);
  }

  template<typename GUM_SCALAR>
  INLINE NodeId MarkovNet<GUM_SCALAR>::add(const DiscreteVariable &var) {
    return add(var, graph().nextNodeId());
  }

  template<typename GUM_SCALAR>
  INLINE NodeId MarkovNet<GUM_SCALAR>::add(const DiscreteVariable &var,
                                           NodeId id) {
    __varMap.insert(id, var);
    this->_graph.addNodeWithId(id);
    return id;
  }

  template<typename GUM_SCALAR>
  INLINE NodeId MarkovNet<GUM_SCALAR>::idFromName(const std::string &name) const {
    return __varMap.idFromName(name);
  }

  template<typename GUM_SCALAR>
  INLINE const DiscreteVariable &
  MarkovNet<GUM_SCALAR>::variableFromName(const std::string &name) const {
    return __varMap.variableFromName(name);
  }

  template<typename GUM_SCALAR>
  INLINE const VariableNodeMap &MarkovNet<GUM_SCALAR>::variableNodeMap() const {
    return __varMap;
  }

  template<typename GUM_SCALAR>
  INLINE void MarkovNet<GUM_SCALAR>::erase(const DiscreteVariable &var) {
    erase(__varMap.get(var));
  }

  template<typename GUM_SCALAR>
  void MarkovNet<GUM_SCALAR>::erase(NodeId varId) {
    GUM_ERROR(FatalError, "Not implemented yet");
    /*
    if (__varMap.exists(varId)) {
      // Reduce the variable child's CPT
      const NodeSet& children = this->children(varId);

      for (const auto c: children) {
        __probaMap[c]->erase(variable(varId));
      }

      delete __probaMap[varId];

      __probaMap.erase(varId);
      __varMap.erase(varId);
      this->_dag.eraseNode(varId);
    }*/
  }

  template<typename GUM_SCALAR>
  void MarkovNet<GUM_SCALAR>::clear() {
    if (!this->empty()) {
      auto l = this->nodes();
      for (const auto no: l) {
        this->erase(no);
      }
    }
  }


  template<typename GUM_SCALAR>
  INLINE std::ostream &operator<<(std::ostream &output,
                                  const MarkovNet<GUM_SCALAR> &bn) {
    output << bn.toString();
    return output;
  }


  template<typename GUM_SCALAR>
  INLINE void MarkovNet<GUM_SCALAR>::generateFactors() const {
    GUM_ERROR(FatalError, "Not implemented yet");
  }

  template<typename GUM_SCALAR>
  INLINE void MarkovNet<GUM_SCALAR>::generateFactor(const NodeSet &vars) const {
    GUM_ERROR(FatalError, "Not implemented yet");
    /*
    SimpleCPTGenerator< GUM_SCALAR > generator;

    generator.generateCPT(cpt(node).pos(variable(node)), cpt(node));*/
  }

  template<typename GUM_SCALAR>
  void MarkovNet<GUM_SCALAR>::changeFactor(const NodeSet &vars,
                                           Potential<GUM_SCALAR> *newPot) {
    GUM_ERROR(FatalError, "Not implemented yet");
    /*
    if (cpt(id).nbrDim() != newPot->nbrDim()) {
      GUM_ERROR(OperationNotAllowed,
                "cannot exchange potentials with different "
                "dimensions for variable with id "
                   << id);
    }

    for (Idx i = 0; i < cpt(id).nbrDim(); i++) {
      if (&cpt(id).variable(i) != &(newPot->variable(i))) {
        GUM_ERROR(OperationNotAllowed,
                  "cannot exchange potentials because, for variable with id "
                     << id << ", dimension " << i << " differs. ");
      }
    }

    _unsafeChangePotential(id, newPot);
     */
  }

  template<typename GUM_SCALAR>
  void MarkovNet<GUM_SCALAR>::__clearFactors() {
    GUM_ERROR(FatalError, "Not implemented yet");
  }

  template<typename GUM_SCALAR>
  void MarkovNet<GUM_SCALAR>::__copyFactors(const MarkovNet<GUM_SCALAR> &source) {
    GUM_ERROR(FatalError, "Not implemented yet");
  }
} /* namespace gum */
