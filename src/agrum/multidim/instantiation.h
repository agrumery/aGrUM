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
 \page instantiation_page How to use a Instatiation ?
 @todo check this (near-automatically translated) english.

 \section inst_sec_1 Basic principles

 MultiDims (see also gum::MultiDimInterface, gum::Potential, gum::CPF, etc ...) are
 aGrUM's
 multidimensional tables and instantiations (@ref gum::Instantiation) are smart
 iterators
 over these tables. Since MultDims are containers over random discrete variables
 (see gum::DiscreteVariable) we need when iterating over a MultiDim to indicate which
 variable and which label of this variable we are on. Instantiations does exactly
 that:
 when you change the value of variable in an instantiation you change the label
 you're
 looking at in the MultiDim.

 If you look at the MultiDim class hierarchy you will see that instantiation are a
 subclass of gum::MultiDimInterface, this is because instantiations behaves as
 one-dimension
 multidimensional table with one value per variable, the value being the index of the
 instantiated label. This imply that you can use an instantiation independently of a
 MultiDim.

 When a instantiation is meant to be used on a MultiDim it is better to register it
 to
 the MultiDim but there is one restriction: an instantiation can be registered in one
 (and only one) MultiDim if they share the same variables than the instantiation.
 Be careful: in the multidim hierarchy two variables are equal if they are their
 memory address are equal.

 For registering an instantiation to a MultiDim see the following functions:
 gum::Instantiation::Instantiation(MultiDimAdressable& aMD) or
 gum::Instantiation::actAsSlave(MultiDimAdressable& aMD).

 When an instantiation is registered to a MutliDim, the MultiDim is called "the
 master" and the instantiation it's "slave".

 When registered to a MultiDim finding a value in a table and other computation
 are optimized. For example, the complexity of iterating over a MultiDim with a
 registered instantiation will be in O(k), if it is not registered the complexity
 will be in O(n*k). With n being the number of variables in the MultiDim, and k the
 Cartesian product of the variables domain's.

 Why should you use non-registered instantiation? Because they allow to iterate
 over a different set of the MultiDim variable's. The only restriction is the
 instantiation having all the variables present in the MultiDim.

 \section inst_sec_2 Advanced features

 Some methods can be used to perform algebraic operations on instantiations, for
 example if you have two instantiations i1 and i2, with i1 being a subset of
 i2 variables, makink a projection of i2 on all variables of i1 will be done by
 i1.chgValIn (i2).

 When incrementing a Instantiation, one does not know the variables order used for
 this incrementation. Typically assuming that two hypermatrices
 t1 and t2 are defined on the same set of variables, and that there are two
 instantiations i1 (registered on t1) and i2 (on t2), then write
 @code for (i1.setFirst (), i2.setFirst ();! i1.end (); i1 ++, i2 ++) t1 [i1] = t2
 [i2]; @endcode
 will not create a copy of t2 in t1, because the variables may not be incremented
 in the same order

 There is a way to force the increment: i1.incIn(i2) allows you to increase i1 in
 the same order as i2. Hence make a copy of the table is:
 @code for (i1 . setFirst (), i2.setFirst ();! i1.end (); i1.incIn (i2), i2 ++) t1
 [i1] = t2 [i2]; @endcode

 There are other cases where it may be intersting to force the order of operations,
 the only thing to remember is that the methods that ends with In
 do not use the same order as their version without In.

 \section inst_sec_3 Code samples
 T is a hypermatrice of integers defined on a superset of variables T1, we would
 like to add achieve such an operation T += T1 :

 @code
 Instantiation i(T);
 Instantiation i1(T1);
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

#ifndef GUM_INSTANTIATION_H
#define GUM_INSTANTIATION_H

#include <ostream>

#include <agrum/config.h>

#include <agrum/core/hashTable.h>
#include <agrum/multidim/multiDimInterface.h>

namespace gum {

