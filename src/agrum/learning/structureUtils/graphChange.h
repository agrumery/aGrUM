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
 * @brief the classes to account for structure changes in a graph
 *
 * The classes included in this file shall be used by learning algorithms to
 * notify scores, structural constraints, etc, how the learnt graph has been
 * modified.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_GRAPH_CHANGE_H
#define GUM_LEARNING_GRAPH_CHANGE_H

#include <iostream>
#include <string>

#include <agrum/config.h>
#include <agrum/core/hashFunc.h>
#include <agrum/graphs/nodeGraphPart.h>

namespace gum {

  namespace learning {

    /// the type of modification that can be applied to the graph
    enum GraphChangeType {
      ARC_ADDITION,
      ARC_DELETION,
      ARC_REVERSAL,
      EDGE_ADDITION,
      EDGE_DELETION
    };

    /* ========================================================================= */
    /* ===                        GRAPH CHANGE CLASS                         === */
    /* ========================================================================= */
    /** @class GraphChange
     * @brief
     * @ingroup learning_group
     */
    class GraphChange {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      GraphChange(GraphChangeType type, NodeId node1, NodeId node2) noexcept;

      /// copy constructor
      GraphChange(const GraphChange &from) noexcept;

      /// move constructor
      GraphChange(GraphChange &&from) noexcept;

      /// destructor
      ~GraphChange() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      GraphChange &operator=(const GraphChange &from) noexcept;

      /// move operator
      GraphChange &operator=(GraphChange &&from) noexcept;

      /// returns whether two graph changes are identical or not
      bool operator==(const GraphChange &from) const noexcept;

      /// returns whether two graph changes are different or not
      bool operator!=(const GraphChange &from) const noexcept;

      /// @}

      // ##########################################################################
      /// @name Accessors/Modifiers
      // ##########################################################################
      /// @{

      /// returns the type of the operation
      GraphChangeType type() const noexcept;

      /// returns the first node involved in the modification
      NodeId node1() const noexcept;

      /// returns the second node involved in the modification
      NodeId node2() const noexcept;

      /// put the content of the graph change into a string
      virtual std::string toString() const;

      /// @}

      private:
      /// the type of modification
      GraphChangeType __type;

      /// the first node in the edge or arc to be modified
      NodeId __node1;

      /// the second node in the edge or arc to be modified
      NodeId __node2;
    };

    /* ========================================================================= */
    /* ===                        ARC ADDITION CLASS                         === */
    /* ========================================================================= */
    /** @class ArcAddition
     * @brief The class for notifying learning algorithms of new arc additions
     * @ingroup learning_group
     *
     * This class is convenient to know at compile time which graph change we
     * are dealing with. Thus, this enables to perform faster code (we can avoid
     * using a switch on GraphChanges to determine which change corresponds to
     * this class.
     */
    class ArcAddition : public GraphChange {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      ArcAddition(NodeId node1, NodeId node2) noexcept;

      /// copy constructor
      ArcAddition(const ArcAddition &from) noexcept;

      /// move constructor
      ArcAddition(ArcAddition &&from) noexcept;

      /// destructor
      ~ArcAddition() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      ArcAddition &operator=(const ArcAddition &from) noexcept;

      /// move operator
      ArcAddition &operator=(ArcAddition &&from) noexcept;

      /// returns whether two arc additions are identical or not
      bool operator==(const ArcAddition &from) const noexcept;

      /// returns whether two arc additions are different or not
      bool operator!=(const ArcAddition &from) const noexcept;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// put the content of the ArcAddition into a string
      virtual std::string toString() const final;

      /// @}
    };

    /* ========================================================================= */
    /* ===                        ARC DELETION CLASS                         === */
    /* ========================================================================= */
    /** @class ArcDeletion
     * @brief The class for notifying learning algorithms of arc removals
     * @ingroup learning_group
     *
     * This class is convenient to know at compile time which graph change we
     * are dealing with. Thus, this enables to perform faster code (we can avoid
     * using a switch on GraphChanges to determine which change corresponds to
     * this class.
     */
    class ArcDeletion : public GraphChange {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      ArcDeletion(NodeId node1, NodeId node2) noexcept;

