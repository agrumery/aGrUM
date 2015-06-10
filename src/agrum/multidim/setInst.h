/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Ariele-Paolo MAESANO  *
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

#ifndef GUM_SETINST_H
#define GUM_SETINST_H

#include <ostream>

#include <agrum/config.h>

#include <agrum/core/bijection.h>
#include <agrum/multidim/multiDimInterface.h>
#include <agrum/multidim/multiDimAdressable.h>

namespace gum {

  class MultiDimAdressable;

  class Instantiation;
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                            GUM_SetInst                               === */
  /* ============================================================================ */
  /* ============================================================================ */
  /**
   * @class SetInst setInst.h agrum/multidim/SetInst.h
   * @brief Class for assigning/browsing values to tuples of discrete variables.
   * @ingroup multidim_group
   *
   * SetInst is designed to assign a set of deterministic values to tuples of
   *variables  for contextual dependencies detection
   *
   * for variable I of possible determinstic instances i1 i2 i3 ... in
   * the expresion of a SET of instantiations is express by the boolean state of each
   *instance transform in an integer
   *
   * for example: I  i1 or i2 or i4
   *
   *                i4 i3 i2 i1      setInst
   * I               1  0  1  1   ->      11
   *
   * two different types of setters:
   *
   * # function ending in val take the instance and encode it.
   * # function ending in vals take allready the encode expression of the intances
   *union
   *
   *
   * @warning The model is based on the implementation of aGrUM Instantiation source
   *code.
   * The only difference is the impossibility to loop since the SetInst is not create
   *to
   * run through, but to collect all union of the possible instantiations of
   * each variable, on the multiDimAdressable.
   *
   * To print information about a SetInst use the following function:
   * @see operator<<(std::ostream&, const SetInst&)
   */

  class SetInst {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Default constructor: creates an empty tuple.

    SetInst();

    /**
     * @brief Copy constructor.
     *
     * @param aI the SetInst we copy
     */
    ///@{

    SetInst(const SetInst &aI);
    SetInst(const Instantiation &aI);
    ///@}

    /**
     * @brief Copy operator.
     * @throw OperationNotAllowed
     */

    SetInst &operator=(const SetInst &aI);

    /**
     * @brief Constructor for a SetInst of all the variables of a
     *        MultiDimAdressable.
     *
     * @param aMD the array the variables of which are those of the SetInst
     */

    SetInst(MultiDimAdressable &aMD);

    /**
     * @brief Constructor for a SetInst of all the variables of a
     *        MultiDimAdressable.
     *
     * @param aMD the array the variables of which are those of the SetInst
     */

    SetInst(const MultiDimAdressable &aMD);

    /**
     * @brief Constructor for a SetInst of all the variables of a
     *        MultiDimAdressable.
     *
     * @param aMD the array the variables of which are those of the SetInst
     */

    SetInst(MultiDimAdressable *aMD);

    /**
     * @brief Constructor for a SetInst of all the variables of a
     *        MultiDimAdressable.
     *
     * @param aMD the array the variables of which are those of the SetInst
     */

    SetInst(const MultiDimAdressable *aMD);

    /// Destructor.

    ~SetInst();

    /// @}

    // ##############################################################################
    /// @name Accessors / Modifiers
    // ##############################################################################
    /// @{

    /// Returns the number of variables in the SetInst.

    Idx nbrDim() const;

    /**
     * @brief Adds a new variable in the SetInst.
     *
     * @param v the new var
     * @warning variable v is known to the SetInst only by a pointer to it.
     *          As a result, this is not a copy of v that is used by SetInst
     *          But rather v itself. As such, v should never be deleted from memory
     *          until the SetInst is removed.
     * @throw DuplicateElement
     * @throw OperationNotAllowed
     */

    void add(const DiscreteVariable &v);

    /**
     * @brief Removes a variable from the SetInst.
     *
     * @param v the variable to be erased from the tuple
     * @throw NotFound
     * @throw OperationNotAllowed raised if the SetInst is a slave.
     */

    void erase(const DiscreteVariable &v);

    /**
     * @brief Erase all variables from an SetInst
     * @throw OperationNotAllowed raised if the SetInst is a slave.
     */
    void clear();

    /// Returns the product of the variable's domain size in the SetInst.

    Size domainSize() const;

    /**
     * Returns the position of the variable v.
     * @throw NotFound raised if v does not belong to the SetInst.
     */

    Idx pos(const DiscreteVariable &v) const;

    /**
     * Returns the current value of the variable at position i.
     *
     * @param i The index of the variable.
     * @throw NotFound raised if the element cannot be found.
     */