  class MultiDimAdressable;

  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                           GUM_INSTANTIATION                          === */
  /* ============================================================================ */
  /* ============================================================================ */
  /**
   * @class Instantiation instantiation.h agrum/multidim/instantiation.h
   * @brief Class for assigning/browsing values to tuples of discrete variables.
   * @ingroup multidim_group
   *
   * Instantiation is designed to assign values to tuples of variables and to
   * efficiently loop over values of subsets of variables. This class can be used in
   * two different flavors:
   *
   * # the tuple of variables in the Instantiation is related to a
   * multidimensional array and, when we loop over the possible values of the tuple,
   * we also loop at the same time over the corresponding values in the array.
   * # the tuple of variables in the Instantiation is not related to a
   * multidimensional array and we can loop over the possible values of the tuple
   * without looping over values fo any array.
   *
   * a Instantiation can be associated/deassociated to a given multidimensional
   * array using the gum::MultiDimAdressable::registerSlave(Instantiation& i) and
   * gum::MultiDimAdressable::unregisterSlave functions. Note that, to be
   * registrable, the Instantiation must have precisely the same variables as
   * the array. As a consequence, adding or removing a variable from a
   * Instantiation associated to an array will unregister it. This behavior
   * is compulsory as, if it were still associated, it would not be possible
   * to retrieve a correct value of the array given a value of the Instantiation.
   * For instance, if M[A,B,C] is an array indexed by Boolean variables A,B,C,
   * Which value of M should be returned if B=0 and C=0? We do not know for sure as
   * we do not know the value of A. Note also that, at any time, you can unregister
   * a Instantiation from its master multidimensional array and you can ask to
   * associate it (provided the tuple of variable match).
   *
   * To print information about a Instantiation use the following function:
   * @see operator<<(std::ostream&, const Instantiation&)
   */

  class Instantiation : public MultiDimInterface {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Default constructor: creates an empty tuple.

    Instantiation();

    /**
     * @brief Copy constructor.
     *
     * Note that the Instantiation is by default associated to the same
     * MultiDimAdressable as aI. This means that looping over values of the tuple
     * will induce looping over the values of the MultiDimAdressable. Similarly,
     * the value of the tuple is that of aI, and, if the Instantiation is slaved,
     * its master is notified of the value of the Instantiation if notifyMaster
     * is true.
     * @param aI the Instantiation we copy
    * @param notifyMaster whether or not notify master if exits
     */

    Instantiation(const Instantiation &aI, const bool notifyMaster = true);

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
    Instantiation &operator=(const Instantiation &aI);

    /**
     * @brief Constructor for a Instantiation of all the variables of a
     *        MultiDimAdressable.
     *
     * The variables of the Instantiation are those of aMD
     * (actually, they are shared in memory). All the variables of aMD belong to
     * the tuple of variables to be instantiated.
     *
     * Note that the Instantiation is by default associated to aMD, i.e., looping
     * over values of the tuple will induce looping over the values of aMD. The
     * value given to the tuple is the first possible value, that is, (0,...,0).
     * If the Instantiation is slaved, its master is notified of the value of the
     * Instantiation.
     *
     * @param aMD the array the variables of which are those of the Instantiation
     */
    Instantiation(MultiDimAdressable &aMD);

    /**
     * @brief Constructor for a Instantiation of all the variables of a
     *        MultiDimAdressable.
     *
     * The variables of the Instantiation are those of aMD
     * (actually, they are shared in memory). All the variables of aMD belong to
     * the tuple of variables to be instantiated.
     *
     * Note that the Instantiation is by default associated to aMD, i.e., looping
     * over values of the tuple will induce looping over the values of aMD. The
     * value given to the tuple is the first possible value, that is, (0,...,0).
     * If the Instantiation is slaved, its master is notified of the value of the
     * Instantiation.
     *
     * @param aMD the array the variables of which are those of the Instantiation
     */
    Instantiation(const MultiDimAdressable &aMD);

    /**
     * @brief Constructor for a Instantiation of all the variables of a
     *        MultiDimAdressable.
     *
     * The variables of the Instantiation are those of aMD
     * (actually, they are shared in memory). All the variables of aMD belong to
     * the tuple of variables to be instantiated.
     *
     * Note that the Instantiation is by default associated to aMD, i.e., looping
     * over values of the tuple will induce looping over the values of aMD. The
     * value given to the tuple is the first possible value, that is, (0,...,0).
     * If the Instantiation is slaved, its master is notified of the value of the
     * Instantiation.
     *
     * @param aMD the array the variables of which are those of the Instantiation
     */
    Instantiation(MultiDimAdressable *aMD);

