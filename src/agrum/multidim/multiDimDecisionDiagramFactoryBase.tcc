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
* @brief Template implementation of MultiDimDecisionDiagramFactoryBase.h classe.
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

#include <string>
#include <iostream>
#include <cstdio>
#include <sstream>

#include <agrum/multidim/multiDimDecisionDiagramFactoryBase.h>

namespace gum {
  /* **********************************************************************************************/
  /*                                                                                                                                            */
  /*                         Constructors, Destructors */
  /*                                                                                                                                            */
  /* **********************************************************************************************/

  // Default constructor.

  template <typename GUM_SCALAR>
  MultiDimDecisionDiagramFactoryBase<
      GUM_SCALAR>::MultiDimDecisionDiagramFactoryBase()
      : _model(500, true, 1500, true), _varMap(500, true, false), _valueMap(125),
        _arcMap(500, true, false), _defaultArcMap(500, true, false) {

    GUM_CONSTRUCTOR(MultiDimDecisionDiagramFactoryBase);

    putOffNoVariableCheckMode();

    _model.addNode();

    _rootId = 0;
  }

  // Destructor.
  // @warnings : this will not destroy properties on node. They ahve to be removed on
  // multidim destruction

  template <typename GUM_SCALAR>
  MultiDimDecisionDiagramFactoryBase<
      GUM_SCALAR>::~MultiDimDecisionDiagramFactoryBase() {

    GUM_DESTRUCTOR(MultiDimDecisionDiagramFactoryBase);

    this->clear();
  }

  /* **********************************************************************************************/
  /*                                                                                                                                            */
  /*                        Graph Manipulation methods */
  /*                                                                                                                                            */
  /* **********************************************************************************************/

  // Returns the sequence of variables on which is based the diagram construction

  template <typename GUM_SCALAR>
  INLINE const Sequence<const DiscreteVariable *> &
  MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::variablesSequence() {

    return _varsSeq;
  }

  // Specifies the order between variable in the diagram
  // To be done before any insertion of node linked to variable if not in
  // NoVariableCheckMode.
  // @param a sequence containing the variable (wich will be the referent )

  template <typename GUM_SCALAR>
  void MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::setVariablesSequence(
      Sequence<const DiscreteVariable *> s) {

    _varsSeq = s;
    _var2NodeIdMap.resize(s.size());
    _varUsedModalitiesMap.resize(s.size());
  }

  /* **********************************************************************************************/
  /*                                                                                                                                            */
  /*                        Nodes Manipulation */
  /*                                                                                                                                            */
  /* **********************************************************************************************/

  // Sets root node of decision diagram

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::setRootNode(const NodeId nody) {
    _rootId = nody;
  }

  // Adds a non-terminal node in the diagram linked to given variable.
  // @throw OperationNotAllowed if no sequence of variable have yet been specified
  // Returns the id of that node

  template <typename GUM_SCALAR>
  NodeId MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::addNonTerminalNode(
      const DiscreteVariable *var) {

    // *******************************************************************************************
    // Verification

    if (_noVariableCheckMode) {
      if (!_varsSeq.exists(var))
        _varsSeq.insert(var);
    } else {
      // First, we check if variable order has been specified
      if (_varsSeq.size() == 0) {
        GUM_ERROR(OperationNotAllowed,
                  "You must first specify the order of variable");
      } else {
        // if so we check if var is in the order or not
        if (!_varsSeq.exists(var)) {
          GUM_ERROR(OperationNotAllowed,
                    "Variable " << var->name() << " is not in the specify order");
        }
      }
    }

    // *********************************************************************************************

    return unsafeAddNonTerminalNode(var);
  }

  // Adds a non-terminal node in the diagram linked to given variable.
  // Inserts also arc between that node and mentionned node in the given
  // hashtablenewValueMap.insert ( valueIter.first(), valueIter.second() );
  // @throw OperationNotAllowed if no sequence of variable have yet been specified
  // Returns the id of that node

  template <typename GUM_SCALAR>
  INLINE NodeId
      MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::addNonTerminalNodeWithArcs(
          const DiscreteVariable *var, const std::vector<NodeId> &nodeArcMap,
          NodeId defaultArcTo) {

    std::pair<bool, NodeId> check = checkredundancy(var, nodeArcMap, defaultArcTo);

    if (check.first)
      return check.second;

    // ***********************************************************************************
    // if we manage to reach this point, this mean we have to insert the node
    // with all his bunch of arc
    NodeId node = addNonTerminalNode(var);

    // GUM_TRACE( "insertion noeud pour Var : " << var->toString() << " - Id : " <<
    // node << " - Fils : " << nodeArcMap );
    // if( defaultArcTo != nullptr )
    // GUM_TRACE( " - Defaut : " << *defaultArcTo );
    // GUM_TRACE( std::endl << std::endl );

    for (std::vector<NodeId>::const_iterator iter = nodeArcMap.begin();
         iter != nodeArcMap.end(); ++iter)
      if (*iter != 0)
        addArc(node, *iter, std::distance(nodeArcMap.begin(), iter));

    if (defaultArcTo != 0)
      addDefaultArc(node, defaultArcTo);

    return node;
  }

