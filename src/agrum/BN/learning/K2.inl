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
// #include <iostream>
// #include <exception>
// #include <cmath>
// #include <vector>
// #include <agrum/core/gumDebug.h>
// #include <agrum/gumId.h>
//
//
//
// /* ============================================================================== */
// /* ============================================================================== */
// /* ===                     GUM_K2_ALGO_BASE IMPLEMENTATION                    === */
// /* ============================================================================== */
// /* ============================================================================== */
//
// // ================================================================================
// /// clears the K2 tree structure and compute the score of the child
// // ================================================================================
// INLINE void gumK2AlgoBase::setChild (unsigned int row) {
//   // be sure to clear the previous K2 tree
//   tree.clear ();
//
//   // set the child appropriately
//   child = row;
//
//   // create a level for the child
//   tree.createLevel (row);
//
//   // reset the parents to the empty set
//   for (int i = parents.size () - 1; i >= 0; --i) {
//     parents[i] = false;
//   }
//
//   // parse the database to fill the box
//   tree.parseDatabase ();
//
//   // compute the child's score
//   best_score = computeScore ();
//
//   // remove the box of the child
//   tree.clear ();
// }
//
//
// // ================================================================================
// /// returns the row number of the next parent, if any, or throws an exception
// // ================================================================================
// INLINE unsigned int gumK2AlgoBase::getNextParent () throw (gumElementNotFound) {
//   // to keep track of the best parent
//   unsigned int new_best_parent = 0;
//   unsigned int new_best_modal  = 0;
//   gumLearnCounting::gumCountingBox* new_best_level_beg = 0;
//   gumLearnCounting::gumCountingBox* new_best_level_end = 0;
//   float new_best_score = best_score;
//
//   // parse all the potential parents
//   for (int i = 0; order[i] != child; ++i) {
//     if (!parents[order[i]]) {
//       // here, node order[i] is not a current parent of child, hence we should
//       // try to add it
//
//       // get the number of modalities of this new parent
//       unsigned int cur_modal = database.nbrModalities (order[i]);
//       current_parent = order[i];
//
//       // first add to the tree the boxes necessary for the new parent as
//       // well as for the child
//       tree.createLevel (order[i]);
//       tree.createLevel (child);
//
//       // parse the database an fill the tree appropriately
//       tree.parseDatabase (tree.getNbrLevels() - 2, tree.getNbrLevels() - 1);
//
//       // compute the new score
//       float new_score = computeScore ();
//
//       // update the best score
//       if ((new_score < new_best_score) ||
//           ((new_score == new_best_score) && (new_best_modal < cur_modal))) {
//         // remove the last two levels of the tree, they are useless
//         tree.deleteLastLevel ();
//         tree.deleteLastLevel ();
//       }
//       else {
//         // in any case, we do not need the child level anymore
//         tree.deleteLastLevel ();
//
//         // substitute the old best parents data by the new one
//         if (new_best_level_beg)
//           tree.deleteLevel (new_best_level_beg, new_best_level_end);
//         new_best_parent = order[i];
//         new_best_modal = cur_modal;
//         std::cerr << "new par " << order[i] << " of " << child << std::endl;
//         std::pair<gumLearnCounting::gumCountingBox*,
//           gumLearnCounting::gumCountingBox*> new_level = tree.removeNGetLastLevel();
//         new_best_level_beg = new_level.first;
//         new_best_level_end = new_level.second;
//         new_best_score = new_score;
//       }
//     }
//   }
//
//   if (new_best_level_beg) {
//     // restore into the tree the best parent found so far, if any
//     tree.insertLevel (new_best_level_beg, new_best_level_end, new_best_parent);
//     parents[new_best_parent] = true;
//     best_score = new_best_score;
//     return new_best_parent;
//   }
//
//   GUM_ERROR (gumElementNotFound, "no new parent could be found");
// }
//
//
// // ================================================================================
// /// computes and return a BN resulting from K2-like algorithm
// // ================================================================================
// INLINE gumK2AlgoBase::operator gumBayesNetStruct () {
//   return compute ();
// }
//
//
//
//
//
//
//
// /* ============================================================================== */
// /* ============================================================================== */
// /* ===                     GUM_K2_BIC_ALGO IMPLEMENTATION                     === */
// /* ============================================================================== */
// /* ============================================================================== */
//
// // ================================================================================
// /// compute the score of the K2 tree
// // ================================================================================
// INLINE float gumK2BICAlgo::computeLog (int n) throw () {
//   if (!logComputed[n]) {
//     logVect[n] = log ((float) n);
//     logComputed[n] = true;
//   }
//   return logVect[n];
// }
//
//
// // ================================================================================
// /// compute the score of the K2 tree
// // ================================================================================
// INLINE float gumK2BICAlgo::computeScore () throw () {
//   float score = 0;
//
//   // if the tree has only one level, just compute the score of this level
//   if (tree.getNbrLevels () == 1) {
//     const std::vector<int>& nb_cases = tree.getLevel(0)->getCounters ();
//     for (int i = database.nbrModalities (child) - 1; i >= 0; --i)
//       score += nb_cases[i] * (computeLog (nb_cases[i]) - logM);
//     score -= logDim;
//     return score;
//   }
//
//   // parse the two lowest levels of the tree
//   unsigned int par_modal = database.nbrModalities (current_parent);
//   unsigned int cur_modal = database.nbrModalities (child);
//   for (gumLearnCounting::gumCountingBox* box = tree.getLevel(tree.getNbrLevels()-2);
//        box != 0; box = box->getNextBox ()) {
//     const std::vector<int>& nb_cases = box->getCounters ();
//     const std::vector<gumLearnCounting::gumCountingBox*>&
//       children = box->getChildren();
//     for (unsigned int i = 0; i != par_modal; ++i) {
//       gumLearnCounting::gumCountingBox* child = children[i];
//       float logNij = computeLog (nb_cases[i]);
//       for (unsigned int j = 0; j != cur_modal; ++j)
//         score += child->getCounter(j) * (computeLog(child->getCounter(j)) - logNij);
//     }
//     score -= par_modal * logDim;
//   }
//   return score;
// }
//
//
// // ================================================================================
// /// sets the (row of the) node that should be on the left of the conditioning mark
// // ================================================================================
// INLINE void gumK2BICAlgo::setChild (unsigned int row) {
//   // initialize the dimension penalty
//   logDim = (database.nbrModalities (row) - 1) * logM / 2;
//
//   // now really set the child
//   gumK2AlgoBase::setChild (row);
// }
//
//
// // ==============================================================================
// /// perform the necessary updates after the insertion of a new parent
// // ==============================================================================
// INLINE void gumK2BICAlgo::updateNewParent (unsigned int row) {
//   logDim *= database.nbrModalities (row);
// }
//
//
//
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