    Size vals(Idx i) const;

    /**
     * Returns the current value of a given variable.
     *
     *  @param var the variable the value of which we wish to know.
     * @throw NotFound raised it var does not belong to the SetInst.
     */

    Size vals(const DiscreteVariable &var) const;

    /**
     * Returns the current value of a given variable.
     *
     * @param var the variable the value of which we wish to know
     * @throw NotFound raised if var does not belong to the SetInst.
     */

    Size vals(const DiscreteVariable *var) const;

    /**
     * Returns the current value of the variable at position i.
     *
     * @param var the variable.
     * @throw NotFound raised if the element cannot be found.
     */

    Idx nbrOccurences(const DiscreteVariable &var) const;

    Idx val(Idx i) const;

    /**
     * Returns the current value of a given variable.
     *
     * @param var the variable the value of which we wish to know.
     * @throw NotFound raised it var does not belong to the SetInst.
     */

    Idx val(const DiscreteVariable &var) const;

    /**
     * Returns the current value of a given variable.
     *
     * @param var the variable the value of which we wish to know
     * @throw NotFound raised if var does not belong to the SetInst.
     */

    Idx val(const DiscreteVariable *var) const;

    /**
     * Returns the variable at position i in the tuple.
     *
     * @param i the index of the variable
     * @throw NotFound raised if the element cannot be found.
     */

    const DiscreteVariable &variable(Idx i) const;

    /**
     * @brief Assign newval to variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newval The index of the value assigned (consider the values of v as
     *               an array indexed from 0 to n of values (which might be anything
     *               from real numbers to strings, etc). Parameter newval indicates
     *               the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     * @throw NotFound raised if variable v does not belong to the SetInst.
     * @throw OutOfBound raised if newval is not a possible value for v.
     */

    SetInst &chgVal(const DiscreteVariable &v, Idx newval);

    /**
     * @brief Assign newval to variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newval The index of the value assigned (consider the values of v as
     *               an array indexed from 0 to n of values (which might be anything
     *               from real numbers to strings, etc). Parameter newval indicates
     *               the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     * @throw NotFound raised if variable v does not belong to the SetInst.
     * @throw OutOfBound raised if newval is not a possible value for v.
     */

    SetInst &chgVal(const DiscreteVariable *v, Idx newval);

    /**
     * @brief Assign newval to variable at position varPos in the SetInst.
     *
     * @param varPos The index of the variable whose value is assigned in the
     *        tuple of variables of the SetInst.
     * @param newval The index of the value assigned (consider the values of the
     *        variable as an array indexed from 0 to n of values (which might be
     *        anything from real numbers to strings, etc). Parameter newval
     *        indicates the index in this array of the new value taken by the
     *        variable.
     * @return A reference to *this in order to chain the chgVal.
     * @throw NotFound raised if the variable does not belong to this
     * @throw OutOfBound raised if newval is not a possible value for
     *        the variable
     */

    SetInst &chgVal(Idx varPos, Idx newval);

    /**
     * @brief Assign newval to variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newval The index of the value assigned (consider the values of v as
     *               an array indexed from 0 to n of values (which might be anything
     *               from real numbers to strings, etc). Parameter newval indicates
     *               the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     * @throw NotFound raised if variable v does not belong to the SetInst.
     * @throw OutOfBound raised if newval is not a possible value for v.
     */

    SetInst &chgVals(const DiscreteVariable &v, const Size newval);
    SetInst &addVal(const DiscreteVariable &v, Idx newVal);
    SetInst &addVals(const DiscreteVariable &v, const Size newVal);
    SetInst &remVal(const DiscreteVariable &v, Idx newVal);
    SetInst &remVals(const DiscreteVariable &v, const Size newVal);
    SetInst &interVals(const DiscreteVariable &v, const Size newVal);
    SetInst &interVal(const DiscreteVariable &v, Idx newVal);

    /**
     * @brief Assign newval to variable v in the SetInst.
     *
     * @param v The variable whose value is assigned.
     * @param newval The index of the value assigned (consider the values of v as
     *               an array indexed from 0 to n of values (which might be anything
     *               from real numbers to strings, etc). Parameter newval indicates
     *               the index in this array of the new value taken by v.
     * @return Returns a reference to *this in order to chain the chgVal.
     * @throw NotFound raised if variable v does not belong to the SetInst.
     * @throw OutOfBound raised if newval is not a possible value for v.
     */

