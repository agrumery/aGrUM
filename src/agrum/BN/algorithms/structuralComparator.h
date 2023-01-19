/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
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


/** @file
 * @brief A class for comparing graphs based on their structures
 *
 * Classifies corresponding arcs, edges and nothing in a graph given a reference
 * graph to then return values of recall, precision and Fscore.
 *
 * @author Quentin FALCAND and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_STRUCTURAL_COMPARATOR_H
#define GUM_LEARNING_STRUCTURAL_COMPARATOR_H

#include <agrum/BN/BayesNet.h>
#include <agrum/tools/graphs/PDAG.h>


namespace gum {

  /** @class StructuralComparator
   * @brief A class for comparing graphs based on their structures
   *
   * @ingroup bn_group
   */
  class StructuralComparator {
    public:
    // ##########################################################################
    /// @name Constructors / Destructors
    // ##########################################################################
    /// @{

    /// default constructor
    StructuralComparator();

    /// destructor
    ~StructuralComparator();

    /// @}

    // ##########################################################################
    /// @name Accessors
    // ##########################################################################
    /// @{
    /// compare two DiGraphs
    void compare(const DiGraph& ref, const DiGraph& test);
    /// compare two UndiGraphs
    void compare(const UndiGraph& ref, const UndiGraph& test);
    /// compare two PDAGs
    void compare(const PDAG& ref, const PDAG& test);
    /// compare two BNs based on their DAG
    template < typename GS1, typename GS2 >
    void compare(const BayesNet< GS1 >& ref, const BayesNet< GS2 >& test);
    /// compare a PDAG with the essential graph of a reference BN
    template < typename GUM_SCALAR >
    void compare(const BayesNet< GUM_SCALAR >& ref, const PDAG& test);
    /// compare the essential graph of a BN with a reference PDAG
    template < typename GUM_SCALAR >
    void compare(const PDAG& ref, const BayesNet< GUM_SCALAR >& test);

    /// Measures for the skeleton, aka graph without orientations
    double precision_skeleton() const;
    double recall_skeleton() const;
    double f_score_skeleton() const;

    /// Measures for the graphs
    double precision() const;
    double recall() const;
    double f_score() const;
    /// @}

    protected:
    private:
    /// Confusion matrix
    /*    __________________________________________________
     *  |   \predicted |           |           |           |
     *  |true\         |    -->    |    ---    |     X     |
     *  |--------------|-----------|-----------|-----------|
     *  |      -->     | true arc  |wrong edge |wrong none |
     *  |              | mis. arc  |      arc  |      arc  |
     *  |--------------|-----------|-----------|-----------|
     *  |      ---     |wrong arc  | true edge |wrong none |
     *  |              |      edge |           |      edge |
     *  |--------------|-----------|-----------|-----------|
     *  |       X      |wrong arc  |wrong edge | true none |
     *  |      	       |      none |      none |           |
     *  | _____________|___________|___________|___________|
     *
     */
    double _true_edge_, _true_arc_, _true_none_;
    double _wrong_edge_arc_, _wrong_edge_none_;
    double _wrong_arc_edge_, _wrong_arc_none_, _misoriented_arc_;
    double _wrong_none_edge_, _wrong_none_arc_;
    // double  _precision_,  _recall_,  _f_score_;
  };

} /* namespace gum */

/// always include templated methods
#include <agrum/BN/algorithms/structuralComparator_tpl.h>

#endif /* GUM_LEARNING_STRUCTURAL_COMPARATOR_H */
