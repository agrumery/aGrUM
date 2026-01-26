/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Headers of ClusteredLayerGenerator.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_CLUSTERED_LAYER_GENERATOR_H
#define GUM_CLUSTERED_LAYER_GENERATOR_H

#include <cstdlib>
#include <ctime>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include <agrum/PRM/generator/layerGenerator.h>

namespace gum {
  namespace prm {

    /**
     * @class ClusteredLayerGenerator clusteredLayerGenerator.h
     *<agrum/PRM/generator/clusteredLayerGenerator.h>
     * @brief This class generates PRMs with a clustered layer structure.
     *
     * @ingroup prm_group
     */
    template < typename GUM_SCALAR >
    class ClusteredLayerGenerator: public PRMGenerator< GUM_SCALAR > {
      public:
      // ========================================================================
      /// @name Constructors and destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      ClusteredLayerGenerator();
      /// Copy constructor.
      ClusteredLayerGenerator(const ClusteredLayerGenerator& source);
      /// Copy operator.
      ClusteredLayerGenerator& operator=(const ClusteredLayerGenerator& source);
      /// Destructor.
      virtual ~ClusteredLayerGenerator();

      /// @}
      // ========================================================================
      /// Getters and setters.
      // ========================================================================
      /// @{

      /// Returns the domain size of generated types.
      Size getDomainSize() const;

      /// Set the domain size of generated types.
      void setDomainSize(Size s);

      /// Returns the max number of parents allowed for any attribute or
      /// aggregator
      Size getMaxParents() const;

      /// Returns the max number of parents allowed for any attribute or
      /// aggregator
      void setMaxParents(Size s);

      /// Returns the odds of a given class to be replaced by a cluster.
      double getClusterRatio() const;

      /// Define the odds of a given class to be replaced by a cluster.
      void setClusterRatio(double ratio);

      /**
       * @brief Defines the structure of each layers.
       * Each value in v defines a layer, were v[i].first is the number of
       * different classes in the i-th layer and v[i].second is the number of
       * instances in the i-th layer.
       * @param v A vector describing each layer.
       */
      void setLayers(const std::vector< typename LayerGenerator< GUM_SCALAR >::LayerData >& v);

      std::vector< typename LayerGenerator< GUM_SCALAR >::LayerData >&       getLayer();
      const std::vector< typename LayerGenerator< GUM_SCALAR >::LayerData >& getLayer() const;

      /// Proceeds with the generation of the PRM<GUM_SCALAR>.
      virtual PRM< GUM_SCALAR >* generate();

      /// @}

      private:
      std::vector< typename LayerGenerator< GUM_SCALAR >::LayerData > _layers_;
      Size                                                            _domain_size_;
      Size                                                            _max_parents_;
      double                                                          _cluster_ratio_;
      HashTable< std::string, std::vector< std::string >* >           _cluster_map_;

      struct MyData {
        // interface name
        std::string                i;
        std::vector< std::string > a;
        std::vector< std::string > g;
        std::string                r;
        std::vector< std::string > c;
      };

      std::string _generateType_(PRMFactory< GUM_SCALAR >& f);

      void _generateInterfaces_(PRMFactory< GUM_SCALAR >& f,
                                const std::string&        type,
                                std::vector< MyData >&    l);

      void _generateClasses_(PRMFactory< GUM_SCALAR >&                                f,
                             const std::string&                                       type,
                             std::vector< typename ClusteredLayerGenerator::MyData >& l);

      void _generateCluster_(PRMFactory< GUM_SCALAR >&                                f,
                             const std::string&                                       type,
                             std::vector< typename ClusteredLayerGenerator::MyData >& l,
                             Size                                                     lvl,
                             Set< std::string >&                                      i);

      void _generateClass_(PRMFactory< GUM_SCALAR >&                                f,
                           const std::string&                                       type,
                           std::vector< typename ClusteredLayerGenerator::MyData >& l,
                           Size                                                     lvl,
                           Set< std::string >&                                      i);

      void _generateClassDag_(Size                                                     lvl,
                              DAG&                                                     dag,
                              Bijection< std::string, NodeId >&                        names,
                              std::vector< typename ClusteredLayerGenerator::MyData >& l);

      void _generateSystem_(PRMFactory< GUM_SCALAR >&                                factory,
                            std::vector< typename ClusteredLayerGenerator::MyData >& l);
    };

  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/generator/clusteredLayerGenerator_tpl.h>

#endif /* GUM_CLUSTERED_LAYER_GENERATOR_H */
