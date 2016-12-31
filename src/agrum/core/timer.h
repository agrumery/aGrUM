/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Class used to compute response times for benchmark purposes.
 *
 * @author Pierre-Henri WUILLEMIN & Jean-Philippe Dubus
 */
#ifndef GUM_TIMER_H
#define GUM_TIMER_H
#include <chrono>

#include <agrum/config.h>


namespace gum {

  /**
   * @class Timer timer.h <agrum/core/timer.h>
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
    Timer( const Timer& timer );

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
    Timer& operator=( const Timer& timer );

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

    /// @}

    protected:
    /// Time of the last call to reset() or the constructor.
    std::chrono::high_resolution_clock::time_point _start;

    /// Time of the last call to pause().
    std::chrono::high_resolution_clock::time_point _pause;

    /// False if running.
    bool _sleeping;
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/core/timer_inl.h>
#endif  // GUM_NO_INLINE

#endif  // GUM_TIMER_H