  // Checks if a node with same variable, same sons and same default arc does not
  // already exists in diagram

  template <typename GUM_SCALAR>
  INLINE std::pair<bool, NodeId>
  MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::checkredundancy(
      const DiscreteVariable *var, const std::vector<NodeId> &nodeArcMap,
      NodeId defaultArcTo) {

    std::pair<bool, NodeId> res;
    res.first = false;
    // GUM_TRACE( "On m'appelle avec : " << var->name() << " - Fils : " <<
    // nodeArcMap);
    // if( defaultArcTo != nullptr ){
    // GUM_TRACE( " - Fils par défaut : " << *defaultArcTo );
    // }else{
    // GUM_TRACE( " - Aucun fils par défaut.");
    // }
    // GUM_TRACE( std::endl);

    // ************************************************************************
    // If var adresses exists (means we already insert a node with same var )
    // we check if nodes tied to this var aren't the same as the one we want to
    // insert
    // if so, we'll return that node id
    if (_var2NodeIdMap.exists(var)) {

      for (ListConstIteratorSafe<NodeId> iterNodeList =
               _var2NodeIdMap[var]->beginSafe();
           iterNodeList != _var2NodeIdMap[var]->endSafe(); ++iterNodeList) {
        // GUM_TRACE( "\t Noeud observé : " << *iterNodeList);
        bool thesame = true;

        if (defaultArcTo == 0 && _defaultArcMap.exists(*iterNodeList)) {
          thesame = false;
          // GUM_TRACE( " -> Inexistant par défaut existant");
        } else {
          if (defaultArcTo != 0 && (!_defaultArcMap.exists(*iterNodeList) ||
                                    defaultArcTo != _defaultArcMap[*iterNodeList])) {
            thesame = false;
            // GUM_TRACE( " -> Inégaux par défaut");
          }
        }

        if (thesame)

          //                 for ( std::vector< NodeId >::const_iterator iterArcMap =
          //                 nodeArcMap.begin(); iterArcMap != nodeArcMap.end();
          //                 ++iterArcMap ) {
          //                     if ( ( *_arcMap[ *iterNodeList ] ) [ std::distance (
          //                     nodeArcMap.begin(), iterArcMap ) ] != *iterArcMap )
          //                     {
          //                         thesame = false;
          //                         // GUM_TRACE( " -> Fils différent avec
          //                         modalité");
          //                         break;
          //                     }
          //                 }
          for (Idx i = 0; i < nodeArcMap.size(); i++)
            if (nodeArcMap[i] != (*this->_arcMap[*iterNodeList])[i] ||
                (nodeArcMap[i] == 0 &&
                 defaultArcTo == (*this->_arcMap[*iterNodeList])[i]) ||
                ((*this->_arcMap[*iterNodeList])[i] == 0 &&
                 nodeArcMap[i] == this->_defaultArcMap[*iterNodeList])) {
              thesame = false;
              // GUM_TRACE( " -> Fils différent avec modalité");
              break;
            }

        // GUM_TRACE( std::endl);
        if (thesame) {
          // GUM_TRACE( std::endl << std::endl << "Calquage sur noeud pour Var : " <<
          // var->toString() << " - Id : " << *iterNodeList << std::endl);
          res.first = true;
          res.second = *iterNodeList;
          return res;
        }
      }
    }

    // *******************************************************************************
    // Next we do some cleaning on arcs, in case where all arcs point to the same
    // node.
    // If so we return the id of that node.
    //     bool sameArc = true;
    //     NodeId nody = 0;
    //     std::vector< NodeId >::const_iterator arcIter = nodeArcMap.begin();
    //
    //     if( defaultArcTo != 0 )
    //         nody = defaultArcTo;
    //     else if( arcIter != nodeArcMap.end() ) {
    //         nody = *arcIter;
    //         ++arcIter;
    //     }
    //
    //     for(; arcIter != nodeArcMap.end(); ++arcIter )
    //         if( *arcIter != 0 && *arcIter != nody ) {
    //             sameArc = false;
    //             break;
    //         }
    //
    //     if( sameArc ) {
    //         // GUM_TRACE( std::endl << std::endl << "Gobage par arc pour Var : "
    //         << var->toString() << " - Id : " << nody << std::endl);
    //         res.first = true;
    //         res.second = nody;
    //     }
    return res;
  }

