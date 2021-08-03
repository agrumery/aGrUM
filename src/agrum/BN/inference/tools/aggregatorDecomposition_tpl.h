/**
 *
 *   Copyright 2005-2019 Pierre-Henri WUILLEMIN & Christophe GONZALES(_at_AMU)
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
 * @brief Inline implementation of AggregatorDecomposition.
 *
 * @author Gaspard Ducamp
 *
 */
#include <agrum/BN/inference/tools/aggregatorDecomposition.h>
#include <typeinfo>
#include <list>

namespace gum {

  template < typename GUM_SCALAR >
  INLINE AggregatorDecomposition< GUM_SCALAR >::AggregatorDecomposition() {
    _arity_ = 2;
    GUM_CONSTRUCTOR(AggregatorDecomposition);
  }

  template < typename GUM_SCALAR >
  AggregatorDecomposition< GUM_SCALAR >::~AggregatorDecomposition() {
    GUM_DESTRUCTOR(AggregatorDecomposition);
  }

  template < typename GUM_SCALAR >
  BayesNet< GUM_SCALAR >&
     AggregatorDecomposition< GUM_SCALAR >::getDecomposedAggregator(BayesNet< GUM_SCALAR >& bn) {
    for (NodeId node: bn.nodes().asNodeSet()) {
      std::string description = bn.cpt(node).toString();
      auto        p = dynamic_cast< const gum::aggregator::MultiDimAggregator< GUM_SCALAR >* >(
         bn.cpt(node).content());
      if (p != nullptr && p->isDecomposable()) { decomposeAggregator_(bn, node); }
    }
    return bn;
  }

  template < typename GUM_SCALAR >
  NodeId AggregatorDecomposition< GUM_SCALAR >::addAggregator_(BayesNet< GUM_SCALAR >& bn,
                                                               std::string             aggType,
                                                               const DiscreteVariable& var,
                                                               Idx                     value) {
    if (toLower(aggType) == "min") {
      return bn.addMIN(var);
    } else if (toLower(aggType) == "max") {
      return bn.addMAX(var);
    } else if (toLower(aggType) == "count") {
      return bn.addCOUNT(var, value);
    } else if (toLower(aggType) == "exists") {
      return bn.addEXISTS(var, value);
    } else if (toLower(aggType) == "or") {
      return bn.addOR(var);
    } else if (toLower(aggType) == "and") {
      return bn.addAND(var);
    } else if (toLower(aggType) == "forall") {
      return bn.addFORALL(var);
    } else if (toLower(aggType) == "amplitude") {
      return bn.addAMPLITUDE(var);
    } else if (toLower(aggType) == "median") {
      return bn.addMEDIAN(var);
    } else if (toLower(aggType) == "sum") {
      return bn.addSUM(var);
    } else {
      std::string msg = "Unknown aggregate: ";
      msg.append(aggType);
      GUM_ERROR(NotFound, msg)
    }
  }

