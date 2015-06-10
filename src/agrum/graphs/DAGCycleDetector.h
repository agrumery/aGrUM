/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
/** @file
 * @brief A class for detecting directed cycles in DAGs when trying to apply
 * many changes to the graph
 *
 * When trying to assess whether multiple changes applied to a given DAG would
 * induce cycles, use class DAGCycleDetector instead of trying to apply the
 * modifications to the DAG itself and check whether and exception is raised:
 * the class is designed to be fast for such modifications. However, the number
 * of modifications checked should be higher than at least 3 for this class to
 * be competitive.
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_DAG_CYCLE_DETECTOR_H
#define GUM_DAG_CYCLE_DETECTOR_H

#include <vector>

#include <agrum/graphs/DAG.h>

namespace gum {

  /* =========================================================================== */
  // DAG CYCLE DETECTOR
  /* =========================================================================== */
  /** @class DAGCycleDetector
   * @ingroup graph_group
   * @brief A class for detecting directed cycles in DAGs when trying to apply
   * many changes to the graph
   *
   * When trying to assess whether multiple changes applied to a given DAG would
   * induce cycles, use class DAGCycleDetector instead of trying to apply the
   * modifications to the DAG itself and check whether and exception is raised:
   * the class is designed to be fast for such modifications. However, the number
   * of modifications checked should be higher than at least 3 for this class to
   * be competitive.
   */
  class DAGCycleDetector {
    public:
    // the type of modification that can be applied to the graph
    enum class ChangeType { ARC_ADDITION, ARC_DELETION, ARC_REVERSAL };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    /// the base class indicating the possible changes
    class Change {
      public:
      Change(ChangeType type, NodeId tail, NodeId head) noexcept;
      Change(const Change &from) noexcept;
      Change(Change &&from) noexcept;
      virtual ~Change() noexcept;

      protected:
      Change &operator=(const Change &from) noexcept;
      Change &operator=(Change &&from) noexcept;

      public:
      // ##########################################################################
      /// @name Accessors/Modifiers
      // ##########################################################################
      /// @{

      /// returns the type of the operation
      ChangeType type() const noexcept;

      /// indicates the tail of the arc involved in the modification
      NodeId tail() const noexcept;

      /// indicates the head of the arc involved in the modification
      NodeId head() const noexcept;

      /// @}

      private:
      /// the type of modification
      ChangeType __type;

      /// the tail of the arc to be modified
      NodeId __tail;

      /// the head of the arc to be modified
      NodeId __head;
    };

    /** @class DAGCycleDetector::ArcAdd
     * @brief the class to indicate that we wish to add a new arc
     * @ingroup graph_group
     */
    class ArcAdd : public Change {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{
      /// default constructor
      ArcAdd(NodeId tail, NodeId head) noexcept;

      /// copy constructor
      ArcAdd(const ArcAdd &from) noexcept;

      /// move constructor
      ArcAdd(ArcAdd &&from) noexcept;

      /// destructor
      ~ArcAdd() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      ArcAdd &operator=(const ArcAdd &from) noexcept;

      /// move operator
      ArcAdd &operator=(ArcAdd &&from) noexcept;

      /// @}
    };

    /** @class DAGCycleDetector::ArcDel
     * @brief the class to indicate that we wish to remove an arc
     * @ingroup graph_group
     */
    class ArcDel : public Change {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{
      /// default constructor
      ArcDel(NodeId tail, NodeId head) noexcept;

      /// copy constructor
      ArcDel(const ArcDel &from) noexcept;

      /// move constructor
      ArcDel(ArcDel &&from) noexcept;

      /// destructor
      ~ArcDel() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      ArcDel &operator=(const ArcDel &from) noexcept;

      /// move operator
      ArcDel &operator=(ArcDel &&from) noexcept;

      /// @}
    };

    /** @class DAGCycleDetector::ArcReverse
     * @brief the class to indicate that we wish to reverse an arc
     * @ingroup graph_group
     */
    class ArcReverse : public Change {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{
      /// default constructor
      ArcReverse(NodeId tail, NodeId head) noexcept;

      /// copy constructor
      ArcReverse(const ArcReverse &from) noexcept;

      /// move constructor
      ArcReverse(ArcReverse &&from) noexcept;

      /// destructor
      ~ArcReverse() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      ArcReverse &operator=(const ArcReverse &from) noexcept;

      /// move operator
      ArcReverse &operator=(ArcReverse &&from) noexcept;

      /// @}
    };

#endif // DOXYGEN_SHOULD_SKIP_THIS

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    DAGCycleDetector() noexcept;

    /// copy constructor
    DAGCycleDetector(const DAGCycleDetector &from);

    /// move constructor
    DAGCycleDetector(DAGCycleDetector &&from);

    /// destructor
    ~DAGCycleDetector();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    DAGCycleDetector &operator=(const DAGCycleDetector &from);

    /// move operator
    DAGCycleDetector &operator=(DAGCycleDetector &&from);

    /// check the equality between two DAGCycleDetectors
    /** used essentally for debugging purposes */
    bool operator==(const DAGCycleDetector &from) const;

    /// check the inequality between two DAGCycleDetectors
    /** used essentally for debugging purposes */
    bool operator!=(const DAGCycleDetector &from) const;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// sets the initial DAG from which changes shall be applied
    void setDAG(const DAG &dag);

    /// adds a new arc to the current DAG
    /** worst case complexity: O(h^2) where h is the height of the DAG
     * @throws InvalidDirectedCycle if the arc would create a cycle in the dag */
    GUM_DEPRECATED(void insertArc(NodeId x, NodeId y));
    void addArc(NodeId x, NodeId y);

    /// removes an arc from the current DAG
    /** worst case complexity: O(h^2) where h is the height of the DAG */
    void eraseArc(NodeId x, NodeId y);

    /// reverses an arc from the DAG
    /**  worst case complexity: O(h^2) where h is the height of the DAG
     * @throws InvalidDirectedCycle if the arc reversal would create a cycle
     * in the dag */
    void reverseArc(NodeId x, NodeId y);

    /// indicates whether an arc addition would create a cycle
    /** worst case complexity: O(1) */
    bool hasCycleFromAddition(NodeId x, NodeId y) const noexcept;

    /// indicates wether an arc reversal would create a cycle
    /** worst case complexity: O(1) */
    bool hasCycleFromReversal(NodeId x, NodeId y) const noexcept;

    /// indicates whether an arc deletion would create a cycle
    /** worst case complexity: O(1) */
    bool hasCycleFromDeletion(NodeId x, NodeId y) const noexcept;

    /// indicates whether a set of modifications would create a cycle
    /** the Boolean returned corresponds to the existence (or not) of a cycle
     * on the graph resulting from the whole sequence of modifications. This
     * means, for instance, that if, among modifs, there exists an arc (X,Y)
     * addition involving a cycle but also the same arc (X,Y) deletion, the final
     * graph obtained does not contains a cycle (due to the deletion) and
     * the method will thus return false. */
    bool hasCycleFromModifications(const std::vector<Change> &modifs) const;

    /// @}

    private:
    /// the initial dag from which modifications are applied
    DiGraph __dag;

    /// the set of ancestors of each node in the dag
    /** for each ancestor, we keep track of the number of paths leading to it */
    NodeProperty<NodeProperty<unsigned int>> __ancestors;

    /// the set of descendants of each node in the dag
    /** for each ancestor, we keep track of the number of paths leading to it */
    NodeProperty<NodeProperty<unsigned int>> __descendants;

    /// adds a weighted nodeset to another (weights are added)
    void __addWeightedSet(NodeProperty<unsigned int> &nodeset,
                          const NodeProperty<unsigned int> &set_to_add,
                          unsigned int multiplier) const;

    /// removes a weighted nodeset from another (weights are subtracted)
    void __delWeightedSet(NodeProperty<unsigned int> &nodeset,
                          const NodeProperty<unsigned int> &set_to_del,
                          unsigned int multiplier) const;

    /** @brief put into a weighted nodeset the nodes of another weighted set that
     * belong to a set of arc extremities */
    void __restrictWeightedSet(NodeProperty<unsigned int> &result_set,
                               const NodeProperty<unsigned int> &set_to_restrict,
                               const NodeSet &extrmities) const;
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/graphs/DAGCycleDetector.inl>
#endif // GUM_NOINLINE

#endif // GUM_DAG_CYCLE_DETECTOR_H
