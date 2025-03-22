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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


// this file should be included at the end of all the StructuralConstraints
// header
// files (see StructuralConstraintUndiGraph to see how it should be included).

#ifdef GUM_CONSTRAINT_CLASS_NAME

// ##########################################################################
/// @name Accessors / Modifiers
// ##########################################################################
/// @{

/// sets a new graph from which we will perform checkings
void setGraph(const UndiGraph& graph);

/// notify the constraint of a modification of the graph
/** @warning If an already existing edge is added nothing is done. In
 * particular, no exception is raised. */
void modifyGraph(const EdgeAddition& change);

/// notify the constraint of a modification of the graph
/** @warning If a nonexisting edge is removed, nothing is done. In
 * particular, no exception is raised. */
void modifyGraph(const EdgeDeletion& change);

/// notify the constraint of a modification of the graph
/** @warning If an already existing edge is added, or if a nonexisting edge
 * is removed, nothing is done. In particular, no exception is raised. */
void modifyGraph(const GraphChange& change);

/// indicates whether a change will always violate the constraint
/** Some learning algorithms need examine several times whether a given
 * graph change can be applied. For instance, the first time edge (X,Y)
 * addition is considered, the learning algorithm may discard this change
 * because it violates the structural constraint, but, later on, other edge
 * removals may induce that the edge addition is now possible. Such change
 * is thus not always invalid. Conversely, there are changes that can be
 * discarded once and for all. For instance,
 * in a 2TBN structure, it is always impossible to add a backward-time arc.
 * Such graph changes are always invalid and are therefore tagged as such
 * by the isAlwaysInvalid method. */
bool isAlwaysInvalid(const GraphChange& change) const;

/// checks whether the constraints enable to add edge (x,y)
bool checkEdgeAddition(NodeId x, NodeId y) const;

/// checks whether the constraints enable to remove edge (x,y)
bool checkEdgeDeletion(NodeId x, NodeId y) const;

/// checks whether the constraints enable to add an edge
bool checkModification(const EdgeAddition& change) const;

/// checks whether the constraints enable to remove an edge
bool checkModification(const EdgeDeletion& change) const;

/// checks whether the constraints enable to perform a graph change
bool checkModification(const GraphChange& change) const;

/// @}

#endif /* GUM_CONSTRAINT_CLASS_NAME */