  /** Add or find a terminal node in the diagram linked to given value.
  * @return the id of that node. If a node with such value already exists, it
  * returns that node id.
  */

  template <typename GUM_SCALAR>
  INLINE NodeId MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::addTerminalNode(
      const GUM_SCALAR &value) {

    GUM_SCALAR approximate = this->fromExact(value);

    if (_valueMap.existsSecond(approximate)) {
      return _valueMap.first(approximate);
    }

    NodeId node = _model.addNode();
    _valueMap.insert(node, approximate);

    return node;
  }

  template <typename GUM_SCALAR>
  void MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::eraseNode(NodeId n) {

    if (!_model.exists(n)) {
      GUM_ERROR(NotFound, "Node " << n << " does not exist in diagram.");
    }

    if (_valueMap.existsFirst(n))
      _valueMap.eraseFirst(n);
    else {
      _var2NodeIdMap[_varMap[n]]->eraseByVal(n);
      _varMap.erase(n);

      delete _arcMap[n];
      _arcMap.erase(n);
    }

    const NodeSet &parents = _model.parents(n);

    for (auto parentIter = parents.beginSafe(); parentIter != parents.endSafe();
         ++parentIter) {
      for (std::vector<NodeId>::iterator iter = _arcMap[*parentIter]->begin();
           iter != _arcMap[*parentIter]->end(); ++iter)
        if (*iter == n)
          *iter = 0;

      if (_defaultArcMap.exists(*parentIter) && _defaultArcMap[*parentIter] == n)
        _defaultArcMap.erase(*parentIter);
    }

    _model.eraseNode(n);

    if (_rootId == n)
      _rootId = 0;
  }

  //*****************************************************************************/
  //**      Arcs Manipulation                                                  **/
  //*****************************************************************************/

  template <typename GUM_SCALAR>
  void MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::insertArc(NodeId from,
                                                                 NodeId to,
                                                                 Idx value) {
    addArc(from, to, value);
  }

  template <typename GUM_SCALAR>
  void MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::addArc(NodeId from, NodeId to,
                                                              Idx value) {

    if (!_model.exists(from)) {
      GUM_ERROR(NotFound, " Origin node " << from << " does not exist.");
    }

    if (!_model.exists(to)) {
      GUM_ERROR(NotFound, " Destination node " << to << " does not exist.");
    }

    if (_valueMap.existsFirst(from)) {
      GUM_ERROR(
          InvalidNode,
          " Origin node "
              << from
              << " is a terminal Node. No arcs can start from a terminal node");
    } else if (!_noVariableCheckMode && !_valueMap.existsFirst(to)) {
      // GUM_TRACE( "From : " << _varMap[from]->toString() << " - To : " << _varMap[
      // to ]->toString() << std::endl );
      if (_varsSeq.pos(_varMap[from]) >= _varsSeq.pos(_varMap[to])) {
        GUM_ERROR(OperationNotAllowed,
                  " This arc does not respect the variable order property. Variable "
                      << _varMap[from]->name() << " tied to node " << from
                      << " is after Variable " << _varMap[to]->name()
                      << " tied to node " << to << " in variable order.");
      }
    }

    for (std::vector<NodeId>::iterator iter = _arcMap[from]->begin();
         iter != _arcMap[from]->end(); ++iter)
      if (*iter == to &&
          (gum::Idx)std::distance(_arcMap[from]->begin(), iter) == value) {
        GUM_ERROR(DuplicateElement, " A same (meaning with same value "
                                        << value << " ) arc linking those two nodes "
                                        << from << " -> " << to
                                        << " already exist.");
        break;
      }

    unsafeAddArc(from, to, value);
  }

  // Adds a default arc in the DD
  // @param from and
  // @param to as NodeId
  // @throw NotFound If from and/or tail are not in the DD.
  // @throw InvalidNode if head is a terminal node
  // @throw OperationNotAllowed arc doesn't respect variable order property
  // @throw DuplicateElement if another arc linking those nodes already exists
  template <typename GUM_SCALAR>
  void MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::insertDefaultArc(NodeId from,
                                                                        NodeId to) {
    addDefaultArc(from, to);
  }

