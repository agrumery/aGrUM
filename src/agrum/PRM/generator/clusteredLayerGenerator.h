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
 * @brief Headers of ClusteredLayerGenerator.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_CLUSTERED_LAYER_GENERATOR_H
#define GUM_CLUSTERED_LAYER_GENERATOR_H

#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <list>
#include <vector>

#include <agrum/BN/generator/simpleBayesNetGenerator.h>

#include <agrum/PRM/PRMFactory.h>
#include <agrum/PRM/generator/PRMGenerator.h>
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
    template <typename GUM_SCALAR>
    class ClusteredLayerGenerator : public PRMGenerator<GUM_SCALAR> {
      public:
      // ========================================================================
      /// @name Constructors and destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      ClusteredLayerGenerator();
      /// Copy constructor.
      ClusteredLayerGenerator(const ClusteredLayerGenerator &source);
      /// Copy operator.
      ClusteredLayerGenerator &operator=(const ClusteredLayerGenerator &source);
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

      /// Returns the max number of parents allowed for any attribute or aggregator
      unsigned int getMaxParents() const;

      /// Returns the max number of parents allowed for any attribute or aggregator
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
      void setLayers(
          const std::vector<typename LayerGenerator<GUM_SCALAR>::LayerData> &v);

      std::vector<typename LayerGenerator<GUM_SCALAR>::LayerData> &getLayer();
      const std::vector<typename LayerGenerator<GUM_SCALAR>::LayerData> &
      getLayer() const;

      /// Proceeds with the generation of the PRM<GUM_SCALAR>.
      virtual PRM<GUM_SCALAR> *generate();

      /// @}
      private:
      std::vector<typename LayerGenerator<GUM_SCALAR>::LayerData> __layers;
      Size __domain_size;
      Size __max_parents;
      double __cluster_ratio;
      HashTable<std::string, std::vector<std::string> *> __cluster_map;

      struct MyData {
        // interface name
        std::string i;
        std::vector<std::string> a;
        std::vector<std::string> g;
        std::string r;
        std::vector<std::string> c;
      };

      std::string __generateType(PRMFactory<GUM_SCALAR> &f);

      void __generateInterfaces(PRMFactory<GUM_SCALAR> &f, const std::string &type,
                                std::vector<MyData> &l);

      void __generateClasses(PRMFactory<GUM_SCALAR> &f, const std::string &type,
                             std::vector<ClusteredLayerGenerator::MyData> &l);

      void __generateCluster(PRMFactory<GUM_SCALAR> &f, const std::string &type,
                             std::vector<ClusteredLayerGenerator::MyData> &l,
                             Size lvl, Set<std::string> &i);

      void __generateClass(PRMFactory<GUM_SCALAR> &f, const std::string &type,
                           std::vector<ClusteredLayerGenerator::MyData> &l, Size lvl,
                           Set<std::string> &i);

      void __generateClassDag(Size lvl, DAG &dag,
                              Bijection<std::string, NodeId> &names,
                              std::vector<ClusteredLayerGenerator::MyData> &l);

      void __generateSystem(PRMFactory<GUM_SCALAR> &factory,
                            std::vector<ClusteredLayerGenerator::MyData> &l);
    };

  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/generator/clusteredLayerGenerator.tcc>

#endif /* GUM_CLUSTERED_LAYER_GENERATOR_H */
