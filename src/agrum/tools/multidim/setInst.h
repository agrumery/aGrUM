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
 * @brief Headers of SetInst.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Ariele MAESANO
 */

#ifndef GUM_SETINST_H
#define GUM_SETINST_H

#include <ostream>

#include <agrum/agrum.h>

#include <agrum/tools/core/bijection.h>
#include <agrum/tools/multidim/implementations/multiDimAdressable.h>

namespace gum {

  class MultiDimAdressable;

  class Instantiation;
  // ==========================================================================
  // ===                            GUM_SetInst                             ===
  // ==========================================================================
  /**
   * @class SetInst
   * @headerfile setInst.h <agrum/tools/multidim/SetInst.h>
   * @ingroup multidim_group
   *
   * @brief Class for assigning/browsing values to tuples of discrete variables.
   *
   * SetInst is designed to assign a set of deterministic values to tuples of
   * variables for contextual dependencies detection.
   *
   * For a variable I of possible determinstic instances \f$i1, i2, i3,
   * \ldots\f$ in the expresion of a set of instantiations is express by the
   * boolean state of each instance transform in an integer.
   * For example: \f$i1\f$ or \f$i2\f$ or \f$i4\f$ will be :
   * \code
   * i4 i3 i2 i1
   *  1  0  1  1 -> 11
   * \endcode
   *
   * There is two different types of setters:
   *  - function ending in val take the instance and encode it.
   *  - function ending in vals take allready the encoded expression of the
   *  intances union.
   *
   * @warning The model is based on the implementation of aGrUM Instantiation
   * source code.  The only difference is the impossibility to loop since the
   * SetInst is not create to run through, but to collect all union of the
   * possible instantiations of each variable, on the multiDimAdressable.
   *
   * To print information about a SetInst use the following function:
   * @see operator<<(std::ostream&, const SetInst&)
   */
  class SetInst {
    public:
    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /**
     * @brief Default constructor: creates an empty tuple.
     */
    SetInst();

    /**
     * @brief Copy constructor.
     * @param aI The SetInst to copy.
     */
    SetInst(const SetInst& aI);

    /**
     * @brief Copy constructor.
     * @param aI The SetInst to copy.
     */
    SetInst(const Instantiation& aI);

    /**
     * @brief Copy operator.
     * @warning An OperationNotAllowed will be raised if you use the copy
     * operator.
     */
    SetInst& operator=(const SetInst& aI);

    /**
     * @brief Constructor for a SetInst of all the variables of a
     * MultiDimAdressable.
     *
     * @param aMD The array the variables of which are those of the SetInst.
     */
    SetInst(MultiDimAdressable& aMD);

    /**
     * @brief Constructor for a SetInst of all the variables of a
     * MultiDimAdressable.
     *
     * @param aMD The array the variables of which are those of the SetInst.
     */
    SetInst(const MultiDimAdressable& aMD);

    /**
     * @brief Constructor for a SetInst of all the variables of a
     * MultiDimAdressable.
     *
     * @param aMD The array the variables of which are those of the SetInst.
     */
    SetInst(MultiDimAdressable* aMD);

    /**
     * @brief Constructor for a SetInst of all the variables of a
     * MultiDimAdressable.
     *
     * @param aMD The array the variables of which are those of the SetInst
     */
    SetInst(const MultiDimAdressable* aMD);

    /**
     * @brief Class destructor.
     */
    ~SetInst();

    /// @}
    // =========================================================================
    /// @name Accessors / Modifiers
    // =========================================================================
    /// @{

    /**
     * @brief Returns the number of variables in the SetInst.
     * @return Returns the number of variables in the SetInst.
     */
    Idx nbrDim() const;

    /**
     * @brief Adds a new variable in the SetInst.
     *
     * @warning Variable v is known to the SetInst only by a pointer to it.  As
     * a result, this is not a copy of v that is used by SetInst But rather v
     * itself. As such, v should never be deleted from memory until the SetInst
     * is removed.
     *
     * @param v The new variable.
     *
     * @throw DuplicateElement Raised if c is already in this SetInst.
     * @throw OperationNotAllowed Raised if the SetInst is a slave.
     */
    void add(const DiscreteVariable& v);

