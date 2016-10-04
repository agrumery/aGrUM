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

#ifndef CONFIG_H
#define CONFIG_H

/// gum is the global namespace for all aGrUM entities
namespace gum {

  /// Internal namespace for aGrUM signaler/listener components
  namespace __sig__ {}

  /// Internal namespace for aGrUM debugging tools
  namespace __debug__ {}

  /// Aggregators are functional description of CPTs
  namespace aggregator {}

  /// Internal namespace for complexity tools (quite empty for now)
  namespace complexity {}

  /// namespace for all credal networks entities
  namespace credal {

    /// namespace for constraint-based description of credal sets
    namespace lp {}
  }

  /// namespace for all particles for approximation inference based on
  /// simulation
  namespace samplers {}

  /// namespace for all probabilistic relational models entities
  namespace prm {}
}
#endif  // CONFIG_H
