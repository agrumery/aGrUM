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
 * @brief the "meta-programming" class for storing several structural
 *constraints
 *
 * In aGrUM, there are two ways to store sets of structural constraints: the
 * first one is to put them into a StructuralConstraintSetDynamic. This class
 * allows to add at runtime any constraint you wish. As such, it is very generic
 * but, by not knowing at compile time the constraints that will be stored into
 * the vector, calling its methods has a slight overhead. On the other hand, if
 * you already know all the constraints you wish to apply, the
 * StructuralConstraintSetStatic is better suited because it will compute at
 * compile time how to call the constraint's methods in a most efficient way: if
 * these methods are inlined, there will be no overhead at all when calling
 *these
 * methods. This file defines the StructuralConstraintSetStatic class.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_STRUCTURAL_CONSTRAINT_SET_STATIC_H
#define GUM_LEARNING_STRUCTURAL_CONSTRAINT_SET_STATIC_H

#include <type_traits>

#include <agrum/agrum.h>
#include <agrum/tools/graphs/diGraph.h>
#include <agrum/BN/learning/structureUtils/graphChange.h>

namespace gum {

  namespace learning {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    // a class that indicates the we inherit from a
    // StructuralConstraintSetStatic
    struct _StructuralRoot_ {};

    // a temporary structure used to help computing the minimal set of
    // constraints
    template < typename FIRST_CONSTRAINT, typename... OTHER_CONSTRAINTS >
    struct _ConstraintSet_;

    // a structure to concatenate  _ConstraintSets_ or simply constraints and
    // produce as a result a new  _ConstraintSet_
    template < typename SET1, typename SET2 >
    struct _ConcatConstraintSet_;

    // a helper function to create minimum structural constraint sets and the
    // methods actually used on all these constraints. This is a helper for
    // the class that the user should use, i.e., StructuralConstraintSetStatic
    template < typename FIRST_CONSTRAINT, typename... OTHER_CONSTRAINTS >
    class _StructuralConstraintSetStatic_;

    // ============================================================================
    // checks whether a given structural constraint belongs to a given set of
    // structural constraints
    template < typename CONSTRAINT, typename SET >
    struct _IsInConstraintSet_;

    template < typename CONSTRAINT, typename SET >
    struct _IsInConstraintSet_< CONSTRAINT, _ConstraintSet_< SET > > {
      constexpr static bool value = std::is_same< CONSTRAINT, SET >::value;
    };

    template < typename CONSTRAINT, typename SET1, typename... SETS >
    struct _IsInConstraintSet_< CONSTRAINT, _ConstraintSet_< SET1, SETS... > > {
      constexpr static bool value
         = std::is_same< CONSTRAINT, SET1 >::value
        || _IsInConstraintSet_< CONSTRAINT, _ConstraintSet_< SETS... > >::value;
    };

    // ============================================================================
    // a temporary structure used to help computing the minimal set of
    // constraints
    // (removing all duplicates) belonging to a given set of structural
    // constraints. For instance, if we have the following structural hierarchy:
    // Z->Y->X, T->Y->X and we have the set of constraints S =  <Z,T>, the set
    // of
    // all structural constraints reachable from S is S' = <Z,Y,X,T,Y,X>. The
    // goal
    // of the following class is to transform S' into S'' = <Z,T,Y,X>, i.e., the
    // set S' without any duplicates.
    template < typename FIRST_CONSTRAINT, typename... OTHER_CONSTRAINTS >
    struct _ConstraintSet_: public _ConstraintSet_< OTHER_CONSTRAINTS... > {
      using minset = typename std::conditional<
         _IsInConstraintSet_< FIRST_CONSTRAINT, _ConstraintSet_< OTHER_CONSTRAINTS... > >::value,
         typename _ConstraintSet_< OTHER_CONSTRAINTS... >::minset,
         typename _ConcatConstraintSet_<
            FIRST_CONSTRAINT,
            typename _ConstraintSet_< OTHER_CONSTRAINTS... >::minset >::type >::type;
      using set = _StructuralConstraintSetStatic_< FIRST_CONSTRAINT, OTHER_CONSTRAINTS... >;
    };

    template < typename CONSTRAINT >
    struct _ConstraintSet_< CONSTRAINT > {
      using minset = _ConstraintSet_< CONSTRAINT >;
      using set    = _StructuralConstraintSetStatic_< CONSTRAINT >;
    };

