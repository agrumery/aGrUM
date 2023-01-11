/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/**
 * @file
 * @brief This file contains getters and setters definition for
 * ApproximationScheme settings.
 *
 * @see ApproximationScheme
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// To help IDE parsers
#include <agrum/tools/core/approximations/IApproximationSchemeConfiguration.h>

namespace gum {

  INLINE
  std::string IApproximationSchemeConfiguration::messageApproximationScheme() const {
    std::stringstream s;

    switch (stateApproximationScheme()) {
      case ApproximationSchemeSTATE::Continue: s << "in progress"; break;

      case ApproximationSchemeSTATE::Epsilon: s << "stopped with epsilon=" << epsilon(); break;

      case ApproximationSchemeSTATE::Rate: s << "stopped with rate=" << minEpsilonRate(); break;

      case ApproximationSchemeSTATE::Limit: s << "stopped with max iteration=" << maxIter(); break;

      case ApproximationSchemeSTATE::TimeLimit: s << "stopped with timeout=" << maxTime(); break;

      case ApproximationSchemeSTATE::Stopped: s << "stopped on request"; break;

      case ApproximationSchemeSTATE::Undefined: s << "undefined state"; break;
    };

    return s.str();
  }

}   // namespace gum