  template <typename GUM_SCALAR>
  void MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::addDefaultArc(NodeId from,
                                                                     NodeId to) {

    if (!_model.exists(from)) {
      GUM_ERROR(NotFound, " Origin node " << from << " does not exist.");
    }

    if (!_model.exists(to)) {
      GUM_ERROR(NotFound, " Destination node " << to << " does not exist.");
    }

    if (_valueMap.existsFirst(from)) {
      GUM_ERROR(
          InvalidNode,
          " Origin node "
              << from
              << " is a terminal Node. No arcs can start from a terminal node");
    } else if (!_valueMap.existsFirst(to) && !_noVariableCheckMode) {
      // GUM_TRACE( "Par defaut - From : " << _varMap[from]->toString() << " - To : "
      // << _varMap[ to ]->toString() << std::endl );
      if (_varsSeq.pos(_varMap[from]) >= _varsSeq.pos(_varMap[to])) {
        GUM_ERROR(OperationNotAllowed,
                  " This arc does not respect the variable order property. Variable "
                      << _varMap[from]->name() << " tied to node " << from
                      << " is after Variable " << _varMap[to]->name()
                      << " tied to node " << to << " in variable order.");
      }
    }

    if (_defaultArcMap.exists(from) && _defaultArcMap[from] != to)
      GUM_ERROR(DuplicateElement, "A default arc starting from this node "
                                      << from << " already exist.");

    unsafeAddDefaultArc(from, to);
  }

  template <typename GUM_SCALAR>
  void MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::eraseSpecificArc(
      NodeId from, NodeId to, Idx modality) {

    if (!_model.existsArc(Arc(from, to))) {
      GUM_ERROR(InvalidArc, " That arc " << from << " - " << to
                                         << " does not exist");
    }

    if ((*_arcMap[from])[modality] == to) {
      (*_arcMap[from])[modality] = 0;
    }

    (*_varUsedModalitiesMap[_varMap[from]])[modality]--;
  }

  // Adds an arc between two nodes in the graph.
  // @throw InvalidArc If arc does not exist
  // @warning due to the possibility that several arc with different value have the
  // same from and to,
  // if several arcs have different value but same parent and child, this method will
  // erase all of them .
  // If you want to erase a specific one, use eraseArcWithValue

  template <typename GUM_SCALAR>
  void MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::eraseArc(NodeId from,
                                                                NodeId to) {

    if (!_model.existsArc(Arc(from, to))) {
      GUM_ERROR(InvalidArc, " That arc " << from << " - " << to
                                         << " does not exist");
    }

    for (std::vector<NodeId>::iterator modaliter = _arcMap[from]->begin();
         modaliter != _arcMap[from]->end(); ++modaliter)
      if (*modaliter == to) {
        _model.eraseArc(Arc(from, to));
        *modaliter = 0;

        (*_varUsedModalitiesMap[_varMap[from]])[std::distance(_arcMap[from]->begin(),
                                                              modaliter)]--;
      }

    if (_defaultArcMap.exists(from) && _defaultArcMap[from] == to) {
      _model.eraseArc(Arc(from, to));
      _defaultArcMap.erase(from);
    }
  }

  /* **********************************************************************************************/
  /*                                                                                                                                            */
  /*                                                  Getting methods */
  /*                                                                                                                                            */
  /* **********************************************************************************************/

  // Displays the current DecisionDiagram structure

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::showProperties() {

    std::stringstream output;
    std::stringstream terminalStream;
    std::stringstream nonTerminalStream;
    std::stringstream arcstream;
    std::stringstream defaultarcstream;
    output << std::endl << "digraph \"no_name\" {" << std::endl;

    terminalStream << "node [shape = box];" << std::endl;
    nonTerminalStream << "node [shape = ellipse];" << std::endl;
    std::string tab = "  ";

    for (auto iter_node = _model.nodes().beginSafe();
         iter_node != _model.nodes().endSafe(); ++iter_node)
      if (*iter_node != 0) {
        if (_valueMap.existsFirst(*iter_node))
          terminalStream << tab << *iter_node << ";" << tab << *iter_node
                         << " [label=\"" << this->_valueMap.second(*iter_node)
                         << "\"]"
                         << ";" << std::endl;
        else {
          nonTerminalStream << tab << *iter_node << ";" << tab << *iter_node
                            << " [label=\"" << _varMap[*iter_node]->name() << "\"]"
                            << ";" << std::endl;

          if (_arcMap[*iter_node] != nullptr)
            for (std::vector<NodeId>::iterator arcIter =
                     _arcMap[*iter_node]->begin();
                 arcIter != _arcMap[*iter_node]->end(); ++arcIter)
              if (*arcIter != 0)
                arcstream << tab << *iter_node << " -> " << *arcIter << " [label=\""
                          << _varMap[*iter_node]->label(std::distance(
                                 _arcMap[*iter_node]->begin(), arcIter))
                          << "\",color=\"#0000ff\"]"
                          << ";" << std::endl;

          if (_defaultArcMap.exists(*iter_node))
            defaultarcstream << tab << *iter_node << " -> "
                             << _defaultArcMap[*iter_node] << " [color=\"#ff0000\"]"
                             << ";" << std::endl;
        }
      }

    output << terminalStream.str() << std::endl << nonTerminalStream.str()
           << std::endl << arcstream.str() << std::endl << defaultarcstream.str()
           << "}" << std::endl;

    GUM_TRACE(output.str());
  }

