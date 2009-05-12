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
/** @file
 * @brief Class used to compute response times for benchmark purposes
 *
 * @author Jean-Philippe Dubus
 */
#ifndef GUM_TIMER_H
#define GUM_TIMER_H

extern "C" {
#include <sys/timeb.h>
}


#include <agrum/core/inline.h>


namespace gum {


  /** @class Timer
   *  @brief Class used to compute response times for benchmark purposes
   *
   *  This class represents a classic timer, it starts in the constructor,
   *  you can reset it with method reset() and you can get the delta time
   *  with the method step().
   *  This class uses double for representing time data, all the values are in
   *  seconds, and the precision is about 0.001 s
   */
  class Timer {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /**
     * Default constructor (launching the timer)
     */
    Timer();

    /**
     * Copy constructor
     */
    Timer (const Timer&);

    /**
     * Destructor
     */
    ~Timer();
    
    /// @}



    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /**
     * copy operator
     */
    Timer& operator= (const Timer&);

    /// @}


    // ############################################################################
    /// @name Timer manipulation
    // ############################################################################
    /// @{

    /**
     * Reset the timer
     */
    void reset();

    /**
     * Returns the delta time between now and the last reset() call
     * (or the constructor)
     *
     * @return delta time in seconds (accuracy : 0.001 s)
     */
    double step() const;

    /// @}
    
  protected:
    /**
     * Convert a struct timeb* representation of time in a double representation
     *
     * @param time_val the first representation
     * @return a double representation (in seconds, accuracy : 0.001 s)
     */
    double _convert( struct timeb* time_val ) const;

    /// time of the last call to reset() or the constructor
    double _start;

  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#include <agrum/core/timer.inl>
#endif // GUM_NO_INLINE

#endif  // GUM_TIMER_H
