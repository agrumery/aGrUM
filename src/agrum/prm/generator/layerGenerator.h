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
 * @brief Headers of LayerGenerator.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <list>
#include <vector>
// ============================================================================
#include <agrum/BN/generator/BayesNetGenerator.h>
// ============================================================================
#include <agrum/prm/PRMFactory.h>
#include <agrum/prm/generator/PRMGenerator.h>
// ============================================================================
#ifndef GUM_LAYER_GENERATOR_H
#define GUM_LAYER_GENERATOR_H
namespace gum {
namespace prm {

/**
 * @class LayerGenerator layerGenerator.h <agrum/prm/generator/layerGenerator.h>
 * @brief This class generates PRMs with a layer structure.
 *
 * @ingroup prm_group
 */
class LayerGenerator: public PRMGenerator {
  public:
    // ========================================================================
    /// @name Constructors and destructor.
    // ========================================================================
    /// @{

    /// Default constructor.
    LayerGenerator();
    /// Copy constructor.
    LayerGenerator(const LayerGenerator& source);
    /// Copy operator.
    LayerGenerator& operator=(const LayerGenerator& source);
    /// Destructor.
    virtual ~LayerGenerator();

    /// @}
    // ========================================================================
    /// Getters and setters.
    // ========================================================================
    /// @{

    /// Inner structure used to describe a Layer of the generated PRM.
    struct LayerData {
      // Number of attributes for the layer's interface.
      Size a;
      // Number of aggregates for the layer's interface.
      Size g;
      // Number of classes for this layer.
      Size c;
      // Number of instances for this layer.
      Size o;
      // Density of arcs between attributes of the same classe in this layer.
      float inner_density;
      // The odds of an instance of the precedent layer to be added to a
      // reference slot of a class of this layer.
      float outter_density;
    };

    /// Returns the domain size of generated types.
    Size getDomainSize() const;

    /// Set the domain size of generated types.
    void setDomainSize(Size s);

    /// Returns the max number of parents allowed for any attribute or aggregator
    unsigned int getMaxParents() const;

    /// Returns the max number of parents allowed for any attribute or aggregator
    void setMaxParents(Size s);

    /**
     * @brief Defines the structure of each layers.
     * Each value in v defines a layer, were v[i].first is the number of
     * different classes in the i-th layer and v[i].second is the number of
     * instances in the i-th layer.
     * @param v A vector describing each layer.
     */
    void setLayers(const std::vector<LayerData>& v);

    std::vector<LayerData>& getLayer();
    const std::vector<LayerData>& getLayer() const;

    /// Proceeds with the generation of the PRM.
    virtual PRM* generate();

    /// @}
  private:
    std::vector<LayerData> __layers;
    Size __domain_size;
    Size __max_parents;

    struct MyData {
      // interface name
      std::string i;
      std::vector<std::string> a;
      std::vector<std::string> g;
      std::string r;
      std::vector<std::string> c;
    };

    std::string __generateType(PRMFactory& f);

    void __generateInterfaces(PRMFactory& f,
                              const std::string& type,
                              std::vector<MyData>& l);

    void __generateClasses(PRMFactory& f, const std::string& type,
                           std::vector<LayerGenerator::MyData>& l);

    void __generateClassDag(Size lvl, DAG& dag,
                            Bijection<std::string, NodeId>& names,
                            std::vector<LayerGenerator::MyData>& l);
    void __generateSystem(PRMFactory& factory,
                          std::vector<LayerGenerator::MyData>& l);
};

} /* namespace prm */
} /* namespace gum */
#ifndef GUM_NO_INLINE
#include <agrum/prm/generator/layerGenerator.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_LAYER_GENERATOR_H */
// ============================================================================