  // Returns the value of associated node if its a terminal one
  // @throw NotFound if it's not a terminal node

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR
      MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::nodeValue(NodeId node) {
    return _valueMap.second(node);
  }

  /* **********************************************************************************************/
  /*                        */
  /*      Various Methods                */
  /*                        */
  /* **********************************************************************************************/

  // Sets the factory from an already existing diagram

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::setMultiDimDecisionDiagram(
      const MultiDimDecisionDiagramBase<GUM_SCALAR> *source) {
    this->clear();
    _varsSeq = source->variablesSequence();
    _model.populateNodes(source->nodesMap());
    _valueMap = source->valuesMap();
    _varMap.resize(source->nodesMap().size());
    _var2NodeIdMap.resize(source->variablesSequence().size());
    _varUsedModalitiesMap.resize(source->variablesSequence().size());
    _arcMap.resize(source->nodesMap().size());
    _defaultArcMap.resize(source->nodesMap().size());

    for (auto iter_node = _model.nodes().beginSafe();
         iter_node != _model.nodes().endSafe(); ++iter_node) {

      if (*iter_node != 0 && !source->isTerminalNode(*iter_node)) {

        _varMap.insert(*iter_node, source->unsafeNodeVariable(*iter_node));

        if (!_var2NodeIdMap.exists(source->unsafeNodeVariable(*iter_node))) {
          _var2NodeIdMap.insert(source->unsafeNodeVariable(*iter_node),
                                new List<NodeId>(*(source->variableNodes(
                                    source->unsafeNodeVariable(*iter_node)))));
          _varUsedModalitiesMap.insert(
              source->unsafeNodeVariable(*iter_node),
              new std::vector<Idx>(*(source->variableUsedModalities(
                  source->unsafeNodeVariable(*iter_node)))));
        }

        _arcMap.insert(*iter_node, new std::vector<NodeId>(
                                       *(source->unsafeNodeSons(*iter_node))));

        for (std::vector<NodeId>::const_iterator sonIter =
                 source->unsafeNodeSons(*iter_node)->begin();
             sonIter != source->unsafeNodeSons(*iter_node)->end(); ++sonIter)
          _model.addArc(*iter_node, *sonIter);

        if (source->unsafeHasNodeDefaultSon(*iter_node)) {
          _defaultArcMap.insert(*iter_node,
                                source->unsafeNodeDefaultSon(*iter_node));
          _model.addArc(*iter_node, source->unsafeNodeDefaultSon(*iter_node));
        }
      }
    }
  }