      /// copy constructor
      ArcDeletion(const ArcDeletion &from) noexcept;

      /// move constructor
      ArcDeletion(ArcDeletion &&from) noexcept;

      /// destructor
      ~ArcDeletion() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      ArcDeletion &operator=(const ArcDeletion &from) noexcept;

      /// move operator
      ArcDeletion &operator=(ArcDeletion &&from) noexcept;

      /// returns whether two arc deletions are identical or not
      bool operator==(const ArcDeletion &from) const noexcept;

      /// returns whether two arc deletions are different or not
      bool operator!=(const ArcDeletion &from) const noexcept;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// put the content of the ArcDeletion into a string
      virtual std::string toString() const final;

      /// @}
    };

    /* ========================================================================= */
    /* ===                        ARC REVERSAL CLASS                         === */
    /* ========================================================================= */
    /** @class ArcReversal
     * @brief The class for notifying learning algorithms of arc reversals
     * @ingroup learning_group
     *
     * This class is convenient to know at compile time which graph change we
     * are dealing with. Thus, this enables to perform faster code (we can avoid
     * using a switch on GraphChanges to determine which change corresponds to
     * this class.
     */
    class ArcReversal : public GraphChange {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      ArcReversal(NodeId node1, NodeId node2) noexcept;

      /// copy constructor
      ArcReversal(const ArcReversal &from) noexcept;

      /// move constructor
      ArcReversal(ArcReversal &&from) noexcept;

      /// destructor
      ~ArcReversal() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      ArcReversal &operator=(const ArcReversal &from) noexcept;

      /// move operator
      ArcReversal &operator=(ArcReversal &&from) noexcept;

      /// returns whether two arc reversals are identical or not
      bool operator==(const ArcReversal &from) const noexcept;

      /// returns whether two arc reversals are different or not
      bool operator!=(const ArcReversal &from) const noexcept;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// put the content of the ArcReversal into a string
      virtual std::string toString() const final;

      /// @}
    };

    /* ========================================================================= */
    /* ===                        EDGE ADDITION CLASS                        === */
    /* ========================================================================= */
    /** @class EdgeAddition
     * @brief The class for notifying learning algorithms of new edge additions
     * @ingroup learning_group
     *
     * This class is convenient to know at compile time which graph change we
     * are dealing with. Thus, this enables to perform faster code (we can avoid
     * using a switch on GraphChanges to determine which change corresponds to
     * this class.
     */
    class EdgeAddition : public GraphChange {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      EdgeAddition(NodeId node1, NodeId node2) noexcept;

      /// copy constructor
      EdgeAddition(const EdgeAddition &from) noexcept;

      /// move constructor
      EdgeAddition(EdgeAddition &&from) noexcept;

      /// destructor
      ~EdgeAddition() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      EdgeAddition &operator=(const EdgeAddition &from) noexcept;

      /// move operator
      EdgeAddition &operator=(EdgeAddition &&from) noexcept;

      /// returns whether two edge additions are identical or not
      bool operator==(const EdgeAddition &from) const noexcept;

      /// returns whether two edge additions are different or not
      bool operator!=(const EdgeAddition &from) const noexcept;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// put the content of the EdgeAddition into a string
      virtual std::string toString() const final;

      /// @}
    };

    /* ========================================================================= */
    /* ===                        EDGE DELETION CLASS                        === */
    /* ========================================================================= */
    /** @class EdgeDeletion
     * @brief The class for notifying learning algorithms of edge removals
     * @ingroup learning_group
     *
     * This class is convenient to know at compile time which graph change we
     * are dealing with. Thus, this enables to perform faster code (we can avoid
     * using a switch on GraphChanges to determine which change corresponds to
     * this class.
     */
    class EdgeDeletion : public GraphChange {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      EdgeDeletion(NodeId node1, NodeId node2) noexcept;

      /// copy constructor
      EdgeDeletion(const EdgeDeletion &from) noexcept;

      /// move constructor
      EdgeDeletion(EdgeDeletion &&from) noexcept;