    /**
     * @brief Constructor for a Instantiation of all the variables of a
     *        MultiDimAdressable.
     *
     * The variables of the Instantiation are those of aMD
     * (actually, they are shared in memory). All the variables of aMD belong to
     * the tuple of variables to be instantiated.
     *
     * Note that the Instantiation is by default associated to aMD, i.e., looping
     * over values of the tuple will induce looping over the values of aMD. The
     * value given to the tuple is the first possible value, that is, (0,...,0).
     * If the Instantiation is slaved, its master is notified of the value of the
     * Instantiation.
     *
     * @param aMD the array the variables of which are those of the Instantiation
     */
    Instantiation(const MultiDimAdressable *aMD);

    /// Destructor.
    ~Instantiation();

    /// @}

    // ##############################################################################
    /// @name Accessors / Modifiers
    // ##############################################################################
    /// @{

    /// Returns the number of variables in the Instantiation.
    Idx nbrDim() const;

    /**
     * @brief Adds a new variable in the Instantiation.
     *
     * If variable v already belongs to the Instantiation tuple of variables,
     * then DuplicateElement is thrown in this case. The
     * value of the new variable is set to that of index 0, that is, the first
     * possible value for the variable.
     * Since an instantiation must share the same set of variables with his master
     * an OperationNotAllowed is raised if you try to add a variable of a slaved
     * instantiation.
     *
     * @param v the new var
     * @warning variable v is known to the Instantiation only by a pointer to it.
     *          As a result, this is not a copy of v that is used by Instantiation
     *          But rather v itself. As such, v should never be deleted from memory
     *          until the Instantiation is removed.
     * @throw DuplicateElement
     * @throw OperationNotAllowed
     */
    void add(const DiscreteVariable &v);

    /**
     * @brief Removes a variable from the Instantiation.
     *
     * If variable v does not belong to the Instantiation tuple of variables,
     * then NotFound is thrown.
     * Since an instantiation must share the same set of variables with his master
     * an OperationNotAllowed is raised if you try to remove a variable of a slaved
     * instantiation.
     *
     * @param v the variable to be erased from the tuple
     * @throw NotFound
     * @throw OperationNotAllowed Raised if the instantiation is a slave.
     */
    void erase(const DiscreteVariable &v);

    /**
     * @brief Erase all variables from an Instantiation
     * @throw OperationNotAllowed Raised if the instantiation is a slave.
     */
    void clear();

    /// Returns the product of the variable's domain size in the Instantiation.
    Size domainSize() const;

    /**
     * Returns the position of the variable v.
     * @throw NotFound Raised if v does not belong to the instantiation.
     */
    Idx pos(const DiscreteVariable &v) const;

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
    Idx val(Idx i) const;

    /**
     * Returns the current value of a given variable.
     *
     * @warning For speed issues, the function does not actually check whether the
     *          overflow flag is set before returning the current value of the
     *          variable as, usually, it is not necessary. If need be, use function
     *          inOverflow() to check.
     *
     * @param var The variable the value of which we wish to know.
     * @throw NotFound Raised it var does not belong to the instantiation.
     */
    Idx val(const DiscreteVariable &var) const;

    /**
     * Returns the current value of a given variable.
     *
     * @warning For speed issues, the function does not actually check whether the
     *          overflow flag is set before returning the current value of the
     *          variable as, usually, it is not necessary. If need be, use function
     *          inOverflow() to check.
     *
     * @param pvar a pointer to the variable the value of which we wish to know
     * @throw NotFound Raised if var does not belong to the instantiation.
     */

    Idx valFromPtr(const DiscreteVariable *pvar) const;

    /**
     * Returns the variable at position i in the tuple.
     *
     * @param i the index of the variable
     * @throw NotFound Raised if the element cannot be found.
     */

    const DiscreteVariable &variable(Idx i) const;

    /**
     * @brief Assign newval to variable v in the Instantiation.
     *
     * In addition to modifying the value of the variable, the Instantiation
     * informs its master of the modification. This function also unsets the
     * overflow flag.
     *
     * @param v The variable whose value is assigned.
     * @param newval The index of the value assigned (consider the values of v as
     *               an array indexed from 0 to n of values (which might be anything
     *               from real numbers to strings, etc). Parameter newval indicates
     *               the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     * @throw NotFound Raised if variable v does not belong to the instantiation.
     * @throw OutOfBound Raised if newval is not a possible value for v.
     */

    Instantiation &chgVal(const DiscreteVariable &v, Idx newval);

