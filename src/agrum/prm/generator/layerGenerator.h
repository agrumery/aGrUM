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
 * @brief This class generates PRMs with a pyramidal structure.
 *
 * The pyramidal structures is composed of layers containing the classes
 * sharing * the same interface. The firs layer, called the root layer,
 * contains classes with no references. The second layer is composed of
 * classes with references over the interface of the classes in the first
 * layer. This schema is reproduced in all the generated system: classes in
 * the nth layer depends on classes in the (n-1)th layer.
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
      /// The number of attributes in each interface.
      size_t interface_size;
      /// The number of input aggregators in each class. This must be lesser
      /// or equal to interface_size of the previous level.
      size_t agg_size;
      /// The odds of an arc existing between an aggregator and a class in the
      /// upper layer.
      float agg_ratio;
      /// The number of classes in this layer.
      size_t c_count;
      /// The number if attributes in each class.
      size_t a_count;
      /// The arc density in this class.
      float density;
      /// The number of instances in this layer.
      size_t i_count;
    };

    /// Returns the domain size of generated types.
    Size getDomainSize() const;

    /// Set the domain size of generated types.
    void setDomainSize(Size s);

    /**
     * @brief Defines the structure of each layers.
     * Each value in v defines a layer, were v[i].first is the number of
     * different classes in the i-th layer and v[i].second is the number of
     * instances in the i-th layer.
     * @param v A vector describing each layer.
     */
    void setLayers(const std::vector<LayerData>& v);

    /// Proceeds with the generation of the PRM.
    virtual PRM* generate();

    /// @}
  private:
    /// A layer.
    struct Layer {
      /// Data about the layer.
      LayerData data;
      /// The interface abstracting all classes at this layer.
      std::string interface;
      /// The set of instances at this layer.
      Set<std::string> instances;
    };

    /// The domain size of the Type used in the generation.
    Size __domainSize;

    /// The vector describing each layer.
    std::vector<Layer> __layers;

    /// Returns the layer of a given level.
    LayerData& __layer(size_t idx);

    /// Returns the interface abstracting the classes at layer idx.
    std::string& __interface(size_t idx);

    /// Generate classes in a factory.
    void __generateClasses(PRMFactory& factory);

    std::string __generateType(PRMFactory& factory);

    std::string __generateInterface(size_t lvl, PRMFactory& factory,
                                    const std::string& type);

    void __pickOutputs(LayerData& data, Interface& i, BayesNet<prm_float>& bn,
                       Property<std::string>::onNodes& outputs);

    std::string __copyClass(LayerGenerator::LayerData& data,
                            PRMFactory& factory, BayesNet<prm_float>& bn,
                            const std::string& i,
                            Property<std::string>::onNodes& outputs,
                            const std::string& type);

    void __addAggregate(std::vector<ReferenceSlot*>& refs,
                        BayesNet<prm_float>& bn,
                        PRMFactory& factory, NodeId id,
                        Property<std::string>::onNodes& name_map,
                        const Set<NodeId>& agg);

    Set<NodeId> __defineAggSet(LayerGenerator::LayerData& data,
                               BayesNet<prm_float>& bn,
                               Property<std::string>::onNodes& outputs);

    std::vector<prm_float> __generateCPF(size_t size);

    /// Generate a system in a factory.
    void __generateSystem(PRMFactory& factory);
};

} /* namespace prm */
} /* namespace gum */
#ifndef GUM_NO_INLINE
#include <agrum/prm/generator/layerGenerator.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_LAYER_GENERATOR_H */
// ============================================================================
