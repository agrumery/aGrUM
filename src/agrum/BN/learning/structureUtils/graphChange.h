/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief the classes to account for structure changes in a graph
 *
 * The classes included in this file shall be used by learning algorithms to
 * notify scores, structural constraints, etc, how the learnt graph has been
 * modified.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_GRAPH_CHANGE_H
#define GUM_LEARNING_GRAPH_CHANGE_H

#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/base/graphs/parts/nodeGraphPart.h>

namespace gum {

  namespace learning {

    /// the internal type of the nodes involved in the arc/edge modifications
    // it is important that the size of this type be a multiple of 32 bits.
    // This is exploited to both reduce the memory consumption and speed-up
    // copying the fields of GraphChange
    using LearnNodeId = uint32_t;

    /// the type of modification that can be applied to the graph
    enum class GraphChangeType : LearnNodeId {
      ARC_ADDITION,
      ARC_DELETION,
      ARC_REVERSAL,
      ARC_TRIANGLE_DELETION1,
      ARC_TRIANGLE_DELETION2,
      EDGE_ADDITION,
      EDGE_DELETION
    };

    // forward the declarations of the Graph changes classes. This will be
    // useful for specifying HashFunc friendships in Class GraphChange
    class ArcAddition;
    class ArcDeletion;
    class ArcReversal;
    class ArcTriangleDeletion1;
    class ArcTriangleDeletion2;
    class EdgeAddition;
    class EdgeDeletion;

    /* ========================================================================= */
    /* ===                        GRAPH CHANGE CLASS                         === */
    /* ========================================================================= */
    /** @class GraphChange
     * @brief
     * @ingroup learning_group
     */
    class GraphChange {
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      protected:
      /// default constructor
      GraphChange(GraphChangeType type, NodeId node1, NodeId node2, NodeId node3 = 0) noexcept;

      public:
      /// copy constructor
      GraphChange(const GraphChange& from) noexcept;

      /// move constructor
      GraphChange(GraphChange&& from) noexcept;

      /// destructor
      ~GraphChange() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      GraphChange& operator=(const GraphChange& from) noexcept;

      /// move operator
      GraphChange& operator=(GraphChange&& from) noexcept;

      /// returns whether two graph changes are identical or not
      bool operator==(const GraphChange& from) const noexcept;

      /// returns whether two graph changes are different or not
      bool operator!=(const GraphChange& from) const noexcept;

      /// @}

      // ##########################################################################
      /// @name Accessors/Modifiers
      // ##########################################################################
      /// @{

      /// returns the type of the operation
      GraphChangeType type() const noexcept;

      /// returns a string corresponding to the type of the change
      std::string typeAsString() const;

      /// returns the first node involved in the modification
      NodeId node1() const noexcept;

      /// returns the second node involved in the modification
      NodeId node2() const noexcept;

      /// returns the third node involved in the modification (if any)
      /** @throws InvalidNode is raised if the modification does not involve
       * a third */
      NodeId node3() const;

      /// put the content of the graph change into a string
      // This method has not been declared as virtual in order to reduce the
      // memory consumption. The drawback is that it is slower than what we
      // would get with a virtual method. But this method should essentially
      // be used for debugging purposes, hence this is not really an issue.
      std::string toString() const;

      /// @}

      protected:
      /// the nodes involved in the edge or arc to be modified
      LearnNodeId nodes_[3];

      /// the type of modification
      // @warning this field must follow _nodes_. This fact is exploited to
      // speed up copying Class GraphChange fields: as LearnNodeId is 32-bit
      // long, _type is aligned as 32 bits, hence, it directly follows _nodes
      // without any padding. Overall, _nodes + _type should be 16-byte long.
      GraphChangeType type_;


      // friendships allow accessing _nodes. This will speed up computations
      // when sizeof(Size) == 2 * sizeof(LearnNodeId)
      friend class HashFunc< GraphChange >;
      friend class HashFunc< ArcAddition >;
      friend class HashFunc< ArcDeletion >;
      friend class HashFunc< ArcReversal >;
      friend class HashFunc< ArcTriangleDeletion1 >;
      friend class HashFunc< ArcTriangleDeletion2 >;
      friend class HashFunc< EdgeAddition >;
      friend class HashFunc< EdgeDeletion >;
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
    class ArcAddition: public GraphChange {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      ArcAddition(NodeId node1, NodeId node2) noexcept;