    /**
     * @brief Assign newval to variable v in the Instantiation.
     *
     * In addition to modifying the value of the variable, the Instantiation
     * informs its master of the modification. This function also unsets the
     * overflow flag.
     *
     * @param v The variable whose value is assigned.
     * @param newval The index of the value assigned (consider the values of v as
     *               an array indexed from 0 to n of values (which might be anything
     *               from real numbers to strings, etc). Parameter newval indicates
     *               the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     * @throw NotFound Raised if variable v does not belong to the instantiation.
     * @throw OutOfBound Raised if newval is not a possible value for v.
     */

    Instantiation &chgVal(const DiscreteVariable *v, Idx newval);

    /**
     * @brief Assign newval to variable at position varPos in the Instantiation.
     *
     * In addition to modifying the value of the variable, the Instantiation
     * informs its master of the modification. This function also unsets the
     * overflow flag.
     *
    * @param varPos The index of the variable whose value is assigned in the
     *        tuple of variables of the Instantiation.
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

    Instantiation &chgVal(Idx varPos, Idx newval);

    /**
     * @brief Assign the values from i in the Instantiation.
     *
     * For any variable in i and in *this, value of the variable in i is assigned
     * to the variable in *this.
     *
     * In addition of modifying the value of the variables in *this, the
     * Instantiation informs its master MultiDimAdressable of the modification.
     * This function also unsets the overflow flag.
     *
     * If no variables in i matches, then no value is changed.
     *
     * @warning Variables has to be "the same". Therefore chgValIn is usefull in a
     *same domain variables (for instance a BN).
     * However two identical variables will not be recognized as same (for instance
     *between 2 BNs).
     * @see Instantiation::setValsFrom for this kind of utilisation.
     *
     * @param i A Instantiation in which the new values are searched.
     * @return Returns a reference to *this in order to chain the chgVal.
     */

    Instantiation &setVals(const Instantiation &i);

    /**
     * @brief Assign the values of external in *this, using map as a bijection
     *between external and
     *        *this variables.
     *
     * @param map keys are variables in external
     * @param external An instantiation used to change the values in j.
     *
     * @throw NotFound Raised if a variable in external does not point to a variable
     *in *this or in external.
     */
    void setValsFrom(
        const HashTable<const DiscreteVariable *, const DiscreteVariable *> &map,
        const Instantiation &external);

    /**
     * Indicates whether a given variable belongs to the Instantiation.
     *
     * @param v The variable for which the test is made.
     * @return Returns true if v is in the Instantiation.
     */
    bool contains(const DiscreteVariable &v) const;

    /**
     * Indicates whether a given variable belongs to the Instantiation.
     *
     * @param v A pointer on the variable for which the test is made.
     * @return Returns true if *v is in the Instantiation.
     */
    bool contains(const DiscreteVariable *v) const;

    /// Returns the sequence of DiscreteVariable of this instantiation.
    const Sequence<const DiscreteVariable *> &variablesSequence() const;

    /// Returns true if the instantiation is empty.
    virtual bool empty(void) const;

    /// @}

    // ############################################################################
    /// @name Overflow management methods.
    // ############################################################################
    /// @{

    /**
     * @brief Indicates whether the current value of the tuple is correct or not.
     *
     * The function inOverflow() is used to flag overflowed operation (for
     * instance, ++ on the last value or -- on the first value will produce an
     * incorrect value of the tuple. Hence inOverflow() is used as an
     * end()/rend() function for loops on Instantiation.
     *
     * @code
     * for(Instantiation i.setFirst(); !i.inOverflow(); ++i) {
     *   // code...
     * }
     * @endcode
     */

    bool inOverflow() const;

    /**
     * @brief Removes the flag overflow. See full documentation for details.
     * (Recommended).
     *
     * When we use multiple inner loops w.r.t. a given Instantiation, it may
     * happen that one inner loop reaches the end() of the Instantiation while
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
     * // create 2 instantiations for the 2 necessary loops
     * Instantiation i(Prob), j;
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

    void unsetOverflow();

    /**
     * Alias for unsetOverflow().
     * @see unsetOverflow().
     */

    void unsetEnd();

    /**
     * Returns true if the Instantiation reached the end.
     *
     * Function end() should be used as in:
     * @code
     * for(Instantiation i.setFirst();! i.end(); ++i) {
     *   // code
     * }
     * @endcode
     */

    bool end() const;

    /**
     * Returns true if the Instantiation reached the rend.
     *
     * Function end() should be used as in:
     * @code
     * for(Instantiation i.setLast();! i.rend(); --i) {
     *   // code
     * }
     * @endcode
     */

