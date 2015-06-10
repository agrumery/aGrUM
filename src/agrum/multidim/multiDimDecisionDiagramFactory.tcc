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
* @brief Template implementation of MultiDimDecisionDiagramFactory.h class.
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

#include <agrum/multidim/multiDimDecisionDiagramFactory.h>

/* **********************************************************************************************/
/*                                                                                                                                            */
/*                              Constructors, Destructors */
/*                                                                                                                                            */
/* **********************************************************************************************/
namespace gum {

  // Default constructor.

  template <typename GUM_SCALAR, template <class> class IApproximationPolicy>
  MultiDimDecisionDiagramFactory<
      GUM_SCALAR, IApproximationPolicy>::MultiDimDecisionDiagramFactory() {
    GUM_CONSTRUCTOR(MultiDimDecisionDiagramFactory);
  }

  // clone constructor.

  template <typename GUM_SCALAR, template <class> class IApproximationPolicy>
  MultiDimDecisionDiagramFactory<GUM_SCALAR, IApproximationPolicy>::
      MultiDimDecisionDiagramFactory(const IApproximationPolicy<GUM_SCALAR> &md)
      : MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>(),
        IApproximationPolicy<GUM_SCALAR>(md) {
    GUM_CONSTRUCTOR(MultiDimDecisionDiagramFactory);
  }

  // Destructor.
  // @warnings : this will not destroy properties on node. They ahve to be removed on
  // multidim destruction

  template <typename GUM_SCALAR, template <class> class IApproximationPolicy>
  MultiDimDecisionDiagramFactory<
      GUM_SCALAR, IApproximationPolicy>::~MultiDimDecisionDiagramFactory() {
    GUM_DESTRUCTOR(MultiDimDecisionDiagramFactory);
  }

  /* **********************************************************************************************/
  /*                                                                                                                                            */
  /*                              Graph Manipulation methods */
  /*                                                                                                                                            */
  /* **********************************************************************************************/

  // Returns the multidimDecisionDiagram made

