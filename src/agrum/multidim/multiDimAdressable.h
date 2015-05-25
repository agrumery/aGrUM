/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
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
 * @brief Abstract base class for all multi dimensionnal containers.
 *
 * MultiDimAdressable is the abstract base class for all multi dimensionnal
 * adressable. Its purpose is to deal with slave Instantiation and notification
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */

#ifndef GUM_MULTIDIM_ADRESSABLE_H
#define GUM_MULTIDIM_ADRESSABLE_H

#include <iostream>
#include <vector>

#include <agrum/config.h>

#include <agrum/multidim/instantiation.h>

namespace gum {

  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                             GUM_MULTI_DIM_ADRESSABLE                 === */
  /* ============================================================================ */
  /* ============================================================================ */
  /**
   * @class MultiDimAdressable
   * @brief Abstract base class for all multi dimensionnal addressable.
   * @ingroup multidim_group
   *
   * MultiDimAdressable is the abstract base class for all multi dimensionnal
   * adressable. Its purpose is to deal with slave Instantiation and notification.
   */
  /* ============================================================================ */

  class MultiDimAdressable : public MultiDimInterface {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Default constructor.

    MultiDimAdressable();

    /// Copy constructor.

    MultiDimAdressable(const MultiDimAdressable &from);

    /// Destructor.

    virtual ~MultiDimAdressable();

    /// @}

    // ############################################################################
    /// @name Slave management and extension due to slave management
    // ############################################################################
    /// @{

    /*
     * Register i as a slave of this gum::MultiDimAdressable.
     * @return Returns true if i becomes a slave of this gum::MultiDimAdressable.
     */

    virtual bool registerSlave(Instantiation &i) = 0;

    /**
     * Unregister i as a slave of this gum::MultiDimAdressable.
     * @return Returns true, whatever happens.
     */

    virtual bool unregisterSlave(Instantiation &i) = 0;

    /**
     * Listen to change in a given Instantiation.
     * @param i the Instantiation
    * @param var The changed dim.
    * @param oldval The old value.
    * @param newval The changed value.
     */

    virtual void changeNotification(Instantiation &i,
                                    const DiscreteVariable *const var,
                                    const Idx &oldval, const Idx &newval) = 0;

    /**
     * Listen to setFirst in a given Instantiation.
     * @param i The Instantiation.
     */

    virtual void setFirstNotification(Instantiation &i) = 0;

    /**
     * Listen to setLast in a given Instantiation.
     * @param i The Instantiation.
     */

    virtual void setLastNotification(Instantiation &i) = 0;

    /**
     * Listen to increment in a given Instantiation.
     * @param i The Instantiation
     */

    virtual void setIncNotification(Instantiation &i) = 0;

    /**
     * Listen to increment in each recorded Instantiation.
     * @param i The Instantiation
     */

    virtual void setDecNotification(Instantiation &i) = 0;

    /// Listen to an assignment of a value in a Instantiation.
    // ===========================================================================
    virtual void setChangeNotification(Instantiation &i) = 0;

    /**
     * In order to insure the dereference for decorators, we need to virtualize
     * the access to master pointer.
     */

    virtual MultiDimAdressable &getMasterRef(void) = 0;

    /**
     * In order to insure the dereference for decorators, we need to virtualize
     * the access to master pointer.
     */

    virtual const MultiDimAdressable &getMasterRef(void) const = 0;

    /// String representation of internal data about i in this.

    virtual const std::string toString(const Instantiation *i) const = 0;

    /// @}
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/multidim/instantiation.inl>
#include <agrum/multidim/multiDimAdressable.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_MULTIDIM_ADRESSABLE_H */