    bool rend() const;

    /// @}

    // ############################################################################
    /// @name Incrementation and decrementation methods
    // ############################################################################
    /// @{

    /**
     * @brief Operator ++.
     *
     * Note that this operator never throws an exception when it reaches the end
     * of the possible values of the tuple of variables of the Instantiation.
     * To know if we reached the end, use function end(). If the Instantiation
     * is related to a gum::MultiDimAdressable, then the corresponding value in
     * the latter is updated. If we already reached the end() or the rend() of
     * the possible values, function inc() will perform nothing (this prevents
     * looping inadvertently several times within the same loop). To unset the end
     * flag, use functions unsetOverflow(), unsetEnd() or one of the setFirst() or
     * setLast().
     *
     * Usage example:
     * @code
     * for(Instantiation i.setFirst();! i.end(); i.inc()) {
     *   // code
     * }
     * @endcode
     */

    void inc();

    /**
     * @brief Operator --.
     *
     * Note that this operator never throws an exception when it reaches the end
     * of the possible values of the tuple of variables of the Instantiation.
     * To know if we reached the end, use function end(). If the Instantiation
     * is related to a gum::MultiDimAdressable, then the corresponding value in
     * the latter is updated. If we already reached the end() or the rend() of
     * the possible values, function inc() will perform nothing (this prevents
     *looping
     * inadvertently several times within the same loop). To unset the end flag,
     * use functions unsetOverflow(), unsetEnd() or one of the setFirst() or
     *setLast().
     *
     * Usage example:
     * @code
     * for(Instantiation i.setLast();! i.rend(); i.dec()) {
     *   // code
     * }
     * @endcode
     */

    void dec();

    /**
     * @brief Operator ++ for the variables in i.
     *
     * Note that, if the Instantiation is related to a gum::MultiDimAdressable,
     * then the corresponding value in the latter is updated.
     *
     * Note also that this operator
     * never throws an exception when it reaches the end of the possible values of
     * the tuple of variables of the Instantiation. To know if we reached the end,
     * use function end(). Finally, let us mention that the value of instantiation i
     * is not taken into account, that is, only the variables belonging to i are
     * taken into account. The next value of *this is thus computed w.r.t. to the
     * current value of *this. If we already reached the end() or the rend() of the
     * possible values, function incIn() will perform nothing (this prevents looping
     * inadvertently several times within the same loop). To unset the end flag,
     * use functions unsetOverflow(), unsetEnd() or one of the setFirst() or
     *setLast().
     */

    void incIn(const Instantiation &i);

    /**
     * @brief Operator -- for the variables in i.
     *
     * Note that, if the Instantiation is related to a gum::MultiDimAdressable,
     * then the corresponding value in the latter is updated.
     *
     * Note also that this operator
     * never throws an exception when it reaches the end of the possible values of
     * the tuple of variables of the Instantiation. To know if we reached the end,
     * use function end(). Finally, let us mention that the value of instantiation i
     * is not taken into account, that is, only the variables belonging to i are
     * taken into account. The next value of *this is thus computed w.r.t. to the
     * current value of *this. If we already reached the end() or the rend() of the
     * possible values, function incIn() will perform nothing (this prevents looping
     * inadvertently several times within the same loop). To unset the end flag,
     * use functions unsetOverflow(), unsetEnd() or one of the setFirst() or
     *setLast().
     */

    void decIn(const Instantiation &i);

    /**
     * @brief Operator ++ for the variables not in i.
     *
     * Note that, if the Instantiation is related to a gum::MultiDimAdressable,
     * then the corresponding value in the latter is updated.
     *
     * Note also that this operator
     * never throws an exception when it reaches the end of the possible values of
     * the tuple of variables of the Instantiation. To know if we reached the end,
     * use function end(). Finally, let us mention that the value of instantiation i
     * is not taken into account, that is, only the variables not belonging to i are
     * taken into account. The next value of *this is thus computed w.r.t. to the
     * current value of *this. If we already reached the end() or the rend() of the
     * possible values, function incerr() will perform nothing (this prevents looping
     * inadvertently several times within the same loop). To unset the end flag,
     * use functions unsetOverflow(), unsetEnd() or one of the setFirst() or
     *setLast().
     */

    void incOut(const Instantiation &i);