    /**
     * @brief Removes a variable from the SetInst.
     *
     * @param v The variable to erase.
     * @throw NotFound Raised if v is not in this SetInst.
     * @throw OperationNotAllowed Raised if the SetInst is a slave.
     */
    void erase(const DiscreteVariable& v);

    /**
     * @brief Erase all variables from an SetInst
     * @throw OperationNotAllowed Raised if the SetInst is a slave.
     */
    void clear();

    /**
     * @brief Returns the product of the variable's domain size in the SetInst.
     * @return Returns the product of the variable's domain size in the
     * SetInst.
     */
    Size domainSize() const;

    /**
     * @brief Returns the position of the variable v.
     * @throw NotFound Raised if v does not belong to the SetInst.
     */
    Idx pos(const DiscreteVariable& v) const;

    /**
     * @brief Returns the current value of the variable at position i.
     *
     * @param i The index of the variable.
     * @return Returns the current value of the variable at position i.
     *
     * @throw NotFound Raised if the element cannot be found.
     */
    Size vals(Idx i) const;

    /**
     * @brief Returns the current value of a given variable.
     *
     * @param var The variable the value of which we wish to know.
     * @return Returns the current value of a given variable.
     *
     * @throw NotFound Raised it var does not belong to the SetInst.
     */
    Size vals(const DiscreteVariable& var) const;

    /**
     * @brief Returns the current value of a given variable.
     *
     * @param var The variable the value of which we wish to know.
     * @return Returns the current value of a given variable.
     *
     * @throw NotFound Raised if var does not belong to the SetInst.
     */
    Size vals(const DiscreteVariable* var) const;

    /**
     * @brief Returns the current value of the variable at position i.
     *
     * @param var the variable.
     * @return Returns the current value of the variable at position i.
     *
     * @throw NotFound raised if the element cannot be found.
     */
    Idx nbrOccurences(const DiscreteVariable& var) const;


    /**
     * @brief Returns the current value of a variable at a given position.
     *
     * @param i The position of the variable for which its value is returned.
     * @return Returns the current value of a variable at a given position.
     *
     * @throw NotFound raised it var does not belong to the SetInst.
     */
    Idx val(Idx i) const;

    /**
     * @brief Returns the current value of a given variable.
     *
     * @param var The variable the value of which we wish to know.
     * @return Returns the current value of a given variable.
     *
     * @throw NotFound Raised it var does not belong to the SetInst.
     */
    Idx val(const DiscreteVariable& var) const;

    /**
     * @brief Returns the current value of a given variable.
     *
     * @param var The variable the value of which we wish to know.
     * @return Returns the current value of a given variable.
     *
     * @throw NotFound Raised if var does not belong to the SetInst.
     */
    Idx val(const DiscreteVariable* var) const;

    /**
     * @brief Returns the variable at position i in the tuple.
     *
     * @param i The index of the variable.
     * @return Returns the variable at position i in the tuple.
     *
     * @throw NotFound Raised if the element cannot be found.
     */
    const DiscreteVariable& variable(Idx i) const;

    /**
     * @brief Assign newVal to variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The index of the value assigned (consider the values of v
     * as an array indexed from 0 to n of values (which might be anything from
     * real numbers to strings, etc). Parameter newVal indicates the index in
     * this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& chgVal(const DiscreteVariable& v, Idx newVal);

    /**
     * @brief Assign newVal to variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The index of the value assigned (consider the values of v
     * as an array indexed from 0 to n of values (which might be anything from
     * real numbers to strings, etc). Parameter newVal indicates the index in
     * this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& chgVal(const DiscreteVariable* v, Idx newVal);

    /**
     * @brief Assign newVal to variable at position varPos in the SetInst.
     *
     * @param varPos The index of the variable whose value is assigned in the
     * tuple of variables of the SetInst.
     * @param newVal The index of the value assigned (consider the values of
     * the variable as an array indexed from 0 to n of values (which might be
     * anything from real numbers to strings, etc). Parameter newVal indicates
     * the index in this array of the new value taken by the variable.
     * @return A reference to *this in order to chain the chgVal.
     *
     * @throw NotFound raised if the variable does not belong to this
     * @throw OutOfBounds raised if newVal is not a possible value for
     *        the variable
     */
    SetInst& chgVal(Idx varPos, Idx newVal);