    // ============================================================================
    // a structure to concatenate  _ConstraintSets_ or simply constraints and
    // produce as a result a new  _ConstraintSet_
    template < typename SET1, typename SET2 >
    struct _ConcatConstraintSet_;

    template < typename CONSTRAINT1, typename CONSTRAINT2 >
    struct _ConcatConstraintSet_< CONSTRAINT1, _ConstraintSet_< CONSTRAINT2 > > {
      using type = _ConstraintSet_< CONSTRAINT1, CONSTRAINT2 >;
    };

    template < typename CONSTRAINT1, typename CONSTRAINT2 >
    struct _ConcatConstraintSet_< _ConstraintSet_< CONSTRAINT1 >, _ConstraintSet_< CONSTRAINT2 > > {
      using type = _ConstraintSet_< CONSTRAINT1, CONSTRAINT2 >;
    };

    template < typename CONSTRAINT1, typename CONSTRAINT2, typename... OTHER_CONSTRAINT2 >
    struct _ConcatConstraintSet_< CONSTRAINT1,
                                  _ConstraintSet_< CONSTRAINT2, OTHER_CONSTRAINT2... > > {
      using type = _ConstraintSet_< CONSTRAINT1, CONSTRAINT2, OTHER_CONSTRAINT2... >;
    };

    template < typename CONSTRAINT1, typename CONSTRAINT2, typename... OTHER_CONSTRAINT1 >
    struct _ConcatConstraintSet_< _ConstraintSet_< CONSTRAINT1, OTHER_CONSTRAINT1... >,
                                  _ConstraintSet_< CONSTRAINT2 > > {
      using type = _ConstraintSet_< CONSTRAINT1, OTHER_CONSTRAINT1..., CONSTRAINT2 >;
    };

    template < typename CONSTRAINT1,
               typename CONSTRAINT2,
               typename... OTHER_CONSTR1,
               typename... OTHER_CONSTR2 >
    struct _ConcatConstraintSet_< _ConstraintSet_< CONSTRAINT1, OTHER_CONSTR1... >,
                                  _ConstraintSet_< CONSTRAINT2, OTHER_CONSTR2... > > {
      using type = _ConstraintSet_< CONSTRAINT1, OTHER_CONSTR1..., CONSTRAINT2, OTHER_CONSTR2... >;
    };