    /**
     * @brief Operator -- for the variables not in i.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then the
     * corresponding value in the latter is updated. Note also that this operator
     * never throws an exception when it reaches the end of the possible values of
     * the tuple of variables of the Instantiation. To know if we reached the end,
     * use function end(). Finally, let us mention that the value of instantiation i
     * is not taken into account, that is, only the variables not belonging to i are
     * taken into account. The next value of *this is thus computed w.r.t. to the
     * current value of *this. If we already reached the end() or the rend() of the
     * possible values, function incerr() will perform nothing (this prevents looping
     * inadvertently several times within the same loop). To unset the end flag,
     * use functions unsetOverflow(), unsetEnd() or one of the setFirst() or
     *setLast().
     */

    void decOut(const Instantiation &i);

    /**
     * @brief Operator ++ for vars which are not v.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then the
     * corresponding value in the latter is updated. Note also that this operator
     * never throws an exception when it reaches the end of the possible values of
     * the tuple of variables of the Instantiation. To know if we reached the end,
     * use function end(). If we already reached the end() or the rend() of the
     * possible values, function incNotVar() will perform nothing (this prevents
     * looping inadvertently several times within the same loop). To unset the end
     * flag, use functions unsetOverflow(), unsetEnd() or one of the setFirst() or
     * setLast().
     */

    void incNotVar(const DiscreteVariable &v);

    /**
     * @brief Operator -- for vars which are not v.
     *
     * Note that, if the Instantiation is related to a gum::MultiDimAdressable,
     * then the corresponding value in the latter is updated.
     *
     * Note also that this operator
     * never throws an exception when it reaches the end of the possible values of
     * the tuple of variables of the Instantiation. To know if we reached the end,
     * use function end(). If we already reached the end() or the rend() of the
     * possible values, function incNotVar() will perform nothing (this prevents
     * looping inadvertently several times within the same loop). To unset the end
     * flag, use functions unsetOverflow(), unsetEnd() or one of the setFirst() or
     * setLast().
     */

    void decNotVar(const DiscreteVariable &v);

    /**
     * @brief Operator ++ for variable v only.
     *
     * This function increment only variable v. Trying to increment the last possible
     * value results in an overflow (no exception is thrown in this case).
     * If we already reached the end() or the rend() of the possible
     * values, function incVar() will perform nothing (this prevents looping
     * inadvertently several times within the same loop). To unset the end flag,
     * use functions unsetOverflow(), unsetEnd() or one of the setFirst() or
     *setLast().
     *
     * @throw NotFound Raised if variable v does not belong to the Instantiation.
     */

    void incVar(const DiscreteVariable &v);

    /**
     * @brief Operator -- for variable v only.
     *
     * This function decrement only variable v. Trying to decrement the last possible
     * value results in an overflow (no exception is thrown in this case).
     * If we already reached the end() or the rend() of the possible
     * values, function incVar() will perform nothing (this prevents looping
     * inadvertently several times within the same loop). To unset the end flag,
     * use functions unsetOverflow(), unsetEnd() or one of the setFirst() or
     *setLast().
     *
     * @throw NotFound Raised if variable v does not belong to the Instantiation.
     */

    void decVar(const DiscreteVariable &v);

    /// @}

    // ############################################################################
    /// @name Initialization methods
    // ############################################################################
    /// @{

    /**
     * @brief Assign the first values to the tuple of the Instantiation.
     *
     * Note that, if the Instantiation is related to a gum::MultiDimAdressable, then
     *the
     * corresponding value in the latter is updated. This function naturally unsets
     * the overFlow flag.
     */

    void setFirst();

    /**
     * @brief Assign the last values in the Instantiation.
     *
     * Note that, if the Instantiation is related to a gum::MultiDimAdressable, then
     *the
     * corresponding value in the latter is updated. This function naturally unsets
     * the overFlow flag.
     */

    void setLast();

    /**
     * @brief Assign the first values in the Instantiation for the variables in i.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then the
     * corresponding value in the latter is updated. Note also that the value of
     * instantiation i is not taken into account, that is, only the variables
     * not belonging to i are taken into account. This function naturally unsets the
     * overFlow flag.
     */

    void setFirstIn(const Instantiation &i);

    /**
     * @brief Assign the last values in the Instantiation for the variables in i.
     *
     * Where Di is the domain size of variable i in the Instantiation) for the i
     *vars.
     *
     * Note that, if the Instantiation is related to a gum::MultiDimAdressable, then
     *the
     * corresponding value in the latter is updated. Note also that the value of
     * instantiation i is not taken into account, that is, only the variables
     * belonging to i are taken into account. This function naturally unsets the
     * overFlow flag.
     */