  template <typename GUM_SCALAR, template <class> class IApproximationPolicy>
  MultiDimDecisionDiagramBase<GUM_SCALAR> *MultiDimDecisionDiagramFactory<
      GUM_SCALAR,
      IApproximationPolicy>::getMultiDimDecisionDiagram(bool fillWithDefaultArc,
                                                        GUM_SCALAR defaultValue,
                                                        bool doCompress) {

    // **************************************************************************************************************
    // First of all we ensure there's no unused value in our value map
    Bijection<NodeId, GUM_SCALAR> newValueMap;

    if (this->_model.size() == 2 && !this->_valueMap.empty())
      newValueMap = this->_valueMap;
    else
      for (BijectionIteratorSafe<NodeId, GUM_SCALAR> valueIter =
               this->_valueMap.beginSafe();
           valueIter != this->_valueMap.endSafe(); ++valueIter)
        if (this->_model.parents(valueIter.first()).empty() &&
            valueIter.first() != this->_rootId)
          this->_model.eraseNode(valueIter.first());
        else
          newValueMap.insert(valueIter.first(), valueIter.second());

    // **************************************************************************************************************
    // Then if required by caller, we fill diagram with default arcs where needed
    if (fillWithDefaultArc) {
      NodeId zeroId = 0;
      bool zeroNotCreated = true;

      for (auto iter = this->_model.nodes().beginSafe();
           iter != this->_model.nodes().endSafe(); ++iter) {
        if (*iter != 0 && !newValueMap.existsFirst(*iter)) {
          Idx idxDefault = 0;
          Idx nbDefault = 0;

          for (auto sonIter = this->_arcMap[*iter]->begin();
               sonIter != this->_arcMap[*iter]->end(); ++sonIter)
            if (*sonIter == 0) {
              idxDefault = std::distance(this->_arcMap[*iter]->begin(), sonIter);
              nbDefault++;
            }

          if (nbDefault >= 1 && zeroNotCreated) {
            zeroId = this->addTerminalNode(defaultValue);

            if (!newValueMap.existsSecond(defaultValue))
              newValueMap.insert(zeroId, defaultValue);

            zeroNotCreated = false;
          }

          if (nbDefault == 1) {
            (*(this->_arcMap[*iter]))[idxDefault] = zeroId;
            (*(this->_varUsedModalitiesMap[this->_varMap[*iter]]))[idxDefault]++;

            if (this->_defaultArcMap.exists(*iter))
              this->_defaultArcMap.erase(*iter);

            continue;
          }

          if (nbDefault > 1 && !this->_defaultArcMap.exists(*iter)) {
            this->_defaultArcMap.insert(*iter, zeroId);
          }
        }
      }
    }

    if (this->_noVariableCheckMode)
      this->_varsSeq = this->_findVariableOrder();

    if (doCompress) {

      // ************************************************************************************************
      // First we remove any redundant nodes
      // ************************************************************************************************
      for (SequenceIteratorSafe<const DiscreteVariable *> varIter =
               this->_varsSeq.rbeginSafe();
           varIter != this->_varsSeq.rendSafe(); --varIter) {

        if (this->_var2NodeIdMap.exists(*varIter))
          for (ListConstIteratorSafe<NodeId> riterNodeList =
                   this->_var2NodeIdMap[*varIter]->rbeginSafe();
               riterNodeList != this->_var2NodeIdMap[*varIter]->rendSafe();
               --riterNodeList) {

            bool hasDoublon = false;
            NodeId doublon = 0;

            for (ListConstIteratorSafe<NodeId> iterNodeList =
                     this->_var2NodeIdMap[*varIter]->beginSafe();
                 iterNodeList != riterNodeList; ++iterNodeList) {
              bool thesame = true;

              if ((!this->_defaultArcMap.exists(*riterNodeList) &&
                   this->_defaultArcMap.exists(*iterNodeList)) ||
                  (this->_defaultArcMap.exists(*riterNodeList) &&
                   !this->_defaultArcMap.exists(*iterNodeList)))
                thesame = false;
              else if (this->_defaultArcMap.exists(*riterNodeList) &&
                       this->_defaultArcMap[*iterNodeList] !=
                           this->_defaultArcMap[*riterNodeList])
                thesame = false;

              if (thesame)
                for (Idx i = 0; i < this->_arcMap[*riterNodeList]->size(); i++)
                  if ((*this->_arcMap[*riterNodeList])[i] !=
                          (*this->_arcMap[*iterNodeList])[i] ||
                      ((*this->_arcMap[*riterNodeList])[i] == 0 &&
                       this->_defaultArcMap[*riterNodeList] ==
                           (*this->_arcMap[*iterNodeList])[i]) ||
                      ((*this->_arcMap[*iterNodeList])[i] == 0 &&
                       (*this->_arcMap[*riterNodeList])[i] ==
                           this->_defaultArcMap[*iterNodeList])) {
                    thesame = false;
                    break;
                  }

              if (thesame) {
                hasDoublon = true;
                doublon = *iterNodeList;
                break;
              }
            }

            if (hasDoublon) {
              __mergedNode(*riterNodeList, doublon);
              continue;
            }

            bool sameArc = true;
            NodeId nody = 0;
            std::vector<NodeId>::const_iterator arcIter =
                this->_arcMap[*riterNodeList]->begin();

            if (this->_defaultArcMap.exists(*riterNodeList))
              nody = this->_defaultArcMap[*riterNodeList];
            else if (arcIter != this->_arcMap[*riterNodeList]->end()) {
              nody = *arcIter;
              ++arcIter;
            }

            for (; arcIter != this->_arcMap[*riterNodeList]->end(); ++arcIter)
              if (*arcIter != 0 && *arcIter != nody) {
                sameArc = false;
                break;
              }

            if (sameArc)
              __mergedNode(*riterNodeList, nody);
          }
      }

      // ************************************************************************************************
      //  Next we create or displace default arc where needed
      // ************************************************************************************************
      for (auto iter = this->_model.nodes().beginSafe();
           iter != this->_model.nodes().endSafe(); ++iter) {
        if (*iter != 0 && !newValueMap.existsFirst(*iter)) {

          Idx nbDefault = 0;
          NodeId defaultSon = 0;

          if (this->_defaultArcMap.exists(*iter))
            defaultSon = this->_defaultArcMap[*iter];

          HashTable<NodeId, Idx> nodeCount(this->_arcMap[*iter]->size(), false,
                                           false);

          Idx dist = 0;

          for (const auto iterArcMap : *(this->_arcMap[*iter])) {
            if (iterArcMap == 0)
              ++nbDefault;

            if (iterArcMap == defaultSon) {
              ++nbDefault;
              (*(this->_arcMap[*iter]))[dist] = 0; // std::distance (
                                                   // this->_arcMap[iter]->begin(),
                                                   // iterArcMap ) ] = 0;
              (*(this->_varUsedModalitiesMap[this->_varMap[*iter]]))
                  [dist]--; //  std::distance ( this->_arcMap[iter]->begin(),
                            //  iterArcMap ) ]--;
            }

            try {
              nodeCount[iterArcMap]++;
            } catch (gum::NotFound n) {
              nodeCount.insert(iterArcMap, 1);
            }

            dist++;
          }

          NodeId maxNodeId = defaultSon;
          Idx maxCall = nbDefault;

          for (HashTableIteratorSafe<NodeId, Idx> hIter = nodeCount.beginSafe();
               hIter != nodeCount.endSafe(); ++hIter)
            if (hIter.val() > maxCall) {
              maxCall = hIter.val();
              maxNodeId = hIter.key();
            }

          if (maxCall == 1) {
            dist = 0;

            for (const auto iterArcMap : *(this->_arcMap[*iter])) {
              if (iterArcMap == 0) {
                (*(this->_arcMap[*iter]))[dist] =
                    defaultSon; // std::distance ( this->_arcMap[iter]->begin(),
                                // iterArcMap ) ] = defaultSon;

                if (this->_defaultArcMap.exists(*iter))
                  this->_defaultArcMap.erase(*iter);

                (*(this->_varUsedModalitiesMap[this->_varMap[*iter]]))
                    [dist]++; // std::distance ( this->_arcMap[iter]->begin(),
                              // iterArcMap ) ]++;
                break;
              }

              dist++;
            }
          } else {
            if (maxNodeId != defaultSon) {
              dist = 0;

              for (const auto iterArcMap : *(this->_arcMap[*iter])) {
                if (iterArcMap == 0) {
                  (*(this->_arcMap[*iter]))[dist] =
                      defaultSon; // std::distance ( this->_arcMap[iter]->begin(),
                                  // iterArcMap ) ] = defaultSon;
                  (*(this->_varUsedModalitiesMap[this->_varMap[*iter]]))
                      [dist]++; // std::distance ( this->_arcMap[iter]->begin(),
                                // iterArcMap ) ]++;
                } else if (iterArcMap == maxNodeId) {
                  (*(this->_arcMap[*iter]))[dist] =
                      0; // std::distance ( this->_arcMap[iter]->begin(), iterArcMap
                         // ) ] = 0;
                  (*(this->_varUsedModalitiesMap[this->_varMap[*iter]]))
                      [dist]--; // std::distance ( this->_arcMap[iter]->begin(),
                                // iterArcMap ) ]--;
                }

                dist++;
              }

              if (this->_defaultArcMap.exists(*iter))
                this->_defaultArcMap[*iter] = maxNodeId;
              else
                this->_defaultArcMap.insert(*iter, maxNodeId);
            }
          }
        }
      }
    }

    // **************************************************************************************************************

    // **************************************************************************************************************
    // And finally we do the instantiation of the MultiDim itself
    MultiDimDecisionDiagram<GUM_SCALAR, IApproximationPolicy> *ret =
        new MultiDimDecisionDiagram<GUM_SCALAR, IApproximationPolicy>(*this);

    ret->beginInstantiation();

    ret->setVariableSequence(this->_varsSeq);

    ret->setDiagramNodes(this->_model);

    ret->setVariableMap(this->_varMap);
    ret->setVar2NodeMap(this->_var2NodeIdMap);
    ret->setVarUsedModalitiesMap(this->_varUsedModalitiesMap);
    ret->setValueMap(newValueMap);
    ret->setDiagramArcs(this->_arcMap, this->_defaultArcMap);

    if (this->_rootId == 0) {
      for (auto iter = this->_model.nodes().beginSafe();
           iter != this->_model.nodes().endSafe(); ++iter) {
        if (newValueMap.existsFirst(*iter) && this->_model.size() == 2) {
          this->_rootId = *iter;
          break;
        }

        if (*iter != 0 && !newValueMap.existsFirst(*iter) &&
            !this->_model.children(*iter).empty() &&
            this->_model.parents(*iter).empty()) {
          this->_rootId = *iter;
          break;
        }
      }
    }

    ret->setRoot(this->_rootId);

    ret->endInstantiation();

    return ret;
  }

  template <typename GUM_SCALAR, template <class> class IApproximationPolicy>
  void
  MultiDimDecisionDiagramFactory<GUM_SCALAR, IApproximationPolicy>::__mergedNode(
      NodeId from, NodeId to) {

    const NodeSet &parents = this->_model.parents(from);

    for (auto parentIter = parents.beginSafe(); parentIter != parents.endSafe();
         ++parentIter) {
      for (std::vector<NodeId>::iterator iter = this->_arcMap[*parentIter]->begin();
           iter != this->_arcMap[*parentIter]->end(); ++iter)
        if (*iter == from) {
          Idx modality = std::distance(this->_arcMap[*parentIter]->begin(), iter);
          this->eraseSpecificArc(*parentIter, from, modality);
          this->addArc(*parentIter, to, modality);
        }

      if (this->_defaultArcMap.exists(*parentIter) &&
          this->_defaultArcMap[*parentIter] == from)
        this->_defaultArcMap[*parentIter] = to;
    }

    this->eraseNode(from);
  }
} // namespace gum
