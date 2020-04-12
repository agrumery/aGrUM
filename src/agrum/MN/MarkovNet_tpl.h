
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES
 * (@AMU) info_at_agrum_dot_org
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
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES (@AMU)
 */

#include <limits>
#include <set>

#include <agrum/MN/MarkovNet.h>

#include <agrum/tools/variables/rangeVariable.h>
#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/tools/variables/discretizedVariable.h>

#include <agrum/tools/multidim/aggregators/amplitude.h>
#include <agrum/tools/multidim/aggregators/and.h>
#include <agrum/tools/multidim/aggregators/count.h>
#include <agrum/tools/multidim/aggregators/exists.h>
#include <agrum/tools/multidim/aggregators/forall.h>
#include <agrum/tools/multidim/aggregators/max.h>
#include <agrum/tools/multidim/aggregators/median.h>
#include <agrum/tools/multidim/aggregators/min.h>
#include <agrum/tools/multidim/aggregators/or.h>

#include <agrum/tools/multidim/ICIModels/multiDimNoisyAND.h>
#include <agrum/tools/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/tools/multidim/ICIModels/multiDimNoisyORNet.h>

#include <agrum/tools/multidim/ICIModels/multiDimLogit.h>

#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/tools/core/utils_string.h>

namespace gum {
  template < typename GUM_SCALAR >
  NodeId build_node(MarkovNet< GUM_SCALAR >& mn,
                    std::string              node,
                    Size                     default_domain_size) {
    std::string                name = node;
    auto                       ds = default_domain_size;
    long                       range_min = 0;
    long                       range_max = long(ds) - 1;
    std::vector< std::string > labels;
    std::vector< GUM_SCALAR >  ticks;

    if (*(node.rbegin()) == ']') {
      auto posBrack = node.find('[');
      if (posBrack != std::string::npos) {
        name = node.substr(0, posBrack);
        const auto& s_args = node.substr(posBrack + 1, node.size() - posBrack - 2);
        const auto& args = split(s_args, ",");
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
          for (const auto& tick: args) {
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
      idVar = mn.idFromName(name);
    } catch (NotFound&) {
      if (!labels.empty()) {
        idVar = mn.add(LabelizedVariable(name, name, labels));
      } else if (!ticks.empty()) {
        idVar = mn.add(DiscretizedVariable< GUM_SCALAR >(name, name, ticks));
      } else {
        idVar = mn.add(RangeVariable(name, name, range_min, range_max));
      }
    }

    return idVar;
  }

  template < typename GUM_SCALAR >
  MarkovNet< GUM_SCALAR >
     MarkovNet< GUM_SCALAR >::fastPrototype(const std::string& dotlike,
                                            Size               domainSize) {
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

  template < typename GUM_SCALAR >
  INLINE MarkovNet< GUM_SCALAR >::MarkovNet() :
      IMarkovNet< GUM_SCALAR >(), __topologyTransformationInProgress(false) {
    GUM_CONSTRUCTOR(MarkovNet);
  }

  template < typename GUM_SCALAR >
  INLINE MarkovNet< GUM_SCALAR >::MarkovNet(std::string name) :
      IMarkovNet< GUM_SCALAR >(name), __topologyTransformationInProgress(false) {
    GUM_CONSTRUCTOR(MarkovNet);
  }

  template < typename GUM_SCALAR >
  MarkovNet< GUM_SCALAR >::MarkovNet(const MarkovNet< GUM_SCALAR >& source) :
      IMarkovNet< GUM_SCALAR >(source), __varMap(source.__varMap),
      __topologyTransformationInProgress(false) {
    GUM_CONS_CPY(MarkovNet);
    __copyFactors(source);
  }

  template < typename GUM_SCALAR >
  MarkovNet< GUM_SCALAR >&
     MarkovNet< GUM_SCALAR >::operator=(const MarkovNet< GUM_SCALAR >& source) {
    if (this != &source) {
      IMarkovNet< GUM_SCALAR >::operator=(source);
      __varMap = source.__varMap;
      __topologyTransformationInProgress = false;
      __copyFactors(source);
    }

    return *this;
  }

  template < typename GUM_SCALAR >
  MarkovNet< GUM_SCALAR >::~MarkovNet() {
    __clearFactors();
    GUM_DESTRUCTOR(MarkovNet);
  }

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable&
               MarkovNet< GUM_SCALAR >::variable(NodeId id) const {
    return __varMap.get(id);
  }

  template < typename GUM_SCALAR >
  INLINE void
     MarkovNet< GUM_SCALAR >::changeVariableName(NodeId             id,
                                                 const std::string& new_name) {
    __varMap.changeName(id, new_name);
  }

  template < typename GUM_SCALAR >
  INLINE void MarkovNet< GUM_SCALAR >::changeVariableLabel(
     NodeId id, const std::string& old_label, const std::string& new_label) {
    if (variable(id).varType() != VarType::Labelized) {
      GUM_ERROR(NotFound, "Variable " << id << " is not a LabelizedVariable.");
    }
    LabelizedVariable* var = dynamic_cast< LabelizedVariable* >(
       const_cast< DiscreteVariable* >(&variable(id)));

    var->changeLabel(var->posLabel(old_label), new_label);
  }

  template < typename GUM_SCALAR >
  INLINE NodeId
     MarkovNet< GUM_SCALAR >::nodeId(const DiscreteVariable& var) const {
    return __varMap.get(var);
  }

  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     MarkovNet< GUM_SCALAR >::factor(const NodeSet& varIds) const {
    return *__factors[varIds];
  }

  template < typename GUM_SCALAR >
  const FactorTable< GUM_SCALAR >& MarkovNet< GUM_SCALAR >::factors() const {
    return __factors;
  }

  template < typename GUM_SCALAR >
  INLINE NodeId MarkovNet< GUM_SCALAR >::add(const std::string& name,
                                             unsigned int       nbrmod) {
    if (nbrmod < 2) {
      GUM_ERROR(OperationNotAllowed,
                "Variable " << name << "needs more than " << nbrmod
                            << " modalities");
    }

    RangeVariable v(name, name, 0, nbrmod - 1);
    return add(v);
  }

  template < typename GUM_SCALAR >
  INLINE void MarkovNet< GUM_SCALAR >::__rebuildGraph() {
    if (__topologyTransformationInProgress) return;

    this->_graph.clearEdges();

    for (const auto& kv: __factors) {
      const Potential< double >& c = *kv.second;
      for (Idx i = 0; i < c.nbrDim(); i++)
        for (Idx j = i + 1; j < c.nbrDim(); j++)
          this->_graph.addEdge(__varMap.get(c.variable(i)),
                               __varMap.get(c.variable(j)));
    }
  }


  template < typename GUM_SCALAR >
  INLINE NodeId MarkovNet< GUM_SCALAR >::add(const DiscreteVariable& var) {
    return add(var, graph().nextNodeId());
  }

  template < typename GUM_SCALAR >
  INLINE NodeId MarkovNet< GUM_SCALAR >::add(const DiscreteVariable& var,
                                             NodeId                  id) {
    __varMap.insert(id, var);
    this->_graph.addNodeWithId(id);
    return id;
  }

  template < typename GUM_SCALAR >
  INLINE NodeId
     MarkovNet< GUM_SCALAR >::idFromName(const std::string& name) const {
    return __varMap.idFromName(name);
  }

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable&
               MarkovNet< GUM_SCALAR >::variableFromName(const std::string& name) const {
    return __varMap.variableFromName(name);
  }

  template < typename GUM_SCALAR >
  INLINE const VariableNodeMap& MarkovNet< GUM_SCALAR >::variableNodeMap() const {
    return __varMap;
  }

  template < typename GUM_SCALAR >
  INLINE void MarkovNet< GUM_SCALAR >::erase(const DiscreteVariable& var) {
    erase(__varMap.get(var));
  }

  template < typename GUM_SCALAR >
  INLINE void MarkovNet< GUM_SCALAR >::erase(const std::string& name) {
    erase(idFromName(name));
  }

  template < typename GUM_SCALAR >
  void MarkovNet< GUM_SCALAR >::erase(NodeId varId) {
    if (!__varMap.exists(varId)) {
      GUM_ERROR(InvalidArgument, "No node with id " << varId << ".")
    }
    __varMap.erase(varId);
    this->_graph.eraseNode(varId);

    std::vector< NodeSet > vs;
    for (const auto& kv: __factors) {
      if (kv.first.contains(varId)) { vs.push_back(kv.first); }
    }
    for (const auto& ns: vs) {
      __eraseFactor(ns);
    }
    for (const auto& ns: vs) {
      NodeSet nv = ns;
      nv.erase(varId);
      if (nv.size() > 1) __addFactor(nv);
    }
    __rebuildGraph();
  }

  template < typename GUM_SCALAR >
  void MarkovNet< GUM_SCALAR >::clear() {
    if (!this->empty()) {
      auto l = this->nodes();
      for (const auto no: l) {
        this->erase(no);
      }
    }
    __rebuildGraph();
  }


  template < typename GUM_SCALAR >
  INLINE std::ostream& operator<<(std::ostream&                  output,
                                  const MarkovNet< GUM_SCALAR >& mn) {
    output << mn.toString();
    return output;
  }

  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >&
               MarkovNet< GUM_SCALAR >::addFactor(const NodeSet& vars) {
    if (vars.size() == 0) {
      GUM_ERROR(InvalidArgument, "Empty factor cannot be added.")
    }

    if (__factors.exists(vars)) {
      GUM_ERROR(InvalidArgument, "A factor for (" << vars << ") already exists.")
    }

    auto res = __addFactor(vars);
    __rebuildGraph();

    return *res;
  }

  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >& MarkovNet< GUM_SCALAR >::addFactor(
     const std::vector< std::string >& varnames) {
    NodeSet vars;
    for (const auto name: varnames) {
      vars.insert(idFromName(name));
    }

    if (__factors.exists(vars)) {
      GUM_ERROR(InvalidArgument,
                "A factor for (" << varnames << ") already exists.")
    }

    return addFactor(vars);
  }

  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >&
               MarkovNet< GUM_SCALAR >::addFactor(const Potential< GUM_SCALAR >& factor) {
    if (factor.nbrDim() == 0) {
      GUM_ERROR(InvalidArgument, "Empty factor cannot be added.");
    }

    NodeSet key;
    for (Idx i = 0; i < factor.nbrDim(); i++) {
      key.insert(idFromName(factor.variable(i).name()));
    }

    if (__factors.exists(key)) {
      GUM_ERROR(InvalidArgument, "A factor for (" << key << ") already exists.");
    }

    auto res = __addFactor(key, &factor);
    __rebuildGraph();

    return *res;
  }

  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >*
               MarkovNet< GUM_SCALAR >::__addFactor(const NodeSet&                 vars,
                                          const Potential< GUM_SCALAR >* src) {
    Potential< GUM_SCALAR >* factor = new Potential< GUM_SCALAR >();
    for (auto node: vars) {
      factor->add(variable(node));
    }
    if (src != nullptr) { factor->fillWith(*src); }
    __factors.insert(vars, factor);

    return factor;
  }


  template < typename GUM_SCALAR >
  INLINE void MarkovNet< GUM_SCALAR >::generateFactors() const {
    for (const auto& elt: __factors) {
      elt.second->random();
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MarkovNet< GUM_SCALAR >::generateFactor(const NodeSet& vars) const {
    __factors[vars]->random();
  }

  template < typename GUM_SCALAR >
  INLINE void MarkovNet< GUM_SCALAR >::eraseFactor(const NodeSet& vars) {
    if (__factors.exists(vars)) {
      __eraseFactor(vars);
      __rebuildGraph();
    } else {
      GUM_ERROR(InvalidArgument, "No factor for " << vars << ".")
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MarkovNet< GUM_SCALAR >::eraseFactor(
     const std::vector< std::string >& varnames) {
    NodeSet vars;
    for (const auto name: varnames) {
      vars.insert(idFromName(name));
    }
    if (__factors.exists(vars)) {
      __eraseFactor(vars);
      __rebuildGraph();
    } else {
      GUM_ERROR(InvalidArgument, "No factor for " << varnames << ".")
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MarkovNet< GUM_SCALAR >::__eraseFactor(const NodeSet& vars) {
    delete __factors[vars];
    __factors.erase(vars);
  }

  template < typename GUM_SCALAR >
  void MarkovNet< GUM_SCALAR >::__clearFactors() {
    for (const auto& kv: __factors) {
      delete kv.second;
    }
    __factors.clear();
    __rebuildGraph();
  }

  template < typename GUM_SCALAR >
  void MarkovNet< GUM_SCALAR >::__copyFactors(
     const MarkovNet< GUM_SCALAR >& source) {
    __clearFactors();
    for (const auto& pf: source.factors()) {
      __addFactor(pf.first, pf.second);
    }
    __rebuildGraph();
  }

  template < typename GUM_SCALAR >
  INLINE void MarkovNet< GUM_SCALAR >::beginTopologyTransformation() {
    __topologyTransformationInProgress = true;
  }
  template < typename GUM_SCALAR >
  INLINE void MarkovNet< GUM_SCALAR >::endTopologyTransformation() {
    if (__topologyTransformationInProgress) {
      __topologyTransformationInProgress =
         false;   // before rebuildGraph of course
      __rebuildGraph();
    }
  }
} /* namespace gum */