    void setLastIn(const Instantiation &i);

    /**
     * @brief Assign the first values in the Instantiation for the variables not in
     *i.
     *
     * Note that, if the Instantiation is related to a qum::MultiDimAdressable, then
     *the
     * corresponding value in the latter is updated. Note also that the value of
     * instantiation i is not taken into account, that is, only the variables
     * not belonging to i are taken into account. This function naturally unsets the
     * overFlow flag.
     */

    void setFirstOut(const Instantiation &i);

    /**
     * @brief Assign the last values in the Instantiation for the variables not in i.
     *
     * Note that, if the Instantiation is related to a gum::MultiDimAdressable, then
     *the
     * corresponding value in the latter is updated. Note also that the value of
     * instantiation i is not taken into account, that is, only the variables
     * not belonging to i are taken into account. This function naturally unsets the
     * overFlow flag.
     */

    void setLastOut(const Instantiation &i);

    /**
     * @brief Assign the first values to variables different of v.
     *
     * Note that, if the Instantiation is related to a gum::MultiDimAdressable, then
     *the
     * corresponding value in the latter is updated. This function naturally unsets
     * the overFlow flag.
     */

    void setFirstNotVar(const DiscreteVariable &v);

    /**
     * @brief  Assign the last values to variables different of v.
     *
     * Note that, if the Instantiation is related to a gum::MultiDimAdressable, then
     *the
     * corresponding value in the latter is updated. This function naturally unsets
     * the overFlow flag.
     */

    void setLastNotVar(const DiscreteVariable &v);

    /**
     * @brief Assign the first value in the Instantiation for var v.
     *
     * Note that, if the Instantiation is related to a gum::MultiDimAdressable, then
     *the
     * corresponding value in the latter is updated. This function naturally unsets
     * the overFlow flag.
     */

    void setFirstVar(const DiscreteVariable &v);

    /**
     * @brief Assign the last value in the Instantiation for var v.
     *
     * Note that, if the Instantiation is related to a gum::MultiDimAdressable, then
     *the
     * corresponding value in the latter is updated. This function naturally unsets
     * the overFlow flag.
     */

    void setLastVar(const DiscreteVariable &v);

    /// @}

    // ############################################################################
    /// @name Notification methods
    // ############################################################################
    /// @{

    /**
     * @brief Tries to register the Instantiation to a gum::MultiDimAdressable.
     *
     * The function will actually register the Instantiation if and only if it
     * has precisely the same variables as the gum::MultiDimAdressable (by precisely,
     * we mean a physical equality, that is, the variables are at the same places
     * in memory).
     *
     * @param aMD the multidimensional array which will be the master of *this
     * @returns true if and only if the Instantiation has been associated
     *          successfully to aMD.
     * @throw OperationNotAllowed Raised if this instantiation has already a master.
     */

    bool actAsSlave(MultiDimAdressable &aMD);

    /**
     * Deassociate the master gum::MultiDimAdressable, if any.
     * @returns true if and only if the Instantiation has been unregistered.
     */

    bool forgetMaster();

    /// Indicates whether the Instantiation has a master.

    bool isSlave() const;

    /// Indicates whether m is the master of this instantiation.

    bool isMaster(const MultiDimAdressable *m) const;

    /// Indicates whether m is the master of this instantiation.

    bool isMaster(const MultiDimAdressable &m) const;

    /**
     * @brief Force the variables sequence to be the same as the master one.
     *
     * The master should be a friend to notify dimensions changes
     * friend class gum::MultiDimAdressable.
     *
     * @param m The master of this instantiation.
     * @throw OperationNotAllowed Raised if m is not the master of instantiation.
     */

    void synchronizeWithMaster(const MultiDimAdressable *m);

    /**
     * Call __add by master.
     *
     * @param m The master of this instantiation.
     * @param v dim to add
     * @throw OperationNotAllowed Raised if m is not hte master of this
     *instantiation.
     */

    void addWithMaster(const MultiDimAdressable *m, const DiscreteVariable &v);

    /**
     * Call __erase by master.
     *
     * @param m The master of this instantiation.
     * @param v The variable to remove.
     * @throw OperationNotAllowed Raised if m is not the master of this
     *instantiation.
     */

