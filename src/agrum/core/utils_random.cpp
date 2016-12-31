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
 * @brief Contains usefull methods for random stuff.
 *
 * @author Vincent RENAUDINEAU and Pierre-Henri WUILLEMIN
 */
#include <chrono>

#include <agrum/config.h>
#include <agrum/core/utils_misc.h>

#ifdef GUM_NO_INLINE
#include <agrum/core/utils_random_inl.h>
#endif  // GUM_NO_INLINE

namespace gum {

  // returns the aGrUM's seed used by the std::generators
  unsigned int randomGeneratorSeed() {
    return (unsigned int)( ( GUM_RANDOMSEED == 0 )
                               ? std::chrono::system_clock::now()
                                     .time_since_epoch()
                                     .count()
                               : GUM_RANDOMSEED );
  }


  std::default_random_engine getRandomGenerator( unsigned int seed ) {
    std::default_random_engine generator;
    generator.seed( seed ? seed : randomGeneratorSeed() );
    return generator;
  }

} /* namespace gum */