      /// copy constructor
      ArcAddition(const ArcAddition& from) noexcept;

      /// move constructor
      ArcAddition(ArcAddition&& from) noexcept;

      /// destructor
      ~ArcAddition() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      ArcAddition& operator=(const ArcAddition& from) noexcept;

      /// move operator
      ArcAddition& operator=(ArcAddition&& from) noexcept;

      /// returns whether two arc additions are identical or not
      bool operator==(const ArcAddition& from) const noexcept;

      /// returns whether two arc additions are different or not
      bool operator!=(const ArcAddition& from) const noexcept;

      /// @}
    };

    /* =========================================================================*/
    /* ===                        ARC DELETION CLASS                         ===*/
    /* =========================================================================*/
    /** @class ArcDeletion
     * @brief The class for notifying learning algorithms of arc removals
     * @ingroup learning_group
     *
     * This class is convenient to know at compile time which graph change we
     * are dealing with. Thus, this enables to perform faster code (we can avoid
     * using a switch on GraphChanges to determine which change corresponds to
     * this class.
     */
    class ArcDeletion: public GraphChange {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      ArcDeletion(NodeId node1, NodeId node2) noexcept;

      /// copy constructor
      ArcDeletion(const ArcDeletion& from) noexcept;

      /// move constructor
      ArcDeletion(ArcDeletion&& from) noexcept;

      /// destructor
      ~ArcDeletion() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      ArcDeletion& operator=(const ArcDeletion& from) noexcept;

      /// move operator
      ArcDeletion& operator=(ArcDeletion&& from) noexcept;

      /// returns whether two arc deletions are identical or not
      bool operator==(const ArcDeletion& from) const noexcept;

      /// returns whether two arc deletions are different or not
      bool operator!=(const ArcDeletion& from) const noexcept;

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
    class ArcReversal: public GraphChange {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      ArcReversal(NodeId node1, NodeId node2) noexcept;

      /// copy constructor
      ArcReversal(const ArcReversal& from) noexcept;

      /// move constructor
      ArcReversal(ArcReversal&& from) noexcept;

      /// destructor
      ~ArcReversal() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      ArcReversal& operator=(const ArcReversal& from) noexcept;

      /// move operator
      ArcReversal& operator=(ArcReversal&& from) noexcept;

      /// returns whether two arc reversals are identical or not
      bool operator==(const ArcReversal& from) const noexcept;

      /// returns whether two arc reversals are different or not
      bool operator!=(const ArcReversal& from) const noexcept;

      /// @}
    };

    /* ========================================================================= */
    /* ===                 ARC TRIANGLE DELETION 1ST CLASS                   === */
    /* ========================================================================= */
    /** @class ArcTriangleDeletion1
     * @brief The graph change substituting a triangle node1->node2->node3 +
     * node1->node3 into v-structure node2->node1<-node3
     * @ingroup learning_group
     *
     * This class is convenient to know at compile time which graph change we
     * are dealing with. Thus, this enables to perform faster code (we can avoid
     * using a switch on GraphChanges to determine which change corresponds to
     * this class.
     */
    class ArcTriangleDeletion1: public GraphChange {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** The triangle that we wish to delete (substitute by a v-structure) is the
       * following: node1 -> node2 -> node3 + node1 -> node3. It is substituted
       * by v-structure node2 -> node1 <- node3 */
      ArcTriangleDeletion1(NodeId node1, NodeId node2, NodeId node3) noexcept;

      /// copy constructor
      ArcTriangleDeletion1(const ArcTriangleDeletion1& from) noexcept;

      /// move constructor
      ArcTriangleDeletion1(ArcTriangleDeletion1&& from) noexcept;

      /// destructor
      ~ArcTriangleDeletion1() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      ArcTriangleDeletion1& operator=(const ArcTriangleDeletion1& from) noexcept;