    void eraseWithMaster(const MultiDimAdressable *m, const DiscreteVariable &v);

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// Alias of inc().

    Instantiation &operator++();

    /// Alias of dec().

    Instantiation &operator--();

    /// Calls depl times inc().

    Instantiation &operator+=(Id depl);

    /// Calls depl times dec()

    Instantiation &operator-=(Id depl);

    /// @}

    // ############################################################################
    /// @name Various methods
    // ############################################################################
    /// @{

    /// Returns the hamming distance of this instantiation.

    Idx hamming() const;

    /// Give a string version of instantiation.

    std::string toString() const;

    /**
     * @brief Reorder vars of this instantiation giving the order in v.
     *
     * In the new order variables common to v and *this are placed first, then
     * variables only in *this.
     *
     * The variables only in v are ignored.
     *
     * @throw OperationNotAllowed if slave instantiation
     */

    void reorder(const Sequence<const DiscreteVariable *> &v);

    /**
     * Calls reorder(const Sequence<const DiscreteVariable*>&) with
     * i.variablesSequence()
     */

    void reorder(const Instantiation &i);
    /// @}

    protected:
    /// Replace x by y.
    virtual void _swap(const DiscreteVariable *x, const DiscreteVariable *y);

    private:
    /**
     * The master, if any, contains precisely the set of variables to be
     * instantiated.
     */

    MultiDimAdressable *__master;

    /// The tuple of variables to be instantiated.

    Sequence<const DiscreteVariable *> __vars;

    /// The current instantiation: the value of the tuple.

    std::vector<Idx> __vals;

    /**
     * indicates whether the current value of the tuple is valid
     * when we loop sufficiently over values of the tuple, we may have browsed
     * all the possible values and we have to know in a way or another that
     * the tuple contains no more value. This is precisely the meaning of
     * Boolean overflow
     */

    bool __overflow;

    /// swap 2 vars in the Instantiation

    void __swap(Idx i, Idx j);

    /**
     *  modifies internally the value of a given variable of the sequence
     *  In addition to modifying the value of the variable, the Instantiation
     *  informs its master MultiDimAdressable of the modification
     */

    void __chgVal(Idx varPos, Idx newVal);

    /**
     *  adds a new var to the sequence of vars
     *  If variable v already belongs to the Instantiation tuple of variables,
     *  then nothing is done. In particular, no exception is thrown in this case.
     *  @param v the new var
     *  @ warning note that this function does not deassociate the Instantiation
     *  from its master MultiDimAdressable, if any. To do so, use function
     *  add instead.
     *  @warning this function does not notify the master MultiDimAdressable,
     *  if any. Use in addition function chgVal or __chgVal if need be.
     *  @warning variable v is known to the Instantiation only by a pointer to it.
     *  As a result, this is not a copy of v that is used by Instantiation but
     *  rather v itself. As such, v should never be deleted from memory until the
     *  Instantiation is removed.
     *  @throw DuplicateElement
     */

    void __add(const DiscreteVariable &v);

    /**
     *  removes a variable from the sequence of vars
     *  If variable v does not belong to the Instantiation tuple of variables,
     *  then nothing is done. In particular, no exception is thrown in this case.
     *  @param v the variable to be erased from the tuple
     *  @warning this function does not notify the master MultiDimAdressable, if any.
     *  @warning note that this function does not deassociate the Instantiation
     *  from its master MultiDimAdressable, if any. To do so, use function
     *  removeDim instead.
     */

    void __erase(const DiscreteVariable &v);

    /**
     *  This function is called by the master (if any) when changes arise in its
     *  vars list.
     *  @warning No implementation of this method?
     *
     *  @param v the new vars list
     */

    void __notifiedDimChanged(const Sequence<const DiscreteVariable *> &v);

    /// initialisation (same in 4 constructors)

    void __init(MultiDimAdressable *master);

    /**
     * @brief Reorder vars of this instantiation giving the order in v.
     *
     * In the new order variables common to v and *this are placed first, then
     * variables only in *this.
     *
     * The variables only in v are ignored.
     */

    void __reorder(const Sequence<const DiscreteVariable *> &v);
  };

  /// Print information of the instantiation in the stream.

  std::ostream &operator<<(std::ostream &, const Instantiation &);

} /* namespace gum */

#ifndef GUM_NO_INLINE
// #include <agrum/multidim/instantiation.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_INSTANTIATION_H */
