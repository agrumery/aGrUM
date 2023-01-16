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


/**
 * @file
 * @brief Header files of gum::Instantiation.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_INSTANTIATION_H
#define GUM_INSTANTIATION_H

#include <ostream>
#include <sstream>

#include <agrum/agrum.h>

#include <agrum/tools/multidim/implementations/multiDimInterface.h>

namespace gum {

  class MultiDimAdressable;
  // =========================================================================
  // ===                           GUM_INSTANTIATION                       ===
  // =========================================================================
  /**
   * @class Instantiation
   * @headerfile instantiation.h <agrum/tools/multidim/instantiation.h>
   * @brief Class for assigning/browsing values to tuples of discrete
   * variables.
   * @ingroup multidim_group
   *
   * Instantiation is designed to assign values to tuples of variables and to
   * efficiently loop over values of subsets of variables. This class can be
   * used in two different flavors:
   * - the tuple of variables in the Instantiation is related to a
   * multidimensional array and, when we loop over the possible values of the
   * tuple, we also loop at the same time over the corresponding values in the
   * array.
   * - the tuple of variables in the Instantiation is not related to a
   * multidimensional array and we can loop over the possible values of the
   * tuple without looping over values fo any array.
   *
   * An Instantiation can be associated/deassociated to a given
   * multidimensional array using the
   * MultiDimAdressable::registerSlave(Instantiation& i) and
   * MultiDimAdressable::unregisterSlave functions. Note that, to be
   * registrable, the Instantiation must have precisely the same variables as
   * the array. As a consequence, adding or removing a variable from a
   * Instantiation associated to an array will unregister it. This behavior is
   * compulsory as, if it were still associated, it would not be possible to
   * retrieve a correct value of the array given a value of the Instantiation.
   * For instance, if M[A,B,C] is an array indexed by Boolean variables A,B,C,
   * Which value of M should be returned if B=0 and C=0? We do not know for
   * sure as we do not know the value of A. Note also that, at any time, you
   * can unregister a Instantiation from its master multidimensional array and
   * you can ask to associate it (provided the tuple of variable match).
   *
   * To print information about a Instantiation use the following function:
   * @see operator<<(std::ostream&, const Instantiation&).
   */

  class Instantiation: public MultiDimInterface {
    public:
    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /**
     * @brief Default constructor: creates an empty tuple.
     */
    Instantiation();

    /**
     * @brief Copy constructor.
     *
     * Note that the Instantiation is by default associated to the same
     * MultiDimAdressable as aI. This means that looping over values of the
     * tuple will induce looping over the values of the MultiDimAdressable.
     * Similarly, the value of the tuple is that of aI, and, if the
     * Instantiation is slaved, its master is notified of the value of the
     * Instantiation if notifyMaster is true.
     *
     * @param aI The Instantiation we copy.
     * @param notifyMaster Whether or not notify master if exits.
     */
    Instantiation(const Instantiation& aI, const bool notifyMaster = true);

    /**
     * @brief Copy operator.
     *
     * If this is a slave but not with the same as aI's master: if aI and this
     * does not share the same variables then an OperationNotAllowed will we be
     * raised. Otherwise calls this->setVals( aI ).
     *
     * If this is not a slave, copies aI.
     *
     * @param aI The Instantiation to copy.
     *
     * @throw OperationNotAllowed Raised if copy is not allowed.
     */
    Instantiation& operator=(const Instantiation& aI);

    /**
     * @brief Constructor for a Instantiation of all the variables of a
     * MultiDimAdressable.
     *
     * The variables of the Instantiation are those of aMD (actually, they are
     * shared in memory). All the variables of aMD belong to the tuple of
     * variables to be instantiated.
     *
     * Note that the Instantiation is by default associated to aMD, i.e.,
     * looping over values of the tuple will induce looping over the values of
     * aMD. The value given to the tuple is the first possible value, that is,
     * (0,...,0). If the Instantiation is slaved, its master is notified of
     * the value of the Instantiation.
     *
     * @param aMD The array the variables of which are those of the
     * Instantiation.
     */
    Instantiation(MultiDimAdressable& aMD);

    /**
     * @brief Constructor for a Instantiation of all the variables of a
     * MultiDimAdressable.
     *
     * The variables of the Instantiation are those of aMD (actually, they are
     * shared in memory). All the variables of aMD belong to the tuple of
     * variables to be instantiated.
     *
     * Note that the Instantiation is by default associated to aMD, i.e.,
     * looping over values of the tuple will induce looping over the values of
     * aMD. The value given to the tuple is the first possible value, that is,
     * (0,...,0).  If the Instantiation is slaved, its master is notified of
     * the value of the Instantiation.
     *
     * @param aMD The array the variables of which are those of the
     * Instantiation.
     */
    Instantiation(const MultiDimAdressable& aMD);

    /**
     * @brief Constructor for a Instantiation of all the variables of a
     * MultiDimAdressable.
     *
     * The variables of the Instantiation are those of aMD (actually, they are
     * shared in memory). All the variables of aMD belong to the tuple of
     * variables to be instantiated.
     *
     * Note that the Instantiation is by default associated to aMD, i.e.,
     * looping over values of the tuple will induce looping over the values of
     * aMD. The value given to the tuple is the first possible value, that is,
     * (0,...,0).  If the Instantiation is slaved, its master is notified of
     * the value of the Instantiation.
     *
     * @param aMD The array the variables of which are those of the
     * Instantiation.
     */
    Instantiation(MultiDimAdressable* aMD);

    /**
     * @brief Constructor for a Instantiation of all the variables of a
     * MultiDimAdressable.
     *
     * The variables of the Instantiation are those of aMD (actually, they are
     * shared in memory). All the variables of aMD belong to the tuple of
     * variables to be instantiated.
     *
     * Note that the Instantiation is by default associated to aMD, i.e.,
     * looping over values of the tuple will induce looping over the values of
     * aMD. The value given to the tuple is the first possible value, that is,
     * (0,...,0).  If the Instantiation is slaved, its master is notified of
     * the value of the Instantiation.
     *
     * @param aMD The array the variables of which are those of the
     * Instantiation.
     */
    Instantiation(const MultiDimAdressable* aMD);

    /**
     * @brief Destructor.
     */
    ~Instantiation();

    /// @}
    // =========================================================================
    /// @name Accessors / Modifiers
    // =========================================================================
    /// @{

    /**
     * @brief Returns the number of variables in the Instantiation.
     * @return Returns the number of variables in the Instantiation.
     */
    Idx nbrDim() const final;

    /**
     * @brief Adds a new variable in the Instantiation.
     *
     * If variable v already belongs to the Instantiation tuple of variables,
     * then DuplicateElement is thrown in this case. The value of the new
     * variable is set to that of index 0, that is, the first possible value
     * for the variable.  Since an instantiation must share the same set of
     * variables with his master an OperationNotAllowed is raised if you try to
     * add a variable of a slaved instantiation.
     *
     * @warning Variable v is known to the Instantiation only by a pointer to
     * it.  As a result, this is not a copy of v that is used by the
     * Instantiation, but rather v itself. As such, v should never be deleted
     * from memory until the Instantiation is removed.
     *
     * @param v The new variable added to this Instantiation.
     *
     * @throw DuplicateElement Raised if v is already in this Instantiation.
     * @throw InvalidArgument Raised if the name of v is already used in this
     * Instantiation.
     * @throw OperationNotAllowed Raised if this is a slave Instantiation.
     */
    void add(const DiscreteVariable& v) final;

    /**
     * @brief Removes a variable from the Instantiation.
     *
     * If variable v does not belong to the Instantiation tuple of variables,
     * then NotFound is thrown.  Since an instantiation must share the same set
     * of variables with his master an OperationNotAllowed is raised if you try
     * to remove a variable from a slaved instantiation.
     *
     * @param v The variable to remove from this Instantiation.
     *
     * @throw NotFound Raised if v does not belong to this Instantiation.
     * @throw OperationNotAllowed Raised if the instantiation is a slave.
     */
    void erase(const DiscreteVariable& v) final;
    void erase(const std::string& name);

    /**
     * @brief Erase all variables from an Instantiation
     * @throw OperationNotAllowed Raised if the instantiation is a slave.
     */
    void clear();

    /**
     * @brief Returns the product of the variable's domain size in the
     * Instantiation.
     * @return Returns the product of the variable's domain size in the
     * Instantiation.
     */
    Size domainSize() const final;

    /**
     * @brief Returns the position of the variable v.
     * @return Returns the position of the variable v.
     * @param v The variable for which its position is return.
     *
     * @throw NotFound Raised if v does not belong to the instantiation.
     */
    Idx pos(const DiscreteVariable& v) const final;

    /**
     * @brief Returns the current value of the variable at position i.
     *
     * @warning For speed issues, the function does not actually check whether
     * the overflow flag is set before returning the current value of the
     * variable as, usually, it is not necessary. If need be, use function
     * inOverflow() to check.
     *
     * @param i The index of the variable.
     * @return Returns the current value of the variable at position i.
     *
     * @throw NotFound Raised if the element cannot be found.
     */
    Idx val(Idx i) const;

    /**
     * @brief Returns the current value of a given variable.
     *
     * @warning For speed issues, the function does not actually check whether
     * the overflow flag is set before returning the current value of the
     * variable as, usually, it is not necessary. If need be, use function
     * inOverflow() to check.
     *
     * @param var The variable the value of which we wish to know.
     * @return Returns the current value of a given variable.
     *
     * @throw NotFound Raised it var does not belong to the instantiation.
     */
    Idx val(const DiscreteVariable& var) const;
    Idx val(const std::string& name) const;

    /**
     * @brief Returns the current value of a given variable.
     *
     * @warning For speed issues, the function does not actually check whether
     * the overflow flag is set before returning the current value of the
     * variable as, usually, it is not necessary. If need be, use function
     * inOverflow() to check.
     *
     * @param pvar The variable for which the value is returned.
     * @return Returns the current value of a given variable.
     *
     * @throw NotFound Raised if var does not belong to the instantiation.
     */
    Idx valFromPtr(const DiscreteVariable* pvar) const;

    /**
     * @brief Returns the variable at position i in the tuple.
     *
     * @param i The index of the variable
     * @return Returns the variable at position i in the tuple.
     * @throw NotFound Raised if the element cannot be found.
     */
    const DiscreteVariable& variable(Idx i) const final;

    /**
     * @brief Returns the variable with the name
     *
     * @param name The index of the variable
     * @return Returns the variable qith the name in the tuple.
     * @warging This function is not O(1)
     * @throw NotFound Raised if the element cannot be found.
     */
    const DiscreteVariable& variable(const std::string& name) const final;

    /**
     * @brief Assign newval to variable v in the Instantiation.
     *
     * Consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newval indicates the index in this array of the new value taken by v.
     *
     * In addition to modifying the value of the variable, the Instantiation
     * informs its master of the modification. This function also unsets the
     * overflow flag.
     *
     * @param v The variable whose value is assigned.
     * @param newval The index of the value assigned.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the
     * instantiation.
     * @throw OutOfBounds Raised if newval is not a possible value for v.
     */
    Instantiation& chgVal(const DiscreteVariable& v, Idx newval);

    /**
     * @brief Assign newval to variable v in the Instantiation.
     *
     * Consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newval indicates the index in this array of the new value taken by v.
     *
     * In addition to modifying the value of the variable, the Instantiation
     * informs its master of the modification. This function also unsets the
     * overflow flag.
     *
     * @param v The variable whose value is assigned.
     * @param newval The index of the value assigned.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the
     * instantiation.
     * @throw OutOfBounds Raised if newval is not a possible value for v.
     */
    Instantiation& chgVal(const DiscreteVariable* v, Idx newval);

    /**
     * @brief Assign newval to variable at position varPos in the Instantiation.
     *
     * Consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newval indicates the index in this array of the new value taken by v.
     *
     * In addition to modifying the value of the variable, the Instantiation
     * informs its master of the modification. This function also unsets the
     * overflow flag.
     *
     * @param varPos The index of the variable whose value is assigned in the
     * tuple of variables of the Instantiation.
     * @param newval The index of the value assigned.
     * @return A reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if the variable does not belong to this
     * @throw OutOfBounds Raised if newval is not a possible value for the
     * variable
     */
    Instantiation& chgVal(Idx varPos, Idx newval);

    /**
     * @brief Assign newval to variable at position varPos in the Instantiation.
     *
     * Consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newval indicates the index in this array of the new value taken by v.
     *
     * In addition to modifying the value of the variable, the Instantiation
     * informs its master of the modification. This function also unsets the
     * overflow flag.
     *
     * @param var the name of the variable whose value is assigned in the
     * tuple of variables of the Instantiation.
     * @param newval The index of the value assigned.
     * @return A reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if the variable does not belong to this
     * @throw NotFound Raised if newval is not a possible value for the
     * variable
     */
    Instantiation& chgVal(const std::string& var, Idx newval);

    /**
     * @brief Assign newval to variable at position varPos in the Instantiation.
     *
     * Consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newval indicates the index in this array of the new value taken by v.
     *
     * In addition to modifying the value of the variable, the Instantiation
     * informs its master of the modification. This function also unsets the
     * overflow flag.
     *
     * @param var the name of the variable whose value is assigned in the
     * tuple of variables of the Instantiation.
     * @param newval The label of the value assigned.
     * @return A reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if the variable does not belong to this
     * @throw OutOfBounds Raised if newval is not a possible value for the
     * variable
     */
    Instantiation& chgVal(const std::string& var, const std::string& newval);

    /**
     * @brief Assign the values from i in the Instantiation.
     *
     * For any variable in i and in *this, value of the variable in i is
     * assigned to the variable in *this.
     *
     * In addition of modifying the value of the variables in *this, the
     * Instantiation informs its master of the modification.  This function
     * also unsets the overflow flag.
     *
     * If no variables in i matches, then no value is changed.
     *
     * @warning Variables has to be "the same". Therefore chgValIn is usefull
     * in a same domain variables (for instance a BN). However two identical
     * variables will not be recognized as same (for instance between 2 BNs).
     *
     * @see Instantiation::setValsFrom for this kind of utilisation.
     *
     * @param i A Instantiation in which the new values are searched.
     * @return Returns a reference to *this in order to chain the chgVal.
     */
    Instantiation& setVals(const Instantiation& i);

    /**
     * @brief Assign the values of external in *this, using map as a bijection
     * between external and this variables.
     *
     * @param map Keys are variables in external.
     * @param external An instantiation used to change the values in j.
     *
     * @throw NotFound Raised if a variable in external does not point to a
     * variable in *this or in external.
     */
    void setValsFrom(const HashTable< const DiscreteVariable*, const DiscreteVariable* >& map,
                     const Instantiation&                                                 external);

    /**
     * Indicates whether a given variable belongs to the Instantiation.
     *
     * @param v The variable for which the test is made.
     * @return Returns true if v is in the Instantiation.
     */
    bool contains(const DiscreteVariable& v) const final;
    bool contains(const std::string& name) const;

    /**
     * Indicates whether a given variable belongs to the Instantiation.
     *
     * @param v A pointer on the variable for which the test is made.
     * @return Returns true if *v is in the Instantiation.
     */
    bool contains(const DiscreteVariable* v) const;

    /**
     * @brief Returns the sequence of DiscreteVariable of this instantiation.
     * @return Returns the sequence of DiscreteVariable of this instantiation.
     */
    const Sequence< const DiscreteVariable* >& variablesSequence() const final;

    /**
     * @brief Returns true if the instantiation is empty.
     * @return Returns true if the instantiation is empty.
     */
    virtual bool empty() const final;

    /// @}
    // =========================================================================
    /// @name Overflow management methods.
    // =========================================================================
    /// @{

    /**
     * @brief Indicates whether the current value of the tuple is correct or
     * not.
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
     * the outer loops do not have reached it. This means that the inner loop
     * has toggled the overflow flag. To enable the other loops to go on, we
     * must unset this flag using function unsetOverflow(). For instance,
     * assume that Prob represents probability P(b|a,c), then normalizing this
     * proba can be performed using the following code:
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
     * @brief Alias for unsetOverflow().
     * @see unsetOverflow().
     */
    void unsetEnd();

    /**
     * @brief Returns true if the Instantiation reached the end.
     *
     * Function end() should be used as in:
     * @code
     * for(Instantiation i.setFirst();! i.end(); ++i) {
     *   // code
     * }
     * @endcode
     *
     * @return Returns true if the Instantiation reached the end.
     */
    bool end() const;

    /**
     * @brief Returns true if the Instantiation reached the rend.
     *
     * Function end() should be used as in:
     * @code
     * for(Instantiation i.setLast();! i.rend(); --i) {
     *   // code
     * }
     * @endcode
     *
     * @return Returns true if the Instantiation reached the rend.
     */
    bool rend() const;

    /// @}
    // =========================================================================
    /// @name Incrementation and decrementation methods
    // =========================================================================
    /// @{

    /**
     * @brief Operator increment.
     *
     * Note that this operator never throws an exception when it reaches the
     * end of the possible values of the tuple of variables of the
     * Instantiation.  To know if we reached the end, use function end(). If
     * the Instantiation is related to a MultiDimAdressable, then the
     * corresponding value in the latter is updated. If we already reached the
     * end() or the rend() of the possible values, function inc() will perform
     * nothing (this prevents looping inadvertently several times within the
     * same loop). To unset the end flag, use functions unsetOverflow(),
     * unsetEnd() or one of the setFirst() or setLast().
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
     * @brief Operator decrement
     *
     * Note that this operator never throws an exception when it reaches the
     * end of the possible values of the tuple of variables of the
     * Instantiation.  To know if we reached the end, use function end(). If
     * the Instantiation is related to a MultiDimAdressable, then the
     * corresponding value in the latter is updated. If we already reached the
     * end() or the rend() of the possible values, function inc() will perform
     * nothing (this prevents looping inadvertently several times within the
     * same loop). To unset the end flag, use functions unsetOverflow(),
     * unsetEnd() or one of the setFirst() or setLast().
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
     * @brief Operator increment for the variables in i.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then
     * the corresponding value in the latter is updated.
     *
     * Note also that this operator never throws an exception when it reaches
     * the end of the possible values of the tuple of variables of the
     * Instantiation. To know if we reached the end, use function end().
     * Finally, let us mention that the value of instantiation i is not taken
     * into account, that is, only the variables belonging to i are taken into
     * account. The next value of *this is thus computed w.r.t. to the current
     * value of *this. If we already reached the end() or the rend() of the
     * possible values, function incIn() will perform nothing (this prevents
     * looping inadvertently several times within the same loop). To unset the
     * end flag, use functions unsetOverflow(), unsetEnd() or one of the
     * setFirst() or setLast().
     *
     * @param i The set of variables to increment in this Instantiation.
     */
    void incIn(const Instantiation& i);

    /**
     * @brief Operator decrement for the variables in i.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then
     * the corresponding value in the latter is updated.
     *
     * Note also that this operator never throws an exception when it reaches
     * the end of the possible values of the tuple of variables of the
     * Instantiation. To know if we reached the end, use function end().
     * Finally, let us mention that the value of instantiation i is not taken
     * into account, that is, only the variables belonging to i are taken into
     * account. The next value of *this is thus computed w.r.t. to the current
     * value of *this. If we already reached the end() or the rend() of the
     * possible values, function incIn() will perform nothing (this prevents
     * looping inadvertently several times within the same loop). To unset the
     * end flag, use functions unsetOverflow(), unsetEnd() or one of the
     * setFirst() or setLast().
     *
     * @param i The set of variables to decrement in this Instantiation.
     */
    void decIn(const Instantiation& i);

    /**
     * @brief Operator increment for the variables not in i.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then
     * the corresponding value in the latter is updated.
     *
     * Note also that this operator never throws an exception when it reaches
     * the end of the possible values of the tuple of variables of the
     * Instantiation. To know if we reached the end, use function end().
     * Finally, let us mention that the value of instantiation i is not taken
     * into account, that is, only the variables not belonging to i are taken
     * into account. The next value of *this is thus computed w.r.t. to the
     * current value of *this. If we already reached the end() or the rend() of
     * the possible values, function incerr() will perform nothing (this
     * prevents looping inadvertently several times within the same loop). To
     * unset the end flag, use functions unsetOverflow(), unsetEnd() or one of
     * the setFirst() or setLast().
     *
     * @param i The set of variable to not increment in this Instantiation.
     */
    void incOut(const Instantiation& i);

    /**
     * @brief Operator decrement for the variables not in i.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then
     * the corresponding value in the latter is updated. Note also that this
     * operator never throws an exception when it reaches the end of the
     * possible values of the tuple of variables of the Instantiation. To know
     * if we reached the end, use function end(). Finally, let us mention that
     * the value of instantiation i is not taken into account, that is, only
     * the variables not belonging to i are taken into account. The next value
     * of *this is thus computed w.r.t. to the current value of *this. If we
     * already reached the end() or the rend() of the possible values, function
     * incerr() will perform nothing (this prevents looping inadvertently
     * several times within the same loop). To unset the end flag, use
     * functions unsetOverflow(), unsetEnd() or one of the setFirst() or
     * setLast().
     *
     * @param i The set of variables to not decrement in this Instantiation.
     */
    void decOut(const Instantiation& i);

    /**
     * @brief Operator increment for vars which are not v.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then
     * the corresponding value in the latter is updated. Note also that this
     * operator never throws an exception when it reaches the end of the
     * possible values of the tuple of variables of the Instantiation. To know
     * if we reached the end, use function end(). If we already reached the
     * end() or the rend() of the possible values, function incNotVar() will
     * perform nothing (this prevents looping inadvertently several times
     * within the same loop). To unset the end flag, use functions
     * unsetOverflow(), unsetEnd() or one of the setFirst() or setLast().
     *
     * @param v The variable not to increment in this Instantiation.
     */
    void incNotVar(const DiscreteVariable& v);

    /**
     * @brief Operator decrement for vars which are not v.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable,
     * then the corresponding value in the latter is updated.
     *
     * Note also that this operator never throws an exception when it reaches
     * the end of the possible values of the tuple of variables of the
     * Instantiation. To know if we reached the end, use function end(). If we
     * already reached the end() or the rend() of the possible values, function
     * incNotVar() will perform nothing (this prevents looping inadvertently
     * several times within the same loop). To unset the end flag, use
     * functions unsetOverflow(), unsetEnd() or one of the setFirst() or
     * setLast().
     *
     * @param v The varaible not to decrement in this Instantiation.
     */
    void decNotVar(const DiscreteVariable& v);

    /**
     * @brief Operator increment for variable v only.
     *
     * This function increment only variable v. Trying to increment the last
     * possible value results in an overflow (no exception is thrown in this
     * case).  If we already reached the end() or the rend() of the possible
     * values, function incVar() will perform nothing (this prevents looping
     * inadvertently several times within the same loop). To unset the end
     * flag, use functions unsetOverflow(), unsetEnd() or one of the setFirst()
     * or setLast().
     *
     * @param v The variable to increment in this Instantiation.
     * @throw NotFound Raised if variable v does not belong to the
     * Instantiation.
     */

    void incVar(const DiscreteVariable& v);

    /**
     * @brief Operator decrement for variable v only.
     *
     * This function decrement only variable v. Trying to decrement the last
     * possible value results in an overflow (no exception is thrown in this
     * case).  If we already reached the end() or the rend() of the possible
     * values, function incVar() will perform nothing (this prevents looping
     * inadvertently several times within the same loop). To unset the end
     * flag, use functions unsetOverflow(), unsetEnd() or one of the setFirst()
     * or setLast().
     *
     * @param v The variable to decrement in this Instantiation.
     * @throw NotFound Raised if variable v does not belong to the
     * Instantiation.
     */

    void decVar(const DiscreteVariable& v);

    /// @}
    // =========================================================================
    /// @name Initialization methods
    // =========================================================================
    /// @{

    /**
     * @brief Assign the first values to the tuple of the Instantiation.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then
     * the corresponding value in the latter is updated. This function
     * naturally unsets the overFlow flag.
     */
    void setFirst();

    /**
     * @brief Assign the last values in the Instantiation.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then
     * the corresponding value in the latter is updated. This function
     * naturally unsets the overFlow flag.
     */
    void setLast();

    /**
     * @brief Assign the first values in the Instantiation for the variables in
     * i.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then
     * the corresponding value in the latter is updated. Note also that the
     * value of instantiation i is not taken into account, that is, only the
     * variables not belonging to i are taken into account. This function
     * naturally unsets the overFlow flag.
     *
     * @param i The variables to which their first value is assigned in this
     * Instantiation.
     */
    void setFirstIn(const Instantiation& i);

    /**
     * @brief Assign the last values in the Instantiation for the variables in
     * i.
     *
     * Where Di is the domain size of variable i in the Instantiation) for the
     * i vars.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then
     * the corresponding value in the latter is updated. Note also that the
     * value of instantiation i is not taken into account, that is, only the
     * variables belonging to i are taken into account. This function naturally
     * unsets the overFlow flag.
     *
     * @param i The variables to which their last value is assigned in this
     * Instantiation.
     */
    void setLastIn(const Instantiation& i);

    /**
     * @brief Assign the first values in the Instantiation for the variables
     * not in i.
     *
     * Note that, if the Instantiation is related to a qum::MultiDimAdressable,
     * then the corresponding value in the latter is updated. Note also that
     * the value of instantiation i is not taken into account, that is, only
     * the variables not belonging to i are taken into account. This function
     * naturally unsets the overFlow flag.
     *
     * @param i The variable that will not be set to their first value in this
     * Instantiation.
     */

    void setFirstOut(const Instantiation& i);

    /**
     * @brief Assign the last values in the Instantiation for the variables not
     * in i.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then
     * the corresponding value in the latter is updated. Note also that the
     * value of instantiation i is not taken into account, that is, only the
     * variables not belonging to i are taken into account. This function
     * naturally unsets the overFlow flag.
     *
     * @param i The variables that will not be set to their last value in this
     * Instantiation.
     */
    void setLastOut(const Instantiation& i);

    /**
     * @brief Assign the first values to variables different of v.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then
     * the corresponding value in the latter is updated. This function
     * naturally unsets the overFlow flag.
     *
     * @param v Tha variable that will not be set to its first value in this
     * Instantiation.
     */

    void setFirstNotVar(const DiscreteVariable& v);

    /**
     * @brief  Assign the last values to variables different of v.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then
     * the corresponding value in the latter is updated. This function
     * naturally unsets the overFlow flag.
     *
     * @param v The variable that will not be set to its last value in this
     * Instantiation.
     */
    void setLastNotVar(const DiscreteVariable& v);

    /**
     * @brief Assign the first value in the Instantiation for var v.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then
     * the corresponding value in the latter is updated. This function
     * naturally unsets the overFlow flag.
     *
     * @param v The variable that will be set to its first value in this
     * Instantiation.
     */
    void setFirstVar(const DiscreteVariable& v);

    /**
     * @brief Assign the last value in the Instantiation for var v.
     *
     * Note that, if the Instantiation is related to a MultiDimAdressable, then
     * the corresponding value in the latter is updated. This function
     * naturally unsets the overFlow flag.
     *
     * @param v The variable that will be set to its last value in this
     * Instantiation.
     */
    void setLastVar(const DiscreteVariable& v);

    /// @}
    // =========================================================================
    /// @name Notification methods
    // =========================================================================
    /// @{

    /**
     * @brief Tries to register the Instantiation to a MultiDimAdressable.
     *
     * The function will actually register the Instantiation if and only if it
     * has precisely the same variables as the MultiDimAdressable (by
     * precisely, we mean a physical equality, that is, the variables are at
     * the same places in memory).
     *
     * @param aMD The multidimensional array which will be the master of *this
     * @returns Returns true if and only if the Instantiation has been
     * associated successfully to aMD.
     *
     * @throw OperationNotAllowed Raised if this instantiation has already a
     * master.
     */
    bool actAsSlave(MultiDimAdressable& aMD);

    /**
     * @brief Deassociate the master MultiDimAdressable, if any.
     * @returns Returns true if and only if the Instantiation has been
     * unregistered.
     */
    bool forgetMaster();

    /**
     * @brief Indicates whether the Instantiation has a master.
     * @return Returns true if the Instantiation has a master.
     */
    bool isSlave() const;

    /**
     * @brief Indicates whether m is the master of this instantiation.
     * @return Returns true if m is the master of this instantiation.
     */
    bool isMaster(const MultiDimAdressable* m) const;

    /**
     * @brief Indicates whether m is the master of this instantiation.
     * @return Returns true if m is the master of this instantiation.
     */
    bool isMaster(const MultiDimAdressable& m) const;

    /**
     * @brief Force the variables sequence to be the same as the master one.
     *
     * The master should be a friend to notify dimensions changes
     * friend class MultiDimAdressable.
     *
     * @param m The master of this instantiation.
     * @throw OperationNotAllowed Raised if m is not the master of
     * instantiation.
     */
    void synchronizeWithMaster(const MultiDimAdressable* m);

    /**
     * @brief Call Instantiation:: _add_(const DiscreteVariable&) by master.
     *
     * @param m The master of this instantiation.
     * @param v The varaible to add.
     * @throw OperationNotAllowed Raised if m is not hte master of this
     * instantiation.
     */
    void addWithMaster(const MultiDimAdressable* m, const DiscreteVariable& v);

    /**
     * @brief Call Instantiation:: _erase_(const DiscreteVariable&) by master.
     *
     * @param m The master of this instantiation.
     * @param v The variable to remove.
     * @throw OperationNotAllowed Raised if m is not the master of this
     * instantiation.
     */
    void eraseWithMaster(const MultiDimAdressable* m, const DiscreteVariable& v);

    /// @}
    // =========================================================================
    /// @name Operators
    // =========================================================================
    /// @{

    /**
     * @brief operator==
     */
    bool operator==(const Instantiation& other) const;

    /**
     * @brief Alias of Instantiation::inc().
     * @return Returns this Instantiation.
     */
    Instantiation& operator++();

    /**
     * @brief Alias of Instantiation::dec().
     * @return Returns this Instantiation.
     */
    Instantiation& operator--();

    /**
     * @brief Calls depl times Instantiation::inc().
     * @return Returns this Instantiation.
     */
    Instantiation& operator+=(Size depl);

    /**
     * @brief Calls depl times Instantiation::dec().
     * @return Returns this Instantiation.
     */
    Instantiation& operator-=(Size depl);

    /// @}
    // =========================================================================
    /// @name Various methods
    // =========================================================================
    /// @{

    /**
     * @brief Returns the hamming distance of this instantiation.
     * @return Returns the hamming distance of this instantiation.
     */
    Idx hamming() const;

    /**
     * @brief Give a string version of instantiation.
     * @return Returns a string version of instantiation.
     */
    std::string toString() const;

    /**
     * @brief Reorder vars of this instantiation giving the order in v.
     *
     * In the new order variables common to v and *this are placed first, then
     * variables only in *this.
     *
     * The variables only in v are ignored.
     *
     * @param v The new order of variables for this Instantiation.
     * @throw OperationNotAllowed if slave instantiation
     */
    void reorder(const Sequence< const DiscreteVariable* >& v);

    /**
     * @brief Calls reorder(const Sequence<const DiscreteVariable*>&) with
     * i.variablesSequence()
     * @param i The sequence of variables with which to reorder this
     * Instantiation.
     */
    void reorder(const Instantiation& i);

    /// @}

    protected:
    /**
     * @brief Replace x by y.
     * @param x The variable to replace.
     * @param y The variable replacing x.
     */
    virtual void replace_(const DiscreteVariable* x, const DiscreteVariable* y) final;

    private:
    /// The master, if any, contains precisely the set of variables to be
    /// instantiated.
    MultiDimAdressable* _master_;

    /// The tuple of variables to be instantiated.
    Sequence< const DiscreteVariable* > _vars_;

    /// The current instantiation: the value of the tuple.
    std::vector< Idx > _vals_;

    /// Indicates whether the current value of the tuple is valid when we loop
    /// sufficiently over values of the tuple, we may have browsed all the
    /// possible values and we have to know in a way or another that the tuple
    /// contains no more value. This is precisely the meaning of Boolean
    /// overflow
    bool _overflow_;

    /**
     * @brief Swap two variables in the Instantiation.
     * @param i The first variable.
     * @param j The second variable.
     */
    void _swap_(Idx i, Idx j);

    /**
     * @brief Modifies internally the value of a given variable of the sequence.
     *
     * In addition to modifying the value of the variable, the Instantiation
     * informs its master MultiDimAdressable of the modification.
     *
     * @param varPos The variable to change.
     * @param newVal The variable new value.
     */
    void _chgVal_(Idx varPos, Idx newVal);

    /**
     *  @brief Adds a new var to the sequence of vars.
     *
     *  If variable v already belongs to the Instantiation tuple of variables,
     *  then nothing is done. In particular, no exception is thrown in this
     *  case.
     *
     *  @warning note that this function does not deassociate the Instantiation
     *  from its master MultiDimAdressable, if any. To do so, use function add
     *  instead.
     *
     *  @warning this function does not notify the master MultiDimAdressable,
     *  if any. Use in addition function chgVal or  _chgVal_ if need be.
     *
     *  @warning variable v is known to the Instantiation only by a pointer to
     *  it.  As a result, this is not a copy of v that is used by Instantiation
     *  but rather v itself. As such, v should never be deleted from memory
     *  until the Instantiation is removed.
     *
     *  @param v The new var.
     *  @throw DuplicateElement Raised if v is already in this Instantiation.
     */
    void _add_(const DiscreteVariable& v);

    /**
     *  @brief Removes a variable from the sequence of vars.
     *
     *  If variable v does not belong to the Instantiation tuple of variables,
     *  then nothing is done. In particular, no exception is thrown in this
     *  case.
     *
     *  @warning this function does not notify the master MultiDimAdressable,
     *  if any.
     *
     *  @warning note that this function does not deassociate the Instantiation
     *  from its master MultiDimAdressable, if any. To do so, use function
     *  removeDim instead.
     *
     *  @param v The variable to be erased from the tuple.
     */
    void _erase_(const DiscreteVariable& v);

    /**
     *  This function is called by the master (if any) when changes arise in its
     *  vars list.
     *
     *  @warning No implementation of this method?
     *
     *  @param v the new vars list
     */
    void _notifiedDimChanged_(const Sequence< const DiscreteVariable* >& v);

    /**
     * @brief Initialize this Instantiation.
     * @param master This Instantiation's master.
     */
    void _init_(MultiDimAdressable* master);

    /**
     * @brief Reorder vars of this instantiation giving the order in v.
     *
     * In the new order variables common to v and *this are placed first, then
     * variables only in *this.
     *
     * The variables only in v are ignored.
     *
     * @param v The new order of variables in this Instantiation.
     */
    void _reorder_(const Sequence< const DiscreteVariable* >& v);

    void _masterChangeNotification_(Idx varPos, Idx newVal, Idx oldVal) const;
    void _masterFirstNotification_() const;
    void _masterIncNotification_() const;
    void _masterLastNotification_() const;
    void _masterDecNotification_() const;
  };

  /**
   * @brief Print information of the instantiation in the stream.
   */
  std::ostream& operator<<(std::ostream&, const Instantiation&);

  /**
   * @brief Hash function for gum::Instantiation.
   * @ingroup hashfunctions_group
   */
  template <>
  // class HashFunc< Instantiation > : public HashFuncBase< Instantiation > {
  class HashFunc< Instantiation >: public HashFuncBase< Instantiation > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const Instantiation& key);

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    virtual Size operator()(const Instantiation& key) const override final;
  };
} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/multidim/instantiation_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_INSTANTIATION_H */