    /**
     * @brief Assign newVal to variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The index of the value assigned (consider the values of v
     * as an array indexed from 0 to n of values (which might be anything from
     * real numbers to strings, etc). Parameter newVal indicates the index in
     * this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& chgVals(const DiscreteVariable& v, const Size newVal);

    /**
     * @brief Add newVal to variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The value added to the index of the value assigned
     * (consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& addVal(const DiscreteVariable& v, Idx newVal);

    /**
     * @brief Add newVal to variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The value added to the index of the value assigned
     * (consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& addVals(const DiscreteVariable& v, const Size newVal);

    /**
     * @brief Remove newVal from the variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The value removed to the index of the value assigned
     * (consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& remVal(const DiscreteVariable& v, Idx newVal);

    /**
     * @brief Remove newVal from the variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The value removed to the index of the value assigned
     * (consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& remVals(const DiscreteVariable& v, const Size newVal);

    /**
     * @brief Does an intersection (binary and) between the old value and new
     * value.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The value intersected with the index of the value assigned
     * (consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& interVals(const DiscreteVariable& v, const Size newVal);

    /**
     * @brief Does an intersection (binary and) between the old value and new
     * value.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The value intersected with the index of the value assigned
     * (consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& interVal(const DiscreteVariable& v, Idx newVal);

    /**
     * @brief Assign newVal to variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The index of the value assigned (consider the values of v
     * as an array indexed from 0 to n of values (which might be anything from
     * real numbers to strings, etc). Parameter newVal indicates the index in
     * this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& chgVals(const DiscreteVariable* v, const Size newVal);

    /**
     * @brief Add newVal to variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The value added to the index of the value assigned
     * (consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& addVal(const DiscreteVariable* v, Idx newVal);

    /**
     * @brief Add newVal to variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The value added to the index of the value assigned
     * (consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& addVals(const DiscreteVariable* v, const Size newVal);

    /**
     * @brief Remove newVal from the variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The value removed to the index of the value assigned
     * (consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& remVal(const DiscreteVariable* v, Idx newVal);

    /**
     * @brief Remove newVal from the variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The value removed to the index of the value assigned
     * (consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& remVals(const DiscreteVariable* v, const Size newVal);

    /**
     * @brief Does an intersection (binary and) between the old value and new
     * value.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The value intersected with the index of the value assigned
     * (consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& interVals(const DiscreteVariable* v, const Size newVal);

    /**
     * @brief Does an intersection (binary and) between the old value and new
     * value.
     *
     * @param v The variable whose value is assigned.
     * @param newVal The value intersected with the index of the value assigned
     * (consider the values of v as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable v does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for v.
     */
    SetInst& interVal(const DiscreteVariable* v, Idx newVal);

    /**
     * @brief Assign newVal to variable at position varPos in the SetInst.
     *
     * @param varPos The index of the variable whose value is assigned in the
     *        tuple of variables of the SetInst.
     * @param newVal The index of the value assigned (consider the values of the
     *        variable as an array indexed from 0 to n of values (which might be
     *        anything from real numbers to strings, etc). Parameter newVal
     *        indicates the index in this array of the new value taken by the
     *        variable.
     * @return A reference to *this in order to chain the chgVal.
     * @throw NotFound Raised if the variable does not belong to this
     * @throw OutOfBounds Raised if newVal is not a possible value for
     *        the variable
     */
    SetInst& chgVals(Idx varPos, const Size newVal);

    /**
     * @brief Add newVal to variable v in the SetInst.
     *
     * @param varPos The variable whose value is assigned.
     * @param newVal The value added to the index of the value assigned
     * (consider the values of varPos as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by
     * varPos.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable varPos does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for varPos.
     */
    SetInst& addVal(Idx varPos, Idx newVal);

    /**
     * @brief Add newVal to variable varPos in the SetInst.
     *
     * @param varPos The variable whose value is assigned.
     * @param newVal The value added to the index of the value assigned
     * (consider the values of varPos as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by
     * varPos.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable varPos does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for varPos.
     */
    SetInst& addVals(Idx varPos, const Size newVal);

    /**
     * @brief Remove newVal from the variable varPos in the SetInst.
     *
     * @param varPos The variable whose value is assigned.
     * @param newVal The value removed to the index of the value assigned
     * (consider the values of varPos as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by
     * varPos.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable varPos does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for varPos.
     */
    SetInst& remVal(Idx varPos, Idx newVal);

