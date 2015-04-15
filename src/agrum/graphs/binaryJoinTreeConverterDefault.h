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
/** @file
 * @brief An algorithm for converting a join tree into a binary join tree
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_BINARY_JOIN_TREE_CONVERTER_DEFAULT_H
#define GUM_BINARY_JOIN_TREE_CONVERTER_DEFAULT_H

#include <agrum/graphs/binaryJoinTreeConverter.h>

namespace gum {

  class BinaryJoinTreeConverterDefault {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    BinaryJoinTreeConverterDefault();

    /// destructor
    virtual ~BinaryJoinTreeConverterDefault();

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// returns a binary join tree corresponding to clique graph JT
    /** This method creates and returns a new binary join tree compatible with
     * that passed in argument (JT) and optimized for inference. As such, this
     * requires knowing the join tree to be converted (of course), but also
     * which roots will be used by the collect/diffusion inference engine and
     * the domain size of the variables contained in the cliques of JT (to
     * optimize the combination of the potentials contained in the cliques.
     * @throws InvalidNode exception is thrown if some roots do not belong to
     * JT or if several roots belong to the same connected component.
     * @warning If you do not pass in argument a root for each connected component,
     * then for those with unspecified roots, an arbitrary root will be computed
     * and used for the binarization. */
    CliqueGraph convert(const CliqueGraph &JT,
                        const NodeProperty<Size> &domain_sizes,
                        const NodeSet &roots);

    /// returns all the roots considered for all the connected components
    const NodeSet &roots() const;

    /// @}

    private:
    /// the new roots that have been created to compute the last query
    NodeSet __roots;

    /// forbid copy constructor
    BinaryJoinTreeConverterDefault(const BinaryJoinTreeConverterDefault &);

    /// forbid copy operator
    BinaryJoinTreeConverterDefault &
    operator=(const BinaryJoinTreeConverterDefault &);

    /** @brief a function used to mark the nodes belonging to a given
     * connected component */
    void __markConnectedComponent(const CliqueGraph &JT, NodeId root,
                                  NodeProperty<bool> &mark) const;

    /// convert a whole connected component into a binary join tree
    void __convertConnectedComponent(CliqueGraph &JT, NodeId current_node,
                                     NodeId from,
                                     const NodeProperty<Size> &domain_sizes,
                                     NodeProperty<bool> &mark) const;

    /// convert a clique and its adjacent cliques into a binary join tree
    void __convertClique(CliqueGraph &JT, NodeId clique, NodeId from,
                         const NodeProperty<Size> &domain_sizes) const;

    /// returns the domain size of the union of two cliques
    float __combinedSize(const NodeSet &nodes1, const NodeSet &nodes2,
                         const NodeProperty<Size> &domain_sizes) const;
  };

} /* namespace gum */

#endif /* GUM_BINARY_JOIN_TREE_CONVERTER_DEFAULT_H */
