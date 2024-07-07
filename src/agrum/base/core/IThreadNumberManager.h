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
 * @brief The interface for the ThreadNumberManager
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef AGRUM_ITHREAD_NUMBER_MANAGER_H
#define AGRUM_ITHREAD_NUMBER_MANAGER_H

#include <agrum/agrum.h>

namespace gum {

  struct IThreadNumberManager {
    /// sets the number max of threads that can be used
    /**
     * @param nb the number max of threads to be used. If this number is set to 0, then
     * it is defaulted to aGrUM's max number of threads
     */
    virtual void setNumberOfThreads(Size nb) = 0;

    /// returns the current max number of threads of the scheduler
    virtual Size getNumberOfThreads() const = 0;

    /// indicates whether the user set herself the number of threads
    virtual bool isGumNumberOfThreadsOverriden() const = 0;
  };

}   // namespace gum

#endif   // AGRUM_ITHREAD_NUMBER_MANAGER_H