    /**
     * @brief Remove newVal from the variable varPos in the SetInst.
     *
     * @param varPos The variable whose value is assigned.
     * @param newVal The value removed to the index of the value assigned
     * (consider the values of varPos as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by
     * varPos.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable varPos does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for varPos.
     */
    SetInst& remVals(Idx varPos, const Size newVal);

    /**
     * @brief Does an intersection (binary and) between the old value and new
     * value.
     *
     * @param varPos The variable whose value is assigned.
     * @param newVal The value intersected with the index of the value assigned
     * (consider the values of varPos as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by
     * varPos.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable varPos does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for varPos.
     */
    SetInst& interVals(Idx varPos, const Size newVal);

    /**
     * @brief Does an intersection (binary and) between the old value and new
     * value.
     *
     * @param varPos The variable whose value is assigned.
     * @param newVal The value intersected with the index of the value assigned
     * (consider the values of varPos as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by
     * varPos.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable varPos does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for varPos.
     */
    SetInst& interVal(Idx varPos, Idx newVal);

    /**
     * @brief Does the difference (binary or) between the old value and new
     * value.
     *
     * @param varPos The variable whose value is assigned.
     * @param newVal The value intersected with the index of the value assigned
     * (consider the values of varPos as an array indexed from 0 to n of values
     * (which might be anything from real numbers to strings, etc). Parameter
     * newVal indicates the index in this array of the new value taken by
     * varPos.
     * @return Returns a reference to *this in order to chain the chgVal.
     *
     * @throw NotFound Raised if variable varPos does not belong to the SetInst.
     * @throw OutOfBounds Raised if newVal is not a possible value for varPos.
     */
    SetInst& chgDifVal(Idx varPos, const Size newVal);

    /**
     * @brief Change all the values to match does in i.
     * @param i The new set of value for this SetInst.
     * @return Returns a reference to *this in order to chain the chgVal.
     */
    SetInst& chgValIn(const SetInst& i);

    /**
     * Indicates whether a given variable belongs to the SetInst.
     *
     * @param v The variable for which the test is made.
     * @return Returns true if v is in the SetInst.
     */
    bool contains(const DiscreteVariable& v) const;

    /**
     * Indicates whether a given variable belongs to the SetInst.
     *
     * @param v A pointer on the variable for which the test is made.
     * @return Returns true if *v is in the SetInst.
     */
    bool contains(const DiscreteVariable* v) const;

    /**
     * @brief Returns the sequence of DiscreteVariable of this SetInst.
     * @return Returns the sequence of DiscreteVariable of this SetInst.
     */
    const Sequence< const DiscreteVariable* >& variablesSequence() const;

    /**
     * @brief Returns true if the SetInst is empty.
     * @return Returns true if the SetInst is empty.
     */
    virtual bool empty() const;

    /**
     * @brief Give a string version of SetInst.
     * @return Returns a string version of SetInst.
     */
    std::string toString() const;

    /**
     * @brief Reorder the variables given v.
     * @param v The new order of variables in this SetInst.
     */
    void reorder(const Sequence< const DiscreteVariable* >& v);

    /**
     * @brief Reorder the variables given i.
     * @param i The new order of variables in this SetInst.
     */
    void reorder(const SetInst& i);

    /// @}
    // =========================================================================
    /// @name Overflow management methods.
    // =========================================================================
    /// @{

    /**
     * @brief Indicates whether the current value of the tuple is correct or
     * not.
     *
     * @code
     * for(SetInst i.setFirst(); !i.inOverflow(); ++i) {
     *   // code...
     * }
     * @endcode
     */
    bool inOverflow() const;

    /**
     * @brief Removes the flag overflow. See full documentation for details.
     * (Recommended).
     *
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
    void unsetOverflow();

    /**
     * @brief Alias for unsetOverflow().
     * @see unsetOverflow().
     */
    void unsetEnd();

    /**
     * @brief Returns true if the SetInst reached the end.
     *
     * Function end() should be used as in:
     * @code
     * for(SetInst i.setFirst();! i.end(); ++i) {
     *   // code
     * }
     * @endcode
     */
    bool end() const;

