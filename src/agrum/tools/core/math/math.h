
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief Useful macros for maths.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_MATH_H
#define GUM_MATH_H

#include <cmath>


#ifndef M_LOG2E
#  define M_LOG2E 1.4426950408889634074 /* log_2 e */
#endif

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#ifndef M_LN2
#  define M_LN2 0.69314718055994530942 /* log_e 2 */
#endif

#ifndef GUM_LOG_SQRT_2PI   // std::log ( std::sqrt ( 2pi ) )
#  define GUM_LOG_SQRT_2PI .91893853320467274177
#endif

#endif /* GUM_MATH_H */
