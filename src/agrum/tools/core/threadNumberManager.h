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
 * @brief A class to manage the number of threads to use in an algorithm
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef AGRUM_THREAD_NUMBER_MANAGER_H
#define AGRUM_THREAD_NUMBER_MANAGER_H

#include <agrum/agrum.h>
#include <agrum/tools/core/threads.h>
#include <agrum/tools/core/IThreadNumberManager.h>

namespace gum {


  class ThreadNumberManager : public IThreadNumberManager {
    public:

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    ThreadNumberManager(Size nb_threads = 0);

    /// copy constructor
    ThreadNumberManager(const ThreadNumberManager& from);

    /// move constructor
    ThreadNumberManager(ThreadNumberManager&& from);

    /// destructor
    virtual ~ThreadNumberManager();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    ThreadNumberManager& operator=(const ThreadNumberManager& from);

    /// move operator
    ThreadNumberManager& operator=(ThreadNumberManager&& from);

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// sets the number max of threads that can be used
    /**
     * @param nb the number max of threads to be used. If this number is set to 0, then
     * it is defaulted to aGrUM's max number of threads
     */
    virtual void setMaxNumberOfThreads(Size nb);

    /// returns the current max number of threads of the scheduler
    virtual Size getMaxNumberOfThreads() const;

    /// indicates whether the user set herself the number of threads
    bool isNbThreadsUserDefined() const;

    /// @}

    private:
    /// the max number of threads
    Size _max_nb_threads_{0};

  };

} // namespace gum

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/tools/core/threadNumberManager_inl.h>
#endif /* GUM_NO_INLINE */

#endif   // AGRUM_THREAD_NUMBER_MANAGER_H
