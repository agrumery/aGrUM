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
 * @brief Inline implementation of LayerGenerator.
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {

INLINE
LayerGenerator::LayerGenerator():
  __layers(), __domain_size(2), __max_parents(INT_MAX) {
  GUM_CONSTRUCTOR(LayerGenerator);
      initRandom();
    }

INLINE
LayerGenerator::LayerGenerator(const LayerGenerator& source):
  __layers(source.__layers), __domain_size(source.__domain_size),
  __max_parents(source.__max_parents)
{
  GUM_CONS_CPY(LayerGenerator);
}

INLINE
LayerGenerator::~LayerGenerator() {
  GUM_DESTRUCTOR(LayerGenerator);
}

INLINE
LayerGenerator&
LayerGenerator::operator=(const LayerGenerator& source) {
  __layers = source.__layers;
  __domain_size = source.__domain_size;
  __max_parents = source.__max_parents;
  return *this;
}

INLINE
Size
LayerGenerator::getDomainSize() const {
  return __domain_size;
}

INLINE
void
LayerGenerator::setDomainSize(Size s) {
  __domain_size = s;
}

INLINE
unsigned int
LayerGenerator::getMaxParents() const {
  return __max_parents;
}

INLINE
void
LayerGenerator::setMaxParents(Size s) {
  __max_parents = s;
}

INLINE
void
LayerGenerator::setLayers(const std::vector<LayerGenerator::LayerData>& v) {
  __layers = v;
}

INLINE
std::vector<LayerGenerator::LayerData>&
LayerGenerator::getLayer() {
  return __layers;
}

INLINE
const std::vector<LayerGenerator::LayerData>&
LayerGenerator::getLayer() const {
  return __layers;
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