  // Swaps two variables in this multidim
  // @throw OperationNotAllowed if the variable aren't adajcent in variable order

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::swap(const DiscreteVariable *x,
                                                       const DiscreteVariable *y) {

    if (_varsSeq.pos(y) != _varsSeq.pos(x) + 1) {
      GUM_ERROR(OperationNotAllowed,
                "Swap must be between two adjacent var. Var "
                    << y->name() << " is at pos " << _varsSeq.pos(y) << " and var "
                    << x->name() << " at pos " << _varsSeq.pos(x));
    }

    _varsSeq.swap(_varsSeq.pos(x), _varsSeq.pos(y));

    if (!_var2NodeIdMap.exists(x) || !_var2NodeIdMap.exists(y))
      return;

    List<NodeId> *yNodes = new List<NodeId>(), *xNodes = new List<NodeId>();

    for (ListIteratorSafe<NodeId> nodeIter = _var2NodeIdMap[x]->beginSafe();
         nodeIter != _var2NodeIdMap[x]->endSafe(); ++nodeIter) {

      std::vector<NodeId> *sonsMap = new std::vector<NodeId>(y->domainSize(), 0);

      for (Idx i = 0; i < y->domainSize(); i++) {

        std::vector<NodeId> *grandSonsMap =
            new std::vector<NodeId>(x->domainSize(), 0);

        for (Idx j = 0; j < x->domainSize(); j++) {

          NodeId nodeIterSonId = 0;

          if ((*_arcMap[*nodeIter])[j] != 0)
            nodeIterSonId = (*_arcMap[*nodeIter])[j];
          else
            nodeIterSonId = _defaultArcMap[*nodeIter];

          NodeId nodeIterGrandSonId = 0;

          if (_valueMap.existsFirst(nodeIterSonId) || _varMap[nodeIterSonId] != y) {
            nodeIterGrandSonId = nodeIterSonId;
          } else {
            if ((*_arcMap[nodeIterSonId])[i] != 0) {
              nodeIterGrandSonId = (*_arcMap[nodeIterSonId])[i];
            } else {
              nodeIterGrandSonId = _defaultArcMap[nodeIterSonId];
            }
          }

          (*grandSonsMap)[j] = nodeIterGrandSonId;
        }

        NodeId sonId = 0;

        bool redundant = true;
        std::vector<NodeId>::iterator grandSonsIter = grandSonsMap->begin();
        NodeId grandSon = *grandSonsIter;

        while (grandSonsIter != grandSonsMap->end()) {
          if (grandSon != *grandSonsIter) {
            redundant = false;
            break;
          }

          ++grandSonsIter;
        }

        if (redundant) {
          sonId = grandSon;
          delete grandSonsMap;
        } else {
          bool existSame = false;
          NodeId sameSon = 0;

          for (ListIteratorSafe<NodeId> sonIter = xNodes->beginSafe();
               sonIter != xNodes->endSafe(); ++sonIter) {
            bool thesame = true;

            for (std::vector<NodeId>::iterator iterArcMap = grandSonsMap->begin();
                 iterArcMap != grandSonsMap->end(); ++iterArcMap)
              if ((*_arcMap[*sonIter])[std::distance(grandSonsMap->begin(),
                                                     iterArcMap)] != *iterArcMap) {
                thesame = false;
                break;
              }

            if (thesame) {
              existSame = true;
              sameSon = *sonIter;
              break;
            }
          }

          if (existSame) {
            sonId = sameSon;
            delete grandSonsMap;
          } else {
            sonId = _model.addNode();
            xNodes->insert(sonId);
            _varMap.insert(sonId, x);
            _arcMap.insert(sonId, grandSonsMap);

            for (std::vector<NodeId>::iterator grandSonsIter = grandSonsMap->begin();
                 grandSonsIter != grandSonsMap->end(); ++grandSonsIter)
              _model.addArc(sonId, *grandSonsIter);
          }
        }

        (*sonsMap)[i] = sonId;
      }

      NodeId replacingNode = 0;

      bool redundant = true;
      std::vector<NodeId>::iterator sonsIter = sonsMap->begin();
      NodeId son = *sonsIter;

      while (sonsIter != sonsMap->end()) {
        if (son != *sonsIter) {
          redundant = false;
          break;
        }

        ++sonsIter;
      }

      if (redundant) {
        replacingNode = son;
        delete sonsMap;
      } else {

        bool existSame = false;
        NodeId sameNode = 0;

        for (ListIteratorSafe<NodeId> newNodeIter = yNodes->beginSafe();
             newNodeIter != yNodes->endSafe(); ++newNodeIter) {
          bool thesame = true;

          for (std::vector<NodeId>::iterator iterArcMap = sonsMap->begin();
               iterArcMap != sonsMap->end(); ++iterArcMap)
            if ((*_arcMap[*newNodeIter])[std::distance(sonsMap->begin(),
                                                       iterArcMap)] != *iterArcMap) {
              thesame = false;
              break;
            }

          if (thesame) {
            existSame = true;
            sameNode = *newNodeIter;
            break;
          }
        }

        if (existSame) {
          replacingNode = sameNode;
          delete sonsMap;
        } else {
          replacingNode = _model.addNode();
          yNodes->insert(replacingNode);
          _varMap.insert(replacingNode, y);
          _arcMap.insert(replacingNode, sonsMap);

          for (std::vector<NodeId>::iterator sonsIter = sonsMap->begin();
               sonsIter != sonsMap->end(); ++sonsIter)
            _model.addArc(replacingNode, *sonsIter);
        }
      }

      delete _arcMap[*nodeIter];
      _arcMap.erase(*nodeIter);
      _defaultArcMap.erase(*nodeIter);
      _varMap.erase(*nodeIter);

      for (auto parentIter = _model.parents(*nodeIter).beginSafe();
           parentIter != _model.parents(*nodeIter).endSafe(); ++parentIter) {
        _model.addArc(*parentIter, replacingNode);

        std::vector<NodeId> *newSonMap =
            new std::vector<NodeId>(_arcMap[*parentIter]->size(), 0);

        for (std::vector<NodeId>::iterator pSIter = _arcMap[*parentIter]->begin();
             pSIter != _arcMap[*parentIter]->end(); ++pSIter) {
          if (*pSIter == *nodeIter)
            (*newSonMap)[std::distance(_arcMap[*parentIter]->begin(), pSIter)] =
                replacingNode;
          else
            (*newSonMap)[std::distance(_arcMap[*parentIter]->begin(), pSIter)] =
                *pSIter;
        }

        delete _arcMap[*parentIter];
        _arcMap.erase(*parentIter);
        _arcMap.insert(*parentIter, newSonMap);

        if (_defaultArcMap.exists(*parentIter) &&
            _defaultArcMap[*parentIter] == *nodeIter) {
          _defaultArcMap.erase(*parentIter);
          _defaultArcMap.insert(*parentIter, replacingNode);
        }
      }

      _model.eraseNode(*nodeIter);
    }

    for (ListIteratorSafe<NodeId> nodeIter = _var2NodeIdMap[y]->beginSafe();
         nodeIter != _var2NodeIdMap[y]->endSafe(); ++nodeIter) {
      if (!_model.parents(*nodeIter).empty()) {
        yNodes->insert(*nodeIter);
      } else {
        delete _arcMap[*nodeIter];
        _arcMap.erase(*nodeIter);
        _defaultArcMap.erase(*nodeIter);
        _varMap.erase(*nodeIter);
        _model.eraseNode(*nodeIter);
      }
    }

    delete _var2NodeIdMap[y];
    _var2NodeIdMap.erase(y);
    _var2NodeIdMap.insert(y, yNodes);

    delete _var2NodeIdMap[x];
    _var2NodeIdMap.erase(x);
    _var2NodeIdMap.insert(x, xNodes);
  }