      /// move operator
      ArcTriangleDeletion1& operator=(ArcTriangleDeletion1&& from) noexcept;

      /// returns whether two ArcTriangleDeletion1 are identical or not
      /** Two ArcTriangleDeletion1 are identical if and only if they involve
       * the same set of nodes, in the same order.
       */
      bool operator==(const ArcTriangleDeletion1& from) const noexcept;

      /// returns whether two ArcTriangleDeletion1 are different or not
      /** Two ArcTriangleDeletion1 differ if and only if either they do not
       * involve the same set of nodes or those are not in the same order.
       */
      bool operator!=(const ArcTriangleDeletion1& from) const noexcept;

      /// @}

      // ##########################################################################
      /// @name Accessors/Modifiers
      // ##########################################################################
      /// @{

      /// returns the third node involved in the modification (if any)
      /** @throws InvalidNode is raised if the modification does not involve
       * a third */
      NodeId node3() const;

      /// @}
    };

    /* ========================================================================= */
    /* ===                 ARC TRIANGLE DELETION 2ND CLASS                   === */
    /* ========================================================================= */
    /** @class ArcTriangleDeletion2
     * @brief The graph change substituting a triangle node1->node2->node3 +
     * node1->node3 into v-structure node1->node2<-node3
     * @ingroup learning_group
     *
     * This class is convenient to know at compile time which graph change we
     * are dealing with. Thus, this enables to perform faster code (we can avoid
     * using a switch on GraphChanges to determine which change corresponds to
     * this class.
     */
    class ArcTriangleDeletion2: public GraphChange {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** The triangle that we wish to delete (substitute by a v-structure) is the
       * following: node1 -> node2 -> node3 + node1 -> node3. It is substituted
       * by v-structure node1 -> node2 <- node3 */
      ArcTriangleDeletion2(NodeId node1, NodeId node2, NodeId node3) noexcept;

      /// copy constructor
      ArcTriangleDeletion2(const ArcTriangleDeletion2& from) noexcept;

      /// move constructor
      ArcTriangleDeletion2(ArcTriangleDeletion2&& from) noexcept;

      /// destructor
      ~ArcTriangleDeletion2() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      ArcTriangleDeletion2& operator=(const ArcTriangleDeletion2& from) noexcept;

      /// move operator
      ArcTriangleDeletion2& operator=(ArcTriangleDeletion2&& from) noexcept;

      /// returns whether two ArcTriangleDeletion2 are identical or not
      /** Two ArcTriangleDeletion2 are identical if and only if they involve
       * the same set of nodes, in the same order.
       */
      bool operator==(const ArcTriangleDeletion2& from) const noexcept;

      /// returns whether two ArcTriangleDeletion2 are different or not
      /** Two ArcTriangleDeletion2 differ if and only if either they do not
       * involve the same set of nodes or those are not in the same order.
       */
      bool operator!=(const ArcTriangleDeletion2& from) const noexcept;

      /// @}

      // ##########################################################################
      /// @name Accessors/Modifiers
      // ##########################################################################
      /// @{

      /// returns the third node involved in the modification (if any)
      /** @throws InvalidNode is raised if the modification does not involve
       * a third */
      NodeId node3() const;

      /// @}
    };

    /* =========================================================================*/
    /* ===                        EDGE ADDITION CLASS                        ===*/
    /* =========================================================================*/
    /** @class EdgeAddition
     * @brief The class for notifying learning algorithms of new edge additions
     * @ingroup learning_group
     *
     * This class is convenient to know at compile time which graph change we
     * are dealing with. Thus, this enables to perform faster code (we can avoid
     * using a switch on GraphChanges to determine which change corresponds to
     * this class.
     */
    class EdgeAddition: public GraphChange {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      EdgeAddition(NodeId node1, NodeId node2) noexcept;

      /// copy constructor
      EdgeAddition(const EdgeAddition& from) noexcept;

      /// move constructor
      EdgeAddition(EdgeAddition&& from) noexcept;