      /// destructor
      ~EdgeDeletion() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      EdgeDeletion &operator=(const EdgeDeletion &from) noexcept;

      /// move operator
      EdgeDeletion &operator=(EdgeDeletion &&from) noexcept;

      /// returns whether two edge deletions are identical or not
      bool operator==(const EdgeDeletion &from) const noexcept;

      /// returns whether two edge deletions are different or not
      bool operator!=(const EdgeDeletion &from) const noexcept;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// put the content of the EdgeDeletion into a string
      virtual std::string toString() const final;

      /// @}
    };

    /// a \c << operator for GraphChanges
    std::ostream &operator<<(std::ostream &stream, const GraphChange &change);

    /// a \c << operator for ArcAddition
    std::ostream &operator<<(std::ostream &stream, const ArcAddition &change);

    /// a \c << operator for ArcDeletion
    std::ostream &operator<<(std::ostream &stream, const ArcDeletion &change);

    /// a \c << operator for ArcReversal
    std::ostream &operator<<(std::ostream &stream, const ArcReversal &change);

    /// a \c << operator for EdgeAddition
    std::ostream &operator<<(std::ostream &stream, const EdgeAddition &change);

    /// a \c << operator for EdgeDeletion
    std::ostream &operator<<(std::ostream &stream, const EdgeDeletion &change);

  } /* namespace learning */

  /// the hash function for Graph Changes
  template <>
  class HashFunc<learning::GraphChange> : public HashFuncSmallKey<NodeId> {
    public:
    /// computes the hashed value of a key
    Size operator()(const learning::GraphChange &key) const {
      return (((unsigned long)key.node1() * HashFuncConst::gold +
               (unsigned long)key.node2() * HashFuncConst::pi) >>
              _right_shift);
    }
  };

  /// the hash function for Arc Additions
  template <>
  class HashFunc<learning::ArcAddition> : public HashFuncSmallKey<NodeId> {
    public:
    /// computes the hashed value of a key
    Size operator()(const learning::ArcAddition &key) const {
      return (((unsigned long)key.node1() * HashFuncConst::gold +
               (unsigned long)key.node2() * HashFuncConst::pi) >>
              _right_shift);
    }
  };

  /// the hash function for Arc Deletions
  template <>
  class HashFunc<learning::ArcDeletion> : public HashFuncSmallKey<NodeId> {
    public:
    /// computes the hashed value of a key
    Size operator()(const learning::ArcDeletion &key) const {
      return (((unsigned long)key.node1() * HashFuncConst::gold +
               (unsigned long)key.node2() * HashFuncConst::pi) >>
              _right_shift);
    }
  };

  /// the hash function for Arc Reversals
  template <>
  class HashFunc<learning::ArcReversal> : public HashFuncSmallKey<NodeId> {
    public:
    /// computes the hashed value of a key
    Size operator()(const learning::ArcReversal &key) const {
      return (((unsigned long)key.node1() * HashFuncConst::gold +
               (unsigned long)key.node2() * HashFuncConst::pi) >>
              _right_shift);
    }
  };

  /// the hash function for Edge Additions
  template <>
  class HashFunc<learning::EdgeAddition> : public HashFuncSmallKey<NodeId> {
    public:
    /// computes the hashed value of a key
    Size operator()(const learning::EdgeAddition &key) const {
      return (((unsigned long)key.node1() * HashFuncConst::gold +
               (unsigned long)key.node2() * HashFuncConst::pi) >>
              _right_shift);
    }
  };

  /// the hash function for Edge Deletions
  template <>
  class HashFunc<learning::EdgeDeletion> : public HashFuncSmallKey<NodeId> {
    public:
    /// computes the hashed value of a key
    Size operator()(const learning::EdgeDeletion &key) const {
      return (((unsigned long)key.node1() * HashFuncConst::gold +
               (unsigned long)key.node2() * HashFuncConst::pi) >>
              _right_shift);
    }
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/learning/structureUtils/graphChange.inl>
#endif // GUM_NOINLINE

#endif /* GUM_LEARNING_GRAPH_CHANGE_H */
