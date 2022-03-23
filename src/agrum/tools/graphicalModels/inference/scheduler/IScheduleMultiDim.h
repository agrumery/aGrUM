/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) et Christophe GONZALES(@AMU)
 *  info_at_agrum_dot_org
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
 * @brief The Table-agnostic base class of scheduleMultiDim
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef GUM_ISCHEDULE_MULTI_DIM_H
#define GUM_ISCHEDULE_MULTI_DIM_H

#include <atomic>
#include <string>
#include <memory>

#include <agrum/agrum.h>
#include <agrum/tools/core/sequence.h>
#include <agrum/tools/variables/discreteVariable.h>

namespace gum {

  /**
   * @class IScheduleMultiDim
   * @brief The Table-agnostic base class of scheduleMultiDim
   * @headerfile IScheduleMultiDim.h <agrum/tools/graphicalModels/inference/scheduler/IScheduleMultiDim.h>
   * @ingroup inference_schedule
   */
  class IScheduleMultiDim {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// constructs a IScheduleMultiDim
    /** @param id if specified and different from 0, the id of the constructed
     * ScheduleMultiDim, else an Id is automatically provided to the
     * ScheduleMultiDim.
     */
    explicit IScheduleMultiDim(const Idx id = 0);

    /// virtual copy constructor
    virtual IScheduleMultiDim* clone() const = 0;

    /// virtual copy constructor enabling to force a copy of the content
    virtual IScheduleMultiDim* clone(bool force_copy) const = 0;

    /// destructor
    virtual ~IScheduleMultiDim();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################

    /// @{

    /// checks whether two IScheduleMultiDim have exactly the same ID
    /** Two IScheduleMultiDim having the same ID should imply that they also
     * contain the same sequences of variables and the same tables. */
    virtual bool operator==(const IScheduleMultiDim&) const;

    /// checks whether two IScheduleMultiDim have different IDs
    /** Two IScheduleMultiDim having different IDs should mean that they either
     * contain the different sequences of variables and/or different tables. */
    virtual bool operator!=(const IScheduleMultiDim&) const;

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// checks whether two IScheduleMultiDim have the same variables
    /** Method hasSameVariables tests whether two IScheduleMultiDim contain the same
     * sequences of variables. It is weaker than operator== in the sense that
     * we do not check for the IScheduleMultiDim's ID nor for the content of
     * the table stored into the IScheduleMultiDim, if any.
     * @param m the IScheduleMultiDim with which we test similarity. */
    virtual bool hasSameVariables(const IScheduleMultiDim& m) const = 0;

    /// checks whether two IScheduleMultiDim contain precisely the same table
    /** By "contain precisely the same table", we mean that the actual tables
     * pointed to by the IScheduleMultiDim are the same (i.e., either they are
     * both not yet allocated but have the same sequences of variables or they
     * have the same content) */
    virtual bool hasSameContent(const IScheduleMultiDim&) const = 0;

    /// returns whether the ScheduleMultiDim contains a real table or not
    /** @returns true if the ScheduleMultiDim is abstract, i.e., it does not
     * actually contains an allocated multidimensional table. */
    virtual bool isAbstract() const = 0;

    /// indicates whether the ScheduleMultiDim contains a table and possesses it
    /** Non-abstract ScheduleMultiDim can either contain a multidimensional table
     * or they can reference an external table. This method indicates whether we
     * are in the first or the second case described above. */
    virtual bool containsMultiDim() const = 0;

    /// if the ScheduleMultiDim is not abstract, make it abstract again
    /** When a ScheduleMultiDim contains a multidimensional table, making it
     * abstract is equivalent to freeing the table from memory and saying that
     * the ScheduleMultiDim contains a null pointer. If the ScheduleMultiDim
     * contains only a reference on an external table, then this table
     * is kept untouched, but the ScheduleMultiDim now points toward a null
     * pointer. Of course, if the ScheduleMultiDim was already abstract, then
     * nothing is executed. */
    virtual void makeAbstract() = 0;

    /// returns the id of the ScheduleMultiDim
    /** Each ScheduleMultiDim has an Id that enables to find it easily. This
     * is especially useful when copying schedule trees.
     * @warning Note that different schedule trees may contain ScheduleMultiDim
     * with the same ids. However, within a given tree, no two ScheduleMultiDim
     * should have the same id. */
    Idx id() const;

    /// returns the set of variables involved in the ScheduleMultiDim
    virtual const Sequence< const DiscreteVariable* >& variablesSequence() const = 0;

    /// returns the domain size the table would have after its creation
    virtual Size domainSize() const = 0;

    /// returns the sizeof of the elements stored into the ScheduleMultiDim
    /** This method is useful for determining schedules' memory usage: this is
     * actually equal to the number of elements times the sizeof of these
     * elements. The goal of sizeOfContent is to return this sizeof */
    virtual double sizeOfContent() const = 0;

    /// displays the content of the ScheduleMultiDim
    virtual std::string toString() const = 0;

    /// reset the id generator to 0
    /** This method is essentially useful for performing aGrUM's testunits
     * (depending on the order in which they are executed, the results could
     * be difficult to predict without ensuring that, at the beginning of a
     * testunit, the id of the first ScheduleMultiDim is always 1). */
    static void resetIdGenerator();

    /// @}

    protected:
    /// copy operator
    IScheduleMultiDim& operator=(const IScheduleMultiDim& from);

    /// move operator
    IScheduleMultiDim& operator=(IScheduleMultiDim&& from);

    /// copy constructor
    IScheduleMultiDim(const IScheduleMultiDim& from);

    /// move constructor
    IScheduleMultiDim(IScheduleMultiDim&& from);


    /// the unique Id of the ScheduleMultiDim
    Idx id_;


    private:
    /// returns a new distinct ID for each abstract scheduleMultiDim
    static Idx _newId_();

    // the id used whenever any ScheduleMultiDim of any type asks for a new id
    static std::atomic< Idx > _multidim_id_;
  };

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphicalModels/inference/scheduler/IScheduleMultiDim_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_ISCHEDULE_MULTI_DIM_H */