      /// destructor
      ~EdgeAddition() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      EdgeAddition& operator=(const EdgeAddition& from) noexcept;

      /// move operator
      EdgeAddition& operator=(EdgeAddition&& from) noexcept;

      /// returns whether two edge additions are identical or not
      bool operator==(const EdgeAddition& from) const noexcept;

      /// returns whether two edge additions are different or not
      bool operator!=(const EdgeAddition& from) const noexcept;

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
    class EdgeDeletion: public GraphChange {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      EdgeDeletion(NodeId node1, NodeId node2) noexcept;

      /// copy constructor
      EdgeDeletion(const EdgeDeletion& from) noexcept;

      /// move constructor
      EdgeDeletion(EdgeDeletion&& from) noexcept;

      /// destructor
      ~EdgeDeletion() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      EdgeDeletion& operator=(const EdgeDeletion& from) noexcept;

      /// move operator
      EdgeDeletion& operator=(EdgeDeletion&& from) noexcept;

      /// returns whether two edge deletions are identical or not
      bool operator==(const EdgeDeletion& from) const noexcept;

      /// returns whether two edge deletions are different or not
      bool operator!=(const EdgeDeletion& from) const noexcept;

      /// @}
    };

    /// a \c << operator for GraphChanges
    std::ostream& operator<<(std::ostream& stream, const GraphChange& change);

  } /* namespace learning */

  /// the hash function for Graph Changes
  template <>
  class HashFunc< learning::GraphChange >: public HashFuncBase< learning::GraphChange > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const learning::GraphChange& key);

    /// computes the hashed value of a key
    Size operator()(const learning::GraphChange& key) const final;
  };

  /// the hash function for Arc Additions
  template <>
  class HashFunc< learning::ArcAddition >: public HashFuncBase< learning::ArcAddition > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const learning::ArcAddition& key);

    /// computes the hashed value of a key
    Size operator()(const learning::ArcAddition& key) const final;
  };

  /// the hash function for Arc Deletions
  template <>
  class HashFunc< learning::ArcDeletion >: public HashFuncBase< learning::ArcDeletion > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const learning::ArcDeletion& key);

    /// computes the hashed value of a key
    Size operator()(const learning::ArcDeletion& key) const final;
  };

  /// the hash function for Arc Reversals
  template <>
  class HashFunc< learning::ArcReversal >: public HashFuncBase< learning::ArcReversal > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const learning::ArcReversal& key);

    /// computes the hashed value of a key
    Size operator()(const learning::ArcReversal& key) const final;
  };

  /// the hash function for Arc Triangle Deletions creating a v-structure in node 1
  template <>
  class HashFunc< learning::ArcTriangleDeletion1 >:
      public HashFuncBase< learning::ArcTriangleDeletion1 > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const learning::ArcTriangleDeletion1& key);

    /// computes the hashed value of a key
    Size operator()(const learning::ArcTriangleDeletion1& key) const final;
  };

  /// the hash function for Arc Triangle Deletions creating a v-structure in node 2
  template <>
  class HashFunc< learning::ArcTriangleDeletion2 >:
      public HashFuncBase< learning::ArcTriangleDeletion2 > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const learning::ArcTriangleDeletion2& key);

    /// computes the hashed value of a key
    Size operator()(const learning::ArcTriangleDeletion2& key) const final;
  };

  /// the hash function for Edge Additions
  template <>
  class HashFunc< learning::EdgeAddition >: public HashFuncBase< learning::EdgeAddition > {
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const learning::EdgeAddition& key);

    /// computes the hashed value of a key
    Size operator()(const learning::EdgeAddition& key) const final;
  };

  /// the hash function for Edge Deletions
  template <>
  class HashFunc< learning::EdgeDeletion >: public HashFuncBase< learning::EdgeDeletion > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const learning::EdgeDeletion& key);

    /// computes the hashed value of a key
    Size operator()(const learning::EdgeDeletion& key) const final;
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/structureUtils/graphChange_inl.h>
#endif   // GUM_NOINLINE

#endif   /* GUM_LEARNING_GRAPH_CHANGE_H */
