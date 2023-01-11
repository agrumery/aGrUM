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
 * @brief Headers for the abstract base class for all multi dimensionnal
 * containers.
 *
 * MultiDimAdressable is the abstract base class for all multi dimensionnal
 * adressable. Its purpose is to deal with slave Instantiation and notification
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#ifndef GUM_MULTIDIM_ADRESSABLE_H
#define GUM_MULTIDIM_ADRESSABLE_H

#include <iostream>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/tools/multidim/instantiation.h>

namespace gum {
  // =========================================================================
  // ===                             GUM_MULTI_DIM_ADRESSABLE              ===
  // =========================================================================
  /**
   * @class MultiDimAdressable
   * @headerfile multiDimAdressable.h <agrum/tools/multidim/multiDimAdressable.h>
   * @ingroup multidim_group
   *
   * @brief Abstract base class for all multi dimensionnal addressable.
   *
   * MultiDimAdressable is the abstract base class for all multi dimensionnal
   * adressable. Its purpose is to deal with slave Instantiation and
   * notification.
   */
  class MultiDimAdressable: public MultiDimInterface {
    public:
    // =======================================================================
    /// @name Constructors / Destructors
    // =======================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    MultiDimAdressable();

    /**
     * @brief Copy constructor.
     */
    MultiDimAdressable(const MultiDimAdressable& from);
    MultiDimAdressable& operator=(const MultiDimAdressable& from);

    /**
     * @brief Class move constructor.
     */
    MultiDimAdressable(MultiDimAdressable&&) noexcept;
    MultiDimAdressable& operator=(MultiDimAdressable&& from) noexcept;


    /**
     * @brief Destructor.
     */
    ~MultiDimAdressable() override;

    /// @}
    // =======================================================================
    /// @name Slave management and extension due to slave management
    // =======================================================================
    /// @{
    /**
     * @brief In order to insure the dereference for decorators, we need to
     * virtualize the access to master pointer.
     * @return Returns the master of this MultiDimAdressable.
     */
    virtual MultiDimAdressable& getMasterRef() = 0;

    /**
     * @brief In order to insure the dereference for decorators, we need to
     * virtualize the access to master pointer.
     * @return Returns the master of this MultiDimAdressable.
     */
    virtual const MultiDimAdressable& getMasterRef() const = 0;

    /**
     * @brief Register i as a slave of this MultiDimAdressable.
     * @param i The Instantiation to enslave.
     * @return Returns true if i becomes a slave of this MultiDimAdressable.
     */
    virtual bool registerSlave(Instantiation& i) = 0;

    /**
     * @brief Unregister i as a slave of this MultiDimAdressable.
     * @param i The Instantiation to free.
     * @return Returns true, whatever happens.
     */
    virtual bool unregisterSlave(Instantiation& i) = 0;

    /**
     * @brief Listen to changes in a given Instantiation.
     * @param i The Instantiation to listen.
     * @param var The changed dim.
     * @param oldval The old value.
     * @param newval The changed value.
     */
    virtual void changeNotification(const Instantiation&          i,
                                    const DiscreteVariable* const var,
                                    Idx                           oldval,
                                    Idx                           newval)
       = 0;

    /**
     * @brief Listen to setFirst in a given Instantiation.
     * @param i The Instantiation to listen.
     */
    virtual void setFirstNotification(const Instantiation& i) = 0;

    /**
     * @brief Listen to setLast in a given Instantiation.
     * @param i The Instantiation to listen.
     */
    virtual void setLastNotification(const Instantiation& i) = 0;

    /**
     * @brief Listen to increment in a given Instantiation.
     * @param i The Instantiation to listen.
     */
    virtual void setIncNotification(const Instantiation& i) = 0;

    /**
     * @brief Listen to increment in each recorded Instantiation.
     * @param i The Instantiation to listen.
     */
    virtual void setDecNotification(const Instantiation& i) = 0;

    /**
     * @brief Listen to an assignment of a value in a Instantiation.
     * @param i The Instantiation to listen.
     */
    virtual void setChangeNotification(const Instantiation& i) = 0;

    /**
     * @brief Return a string representation of internal data about i in this.
     * @param i The Instantiation to represent.
     * @return Return a string representation of internal data about i in this.
     */
    virtual std::string toString(const Instantiation* i) const = 0;

    /// @}
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/multidim/implementations/multiDimAdressable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_MULTIDIM_ADRESSABLE_H */