    SetInst &chgVals(const DiscreteVariable *v, const Size newval);
    SetInst &addVal(const DiscreteVariable *v, Idx newVal);
    SetInst &addVals(const DiscreteVariable *v, const Size newVal);
    SetInst &remVal(const DiscreteVariable *v, Idx newVal);
    SetInst &remVals(const DiscreteVariable *v, const Size newVal);
    SetInst &interVals(const DiscreteVariable *v, const Size newVal);
    SetInst &interVal(const DiscreteVariable *v, Idx newVal);

    /**
     * @brief Assign newval to variable at position varPos in the SetInst.
     *
     * @param varPos The index of the variable whose value is assigned in the
     *        tuple of variables of the SetInst.
     * @param newval The index of the value assigned (consider the values of the
     *        variable as an array indexed from 0 to n of values (which might be
     *        anything from real numbers to strings, etc). Parameter newval
     *        indicates the index in this array of the new value taken by the
     *        variable.
     * @return A reference to *this in order to chain the chgVal.
     * @throw NotFound raised if the variable does not belong to this
     * @throw OutOfBound raised if newval is not a possible value for
     *        the variable
     */

    SetInst &chgVals(Idx varPos, const Size newval);

    SetInst &addVal(Idx varPos, Idx newVal);
    SetInst &addVals(Idx varPos, const Size newVal);
    SetInst &remVal(Idx varPos, Idx newVal);
    SetInst &remVals(Idx varPos, const Size newVal);
    SetInst &chgDifVal(Idx varPos, const Size newVal);
    SetInst &interVals(Idx varPos, const Size newVal);
    SetInst &interVal(Idx varPos, Idx newVal);

    SetInst &chgValIn(const SetInst &i);

    /**
     * Indicates whether a given variable belongs to the SetInst.
     *
     * @param v The variable for which the test is made.
     * @return Returns true if v is in the SetInst.
     */

    bool contains(const DiscreteVariable &v) const;

    /**
     * Indicates whether a given variable belongs to the SetInst.
     *
     * @param v A pointer on the variable for which the test is made.
     * @return Returns true if *v is in the SetInst.
     */

    bool contains(const DiscreteVariable *v) const;

    /// Returns the sequence of DiscreteVariable of this SetInst.

    const Sequence<const DiscreteVariable *> &variablesSequence() const;

    /// Returns true if the SetInst is empty.

    virtual bool empty(void) const;

    /// @}

    // ############################################################################
    /// @name Overflow management methods.
    // ############################################################################
    /// @{

    /**
     * @brief Indicates whether the current value of the tuple is correct or not.
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
     * Alias for unsetOverflow().
     * @see unsetOverflow().
     */

    void unsetEnd();

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

    bool end() const;

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

    bool rend() const;

    /// @}
    /// @{

    /// Returns the hamming distance of this SetInst.

    Idx hamming() const;

    /// Give a string version of SetInst.

    std::string toString() const;

    void reorder(const Sequence<const DiscreteVariable *> &v);

    void reorder(const SetInst &i);

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
     * @throw NotFound raised if a variable in i does not point to a variable in j
     *                 or if a variable in i is missing in bij.
     */
    static void
    assign_values(Bijection<const DiscreteVariable *, const DiscreteVariable *> &bij,
                  const SetInst &i, SetInst &j) {
      try {
        for (const auto var : i.variablesSequence())
          j.chgVal(bij.second(var), i.val(var));
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "missing variable in bijection or SetInst");
      }
    }

    /// @}

    protected:
    /// Replace x by y.
    virtual void _swap(const DiscreteVariable *x, const DiscreteVariable *y);

    private:
    /**
     * The master, if any, contains precisely the set of variables to be
     * instantiated.
     */

    // MultiDimAdressable* __master;

    /// The tuple of variables to be instantiated.

    Sequence<const DiscreteVariable *> __vars;

    /// The current SetInst: the value of the tuple.

    std::vector<Size> __vals;

    bool __overflow;

    /// swap 2 vars in the SetInst

    void __swap(Idx i, Idx j);

    void __chgVal(Idx varPos, Idx newVal);
    void __chgVals(Idx varPos, const Size newVal);

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

    void __add(const DiscreteVariable &v);

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
  };

  /// Print information of the SetInst in the stream.

  std::ostream &operator<<(std::ostream &, const SetInst &);
  gum::SetInst &operator<<(gum::SetInst &inst, const gum::DiscreteVariable &i);
  gum::SetInst &operator>>(gum::SetInst &inst, const gum::DiscreteVariable &i);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/multidim/setInst.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_SetInst_H */
