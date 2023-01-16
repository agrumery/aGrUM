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
 * @brief a Wrapper for multi-dimensional tables used for scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef GUM_SCHEDULE_MULTI_DIM_H
#define GUM_SCHEDULE_MULTI_DIM_H

#include <string>
#include <memory>

#include <agrum/agrum.h>
#include <agrum/tools/graphicalModels/inference/scheduler/IScheduleMultiDim.h>

namespace gum {

  /**
   * @class ScheduleMultiDim
   * @brief a Wrapper for multi-dimensional tables used for scheduling inferences
   * @headerfile scheduleMultiDim.h <agrum/tools/graphicalModels/inference/scheduler/scheduleMultiDim.h>
   * @ingroup inference_schedule
   *
   * A ScheduleMultiDim is a wrapper that contains either a "real" multidmensional
   * table or a nullptr if this table has not been computed yet (by an executor).
   *
   * Here is a brief piece of code that should highlight the concept:
   * @code
   * // some arbitrary potentials (to be initialized before going on)
   * gum::Potential< float > pot1, pot2, pot3;
   *
   * // we wish to schedule ( pot1 + pot2 ) + pot3
   * // so, first, create ScheduleMultiDims for wrapping these potentials
   * gum::ScheduleMultiDim< gum::Potential< float > > f1 ( pot1 );
   * gum::ScheduleMultiDim< gum::Potential< float > > f2 ( pot2 );
   * gum::ScheduleMultiDim< gum::Potential< float > > f3 ( pot3 );
   *
   * // now schedule a combination (+) between f1 and f2
   * gum::ScheduleCombination< gum::Potential< float >,
   *                           gum::Potential< float >,
   *                           gum::Potential< float > > comb1 ( &f1, &f2, add );
   *
   * // get the result and schedule it with f3
   * const ScheduleMultiDim< gum::Potential< float > >& result1 = comb1.result ();
   * gum::ScheduleCombination< gum::Potential< float >,
   *                           gum::Potential< float >,
   *                           gum::Potential< float > > comb2 ( &result2, &f3, add );
   *
   * // get the resulting ScheduleMultiDim
   * const ScheduleMultiDim<gum::Potential< float > >& result2 = comb2.result ();
   *
   * // here, no addition has been performed yet. We just have a structure
   * // that indicates which operations we wish to do. So, for the moment,
   * // result1 and result2 do not contain real Potentials.
   * // As such, they are called abstract and trying to get their "real"
   * // Potential (using method multiDim()) would throw a NullElement exception.
   * std::cout << result1.isAbstract ();
   * std::cout << result2.isAbstract ();
   * std::cout << ! f1.isAbstract ();
   *
   * // now, we can actually perform the operators
   * comb1.execute ();
   * std::cout << ! result1.isAbstract ();
   * comb2.execute ();
   *
   * // here, we can display the content of the real Potential stored
   * // into result2
   * std::cout << result2.multiDim ();
   * @endcode
   *
   * So, to summarize the key idea underlying Schedule* classes: these classes
   * encapsulate operations to perform and multidim tables that should be passed
   * as argument to these operations. But nothing is actually computed until
   * the execute() methods of the scheduled operators are executed.
   */
  template < typename TABLE >
  class ScheduleMultiDim: public IScheduleMultiDim {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// constructs a ScheduleMultiDim by copying/referencing table
    /** @param table the multidimensional table that we wish to be contained
     * into the ScheduleMultiDim
     * @param copy indicates whether the ScheduleMultiDim should use internally
     * a copy of table or rather if it should just use a reference on an
     * already created table.
     * @param id if specified and different from 0, this will be the id of the
     * constructed ScheduleMultiDim, else an Id is automatically provided to the
     * ScheduleMultiDim.
     */
    explicit ScheduleMultiDim(const TABLE& table, const bool copy, const Idx id = 0);

