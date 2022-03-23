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

  /**
   * @class ThreadNumberManager
   * @brief A class to manage the number of threads to use in an algorithm
   * @headerfile threadNumberManager.h <agrum/tools/core/threadNumberManager.h>
   */
  class ThreadNumberManager: public IThreadNumberManager {
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

    /**@brief sets the number max of threads to be used by the class containing
     * this ThreadNumberManager
     *
     * @param nb the number of threads to be used. If this number is set to 0, then
     * it is defaulted to aGrUM's number of threads
     */
    virtual void setNumberOfThreads(Size nb);

    /** @brief returns the current max number of threads used by the class
     * containing this ThreadNumberManager
     */
    virtual Size getNumberOfThreads() const;

    /** @brief indicates whether the class containing this ThreadNumberManager set
     * its own number of threads
     */
    bool isGumNumberOfThreadsOverriden() const;

    /// @}

    private:
    /// the max number of threads used by the class
    Size _nb_threads_{0};
  };

}   // namespace gum

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/core/threadNumberManager_inl.h>
#endif /* GUM_NO_INLINE */

#endif   // AGRUM_THREAD_NUMBER_MANAGER_H
