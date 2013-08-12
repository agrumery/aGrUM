/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
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
#ifndef DOXYGEN_SHOULD_SKIP_THIS
//#include <iostream>
//#include <exception>
//#include <cmath>
//#include <vector>
//#include <agrum/core/gumDebug.h>
//#include <agrum/gumId.h>
//#include <agrum/BN/learning/gumK2.h>
//
//
//
//
//#ifdef GUM_NO_INLINE
//#include "gumK2.inl"
//#endif /* GUM_NO_INLINE */
//
//
//
///* ============================================================================== */
///* ============================================================================== */
///* ===                     GUM_K2_ALGO_BASE IMPLEMENTATION                    === */
///* ============================================================================== */
///* ============================================================================== */
//
//====
///// basic constructor
//====
//gumK2AlgoBase::gumK2AlgoBase (const gumDatabase& data,
//                              const std::vector<gumId>& ordering) :
//  database (data), tree (data), order (ordering.size()),
//  child (0), parents (ordering.size()), best_score (0) {
//  // for debugging purposes
//  GUM_CONSTRUCTOR (gumK2AlgoBase);
//
//  // create a hashtable to check that vector ordering does not contain twice the
//  // same node and that all nodes belong to the database
//  if (ordering.size () > data.getNbrNodes ())
//    GUM_ERROR (gumFatalError, "the ordering vector is not ok");
//  gumHashTable<gumId, bool> checkTable (data.getNbrNodes());
//
//  // ordering represents an order on the id of the nodes. However, the database
//  // is encoded using another ordering, so we should convert the former into
//  // the latter
//  try {
//    for (unsigned int i = 0; i != ordering.size(); ++i) {
//      checkTable.insert (ordering.at(i), true);
//      order[i] = data.getRow (ordering.at(i));
//    }
//  }
//  catch (...) {
//    GUM_ERROR (gumFatalError, "the ordering vector is not ok");
//  }
//}
//
//
//====
///// destructor
////== ==============================================================================
//gumK2AlgoBase::~gumK2AlgoBase () {
//  // for debugging purposes
//  GUM_DESTRUCTOR (gumK2AlgoBase);
//}
//
//
//====
///// returns the graph computed by the K2 like algorithm
//====
//gumBayesNetTplStruct gumK2AlgoBase::compute () {
//  // create an empty graph
//  gumBayesNetTplStruct graph (order.size(), true, order.size (), true);
//
//  // add nodes to the graph
//  for (unsigned int i = 0; i != order.size (); ++i)
//    graph.insertNode (gumNode (database.getId (order[i]),
//                               database.nodeName (order[i])));
//
//  // parse the nodes vector and add parents
//  for (unsigned int i = 1; i < order.size(); ++i) {
//    setChild (order[i]);
//    try {
//      while (true) {
//        unsigned int parent = getNextParent();
//        updateNewParent (parent);
//        graph.insertArc (gumArc(database.getId (parent),
//                                database.getId (order[i])));
//      }
//    }
//    catch (gumElementNotFound&) { }
//  }
//
//  // return the graph constructed
//  return graph;
//}
//
//
//
//
//
//
///* ============================================================================== */
///* ============================================================================== */
///* ===                     GUM_K2_BIC_ALGO IMPLEMENTATION                     === */
///* ============================================================================== */
///* ============================================================================== */
//
//====
///// basic constructor
//====
//gumK2BICAlgo::gumK2BICAlgo (const gumDatabase& data,
//                            const std::vector<gumId>& ordering) :
//  gumK2AlgoBase (data, ordering),
//  logVect (data.nbrLines() + 1), logComputed (data.nbrLines()+1, false) {
//  // for debugging purposes
//  GUM_CONSTRUCTOR (gumK2BICAlgo);
//
//  // 0 x log(0) = 0
//  logVect[0] = 0;
//  logComputed[0] = true;
//
//  // compute the log of the number of cases in the database
//  logM = computeLog (database.nbrLines());
//}
//
//
//====
///// destructor
//====
//gumK2BICAlgo::~gumK2BICAlgo () {
//  // for debugging purposes
//  GUM_DESTRUCTOR (gumK2BICAlgo);
//}
//
//
//====
///// returns the graph computed by the K2 like algorithm
//====
//gumBayesNetTplStruct gumK2BICAlgo::compute () {
//  return gumK2AlgoBase::compute ();
//}
//
//
//====
///// computes and return a BN resulting from K2-like algorithm
//====
//gumK2BICAlgo::operator gumBayesNetTplStruct () {
//  return compute ();
//}
//
//
//
//
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