    /// constructs a ScheduleMultiDim by moving a table inside it
    /** @param table the multidimensional table that we wish to be contained
     * into the ScheduleMultiDim
     * @param id if specified and different from 0, this will be the id of the
     * constructed ScheduleMultiDim, else an Id is automatically provided to the
     * ScheduleMultiDim.
     */
    explicit ScheduleMultiDim(TABLE&& table, const Idx id = 0);

    /// construct a ScheduleMultiDim for a multidimensional table yet to be built
    /** The ScheduleMultiDim created is abstract, i.e., it does not contain a
     * proper multidimensional table yet. However, the variables of the latter
     * need be known to optimize inference processes
     * @param id if specified and different from 0, this will be the id of the
     * constructed ScheduleMultiDim, else an Id is automatically provided to the
     * ScheduleMultiDim.
     * @warning the sequence of variables is copied into the MultiDim. */
    explicit ScheduleMultiDim(const Sequence< const DiscreteVariable* >& vars, const Idx id = 0);

    /// copy constructor
    ScheduleMultiDim(const ScheduleMultiDim< TABLE >& from);

    /// move constructor
    ScheduleMultiDim(ScheduleMultiDim< TABLE >&& from);

    /// virtual copy constructor
    virtual ScheduleMultiDim< TABLE >* clone() const;

    /// virtual copy constructor enabling to force a copy of the content
    virtual ScheduleMultiDim< TABLE >* clone(bool force_copy) const;

    /// destructor
    virtual ~ScheduleMultiDim();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################

    /// @{

    /// copy operator
    ScheduleMultiDim< TABLE >& operator=(const ScheduleMultiDim< TABLE >& from);

    /// move operator
    ScheduleMultiDim< TABLE >& operator=(ScheduleMultiDim< TABLE >&& from);

    /// checks whether two ScheduleMultiDim have exactly the same ID
    /** Two ScheduleMultiDim having the same ID should imply that they also
     * contain the same sequences of variables and the same tables. */
    virtual bool operator==(const ScheduleMultiDim< TABLE >&) const;

    /// checks whether two ScheduleMultiDim have the same ID and type
    /** Two ScheduleMultiDim having the same ID should imply that they also
     * contain the same sequences of variables and the same tables. */
    virtual bool operator==(const IScheduleMultiDim&) const;

    /// checks whether two ScheduleMultiDim have different IDs
    /** Two ScheduleMultiDim having different IDs should mean that they either
     * contain the different sequences of variables and/or different tables. */
    virtual bool operator!=(const ScheduleMultiDim< TABLE >&) const;

    /// checks whether two ScheduleMultiDim have different IDs or types
    /** Two ScheduleMultiDim having different IDs should mean that they either
     * contain the different sequences of variables and/or different tables. */
    virtual bool operator!=(const IScheduleMultiDim&) const;

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// checks whether two ScheduleMultiDim have the same variables and types
    /** Method hasSameVariables tests whether two ScheduleMultiDim contain the same
     * sequences of variables. It is weaker than operator== in the sense that
     * we do not check for the ScheduleMultiDim's ID nor for the content of
     * the table stored into the IScheduleMultiDim, if any.
     * @param m the ScheduleMultiDim with which we test similarity. */
    virtual bool hasSameVariables(const IScheduleMultiDim& m) const;

    /// checks whether two ScheduleMultiDim have the same variables
    /** Method hasSameVariables tests whether two ScheduleMultiDim contain the same
     * sequences of variables. It is weaker than operator== in the sense that
     * we do not check for the ScheduleMultiDim's ID nor for the content of
     * the table stored into the IScheduleMultiDim, if any.
     * @param m the ScheduleMultiDim with which we test similarity. */
    virtual bool hasSameVariables(const ScheduleMultiDim< TABLE >& m) const;

    /// checks whether two ScheduleMultiDim contain precisely the same table
    /** By "contain precisely the same table", we mean that the actual tables
     * pointed to by the ScheduleMultiDim are the same (i.e., either they are
     * both not yet allocated but have the same sequences of variables or they
     * have the same content) */
    virtual bool hasSameContent(const IScheduleMultiDim&) const;