    /**
     * @brief Returns true if the SetInst reached the rend.
     *
     * Function end() should be used as in:
     * @code
     * for(SetInst i.setLast();! i.rend(); --i) {
     *   // code
     * }
     * @endcode
     */

    bool rend() const;

    /// @}
    /// @{


    /// @}

    // ############################################################################
    /// @name Static methods
    // ############################################################################
    /// @{

    /**
     * @brief Assign the values of i in j, using bij as a bijection between i
     * and j variables.
     *
     * @param bij Firsts are variables in i and seconds are variables in j.
     * @param i An SetInst used to change the values in j.
     * @param j An SetInst which receives new values from i using bij.
     *
     * @throw NotFound raised if a variable in i does not point to a variable
     * in j or if a variable in i is missing in bij.
     */
    static void assign_values(Bijection< const DiscreteVariable*, const DiscreteVariable* >& bij,
                              const SetInst&                                                 i,
                              SetInst&                                                       j);

    /// @}

    protected:
    virtual void replace_(const DiscreteVariable* x, const DiscreteVariable* y);

    private:
    /// The tuple of variables to be instantiated.
    Sequence< const DiscreteVariable* > _vars_;

    /// The current SetInst: the value of the tuple.
    std::vector< Size > _vals_;

    /// The overflow flag.
    bool _overflow_;

    /**
     * @brief Swap 2 vars in the SetInst.
     * @param i The first variable to swap.
     * @param j The second variable to swap.
     */
    void _swap_(Idx i, Idx j);

    /**
     * @brief Change the value of a variable.
     * @param varPos The variable index.
     * @param newVal The new value.
     */
    void _chgVal_(Idx varPos, Idx newVal);

    /**
     * @brief Change the value of a variable.
     * @param varPos The variable index.
     * @param newVal The new value.
     */
    void _chgVals_(Idx varPos, const Size newVal);

    /**
     *  @brief Adds a new var to the sequence of vars.
     *
     *  If variable v already belongs to the SetInst tuple of variables, then
     *  nothing is done. In particular, no exception is thrown in this case.
     *
     *  @ warning note that this function does not deassociate the SetInst from
     *  its master MultiDimAdressable, if any. To do so, use function add
     *  instead.
     *
     *  @warning this function does not notify the master MultiDimAdressable,
     *  if any. Use in addition function chgVal or  _chgVal_ if need be.
     *
     *  @warning variable v is known to the SetInst only by a pointer to it.
     *  As a result, this is not a copy of v that is used by SetInst but rather
     *  v itself. As such, v should never be deleted from memory until the
     *  SetInst is removed.
     *
     *  @param v The new var.
     *  @throw DuplicateElement Raised if v is already in the SetInst.
     */
    void _add_(const DiscreteVariable& v);

    /**
     *  @brief Removes a variable from the sequence of vars.
     *
     *  If variable v does not belong to the SetInst tuple of variables, then
     *  nothing is done. In particular, no exception is thrown in this case.
     *
     *  @warning this function does not notify the master MultiDimAdressable,
     *  if any.
     *
     *  @warning note that this function does not deassociate the SetInst from
     *  its master MultiDimAdressable, if any. To do so, use function removeDim
     *  instead.
     *
     *  @param v The variable to remove.
     */
    void _erase_(const DiscreteVariable& v);

    /**
     * @brief Intialize the SetInst.
     * @param master The master of this SetInst.
     */
    void _init_(MultiDimAdressable* master);
  };

  /// Print information of the SetInst in the stream.
  std::ostream& operator<<(std::ostream&, const SetInst&);

  /**
   * @brief Adds a variable to inst.
   * @param inst The SetInst to which a variable is added.
   * @param i The variable to add.
   * @return Returns a reference over inst.
   * @throw DuplicateElement If i is already in the SetInst.
   */
  gum::SetInst& operator<<(gum::SetInst& inst, const gum::DiscreteVariable& i);

  /**
   * @brief Removes a variable to inst.
   * @param inst The SetInst to which a variable is removed.
   * @param i The variable to remove.
   * @return Returns a reference over inst.
   * @throw NotFound Raised if i is not found in inst.
   */
  gum::SetInst& operator>>(gum::SetInst& inst, const gum::DiscreteVariable& i);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/multidim/setInst_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_SetInst_H */
