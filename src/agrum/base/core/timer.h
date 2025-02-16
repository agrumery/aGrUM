/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Class used to compute response times for benchmark purposes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Jean-Philippe Dubus
 */
#ifndef GUM_TIMER_H
#define GUM_TIMER_H
#include <chrono>

#include <agrum/agrum.h>

namespace gum {

  /**
   * @class Timer
   * @headerfile timer.h <agrum/base/core/timer.h>
   * @brief Class used to compute response times for benchmark purposes
   * @ingroup basicstruct_group
   *
   * This class represents a classic timer, it starts in the constructor, you
   * can reset it with method reset() and you can get the delta time with the
   * method step().
   *
   * This class uses double for representing time data, all the values are in
   * seconds, and the precision is about 0.001 s
   */
  class Timer {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor (launching the timer).
     */
    Timer();

    /**
     * @brief Copy constructor.
     * @param timer The gum::Timer to copy.
     */
    Timer(const Timer& timer);

    /**
     * Destructor
     */
    ~Timer();

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param timer The gum::Timer to copy.
     */
    Timer& operator=(const Timer& timer);

    /// @}
    // ============================================================================
    /// @name Timer manipulation
    // ============================================================================
    /// @{

    /**
     * @brief Reset the timer.
     */
    void reset();
    /**
     * @brief Pause the timer and return the delta (@see gum::Timer::step() ).
     * @brief Returns the delta (@see gum::Timer::step() ).
     */
    double pause();
    /**
     * @brief Resume the timer and return the delta (@see gum::Timer::step() ).
     * @brief Returns the delta (@see gum::Timer::step() ).
     */
    double resume();

    /**
     * @brief Returns the delta time between now and the last reset() call (or
     * the constructor).
     *
     * @return Returns the delta time in seconds (accuracy : 0.001 ms).
     */
    double step() const;

    /**
     * @brief Returns the delta time between now and the last reset() in a human format
     *
     * @return a string representing the delta time
     */
    std::string toString() const;

    /// @}

    protected:
    std::chrono::duration< double, std::milli > step_() const;

    /// Time of the last call to reset() or the constructor.
    std::chrono::high_resolution_clock::time_point start_;

    /// Time of the last call to pause().
    std::chrono::high_resolution_clock::time_point pause_;

    /// False if running.
    bool sleeping_;
  };

  std::ostream& operator<<(std::ostream& os, const Timer& t);
} /* namespace gum */


#ifndef GUM_NO_INLINE
#  include <agrum/base/core/timer_inl.h>
#endif   // GUM_NO_INLINE

#endif   // GUM_TIMER_H