    /// checks whether two ScheduleMultiDim contain precisely the same table
    /** By "contain precisely the same table", we mean that the actual tables
     * pointed to by the ScheduleMultiDim are the same (i.e., either they are
     * both not yet allocated but have the same sequences of variables or they
     * have the same content) */
    virtual bool hasSameContent(const ScheduleMultiDim< TABLE >&) const;

    /// returns whether the ScheduleMultiDim contains a real table or not
    /** @returns true if the ScheduleMultiDim is abstract, i.e., it is does not
     * actually contains an allocated multidimensional table. */
    bool isAbstract() const final;

    /// indicates whether the ScheduleMultiDim contains a table and possesses it
    /** Non-abstract ScheduleMultiDim can either contain a multidimensional table
     * or they can reference an external table. This method indicates whether we
     * are in the first case (true) or the second case (false) described above. */
    bool containsMultiDim() const final;

    /// if the ScheduleMultiDim is not abstract, make it abstract again
    /** When a ScheduleMultiDim contains a multidimensional table, making it
     * abstract is equivalent to freeing the table from memory and saying that
     * the ScheduleMultiDim contains a null pointer. If the ScheduleMultiDim
     * contains only a reference on an external table, then this table
     * is kept untouched, but the ScheduleMultiDim now points toward a null
     * pointer. Of course, if the ScheduleMultiDim was already abstract, then
     * nothing is executed.
     * @warning Making a ScheduleMultiDim abstract has an impact neither on
     * the set of variables of the ScheduleMultiDim nor on its domain size.
     * This just affects the table contained into the ScheduleMultiDim. */
    void makeAbstract() final;

    /** @brief returns the contained multidimensional table, if any, and make
     * the ScheduleMultiDim abstract
     *
     * @throws NullElement is raised if the ScheduleMultiDim is abstract
     * @throws OperationNotAllowed is raised if the ScheduleMultiDim does not own
     * (contain) its table
     */
    TABLE* exportMultiDim();

    /** @brief returns the table actually contained in the ScheduleMultiDim
     *
     * @throws NullElement exception is thrown if the table does not exist
     * yet (because it has not been computed yet) */
    const TABLE& multiDim() const;

    /// returns the set of variables involved in the ScheduleMultiDim
    const Sequence< const DiscreteVariable* >& variablesSequence() const final;

    /// returns the domain size the table would have after its creation
    Size domainSize() const final;

    /// returns the sizeof of the elements stored into the ScheduleMultiDim
    /** This method is useful for determining schedules' memory usage: this is
     * actually equal to the number of elements times the sizeof of these
     * elements. The goal of sizeOfContent is to return this sizeof */
    double sizeOfContent() const final;

    /// assigns a new table inside the wrapper
    /** @param table the multidimensional table that we wish to be contained
     * into the ScheduleMultiDim
     * @param copy indicates whether the ScheduleMultiDim should use internally
     * a copy of table or rather if it should just use a reference on table
     */
    void setMultiDim(const TABLE& table, const bool copy);

    /// sets by move a new table inside the wrapper
    void setMultiDim(TABLE&& pot);

    /// displays the content of the ScheduleMultiDim
    std::string toString() const final;

    /// @}


    private:
    /// the multidimensional table stored into the ScheduleMultiDim
    TABLE* _table_{nullptr};

    /// indicates whether _table_ is contained or referenced
    bool _table_contained_{true};

    /// returns the sequence of variables of the table
    Sequence< const DiscreteVariable* > _var_sequence_;

    /// the domain size of the table
    Size _domain_size_{Size(1)};


    /// remove the table if it is contained in the ScheduleMultiDim
    void _removeTable_();

    /** @brief metaprogramming to get the types of the elements stored into the
     * ScheduleMultidims */
    template < typename T >
    struct ElementType {
      using value_type = T;
    };
    template < template < typename, typename... > class CONTAINER, typename T, typename... Args >
    struct ElementType< CONTAINER< T, Args... > > {
      using value_type = T;
    };
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleMultiDim_tpl.h>

#endif /* GUM_SCHEDULE_MULTI_DIM_H */
