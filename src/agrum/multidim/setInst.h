/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *                            {prenom.nom}_at_lip6.fr                      *
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
 * @brief Class for assigning/browsing values to tuples of variables
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

/*!
 * @ingroup multidim_group
 \page SetInst_page How to use a Instatiation ?
 @todo check this (near-automatically translated) english.

 \section inst_sec_1 Basic principles

 MultiDims (see also gum::MultiDimInterface, gum::Potential, gum::CPF, etc ...) are aGrUM's
 multidimensional tables and SetInsts (@ref gum::SetInst) are smart iterators
 over these tables. Since MultDims are containers over random discrete variables
 (see gum::DiscreteVariable) we need when iterating over a MultiDim to indicate which
 variable and which label of this variable we are on. SetInsts does exactly that:
 when you change the value of variable in an SetInst you change the label you're
 looking at in the MultiDim.

 If you look at the MultiDim class hierarchy you will see that SetInst are a
 subclass of gum::MultiDimInterface, this is because SetInsts behaves as one-dimension
 multidimensional table with one value per variable, the value being the index of the
 instantiated label. This imply that you can use an SetInst independently of a
 MultiDim.

 When a SetInst is meant to be used on a MultiDim it is better to register it to
 the MultiDim but there is one restriction: an SetInst can be registered in one
 (and only one) MultiDim if they share the same variables than the SetInst.
 Be careful: in the multidim hierarchy two variables are equal if they are their
 memory address are equal.

 For registering an SetInst to a MultiDim see the following functions:
 gum::SetInst::SetInst(MultiDimAdressable& aMD) or
 gum::SetInst::actAsSlave(MultiDimAdressable& aMD).

 When an SetInst is registered to a MutliDim, the MultiDim is called "the
 master" and the SetInst it's "slave".

 When registered to a MultiDim finding a value in a table and other computation
 are optimized. For example, the complexity of iterating over a MultiDim with a
 registered SetInst will be in O(k), if it is not registered the complexity
 will be in O(n*k). With n being the number of variables in the MultiDim, and k the
 Cartesian product of the variables domain's.

 Why should you use non-registered SetInst? Because they allow to iterate
 over a different set of the MultiDim variable's. The only restriction is the
 SetInst having all the variables present in the MultiDim.

 \section inst_sec_2 Advanced features

 Some methods can be used to perform algebraic operations on SetInsts, for
 example if you have two SetInsts i1 and i2, with i1 being a subset of
 i2 variables, makink a projection of i2 on all variables of i1 will be done by
 i1.chgValIn (i2).

 When incrementing a SetInst, one does not know the variables order used for
 this incrementation. Typically assuming that two hypermatrices
 t1 and t2 are defined on the same set of variables, and that there are two
 SetInsts i1 (registered on t1) and i2 (on t2), then write
 @code for (i1.setFirst (), i2.setFirst ();! i1.end (); i1 ++, i2 ++) t1 [i1] = t2 [i2]; @endcode
 will not create a copy of t2 in t1, because the variables may not be incremented
 in the same order

 There is a way to force the increment: i1.incIn(i2) allows you to increase i1 in
 the same order as i2. Hence make a copy of the table is:
 @code for (i1 . setFirst (), i2.setFirst ();! i1.end (); i1.incIn (i2), i2 ++) t1 [i1] = t2 [i2]; @endcode

 There are other cases where it may be intersting to force the order of operations,
 the only thing to remember is that the methods that ends with In
 do not use the same order as their version without In.

 \section inst_sec_3 Code samples
 T is a hypermatrice of integers defined on a superset of variables T1, we would
 like to add achieve such an operation T += T1 :

@code
SetInst i(T);
SetInst i1(T1);
for(i.setFirstIn(i1), i1.setFirst();
    ! i1.end();
    ++i1, i.incIn(i1)) {
  int content = T1[i1];
  for(i.setFirstOut(i1);
      ! i.end();
      i.incOut(i1)) {
    T[i] += content;
  }
  i.unsetOverflow();
}
@endcode
*/

