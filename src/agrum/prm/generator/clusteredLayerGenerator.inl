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
 * @brief Inline implementation of ClusteredLayerGenerator.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

namespace gum {
  namespace prm {

    INLINE
    ClusteredLayerGenerator::ClusteredLayerGenerator():
      __layers(), __domain_size( 2 ), __max_parents( INT_MAX ), __cluster_ratio( 0.0 ) {
      GUM_CONSTRUCTOR( ClusteredLayerGenerator );
    }

    INLINE
    ClusteredLayerGenerator::ClusteredLayerGenerator( const ClusteredLayerGenerator& source ):
      __layers( source.__layers ), __domain_size( source.__domain_size ),
      __max_parents( source.__max_parents ) {
      GUM_CONS_CPY( ClusteredLayerGenerator );
    }

    INLINE
    ClusteredLayerGenerator::~ClusteredLayerGenerator() {
      GUM_DESTRUCTOR( ClusteredLayerGenerator );
      // typedef HashTable<std::string, std::vector<std::string>*>::iterator Iter;
      // for (Iter iter = __cluster_map.begin(); iter != __cluster_map.end(); ++iter) {
      //   delete *iter;
      // }
    }

    INLINE
    ClusteredLayerGenerator&
    ClusteredLayerGenerator::operator=( const ClusteredLayerGenerator& source ) {
      __layers = source.__layers;
      __domain_size = source.__domain_size;
      __max_parents = source.__max_parents;
      return *this;
    }

    INLINE
    Size
    ClusteredLayerGenerator::getDomainSize() const {
      return __domain_size;
    }

    INLINE
    void
    ClusteredLayerGenerator::setDomainSize( Size s ) {
      __domain_size = s;
    }

    INLINE
    unsigned int
    ClusteredLayerGenerator::getMaxParents() const {
      return __max_parents;
    }

    INLINE
    void
    ClusteredLayerGenerator::setMaxParents( Size s ) {
      __max_parents = s;
    }

    INLINE
    void
    ClusteredLayerGenerator::setLayers( const std::vector<LayerGenerator::LayerData>& v ) {
      __layers = v;
    }

    INLINE
    std::vector<LayerGenerator::LayerData>&
    ClusteredLayerGenerator::getLayer() {
      return __layers;
    }

    INLINE
    const std::vector<LayerGenerator::LayerData>&
    ClusteredLayerGenerator::getLayer() const {
      return __layers;
    }

    INLINE
    double
    ClusteredLayerGenerator::getClusterRatio() const {
      return __cluster_ratio;
    }

    INLINE
    void
    ClusteredLayerGenerator::setClusterRatio( double ratio ) {
      __cluster_ratio = ratio;
    }

  } /* namespace prm */
} /* namespace gum */

