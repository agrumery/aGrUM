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
 * @brief This file contains definition for a Junction Tree Generator
 *
 * @author Pierre-Henri WUILLEMIN
 */
#include <agrum/config.h>

#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/graphs/algorithms/triangulations/junctionTreeStrategies/defaultJunctionTreeStrategy.h>
#include <agrum/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/graphs/algorithms/triangulations/partialOrderedTriangulation.h>
#include <agrum/graphs/algorithms/binaryJoinTreeConverterDefault.h>
#include <agrum/BN/BayesNet.h>

class JunctionTreeGenerator {
  public:
  gum::JunctionTree junctionTree(const gum::UndiGraph& g,
                                 PyObject* partial_order = nullptr) const {
    return _junctionTree(g, _translatePartialOrder(partial_order));
  }

  gum::JunctionTree junctionTree(const gum::DAG& dag,
                                 PyObject*       partial_order = nullptr) const {
    return _junctionTree(dag.moralGraph(), _translatePartialOrder(partial_order));
  }

  gum::JunctionTree junctionTree(const gum::BayesNet< double >& bn,
                                 PyObject* partial_order = nullptr) const {
    gum::NodeProperty< gum::Size > mods;
    for (const auto node : bn.dag().nodes())
      mods.insert(node, bn.variable(node).domainSize());
    return _junctionTree(
       bn.moralGraph(), _translatePartialOrder(partial_order), mods);
  }

  PyObject* eliminationOrder(const gum::UndiGraph& g,
                             PyObject*             partial_order = nullptr) const {
    return _eliminationOrder(g, _translatePartialOrder(partial_order));
  }

  PyObject* eliminationOrder(const gum::DAG& dag,
                             PyObject*       partial_order = nullptr) const {
    return _eliminationOrder(dag.moralGraph(),
                             _translatePartialOrder(partial_order));
  }

  PyObject* eliminationOrder(const gum::BayesNet< double >& bn,
                             PyObject* partial_order = nullptr) const {
    gum::NodeProperty< gum::Size > mods;
    for (const auto node : bn.dag().nodes())
      mods.insert(node, bn.variable(node).domainSize());
    return _eliminationOrder(
       bn.moralGraph(), _translatePartialOrder(partial_order), mods);
  }


  gum::JunctionTree binaryJoinTree(const gum::UndiGraph& g,
                                   PyObject* partial_order = nullptr) const {
    return _binaryJoinTree(g, _translatePartialOrder(partial_order));
  }

  gum::JunctionTree binaryJoinTree(const gum::DAG& dag,
                                   PyObject*       partial_order = nullptr) const {
    return _binaryJoinTree(dag.moralGraph(),
                           _translatePartialOrder(partial_order));
  }

  gum::JunctionTree binaryJoinTree(const gum::BayesNet< double >& bn,
                                   PyObject* partial_order = nullptr) const {
    gum::NodeProperty< gum::Size > mods;
    for (const auto node : bn.dag().nodes())
      mods.insert(node, bn.variable(node).domainSize());
    return _binaryJoinTree(
       bn.moralGraph(), _translatePartialOrder(partial_order), mods);
  }

  private:
  gum::JunctionTree _junctionTree(const gum::UndiGraph&            g,
                                  const gum::List< gum::NodeSet >& partial_order,
                                  gum::NodeProperty< gum::Size >   mods =
                                     gum::NodeProperty< gum::Size >()) const {
    if (mods.size() == 0) { mods = g.nodesProperty(static_cast< gum::Size >(2)); }
    gum::StaticTriangulation* triangulation;
    if (partial_order.size() == 0) {
      triangulation = new gum::DefaultTriangulation(&g, &mods);
    } else {
      triangulation =
         new gum::PartialOrderedTriangulation(&g, &mods, &partial_order);
    }
    gum::DefaultJunctionTreeStrategy strategy;
    strategy.setTriangulation(triangulation);
    auto res = strategy.junctionTree();
    delete (triangulation);
    return res;
  }

  gum::JunctionTree _binaryJoinTree(const gum::UndiGraph&            g,
                                    const gum::List< gum::NodeSet >& partial_order,
                                    gum::NodeProperty< gum::Size >   mods =
                                       gum::NodeProperty< gum::Size >()) const {
    gum::BinaryJoinTreeConverterDefault bjtc;
    gum::NodeSet                        emptyset;
    if (mods.size() == 0) { mods = g.nodesProperty(static_cast< gum::Size >(2)); }

    return bjtc.convert(_junctionTree(g, partial_order, mods), mods, emptyset);
  }

  PyObject* _eliminationOrder(const gum::UndiGraph&            g,
                              const gum::List< gum::NodeSet >& partial_order,
                              gum::NodeProperty< gum::Size >   mods =
                                 gum::NodeProperty< gum::Size >()) const {
    if (mods.size() == 0) { mods = g.nodesProperty(static_cast< gum::Size >(2)); }
    gum::StaticTriangulation* triangulation;
    if (partial_order.size() == 0) {
      triangulation = new gum::DefaultTriangulation(&g, &mods);
    } else {
      triangulation =
         new gum::PartialOrderedTriangulation(&g, &mods, &partial_order);
    }
    auto res =
       PyAgrumHelper::PyListFromNodeVect(triangulation->eliminationOrder());
    delete (triangulation);
    return res;
  }

  gum::List< gum::NodeSet > _translatePartialOrder(PyObject* p) const {
    gum::List< gum::NodeSet > res;
    if (p == nullptr) return res;

    PyObject* item;
    PyObject* item2;

    PyObject* iter = PyObject_GetIter(p);
    PyObject* iter2;

    if (iter != NULL) {
      while ((item = PyIter_Next(iter))) {
        iter2 = PyObject_GetIter(item);
        if (iter2 != NULL) {
          gum::NodeSet se;
          while ((item2 = PyIter_Next(iter2))) {
            if (PyInt_Check(item2)) {
              se.insert(PyInt_AsLong(item2));
            } else {
              GUM_ERROR(gum::InvalidArgument,
                        "A value is neither a node name nor an node id");
            }
          }
          res.insert(se);
        } else {
          GUM_ERROR(gum::InvalidArgument,
                    "The argument is not a sequence of sequence");
        }
      }
    } else {
      GUM_ERROR(gum::InvalidArgument, "The argument is not a sequence nor a set");
    }
    return res;
  }
};