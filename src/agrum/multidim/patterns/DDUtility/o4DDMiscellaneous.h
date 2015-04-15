/*********************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr *
 *                                                                                                                      *
 *   This program is free software; you can redistribute it and/or modify           *
 *   it under the terms of the GNU General Public License as published by       *
 *   the Free Software Foundation; either version 2 of the License, or              *
 *   (at your option) any later version. *
 *                                                                                                                      *
 *   This program is distributed in the hope that it will be useful, *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details. *
 *                                                                                                                      *
 *   You should have received a copy of the GNU General Public License          *
 *   along with this program; if not, write to the *
 *   Free Software Foundation, Inc., *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                      *
 ********************************************************************************/
/**
* @file
* @brief Files containing structures and functions used to manipulate data structures
*in Decision Diagram Operations
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

#ifndef GUM_DECISION_DIAGRAM_OPERATOR_DATA
#define GUM_DECISION_DIAGRAM_OPERATOR_DATA

#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>

#include <agrum/variables/discreteVariable.h>
#include <agrum/multidim/multiDimDecisionDiagramBase.h>
#include <agrum/multidim/multiDimDecisionDiagramFactoryBase.h>
#include <agrum/multidim/patterns/DDUtility/o4DDContext.h>

namespace gum {

  //========================================================================================================================================
  /// OperatorData contains all data structure needed to compute the outcoming
  /// diagram.
  //========================================================================================================================================
  template <typename T> class OperatorData {

    public:
    /// The factory that build up resulting diagram
    MultiDimDecisionDiagramBase<T> *DD1;

    /// The factory that build up resulting diagram
    MultiDimDecisionDiagramBase<T> *DD2;

    /// The factory that build up resulting diagram
    MultiDimDecisionDiagramFactoryBase<T> *factory;

    /// Context handler
    O4DDContext conti;

    /// Table to remind us wich part of both diagram have already been explored, and
    /// the resulting node
    HashTable<double, NodeId> *explorationTable;

    /**
      ***************************************************************************************************
      ** Default constructor
      **
      ***********************************************************************************************
      ***/
    OperatorData(){};

    /**
      ***************************************************************************************************
      ** Initializes the data structure with operated DD1 et DD2, and compute order
      *for returned diagram
      **
      ***********************************************************************************************
      ***/
    virtual void initialize(const MultiDimDecisionDiagramBase<T> *dD1,
                            const MultiDimDecisionDiagramBase<T> *dD2) {

      // First, we determine leading order and the sens in which we perform operation
      // on diagrams
      Sequence<const DiscreteVariable *> leadingOrder;
      __getLeadingSequence(dD1, dD2, leadingOrder);

      // Next, we initialize node for exploration departure
      conti.setDD1Node(DD1->root());
      conti.setDD2Node(DD2->root());

#ifdef O4DDDEBUG
      GUM_TRACE(" Mixed Sequence variable : ");

      for (cosnt auto var : leadingOrder)
        GUM_TRACE(var->toString() << " - ");

      GUM_TRACE(std::endl);
#endif

      // Then we instantiate the factory that will create the new multidim
      // give it its new bounds, and set its variable sequence
      factory = DD1->getFactory();
      factory->GUM_APPROXIMATION_COMBINE_FUNCTION(DD2);
      factory->setVariablesSequence(leadingOrder);

      // And we finally instantiates the other data structures
      explorationTable = new HashTable<double, NodeId>(
          dD1->realSize() * dD2->realSize(), true, false);
    };

    /**
      ***************************************************************************************************
      ** Default destructor
      **
      ***********************************************************************************************
      ***/
    ~OperatorData() {
      delete factory;
      delete explorationTable;
    };

    protected:
    virtual void
    __getLeadingSequence(const MultiDimDecisionDiagramBase<T> *dD1,
                         const MultiDimDecisionDiagramBase<T> *dD2,
                         gum::Sequence<const DiscreteVariable *> &leadingOrder) = 0;
  };

  //========================================================================================================================================
  /// OperatorData contains all is needed to perform an operation beteween seamingly
  /// ordered decision diagram.
  //========================================================================================================================================
  template <typename T> class OrderedOperatorData : public OperatorData<T> {

    public:
    /**
      ***************************************************************************************************
      ** Default constructor
      **
      ***********************************************************************************************
      ***/
    OrderedOperatorData() : OperatorData<T>(){};

    protected:
    /**
     ***************************************************************************************************
     ** The very implementation of the function that gave order for final diagram
     **
     ***********************************************************************************************
     ***/
    void __getLeadingSequence(const MultiDimDecisionDiagramBase<T> *dD1,
                              const MultiDimDecisionDiagramBase<T> *dD2,
                              Sequence<const DiscreteVariable *> &leadingOrder) {
      this->DD1 = const_cast<MultiDimDecisionDiagramBase<T> *>(dD1);
      this->DD2 = const_cast<MultiDimDecisionDiagramBase<T> *>(dD2);
      leadingOrder = this->DD1->variablesSequence();
    };
  };

  //========================================================================================================================================
  /// OperatorData contains all is needed to perform an operation beteween non
  /// seamingly ordered decision diagram.
  //========================================================================================================================================
  template <typename T> class NonOrderedOperatorData : public OperatorData<T> {

    public:
    /// Table giving for each node of second diagram, the list of variables requiring
    /// instantiation before
    HashTable<NodeId, Set<const DiscreteVariable *> *> *retrogradeVarTable;

    /**
      ***************************************************************************************************
      ** Default constructor
      **
      ***********************************************************************************************
      ***/
    NonOrderedOperatorData() : OperatorData<T>(){};

    virtual void initialize(const MultiDimDecisionDiagramBase<T> *dD1,
                            const MultiDimDecisionDiagramBase<T> *dD2) {

      OperatorData<T>::initialize(dD1, dD2);

      retrogradeVarTable = new HashTable<NodeId, Set<const DiscreteVariable *> *>(
          this->DD2->realSize(), true, false);
      this->DD2->findRetrogradeVariables(&(this->factory->variablesSequence()),
                                         retrogradeVarTable);

      for (const auto &elt : *retrogradeVarTable)
        if (elt.second != nullptr && !elt.second->empty())
          for (const auto var : *elt.second)
            if (!this->conti.isRetrogradeVar(var))
              this->conti.addRetrogradeVar(var);
    }

    ~NonOrderedOperatorData() {
      for (const auto &elt : *retrogradeVarTable)
        delete elt.second;

      delete retrogradeVarTable;
    };

    protected:
    /**
     ***************************************************************************************************
     ** The very implementation of the function that gave order for final diagram
     **
     ***********************************************************************************************
     ***/
    virtual void
    __getLeadingSequence(const MultiDimDecisionDiagramBase<T> *dD1,
                         const MultiDimDecisionDiagramBase<T> *dD2,
                         gum::Sequence<const DiscreteVariable *> &leadingOrder) {

      // #ifdef GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION ==
      // DecisionDiagramRecur4Subtraction ||
      // GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION ==
      // DecisionDiagramRecur4Division
      //         __makeMergedVariableSequence( dD1->variablesSequence(),
      //         dD2->variablesSequence(), fusVarSeq );
      //         leader = dD1;
      //         follower = dD2;
      // #else
      Sequence<const DiscreteVariable *> mergedVarSeq1;
      __makeMergedVariableSequence(dD1->variablesSequence(),
                                   dD2->variablesSequence(), mergedVarSeq1);
      Sequence<const DiscreteVariable *> mergedVarSeq2;
      __makeMergedVariableSequence(dD2->variablesSequence(),
                                   dD1->variablesSequence(), mergedVarSeq2);

      if (__evalRetrogradeVarSpaceSize(mergedVarSeq1, dD2->variablesSequence()) <=
          __evalRetrogradeVarSpaceSize(mergedVarSeq2, dD1->variablesSequence())) {
        leadingOrder = mergedVarSeq1;
        this->DD1 = const_cast<MultiDimDecisionDiagramBase<T> *>(dD1);
        this->DD2 = const_cast<MultiDimDecisionDiagramBase<T> *>(dD2);
      } else {
        leadingOrder = mergedVarSeq2;
        this->DD1 = const_cast<MultiDimDecisionDiagramBase<T> *>(dD2);
        this->DD2 = const_cast<MultiDimDecisionDiagramBase<T> *>(dD1);
      }

      // #endif
    };

    private:
    /**
     ***************************************************************************************************
     ** Creates the sequence of variable for returned diagram considering the first
     *variables sequence
     ** given in parameter is the leading one.
     **
     ***********************************************************************************************
     ***/
    void
    __makeMergedVariableSequence(const Sequence<const DiscreteVariable *> &dD1VarSeq,
                                 const Sequence<const DiscreteVariable *> &dD2VarSeq,
                                 Sequence<const DiscreteVariable *> &mergedVarSeq) {

      auto iterS1 = dD1VarSeq.begin();
      auto iterS2 = dD2VarSeq.begin();

      while (iterS1 != dD1VarSeq.end() || iterS2 != dD2VarSeq.end()) {
        if (iterS1 == dD1VarSeq.end()) {
          for (; iterS2 != dD2VarSeq.end(); ++iterS2)
            if (!mergedVarSeq.exists(*iterS2))
              mergedVarSeq.insert(*iterS2);
        } else if (iterS2 == dD2VarSeq.end()) {
          for (; iterS1 != dD1VarSeq.end(); ++iterS1)
            if (!mergedVarSeq.exists(*iterS1))
              mergedVarSeq.insert(*iterS1);
        } else {
          if (*iterS1 == *iterS2) {
            if (!mergedVarSeq.exists(*iterS1))
              mergedVarSeq.insert(*iterS1);

            ++iterS1;
            ++iterS2;
            continue;
          }

          if (dD1VarSeq.pos(*iterS1) <= dD2VarSeq.pos(*iterS2) ||
              dD1VarSeq.exists(*iterS2)) {
            if (!mergedVarSeq.exists(*iterS1))
              mergedVarSeq.insert(*iterS1);

            ++iterS1;
            continue;
          } else {
            if (!mergedVarSeq.exists(*iterS2))
              mergedVarSeq.insert(*iterS2);

            ++iterS2;
            continue;
          }
        }
      }
    };

    /**
      ***************************************************************************************************
      ** Computes size of retrograde var state space generated by the final sequence
      **
      ***********************************************************************************************
      ***/
    Idx __evalRetrogradeVarSpaceSize(
        const Sequence<const DiscreteVariable *> &leadingVarSeq,
        const Sequence<const DiscreteVariable *> &followingVarSeq) {

      auto iterSfin = followingVarSeq.begin();
      auto iterSfol = followingVarSeq.begin();

      // ******************************************************************************
      // Then we search in second diagram for possible preneeded variable
      Idx sizeRetro = 1;

      for (iterSfol = followingVarSeq.begin(); iterSfol != followingVarSeq.end();
           ++iterSfol)
        for (iterSfin = iterSfol; iterSfin != followingVarSeq.rend(); --iterSfin)
          if (leadingVarSeq.pos(*iterSfin) > leadingVarSeq.pos(*iterSfol))
            sizeRetro *= (*iterSfol)->domainSize();

      return sizeRetro;
    };
  };

  /// Insertion of a node when we know all its sons and its possible default son

  template <typename T>
  NodeId insertNonTerminalNode(OperatorData<T> &opData,
                               const DiscreteVariable *associatedVariable,
                               std::vector<NodeId> &sonsMap, NodeId defaultSon,
                               const HashTable<NodeId, Idx> &countTable) {

    auto ctIter = countTable.begin();

    if (countTable.size() == 1 && (defaultSon == 0 || ctIter.key() == defaultSon))
      return ctIter.key();

    if (associatedVariable->domainSize() < 5) {

      //********************************************************************************************************************************************
      if (defaultSon != 0) {
        Idx nbDefault = 0;

        for (auto iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end();
             ++iterArcMap) {
          if (*iterArcMap == 0)
            ++nbDefault;

          if (*iterArcMap == defaultSon) {
            ++nbDefault;
            sonsMap[std::distance(sonsMap.begin(), iterArcMap)] = 0;
          }
        }

        if (nbDefault == 1)
          for (auto iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end();
               ++iterArcMap)
            if (*iterArcMap == 0) {
              sonsMap[std::distance(sonsMap.begin(), iterArcMap)] = defaultSon;
              defaultSon = 0;
              break;
            }
      }

      //********************************************************************************************************************************************
    } else {

      Idx max = 0;
      NodeId maxNode = 0;

      while (ctIter != countTable.end()) {
        if (ctIter.val() > max) {
          max = ctIter.val();
          maxNode = ctIter.key();
        }

        ++ctIter;
      }

      if (max == 1) {
        if (defaultSon != 0) {
          for (std::vector<NodeId>::iterator iterArcMap = sonsMap.begin();
               iterArcMap != sonsMap.end(); ++iterArcMap)
            if (*iterArcMap == 0) {
              sonsMap[std::distance(sonsMap.begin(), iterArcMap)] = defaultSon;
              defaultSon = 0;
              break;
            }
        }
      } else {
        if (maxNode != defaultSon) {
          for (std::vector<NodeId>::iterator iterArcMap = sonsMap.begin();
               iterArcMap != sonsMap.end(); ++iterArcMap) {
            if (*iterArcMap == 0)
              sonsMap[std::distance(sonsMap.begin(), iterArcMap)] = defaultSon;

            if (*iterArcMap == maxNode)
              sonsMap[std::distance(sonsMap.begin(), iterArcMap)] = 0;
          }

          defaultSon = maxNode;
        } else {
          for (std::vector<NodeId>::iterator iterArcMap = sonsMap.begin();
               iterArcMap != sonsMap.end(); ++iterArcMap)
            if (*iterArcMap == maxNode)
              sonsMap[std::distance(sonsMap.begin(), iterArcMap)] = 0;
        }
      }
    }

#ifdef O4DDDEBUG
    return opData.factory->addNonTerminalNodeWithArcs(associatedVariable, sonsMap,
                                                      defaultSon);
#else
    return opData.factory->unsafeAddNonTerminalNodeWithArcs(associatedVariable,
                                                            sonsMap, defaultSon);
#endif
  }

} /* end of namespace GUM */

#endif /*GUM_DECISION_DIAGRAM_OPERATOR_DATA*/
