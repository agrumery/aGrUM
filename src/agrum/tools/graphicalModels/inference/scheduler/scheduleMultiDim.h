/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief a MultiDimImplementation Wrapper used for scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef GUM_SCHEDULE_MULTI_DIM_H
#define GUM_SCHEDULE_MULTI_DIM_H

#include <string>

#include <agrum/agrum.h>

#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/core/sequence.h>
#include <agrum/tools/multidim/implementations/multiDimDecorator.h>
#include <agrum/tools/multidim/implementations/multiDimImplementation.h>
#include <agrum/tools/variables/discreteVariable.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // we should grant ScheduleDeleteMultiDim the access to the hashtable actually
  // containing the MultiDimImplementation: thus, when ScheduleDeleteMultiDim
  // deletes an implementation, it can remove it from the hashtable and, thus,
  // the ScheduleMultiDims pointing to it will become abstract.
  template < typename GUM_SCALAR >
  class ScheduleDeleteMultiDim;
#endif

  /**
   * @class ScheduleMultiDim
   * @brief a MultiDimImplementation Wrapper used for scheduling inferences
   *
   * A ScheduleMultiDim is a wrapper that contains either a "real" multidim
   * table or an Id that indicates that the multidim table that should be
   * contained has not been computed yet (by the scheduler). This Id enables
   * the scheduler to know which is the operation the result of which will be
   *the
   * multidim table that will eventually be contained in the ScheduleMultiDim.
   * Here is a brief piece of code that should highlight the concept:
   * @code
   * // some arbitrary potentials (to be initialized before going on)
   * gum::Potential<float> pot1, pot2, pot3;
   *
   * // we wish to schedule ( pot1 + pot2 ) + pot3
   * // so, first, create ScheduleMultiDims for wrapping these potentials
   * gum::ScheduleMultiDim<float> f1 ( pot1 ), f2 ( pot2 ), f3 ( pot3 );
   *
   * // now schedule a combination (+) between f1 and f2
   * gum::ScheduleCombine<float> comb1 ( &f1, &f2, add );
   *
   * // get the result and schedule it with f3
   * const ScheduleMultiDim<float>& result1 = comb1.result ();
   * gum::ScheduleCombine<float> comb2 ( &result2, &f3,add );
   *
   * // get the resulting ScheduleMultiDim
   * const ScheduleMultiDim<float>& result2 = comb2.result ();
   *
   * // here, no addition has been performed yet. We just have a structure
   * // that indicates which operations we wish to do. So, for the moment,
   * // result1 and result2 do not contain real multidim tables but just ids.
   * // As such, they are called abstract and trying to get their "real"
   * // multiDim table (using method multiDim()) would throw a NotFound
   *exception.
   * std::cout << result1.isAbstract ();
   * std::cout << result2.isAbstract ();
   * std::cout << ! f1.isAbstract ();
   *
   * // now, we can actually perform the operations
   * comb1.execute ();
   * std::cout << ! result1.isAbstract ();
   * comb2.execute ();
   *
   * // here, we can display the content of the real multidim table stored
   * // into result2
   * std::cout << result2.multiDim ();
   * @endcode
   *
   * So, to summarize the key idea underlying Schedule* classes: these classes
   * encapsulate operations to perform and multidim tables that should be passed
   * as argument to these operations. But nothing is actually computed until
   * the execute() methods of the scheduled operations are executed.
   */
  template < typename GUM_SCALAR >
  class ScheduleMultiDim {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// constructs a ScheduleMultiDim containing an already built implementation
    explicit ScheduleMultiDim(const MultiDimImplementation< GUM_SCALAR >&);

    /// constructs a ScheduleMultiDim containing an already built implementation
    explicit ScheduleMultiDim(const MultiDimDecorator< GUM_SCALAR >&);

    /// construct a ScheduleMultiDim for an implementation yet to be built
    /** The ScheduleMultiDim created is abstract, i.e., it does not contain a
     * proper MultiDimImplementation yet. However, the variables of the latter
     * need be known to optimize inference processes
     * @warning the sequence of variables is copied into the wrapper. */
    explicit ScheduleMultiDim(const Sequence< const DiscreteVariable* >& vars);

    /// copy constructor
    ScheduleMultiDim(const ScheduleMultiDim< GUM_SCALAR >&);

    /// destructor
    ~ScheduleMultiDim();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################

    /// @{

    /// copy operator
    ScheduleMultiDim< GUM_SCALAR >&
       operator=(const ScheduleMultiDim< GUM_SCALAR >&);

    /// checks whether two ScheduleMultiDim are related to the same table
    bool operator==(const ScheduleMultiDim< GUM_SCALAR >&) const;

    /// checks whether two ScheduleMultiDim are related to different tables
    bool operator!=(const ScheduleMultiDim< GUM_SCALAR >&) const;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// returns whether the ScheduleMultiDim contains a real
    /// multiDimImplementation
    /** @returns true if the ScheduleMultiDim is abstract, i.e., it is does not
     * actually contains a real MultiDimImplementation but rather a ID
     * indicating
     * that the real multiDimImplementation is yet to be created as a result of
     * an
     * operation on other multiDimImplementations. */
    bool isAbstract() const;

    /** @brief returns the multiDimImplementation actually contained in the
     * ScheduleMultiDim
     *
     * @throws NotFound exception is thrown if the multidimImplementation does
     *not
     * exist yet (because it has not been computed yet) */
    const MultiDimImplementation< GUM_SCALAR >& multiDim() const;

    /// returns the id of the ScheduleMultiDim
    Idx id() const;

    /// returns the set of variables involved in the multidim
    const Sequence< const DiscreteVariable* >& variablesSequence() const;

    /// returns the domain size of the multidim
    Size domainSize() const;

    /// sets a new multiDimImplementation inside the wrapper
    /** @throws DuplicateElement exception is thrown if the
     * MultiDimImplementation
     * has already been wrapped in a ScheduleMultiDim with another id */
    void setMultiDim(const MultiDimImplementation< GUM_SCALAR >&);

    /// sets a new multiDimDecorator inside the wrapper
    /** @throws DuplicateElement exception is thrown if the MultiDimDecorator
     * has already been wrapped in a ScheduleMultiDim with another id */
    void setMultiDim(const MultiDimDecorator< GUM_SCALAR >&);

    /// displays the content of the multidim
    std::string toString() const;

    /// @}

    private:
    /// grant accesses to ScheduleDeleteMultiDim
    friend class ScheduleDeleteMultiDim< GUM_SCALAR >;

    /// the unique Id of the ScheduleMultiDim
    Idx id__;

    /// returns a new distinct ID for each abtract scheduleMultiDim
    static Idx newId__();

    /// returns a mapping from id to multidimImplementations
    static HashTable< Idx, const MultiDimImplementation< GUM_SCALAR >* >&
       id2multidim__();

    /// returns the id corresponding to a given multidim
    /** useful to assign the same id every time a given MultiDimImplementation
     * is wrapped into a ScheduleMultiDim */
    static HashTable< const MultiDimImplementation< GUM_SCALAR >*, Idx >&
       multidim2id__();

    /// returns a table indicating how many ScheduleMultiDim have the same id
    static HashTable< Idx, Idx >& id2refs__();

    /// returns a table with the variables of the table corresponding to id
    static HashTable< Idx, const Sequence< const DiscreteVariable* >* >&
       id2vars__();

    /// returns a table with the domain size of the table corresponding to id
    static HashTable< Idx, Size >& id2size__();
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleMultiDim_tpl.h>

#endif /* GUM_SCHEDULE_MULTI_DIM_H */