  //===============================================================================================================
  // Resets the factory
  //===============================================================================================================
  template <typename GUM_SCALAR>
  void MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::clear() {
    _rootId = 0;
    _varsSeq.clear();
    _varMap.clear();
    _defaultArcMap.clear();

    for (auto iter_node = _model.nodes().beginSafe();
         iter_node != _model.nodes().endSafe(); ++iter_node)
      if (*iter_node != 0 && !_valueMap.existsFirst(*iter_node) &&
          _arcMap[*iter_node] != nullptr)
        delete _arcMap[*iter_node];

    _arcMap.clear();

    _valueMap.clear();

    _model.clear();
    _model.addNode();

    for (HashTableIteratorSafe<const DiscreteVariable *, List<NodeId> *> iter =
             _var2NodeIdMap.beginSafe();
         iter != _var2NodeIdMap.endSafe(); ++iter)
      delete iter.val();

    _var2NodeIdMap.clear();

    for (HashTableIteratorSafe<const DiscreteVariable *, std::vector<Idx> *> iter =
             _varUsedModalitiesMap.beginSafe();
         iter != _varUsedModalitiesMap.endSafe(); ++iter)
      delete iter.val();

    _varUsedModalitiesMap.clear();
  }

  //===============================================================================================================
  // Puts the factory in a mode where no verification are made on variable
  // ( including arc respecting a variable order )
  // Helpful when diagram is build on stream
  //===============================================================================================================
  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::putOnNoVariableCheckMode() {
    _noVariableCheckMode = true;
  }

  //===============================================================================================================
  // Puts the factory out of no check on variable mode
  //===============================================================================================================
  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::putOffNoVariableCheckMode() {
    _noVariableCheckMode = false;
  }

  //===============================================================================================================
  // Finds an order of variable compatible to the diagram