  template < typename GUM_SCALAR >
  BayesNet< GUM_SCALAR >&
     AggregatorDecomposition< GUM_SCALAR >::decomposeAggregator_(BayesNet< GUM_SCALAR >& bn,
                                                                 NodeId initialAggregator) {
    auto p = static_cast< const gum::aggregator::MultiDimAggregator< GUM_SCALAR >* >(
       bn.cpt(initialAggregator).content());
    auto newAgg = bn.variable(initialAggregator).clone();

    Set< NodeId > parents = bn.parents(initialAggregator);

    std::list< NodeId > orderedParents = {};

    for (const auto& elt: parents) {
      orderedParents.push_back(elt);
    }

    orderedParents.sort();

    Set< NodeId >  newAggs = Set< NodeId >();
    List< NodeId > newAggParents;

    gum::Size arity = getMaximumArity();
    gum::Size q     = 0;
    gum::Size i     = 0;

    long minVal = 0;
    long maxVal = 0;

    int j = 1;

    std::string newName
       = std::string(bn.variable(initialAggregator).name()) + "_" + std::to_string(j);
    std::string aggType = p->aggregatorName();

    for (auto parent: parents) {
      bn.eraseArc(parent, initialAggregator);
    }

    /*
     * We are constructing the new aggregator with a clone of the former
     */
    newAgg->setName(newName);
    newAgg->setDescription(aggType);

    // for(Set<NodeId>::iterator it = parents.begin(); it!= parents.end(); ++it){
    for (auto it = orderedParents.begin(); it != orderedParents.end(); ++it) {
      if (q < parents.size() - parents.size() % arity) {
        if (i == arity) {
          i = 0;
          j++;

          if (newAgg->varType() == VarType::Labelized) {
            addAggregator_(bn, aggType, *newAgg, p->domainSize());
          } else if (newAgg->varType() == VarType::Range) {
            static_cast< RangeVariable* >(newAgg)->setMinVal(minVal);
            static_cast< RangeVariable* >(newAgg)->setMaxVal(maxVal);
            addAggregator_(bn, aggType, *newAgg, 0);
          } else {
            GUM_ERROR(OperationNotAllowed, "Decomposition is not available for type : " + aggType)
          }

          /*
           * Adding arcs in the new node from its parents and adding thoses into
           * the temporary potential
           */
          for (NodeId node: newAggParents) {
            bn.addArc(node, bn.idFromName(newName));
          }

          /*
           * Adding the new aggregator in t
           */
          newAggs.insert(bn.idFromName(newName));

          newAggParents.clear();

          minVal = 0;
          maxVal = 0;

          newName = std::string(bn.variable(initialAggregator).name()) + "_" + std::to_string(j);

          delete (newAgg);
          newAgg = bn.variable(initialAggregator).clone();
          newAgg->setName(newName);
          newAgg->setDescription(aggType);

          if (bn.variable(*it).varType() == VarType::Range) {
            minVal += static_cast< const RangeVariable& >(bn.variable(*it)).minVal();
            maxVal += static_cast< const RangeVariable& >(bn.variable(*it)).maxVal();
          }

          newAggParents.push_back(*it);
          i++;
        } else {
          if (bn.variable(*it).varType() == VarType::Range) {
            minVal += static_cast< const RangeVariable& >(bn.variable(*it)).minVal();
            maxVal += static_cast< const RangeVariable& >(bn.variable(*it)).maxVal();
          }

          newAggParents.push_back(*it);
          i++;
        }
      } else {
        newAggs.insert(*it);
      }
      q++;
    }

    if (newAgg->varType() == VarType::Labelized) {
      addAggregator_(bn, aggType, *newAgg, p->domainSize());
    } else if (newAgg->varType() == VarType::Range) {
      static_cast< RangeVariable* >(newAgg)->setMinVal(minVal);
      static_cast< RangeVariable* >(newAgg)->setMaxVal(maxVal);
      addAggregator_(bn, aggType, *newAgg, 0);
    } else {
      GUM_ERROR(OperationNotAllowed, "Decomposition is not available for type : " + aggType)
    }

    newAggs.insert(bn.idFromName(newName));

    for (NodeId node: newAggParents) {
      bn.addArc(node, bn.idFromName(newName));
    }

    Set< NodeId > final = addDepthLayer_(bn, newAggs, initialAggregator, j);

    for (auto agg: final) {
      bn.addArc(agg, initialAggregator);
    }

    delete (newAgg);
    return bn;
  }