#ifndef GUM_SETINST_H
#define GUM_SETINST_H

#include <ostream>

#include <agrum/core/utils.h>
#include <agrum/core/bijection.h>
#include <agrum/multidim/multiDimInterface.h>
#include <agrum/multidim/multiDimAdressable.h>


namespace gum {

  class MultiDimAdressable;
  class Instantiation;
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                           GUM_SetInst                          === */
  /* ============================================================================ */
  /* ============================================================================ */
  /**
   * @class SetInst SetInst.h agrum/multidim/SetInst.h
   * @brief Class for assigning/browsing values to tuples of discrete variables.
   * @ingroup multidim_group
   *
   * SetInst is designed to assign values to tuples of variables and to
   * efficiently loop over values of subsets of variables. This class can be used in
   * two different flavors:
   *
   * # the tuple of variables in the SetInst is related to a
   * multidimensional array and, when we loop over the possible values of the tuple,
   * we also loop at the same time over the corresponding values in the array.
   * # the tuple of variables in the SetInst is not related to a
   * multidimensional array and we can loop over the possible values of the tuple
   * without looping over values fo any array.
   *
   * a SetInst can be associated/deassociated to a given multidimensional
   * array using the gum::MultiDimAdressable::registerSlave(SetInst& i) and
   * gum::MultiDimAdressable::unregisterSlave functions. Note that, to be
   * registrable, the SetInst must have precisely the same variables as
   * the array. As a consequence, adding or removing a variable from a
   * SetInst associated to an array will unregister it. This behavior
   * is compulsory as, if it were still associated, it would not be possible
   * to retrieve a correct value of the array given a value of the SetInst.
   * For instance, if M[A,B,C] is an array indexed by Boolean variables A,B,C,
   * Which value of M should be returned if B=0 and C=0? We do not know for sure as
   * we do not know the value of A. Note also that, at any time, you can unregister
   * a SetInst from its master multidimensional array and you can ask to
   * associate it (provided the tuple of variable match).
   *
   * To print information about a SetInst use the following function:
   * @see operator<<(std::ostream&, const SetInst&)
   */
  class SetInst  {
    public:
      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{
      // ============================================================================
      /// Default constructor: creates an empty tuple.
      // ============================================================================
      SetInst();

      // ============================================================================
      /**
       * @brief Copy constructor.
       *
       * Note that the SetInst is by default associated to the same
       * MultiDimAdressable as aI. This means that looping over values of the tuple
       * will induce looping over the values of the MultiDimAdressable. Similarly,
       * the value of the tuple is that of aI, and, if the SetInst is slaved,
       * its master is notified of the value of the SetInst if notifyMaster
       * is true.
       * @param aI the SetInst we copy
       * @param notifyMaster whether or not notify master if exits
       */
      // ============================================================================
      SetInst( const SetInst& aI);
      SetInst( const Instantiation& aI);

      // ============================================================================
      /**
       * @brief Copy operator.
       *
       * If this is a slave but not the same as aI's master, if not in the same
       * variables set, throws an exception (OperationNotAllowed) else calls
       * chgValIn(aI).
       * If this is a slave with the same master as aI, calls chgValIn(aI)
       * If this is not a slave, copies aI.
       * @throw OperationNotAllowed
       */
      // ============================================================================
      SetInst& operator=( const SetInst& aI );

      // ============================================================================
      /**
       * @brief Constructor for a SetInst of all the variables of a
       *        MultiDimAdressable.
       *
       * The variables of the SetInst are those of aMD
       * (actually, they are shared in memory). All the variables of aMD belong to
       * the tuple of variables to be instantiated.
       *
       * Note that the SetInst is by default associated to aMD, i.e., looping
       * over values of the tuple will induce looping over the values of aMD. The
       * value given to the tuple is the first possible value, that is, (0,...,0).
       * If the SetInst is slaved, its master is notified of the value of the
       * SetInst.
       *
       * @param aMD the array the variables of which are those of the SetInst
       */
      // ============================================================================
      SetInst( MultiDimAdressable& aMD );

      // ============================================================================
      /**
       * @brief Constructor for a SetInst of all the variables of a
       *        MultiDimAdressable.
       *
       * The variables of the SetInst are those of aMD
       * (actually, they are shared in memory). All the variables of aMD belong to
       * the tuple of variables to be instantiated.
       *
       * Note that the SetInst is by default associated to aMD, i.e., looping
       * over values of the tuple will induce looping over the values of aMD. The
       * value given to the tuple is the first possible value, that is, (0,...,0).
       * If the SetInst is slaved, its master is notified of the value of the
       * SetInst.
       *
       * @param aMD the array the variables of which are those of the SetInst
       */
      // ============================================================================
      SetInst( const MultiDimAdressable& aMD );

      // ============================================================================
      /**
       * @brief Constructor for a SetInst of all the variables of a
       *        MultiDimAdressable.
       *
       * The variables of the SetInst are those of aMD
       * (actually, they are shared in memory). All the variables of aMD belong to
       * the tuple of variables to be instantiated.
       *
       * Note that the SetInst is by default associated to aMD, i.e., looping
       * over values of the tuple will induce looping over the values of aMD. The
       * value given to the tuple is the first possible value, that is, (0,...,0).
       * If the SetInst is slaved, its master is notified of the value of the
       * SetInst.
       *
       * @param aMD the array the variables of which are those of the SetInst
       */
      // ============================================================================
      SetInst( MultiDimAdressable* aMD );

      // ============================================================================
      /**
       * @brief Constructor for a SetInst of all the variables of a
       *        MultiDimAdressable.
       *
       * The variables of the SetInst are those of aMD
       * (actually, they are shared in memory). All the variables of aMD belong to
       * the tuple of variables to be instantiated.
       *
       * Note that the SetInst is by default associated to aMD, i.e., looping
       * over values of the tuple will induce looping over the values of aMD. The
       * value given to the tuple is the first possible value, that is, (0,...,0).
       * If the SetInst is slaved, its master is notified of the value of the
       * SetInst.
       *
       * @param aMD the array the variables of which are those of the SetInst
       */
      // ============================================================================
      SetInst( const MultiDimAdressable* aMD );

      // ============================================================================
      /// Destructor.
      // ============================================================================
      ~SetInst();

      /// @}

      // ##############################################################################
      /// @name Accessors / Modifiers
      // ##############################################################################
      /// @{

      // ============================================================================
      /// Returns the number of variables in the SetInst.
      // ============================================================================
      Idx nbrDim() const ;

      // ============================================================================
      /**
       * @brief Adds a new variable in the SetInst.
       *
       * If variable v already belongs to the SetInst tuple of variables,
       * then DuplicateElement is thrown in this case. The
       * value of the new variable is set to that of index 0, that is, the first
       * possible value for the variable.
       * Since an SetInst must share the same set of variables with his master
       * an OperationNotAllowed is raised if you try to add a variable of a slaved
       * SetInst.
       *
       * @param v the new var
       * @warning variable v is known to the SetInst only by a pointer to it.
       *          As a result, this is not a copy of v that is used by SetInst
       *          But rather v itself. As such, v should never be deleted from memory
       *          until the SetInst is removed.
       * @throw DuplicateElement
       * @throw OperationNotAllowed
       */
      // ============================================================================
      void add( const DiscreteVariable& v );

      // ============================================================================
      /**
       * @brief Removes a variable from the SetInst.
       *
       * If variable v does not belong to the SetInst tuple of variables,
       * then NotFound is thrown.
       * Since an SetInst must share the same set of variables with his master
       * an OperationNotAllowed is raised if you try to remove a variable of a slaved
       * SetInst.
       *
       * @param v the variable to be erased from the tuple
       * @throw NotFound
       * @throw OperationNotAllowed Raised if the SetInst is a slave.
       */
      // ============================================================================
      void erase( const DiscreteVariable& v );

      /**
       * @brief Erase all variables from an SetInst
       * @throw OperationNotAllowed Raised if the SetInst is a slave.
       */
      void clear();

      // ============================================================================
      /// Returns the product of the variable's domain size in the SetInst.
      // ============================================================================
      Size domainSize() const ;

      // ============================================================================
      /**
       * Returns the position of the variable v.
       * @throw NotFound Raised if v does not belong to the SetInst.
       */
      // ============================================================================
      Idx pos( const DiscreteVariable& v ) const;
      // ============================================================================
      /**
       * Returns the current value of the variable at position i.
       *
       * @warning For speed issues, the function does not actually check whether the
       *          overflow flag is set before returning the current value of the
       *          variable as, usually, it is not necessary. If need be, use function
       *          inOverflow() to check.
       *
       * @param i The index of the variable.
       * @throw NotFound Raised if the element cannot be found.
       */
      // ============================================================================
      Size vals( const Idx i ) const;

      // ============================================================================
      /**
       * Returns the current value of a given variable.
       *
       * @warning For speed issues, the function does not actually check whether the
       *          overflow flag is set before returning the current value of the
       *          variable as, usually, it is not necessary. If need be, use function
       *          inOverflow() to check.
       *
       * @param var The variable the value of which we wish to know.
       * @throw NotFound Raised it var does not belong to the SetInst.
       */
      // ============================================================================
      Size vals( const DiscreteVariable& var ) const;

      // ============================================================================
      /**
       * Returns the current value of a given variable.
       *
       * @warning For speed issues, the function does not actually check whether the
       *          overflow flag is set before returning the current value of the
       *          variable as, usually, it is not necessary. If need be, use function
       *          inOverflow() to check.
       *
       * @param var the variable the value of which we wish to know
       * @throw NotFound Raised if var does not belong to the SetInst.
       */
      // ============================================================================
      Size vals( const DiscreteVariable* var ) const;

      // ============================================================================
      /**
       * Returns the current value of the variable at position i.
       *
       * @warning For speed issues, the function does not actually check whether the
       *          overflow flag is set before returning the current value of the
       *          variable as, usually, it is not necessary. If need be, use function
       *          inOverflow() to check.
       *
       * @param i The index of the variable.
       * @throw NotFound Raised if the element cannot be found.
       */
      // ============================================================================

      Idx nbrOccurences(const DiscreteVariable& var) const;

      Idx val( const Idx i ) const;

      // ============================================================================
      /**
       * Returns the current value of a given variable.
       *
       * @warning For speed issues, the function does not actually check whether the
       *          overflow flag is set before returning the current value of the
       *          variable as, usually, it is not necessary. If need be, use function
       *          inOverflow() to check.
       *
       * @param var The variable the value of which we wish to know.
       * @throw NotFound Raised it var does not belong to the SetInst.
       */
      // ============================================================================
      Idx val( const DiscreteVariable& var ) const;

      // ============================================================================
      /**
       * Returns the current value of a given variable.
       *
       * @warning For speed issues, the function does not actually check whether the
       *          overflow flag is set before returning the current value of the
       *          variable as, usually, it is not necessary. If need be, use function
       *          inOverflow() to check.
       *
       * @param var the variable the value of which we wish to know
       * @throw NotFound Raised if var does not belong to the SetInst.
       */
      // ============================================================================
      Idx val( const DiscreteVariable* var ) const;

      // ============================================================================
      /**
       * Returns the variable at position i in the tuple.
       *
       * @param i the index of the variable
       * @throw NotFound Raised if the element cannot be found.
       */
      // ============================================================================
      const DiscreteVariable& variable( const Idx i ) const;

      // ============================================================================
      /**
       * @brief Assign newval to variable v in the SetInst.
       *
       * In addition to modifying the value of the variable, the SetInst
       * informs its master of the modification. This function also unsets the
       * overflow flag.
       *
       * @param v The variable whose value is assigned.
       * @param newval The index of the value assigned (consider the values of v as
       *               an array indexed from 0 to n of values (which might be anything
       *               from real numbers to strings, etc). Parameter newval indicates
       *               the index in this array of the new value taken by v.
       * @return Returns a reference to *this in order to chain the chgVal.
       * @throw NotFound Raised if variable v does not belong to the SetInst.
       * @throw OutOfBound Raised if newval is not a possible value for v.
       */
      // ============================================================================
      SetInst& chgVal( const DiscreteVariable& v, const Idx newval );

      // ============================================================================
      /**
       * @brief Assign newval to variable v in the SetInst.
       *
       * In addition to modifying the value of the variable, the SetInst
       * informs its master of the modification. This function also unsets the
       * overflow flag.
       *
       * @param v The variable whose value is assigned.
       * @param newval The index of the value assigned (consider the values of v as
       *               an array indexed from 0 to n of values (which might be anything
       *               from real numbers to strings, etc). Parameter newval indicates
       *               the index in this array of the new value taken by v.
       * @return Returns a reference to *this in order to chain the chgVal.
       * @throw NotFound Raised if variable v does not belong to the SetInst.
       * @throw OutOfBound Raised if newval is not a possible value for v.
       */
      // ============================================================================
      SetInst& chgVal( const DiscreteVariable* v, const Idx newval );

      // ============================================================================
      /**
       * @brief Assign newval to variable at position varPos in the SetInst.
       *
       * In addition to modifying the value of the variable, the SetInst
       * informs its master of the modification. This function also unsets the
       * overflow flag.
       *
       * @param varPos The index of the variable whose value is assigned in the
       *        tuple of variables of the SetInst.
       * @param newval The index of the value assigned (consider the values of the
       *        variable as an array indexed from 0 to n of values (which might be
       *        anything from real numbers to strings, etc). Parameter newval
       *        indicates the index in this array of the new value taken by the
       *        variable.
       * @return A reference to *this in order to chain the chgVal.
       * @throw NotFound Raised if the variable does not belong to this
       * @throw OutOfBound Raised if newval is not a possible value for
       *        the variable
       */
      // ============================================================================
      SetInst& chgVal( const Idx varPos, const Idx newval );

      // ============================================================================
      /**
       * @brief Assign newval to variable v in the SetInst.
       *
       * In addition to modifying the value of the variable, the SetInst
       * informs its master of the modification. This function also unsets the
       * overflow flag.
       *
       * @param v The variable whose value is assigned.
       * @param newval The index of the value assigned (consider the values of v as
       *               an array indexed from 0 to n of values (which might be anything
       *               from real numbers to strings, etc). Parameter newval indicates
       *               the index in this array of the new value taken by v.
       * @return Returns a reference to *this in order to chain the chgVal.
       * @throw NotFound Raised if variable v does not belong to the SetInst.
       * @throw OutOfBound Raised if newval is not a possible value for v.
       */
      // ============================================================================
       SetInst& chgVals( const DiscreteVariable& v, const Size newval );
       SetInst& addVal(const DiscreteVariable& v, const Idx newVal );
       SetInst& addVals(const DiscreteVariable& v, const Size newVal ); 
       SetInst& remVal(const DiscreteVariable& v, const Idx newVal );
       SetInst& remVals(const DiscreteVariable& v, const Size newVal ); 
       SetInst& interVals(const DiscreteVariable& v, const Size newVal ); 
       SetInst& interVal(const DiscreteVariable& v, const Idx newVal ); 

      // ============================================================================
      /**
       * @brief Assign newval to variable v in the SetInst.
       *
       * In addition to modifying the value of the variable, the SetInst
       * informs its master of the modification. This function also unsets the
       * overflow flag.
       *
       * @param v The variable whose value is assigned.
       * @param newval The index of the value assigned (consider the values of v as
       *               an array indexed from 0 to n of values (which might be anything
       *               from real numbers to strings, etc). Parameter newval indicates
       *               the index in this array of the new value taken by v.
       * @return Returns a reference to *this in order to chain the chgVal.
       * @throw NotFound Raised if variable v does not belong to the SetInst.
       * @throw OutOfBound Raised if newval is not a possible value for v.
       */
      // ============================================================================
      SetInst& chgVals( const DiscreteVariable* v, const Size newval );
      SetInst& addVal( const DiscreteVariable* v, const Idx newVal );
      SetInst& addVals(const DiscreteVariable* v, const Size newVal ); 
      SetInst& remVal(const DiscreteVariable* v, const Idx newVal );
      SetInst& remVals(const DiscreteVariable* v, const Size newVal ); 
      SetInst& interVals(const DiscreteVariable* v, const Size newVal ); 
      SetInst& interVal(const DiscreteVariable* v, const Idx newVal ); 

      // ============================================================================
      /**
       * @brief Assign newval to variable at position varPos in the SetInst.
       *
       * In addition to modifying the value of the variable, the SetInst
       * informs its master of the modification. This function also unsets the
       * overflow flag.
       *
       * @param varPos The index of the variable whose value is assigned in the
       *        tuple of variables of the SetInst.
       * @param newval The index of the value assigned (consider the values of the
       *        variable as an array indexed from 0 to n of values (which might be
       *        anything from real numbers to strings, etc). Parameter newval
       *        indicates the index in this array of the new value taken by the
       *        variable.
       * @return A reference to *this in order to chain the chgVal.
       * @throw NotFound Raised if the variable does not belong to this
       * @throw OutOfBound Raised if newval is not a possible value for
       *        the variable
       */
      // ============================================================================
      SetInst& chgVals( const Idx varPos, const Size newval );

      SetInst& addVal( Idx varPos, const Idx newVal );
      SetInst& addVals( Idx varPos, const Size newVal ); 
      SetInst& remVal( Idx varPos, const Idx newVal );
      SetInst& remVals( Idx varPos, const Size newVal ); 
      SetInst& chgDifVal( Idx varPos, const Size newVal ); 
      SetInst& interVals( Idx varPos, const Size newVal ); 
      SetInst& interVal( Idx varPos, const Idx newVal ); 

      // ============================================================================
      /**
       * @brief Assign the values from i in the SetInst.
       *
       * For any variable in i and in *this, value of the variable in i is assigned
       * to the variable in *this.
       *
       * In addition of modifying the value of the variables in *this, the
       * SetInst informs its master MultiDimAdressable of the modification.
       * This function also unsets the overflow flag.
       *
       * If no variables in i matches, then no value is changed.
       *
       * @param i A SetInst in which the new values are searched.
       * @return Returns a reference to *this in order to chain the chgVal.
       */
      // ============================================================================
      SetInst& chgValIn( const SetInst& i );

      // ============================================================================
      /**
       * Indicates whether a given variable belongs to the SetInst.
       *
       * @param v The variable for which the test is made.
       * @return Returns true if v is in the SetInst.
       */
      // ============================================================================
      bool contains( const DiscreteVariable& v ) const ;

      // ============================================================================
      /**
       * Indicates whether a given variable belongs to the SetInst.
       *
       * @param v A pointer on the variable for which the test is made.
       * @return Returns true if *v is in the SetInst.
       */
      // ============================================================================
      bool contains( const DiscreteVariable* v ) const ;

      // ============================================================================
      /// Returns the sequence of DiscreteVariable of this SetInst.
      // ============================================================================
      const Sequence<const DiscreteVariable *>& variablesSequence() const ;

      // ============================================================================
      /// Returns true if the SetInst is empty.
      // ============================================================================
      virtual bool empty( void ) const ;

      /// @}

      // ############################################################################
      /// @name Overflow management methods.
      // ############################################################################
      /// @{

      // ============================================================================
      /**
       * @brief Indicates whether the current value of the tuple is correct or not.
       *
       * The function inOverflow() is used to flag overflowed operation (for
       * instance, ++ on the last value or -- on the first value will produce an
       * incorrect value of the tuple. Hence inOverflow() is used as an
       * end()/rend() function for loops on SetInst.
       *
       * @code
       * for(SetInst i.setFirst(); !i.inOverflow(); ++i) {
       *   // code...
       * }
       * @endcode
       */
      // ============================================================================
      bool inOverflow() const ;

      // ============================================================================
      /**
       * @brief Removes the flag overflow. See full documentation for details.
       * (Recommended).
       *
       * When we use multiple inner loops w.r.t. a given SetInst, it may
       * happen that one inner loop reaches the end() of the SetInst while
       * the outer loops do not have reached it. This means that the inner loop has
       * toggled the overflow flag. To enable the other loops to go on, we must
       * unset this flag using function unsetOverflow(). For instance, assume that
       * Prob represents probability P(b|a,c), then normalizing this proba can be
       * performed using the following code:
       *
       * @code
       * // assume the probability has been defined somewhere:
       * MultiDimArray<double> Prob;
       *
       * // create 2 SetInsts for the 2 necessary loops
       * SetInst i(Prob), j;
       * j << a << c;
       * double delta;
       *
       * // outer loop: loop over the values of b
       * for(i.setFirstIn(j); !i.end(); i.incIn(j))
       * {
       *   delta = 0.0;
       *   // inner loop: loop over the values of a and c
       *   for(i.setFirstOut(j); !i.end(); i.incerr(j))
       *     delta += dd[i];
       *   for(i.setFirstOut(j); !i.end(); i.incerr(j))
       *     dd[i] /= delta;
       *   // indicate that the end() reached after looping over a and c does not
       *   // correspond to an end() for the loop w.r.t. b
       *   i.unsetOverflow();
       * }
       * @endcode
       */
      // ============================================================================
      void unsetOverflow();

      // ============================================================================
      /**
       * Alias for unsetOverflow().
       * @see unsetOverflow().
       */
      // ============================================================================
      void unsetEnd();

      // ============================================================================
      /**
       * Returns true if the SetInst reached the end.
       *
       * Function end() should be used as in:
       * @code
       * for(SetInst i.setFirst();! i.end(); ++i) {
       *   // code
       * }
       * @endcode
       */
      // ============================================================================
      bool end() const ;

      // ============================================================================
      /**
       * Returns true if the SetInst reached the rend.
       *
       * Function end() should be used as in:
       * @code
       * for(SetInst i.setLast();! i.rend(); --i) {
       *   // code
       * }
       * @endcode
       */
      // ============================================================================
      bool rend() const ;

      /// @}
 /// @{




      // ============================================================================
      /// Returns the hamming distance of this SetInst.
      // ============================================================================
      Idx hamming() const ;

      // ============================================================================
      /// Give a string version of SetInst.
      // ============================================================================
      std::string toString() const ;

      // ============================================================================
      /**
       * @brief Reorder vars of this SetInst giving the order in v.
       *
       * In the new order variables common to v and *this are placed first, then
       * variables only in *this.
       *
       * The variables only in v are ignored.
       */
      // ============================================================================
      void reorder( const Sequence<const DiscreteVariable*>& v );

      // ============================================================================
      /**
       * Calls reorder(const Sequence<const DiscreteVariable*>&) with
       * i.variablesSequence()
       */
      // ============================================================================
      void reorder( const SetInst& i );

      /// @}

      // ############################################################################
      /// @name Static methods
      // ############################################################################
      /// @{

      /**
       * @brief Assign the values of i in j, using bij as a bijection between i and
       *        j variables.
       *
       * @param bij Firsts are variables in i and seconds are variables in j.
       * @param i An SetInst used to change the values in j.
       * @param j An SetInst which receives new values from i using bij.
       *
       * @throw NotFound Raised if a variable in i does not point to a variable in j
       *                 or if a variable in i is missing in bij.
       */
      static void assign_values(Bijection<const DiscreteVariable*, const DiscreteVariable*>& bij,
          const SetInst& i, SetInst& j) {
        try {
          for (Sequence<const DiscreteVariable*>::const_iterator iter = i.variablesSequence().begin();
              iter != i.variablesSequence().end(); ++iter) {
            j.chgVal(bij.second(*iter), i.val(*iter));
          }
        } catch (NotFound&) {
          GUM_ERROR(NotFound, "missing variable in bijection or SetInst");
        }
      }
      /// @}

  protected:

      /// Replace x by y.
      virtual void _swap(const DiscreteVariable* x, const DiscreteVariable* y);

  private:

      // ============================================================================
      /**
       * The master, if any, contains precisely the set of variables to be
       * instantiated.
       */
      // ============================================================================
      //MultiDimAdressable* __master;

      // ============================================================================
      /// The tuple of variables to be instantiated.
      // ============================================================================
      Sequence<const DiscreteVariable*> __vars;

      // ============================================================================
      /// The current SetInst: the value of the tuple.
      // ============================================================================
      std::vector<Size> __vals;

      // ============================================================================
      /**
       * indicates whether the current value of the tuple is valid
       * when we loop sufficiently over values of the tuple, we may have browsed
       * all the possible values and we have to know in a way or another that
       * the tuple contains no more value. This is precisely the meaning of
       * Boolean overflow
       */
      // ============================================================================
      bool __overflow;

      // ============================================================================
      /// swap 2 vars in the SetInst
      // ============================================================================
      void __swap( Idx i,Idx j );

      // ============================================================================
      /**
       *  modifies internally the value of a given variable of the sequence
       *  In addition to modifying the value of the variable, the SetInst
       *  informs its master MultiDimAdressable of the modification
       */
      // ============================================================================
      void __chgVal( const Idx varPos, const Idx newVal );
      void __chgVals( const Idx varPos, const Size newVal );

      // ============================================================================
      /**
       *  adds a new var to the sequence of vars
       *  If variable v already belongs to the SetInst tuple of variables,
       *  then nothing is done. In particular, no exception is thrown in this case.
       *  @param v the new var
       *  @ warning note that this function does not deassociate the SetInst
       *  from its master MultiDimAdressable, if any. To do so, use function
       *  add instead.
       *  @warning this function does not notify the master MultiDimAdressable,
       *  if any. Use in addition function chgVal or __chgVal if need be.
       *  @warning variable v is known to the SetInst only by a pointer to it.
       *  As a result, this is not a copy of v that is used by SetInst but
       *  rather v itself. As such, v should never be deleted from memory until the
       *  SetInst is removed.
       *  @throw DuplicateElement
       */
      // ============================================================================
      void __add( const DiscreteVariable& v );

      // ============================================================================
      /**
       *  removes a variable from the sequence of vars
       *  If variable v does not belong to the SetInst tuple of variables,
       *  then nothing is done. In particular, no exception is thrown in this case.
       *  @param v the variable to be erased from the tuple
       *  @warning this function does not notify the master MultiDimAdressable, if any.
       *  @warning note that this function does not deassociate the SetInst
       *  from its master MultiDimAdressable, if any. To do so, use function
       *  removeDim instead.
       */
      // ============================================================================
      void __erase( const DiscreteVariable& v );

      // ============================================================================
      /**
       *  This function is called by the master (if any) when changes arise in its
       *  vars list.
       *  @warning No implementation of this method?
       *
       *  @param v the new vars list
       */
      // ============================================================================
      void __notifiedDimChanged( const Sequence<const DiscreteVariable *>&  v );

      // ============================================================================
      /// initialisation (same in 4 constructors)
      // ============================================================================
      void __init( MultiDimAdressable* master );
};

// ============================================================================
/// Print information of the SetInst in the stream.
// ============================================================================
std::ostream& operator<< ( std::ostream&, const SetInst& );
gum::SetInst& operator<< ( gum::SetInst& inst, const gum::DiscreteVariable& i );
gum::SetInst& operator>> ( gum::SetInst& inst, const gum::DiscreteVariable& i );

} /* namespace gum */


// ============================================================================
#ifndef GUM_NO_INLINE
#include <agrum/multidim/setInst.inl>
#endif /* GUM_NO_INLINE */
// ============================================================================
#endif /* GUM_SetInst_H */
// ============================================================================