  template <typename GUM_SCALAR>
  Sequence<const DiscreteVariable *>
  MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::_findVariableOrder() {

    Sequence<const DiscreteVariable *> varTopo;
    bool modified = true;

    // ****************************************************************
    // On itère jusqu'à ce qu'il n'y ait plus de modification
    // (signe que toutes les variables sont dans la liste ou qu'un
    // problème existe)
    while (modified) {
      modified = false;

      // ***************************************************************
      // Pour chaque variable
      for (Sequence<const DiscreteVariable *>::iterator_safe varIter =
               _varsSeq.beginSafe();
           varIter != _varsSeq.endSafe(); ++varIter) {

        // ***************************************************************
        // Si elle est déjà dans la liste, on passe
        if (varTopo.exists(*varIter)) {
          continue;
        }

        // ***************************************************************
        // Sinon on cherche les noeuds ratachés à cette variable
        bool addVar = true;

        for (ListIteratorSafe<NodeId> nodeIter =
                 _var2NodeIdMap[*varIter]->beginSafe();
             nodeIter != _var2NodeIdMap[*varIter]->endSafe(); ++nodeIter) {

          const NodeSet &parents = _model.parents(*nodeIter);

          // ***************************************************************
          // Pour chaque noeud lié à cette variable, on voit si la variable
          // associée au noeuds parents est déjà dans la liste
          for (auto parentIter = parents.beginSafe();
               parentIter != parents.endSafe(); ++parentIter) {

            // **********************************************************************
            // Si ce n'est pas le cas, cette variable ci ne sera pas ajoutée
            if (!varTopo.exists(_varMap[*parentIter])) {
              addVar = false;
              break;
            }
          }

          if (!addVar)
            break;
        }

        if (addVar) {
          varTopo.insert(*varIter);
          modified = true;
        }
      }
    }

    return varTopo;
  }

  /* **********************************************************************************************/
  /*                        */
  /*      Unsafe Methods                */
  /*                        */
  /* **********************************************************************************************/

  template <typename GUM_SCALAR>
  INLINE NodeId MultiDimDecisionDiagramFactoryBase<
      GUM_SCALAR>::unsafeAddNonTerminalNodeWithArcs(const DiscreteVariable *var,
                                                    const std::vector<NodeId> &
                                                        nodeArcMap,
                                                    NodeId defaultArcTo) {

    std::pair<bool, NodeId> check = checkredundancy(var, nodeArcMap, defaultArcTo);

    if (check.first)
      return check.second;

    // ***********************************************************************************
    // if we manage to reach this point, this mean we have to insert the node
    // with all his bunch of arc
    NodeId node = unsafeAddNonTerminalNode(var);

    // GUM_TRACE( "insertion noeud pour Var : " << var->toString() << " - Id : " <<
    // node << " - Fils : " << nodeArcMap );
    // if( defaultArcTo != nullptr )
    // GUM_TRACE( " - Defaut : " << *defaultArcTo );
    // GUM_TRACE( std::endl << std::endl );

    for (std::vector<NodeId>::const_iterator iter = nodeArcMap.begin();
         iter != nodeArcMap.end(); ++iter)
      if (*iter != 0)
        unsafeAddArc(node, *iter, std::distance(nodeArcMap.begin(), iter));

    if (defaultArcTo != 0)
      unsafeAddDefaultArc(node, defaultArcTo);

    return node;
  }

  template <typename GUM_SCALAR>
  INLINE NodeId
      MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::unsafeAddNonTerminalNode(
          const DiscreteVariable *var) {

    // *******************************************************************************************
    // Part were we had the variable's new node
    NodeId node = _model.addNode();

    // We mention that new node to the list of node bound to that variable
    _varMap.insert(node, var);

    _arcMap.insert(node, new std::vector<NodeId>(var->domainSize(), 0));

    // **********************************************************************************************
    // Addition of the node to the list of tied to given variable

    // If list hasn't be created yet, we create it
    if (!_var2NodeIdMap.exists(var)) {
      _var2NodeIdMap.insert(var, new List<NodeId>());
      _varUsedModalitiesMap.insert(var, new std::vector<Idx>(var->domainSize(), 0));
    }

    // And finally we add the node to that list
    _var2NodeIdMap[var]->insert(node);

    //*************************************************************************************************

    return node;
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::unsafeInsertArc(
      NodeId from, NodeId to, Idx value) {
    unsafeAddArc(from, to, value);
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::unsafeAddArc(
      NodeId from, NodeId to, Idx value) {

    if (_defaultArcMap.exists(from) && _defaultArcMap[from] == to)
      return;

    _model.addArc(from, to);

    (*_arcMap[from])[value] = to;
    (*_varUsedModalitiesMap[_varMap[from]])[value]++;
  }

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::unsafeInsertDefaultArc(NodeId from,
                                                                         NodeId to) {
    unsafeAddDefaultArc(from, to);
  }

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>::unsafeAddDefaultArc(NodeId from,
                                                                      NodeId to) {

    for (std::vector<NodeId>::iterator iter = _arcMap[from]->begin();
         iter != _arcMap[from]->end(); ++iter)
      if (*iter == to) {
        _model.eraseArc(Arc(from, to));
        (*_arcMap[from])[std::distance(_arcMap[from]->begin(), iter)];
      }

    if (!_defaultArcMap.exists(from)) {
      _model.addArc(from, to);
      _defaultArcMap.insert(from, to);
    }
  }
} // namespace gum
