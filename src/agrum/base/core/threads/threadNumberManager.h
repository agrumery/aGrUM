/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief A class to manage the number of threads to use in an algorithm
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef AGRUM_THREAD_NUMBER_MANAGER_H
#define AGRUM_THREAD_NUMBER_MANAGER_H

#include <agrum/agrum.h>

#include <agrum/base/core/IThreadNumberManager.h>
#include <agrum/base/core/threads/threads.h>

namespace gum {

  /**
   * @class ThreadNumberManager
   * @brief A class to manage the number of threads to use in an algorithm
   * @headerfile threadNumberManager.h <agrum/base/core/threads/threadNumberManager.h>
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
#  include <agrum/base/core/threads/threadNumberManager_inl.h>
#endif   /* GUM_NO_INLINE */

#endif   // AGRUM_THREAD_NUMBER_MANAGER_H