  template < typename GUM_SCALAR >
  Set< NodeId > AggregatorDecomposition< GUM_SCALAR >::addDepthLayer_(BayesNet< GUM_SCALAR >& bn,
                                                                      Set< NodeId >           nodes,
                                                                      NodeId initialAggregator,
                                                                      int&   j) {
    auto p = static_cast< const gum::aggregator::MultiDimAggregator< GUM_SCALAR >* >(
       bn.cpt(initialAggregator).content());

    gum::Size   arity   = getMaximumArity();
    std::string aggType = p->aggregatorName();

    if (nodes.size() <= arity) {
      return nodes;
    } else {
      auto newAgg = bn.variable(initialAggregator).clone();

      Set< NodeId > newAggs = Set< NodeId >();

      List< NodeId > newAggParents;

      std::list< NodeId > orderedParents = {};

      for (const auto& elt: nodes) {
        orderedParents.push_back(elt);
      }

      orderedParents.sort();

      gum::Size i      = 0;
      gum::Size q      = 0;
      long      minVal = 0;
      long      maxVal = 0;

      j++;

      std::string newName
         = std::string(bn.variable(initialAggregator).name()) + "_" + std::to_string(j);

      newAgg->setName(newName);
      newAgg->setDescription(aggType);

      // for(Set<NodeId>::iterator it = nodes.begin(); it!= nodes.end(); ++it){
      for (auto it = orderedParents.begin(); it != orderedParents.end(); ++it) {
        if (q < nodes.size() - nodes.size() % arity) {
          if (i == arity) {
            i = 0;
            j++;

            if (newAgg->varType() == VarType::Labelized) {
              addAggregator_(bn, aggType, *newAgg, p->domainSize());
            } else if (newAgg->varType() == VarType::Range) {
              static_cast< RangeVariable* >(newAgg)->setMinVal(minVal);
              static_cast< RangeVariable* >(newAgg)->setMaxVal(maxVal);
              addAggregator_(bn, aggType, *newAgg, 0);
            } else {
              GUM_ERROR(OperationNotAllowed, "Decomposition is not available for type : " + aggType)
            }

            for (NodeId node: newAggParents) {
              bn.addArc(node, bn.idFromName(newName));
            }

            newAggs.insert(bn.idFromName(newName));

            newAggParents.clear();

            minVal = 0;
            maxVal = 0;

            newName = std::string(bn.variable(initialAggregator).name()) + "_" + std::to_string(j);

            delete (newAgg);
            newAgg = bn.variable(initialAggregator).clone();
            newAgg->setName(newName);
            newAgg->setDescription(aggType);

            if (bn.variable(*it).varType() == VarType::Range) {
              minVal += static_cast< const RangeVariable& >(bn.variable(*it)).minVal();
              maxVal += static_cast< const RangeVariable& >(bn.variable(*it)).maxVal();
            }

            newAggParents.push_back(*it);
            i++;
          } else {
            if (bn.variable(*it).varType() == VarType::Range) {
              minVal += static_cast< const RangeVariable& >(bn.variable(*it)).minVal();
              maxVal += static_cast< const RangeVariable& >(bn.variable(*it)).maxVal();
            }

            newAggParents.push_back(*it);
            i++;
          }
        } else {
          newAggs.insert(*it);
        }
        q++;
      }

      if (newAgg->varType() == VarType::Labelized) {
        addAggregator_(bn, aggType, *newAgg, p->domainSize());
      } else if (newAgg->varType() == VarType::Range) {
        static_cast< RangeVariable* >(newAgg)->setMinVal(minVal);
        static_cast< RangeVariable* >(newAgg)->setMaxVal(maxVal);
        addAggregator_(bn, aggType, *newAgg, 0);
      } else {
        GUM_ERROR(OperationNotAllowed, "Decomposition is not available for type : " + aggType)
      }

      newAggs.insert(bn.idFromName(newName));

      for (NodeId node: newAggParents) {
        bn.addArc(node, bn.idFromName(newName));
      }

      delete (newAgg);
      return addDepthLayer_(bn, newAggs, initialAggregator, j);
    }
  }


  template < typename GUM_SCALAR >
  INLINE void AggregatorDecomposition< GUM_SCALAR >::setMaximumArity(gum::Size arity) {
    if (arity < 2) { GUM_ERROR(OperationNotAllowed, "Maximum arity should be at least 2") }
    _arity_ = arity;
  }

  template < typename GUM_SCALAR >
  gum::Size AggregatorDecomposition< GUM_SCALAR >::getMaximumArity() {
    return _arity_;
  }

  template < typename GUM_SCALAR >
  INLINE std::string AggregatorDecomposition< GUM_SCALAR >::name() const {
    return "aggregator decomposition";
  }

} /* namespace gum */