    // ============================================================================
    // a helper function to create minimum structural constraint sets and the
    // methods actually used on all these constraints. This is a helper for
    // the class that the user should use, i.e., StructuralConstraintSetStatic
    template < typename CONSTRAINT1, typename... OTHER_CONSTRAINTS >
    class _StructuralConstraintSetStatic_:
        public virtual CONSTRAINT1,
        public virtual _StructuralConstraintSetStatic_< OTHER_CONSTRAINTS... > {
      public:
      /// the type of the first constraint
      using first_constraint = CONSTRAINT1;

      /// the type of the next constraints
      using next_constraints = _StructuralConstraintSetStatic_< OTHER_CONSTRAINTS... >;

      // determines the set of all constraints in the set (included inherited
      // ones)
      using allConstraints = typename _ConcatConstraintSet_<
         typename std::conditional<
            std::is_base_of< _StructuralRoot_, CONSTRAINT1 >::value,
            typename _ConcatConstraintSet_< CONSTRAINT1,
                                            typename CONSTRAINT1::allConstraints >::type,
            _ConstraintSet_< CONSTRAINT1 > >::type,
         typename next_constraints::allConstraints >::type;

      /** @brief determines the minimal set of constraints included in the set
       * (removing duplicates)
       *
       * Assume that the class hierarchy is Z->Y->X, T->Y->X and that we have
       *the
       * set of constraints S = <Z,T>, then set of all structural constraints
       * reachable from S is allConstraints = <Z,Y,X,T,Y,X>. minConstraints will
       * reduce this set to <Z,T,Y,X>, i.e., it will remove all duplicates. */
      using minConstraints = typename allConstraints::minset::set;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      _StructuralConstraintSetStatic_();

      /// copy constructor
      _StructuralConstraintSetStatic_(
         const _StructuralConstraintSetStatic_< CONSTRAINT1, OTHER_CONSTRAINTS... >&);

      /// destructor
      ~_StructuralConstraintSetStatic_();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      _StructuralConstraintSetStatic_< CONSTRAINT1, OTHER_CONSTRAINTS... >&
         operator=(const _StructuralConstraintSetStatic_< CONSTRAINT1, OTHER_CONSTRAINTS... >&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new graph from which we will perform checkings
      void setGraph(const DiGraph& graph);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const ArcAddition& change);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const ArcDeletion& change);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const ArcReversal& change);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const GraphChange& change);

      /// indicates whether a change will always violate the constraint
      bool isAlwaysInvalid(const GraphChange& change) const;

      /// checks whether the constraints enable to add arc (x,y)
      bool checkArcAddition(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to remove arc (x,y)
      bool checkArcDeletion(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to reverse arc (x,y)
      bool checkArcReversal(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to add an arc
      bool checkModification(const ArcAddition& change) const;

      /// checks whether the constraints enable to remove an arc
      bool checkModification(const ArcDeletion& change) const;

      /// checks whether the constraints enable to reverse an arc
      bool checkModification(const ArcReversal& change) const;

      /// checks whether the constraints enable to perform a graph change
      bool checkModification(const GraphChange& change) const;

      /// @}
    };

    template < typename CONSTRAINT >
    class _StructuralConstraintSetStatic_< CONSTRAINT >:
        public virtual CONSTRAINT,
        public virtual _StructuralRoot_ {
      public:
      /// the type of the first constraint
      using first_constraint = CONSTRAINT;

      /// the type of the next constraints
      using next_constraints = _StructuralRoot_;

      // determines the set of all constraints in the set (included inherited
      // ones). Rhis produces an  _ConstraintSet_. This type is to be used internally.
      using allConstraints = typename std::conditional<
         std::is_base_of< _StructuralRoot_, CONSTRAINT >::value,
         typename _ConcatConstraintSet_< CONSTRAINT, typename CONSTRAINT::allConstraints >::type,
         _ConstraintSet_< CONSTRAINT > >::type;

      /** @brief determines the minimal set of constraints included in the set
       * (removing duplicates)
       *
       * Assume that the class hierarchy is Z->Y->X, T->Y->X and that we have
       *the
       * set of constraints S = <Z,T>, then set of all structural constraints
       * reachable from S is allConstraints = <Z,Y,X,T,Y,X>. minConstraints will
       * reduce this set to <Z,T,Y,X>, i.e., it will remove all duplicates. */
      using minConstraints = typename allConstraints::minset::set;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      _StructuralConstraintSetStatic_();

      /// copy constructor
      _StructuralConstraintSetStatic_(const _StructuralConstraintSetStatic_< CONSTRAINT >&);

      /// destructor
      ~_StructuralConstraintSetStatic_();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      _StructuralConstraintSetStatic_< CONSTRAINT >&
         operator=(const _StructuralConstraintSetStatic_< CONSTRAINT >&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new graph from which we will perform checkings
      void setGraph(const DiGraph& graph);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const ArcAddition& change);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const ArcDeletion& change);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const ArcReversal& change);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const GraphChange& change);

      /// indicates whether a change will always violate the constraint
      bool isAlwaysInvalid(const GraphChange& change) const;

      /// checks whether the constraints enable to add arc (x,y)
      bool checkArcAddition(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to remove arc (x,y)
      bool checkArcDeletion(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to reverse arc (x,y)
      bool checkArcReversal(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to add an arc
      bool checkModification(const ArcAddition& change) const;

      /// checks whether the constraints enable to remove an arc
      bool checkModification(const ArcDeletion& change) const;

      /// checks whether the constraints enable to reverse an arc
      bool checkModification(const ArcReversal& change) const;

      /// checks whether the constraints enable to perform a graph change
      bool checkModification(const GraphChange& change) const;

      /// @}
    };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    /** @class StructuralConstraintSetStatic
     * @brief the "meta-programming" class for storing structural constraints
     * @ingroup learning_group
     *
     * In aGrUM, there are two ways to store sets of structural constraints: the
     * first one is to put them into a StructuralConstraintSetDynamic. This
     *class
     * allows to add at runtime any constraint you wish. As such, it is very
     * generic but, by not knowing at compile time the constraints that will be
     * stored into the vector, calling its methods has a slight overhead. On the
     * other hand, if you already know all the constraints you wish to apply,
     *the
     * StructuralConstraintSetStatic is better suited because it will compute at
     * compile time how to call the constraint's methods in a most efficient
     *way:
     * if these methods are inlined, there will be no overhead at all when
     *calling
     * these methods.
     *
     * This class is intended to store structural constraints and help applying
     * them during learning in a most efficient way. The idea is that it will
     * compute at compile time the class hierarchy graph of the set of
     *constraints
     * and will determine the set of distinct structural constraints in the set.
     * Then, each time we wish to apply a method, say a graph modification check
     * to all the constraints, the class will apply the methods once on each
     * distinct constraint, hence avoiding duplicates. */
    template < typename CONSTRAINT1, typename... OTHER_CONSTRAINTS >
    class StructuralConstraintSetStatic:
        public virtual _StructuralConstraintSetStatic_< CONSTRAINT1,
                                                        OTHER_CONSTRAINTS... >::minConstraints {
      public:
      using constraints =
         typename _StructuralConstraintSetStatic_< CONSTRAINT1,
                                                   OTHER_CONSTRAINTS... >::minConstraints;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      StructuralConstraintSetStatic();

      /// copy constructor
      StructuralConstraintSetStatic(
         const StructuralConstraintSetStatic< CONSTRAINT1, OTHER_CONSTRAINTS... >&);

      /// destructor
      ~StructuralConstraintSetStatic();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      StructuralConstraintSetStatic< CONSTRAINT1, OTHER_CONSTRAINTS... >&
         operator=(const StructuralConstraintSetStatic< CONSTRAINT1, OTHER_CONSTRAINTS... >&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new graph from which we will perform checkings
      void setGraph(const DiGraph& graph);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const ArcAddition& change);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const ArcDeletion& change);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const ArcReversal& change);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const GraphChange& change);

      /// indicates whether a change will always violate the constraint
      bool isAlwaysInvalid(const GraphChange& change) const;

      /// checks whether the constraints enable to add arc (x,y)
      bool checkArcAddition(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to remove arc (x,y)
      bool checkArcDeletion(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to reverse arc (x,y)
      bool checkArcReversal(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to add an arc
      bool checkModification(const ArcAddition& change) const;

      /// checks whether the constraints enable to remove an arc
      bool checkModification(const ArcDeletion& change) const;

      /// checks whether the constraints enable to reverse an arc
      bool checkModification(const ArcReversal& change) const;

      /// checks whether the constraints enable to perform a graph change
      bool checkModification(const GraphChange& change) const;

      /// @}
    };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    template < typename CONSTRAINT >
    class StructuralConstraintSetStatic< CONSTRAINT >:
        public virtual _StructuralConstraintSetStatic_< CONSTRAINT >::minConstraints {
      public:
      using constraints = typename _StructuralConstraintSetStatic_< CONSTRAINT >::minConstraints;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      StructuralConstraintSetStatic();

      /// copy constructor
      StructuralConstraintSetStatic(const StructuralConstraintSetStatic< CONSTRAINT >&);

      /// destructor
      ~StructuralConstraintSetStatic();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      StructuralConstraintSetStatic< CONSTRAINT >&
         operator=(const StructuralConstraintSetStatic< CONSTRAINT >&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new graph from which we will perform checkings
      void setGraph(const DiGraph& graph);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const ArcAddition& change);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const ArcDeletion& change);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const ArcReversal& change);

      /// notify the constraint of a modification of the graph
      void modifyGraph(const GraphChange& change);

      /// indicates whether a change will always violate the constraint
      bool isAlwaysInvalid(const GraphChange& change) const;

      /// checks whether the constraints enable to add arc (x,y)
      bool checkArcAddition(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to remove arc (x,y)
      bool checkArcDeletion(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to reverse arc (x,y)
      bool checkArcReversal(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to add an arc
      bool checkModification(const ArcAddition& change) const;

      /// checks whether the constraints enable to remove an arc
      bool checkModification(const ArcDeletion& change) const;

      /// checks whether the constraints enable to reverse an arc
      bool checkModification(const ArcReversal& change) const;

      /// checks whether the constraints enable to perform a graph change
      bool checkModification(const GraphChange& change) const;

      /// @}
    };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  } /* namespace learning */

} /* namespace gum */

/// always include the templated function definitions
#include <agrum/BN/learning/constraints/structuralConstraintSetStatic_tpl.h>

#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_SET_STATIC_H */
