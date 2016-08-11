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
 * @brief This file contains definition for a Junction Tree Generator
 *
 * @author Pierre-Henri WUILLEMIN
 */
#include <agrum/config.h>

#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/graphs/triangulations/junctionTreeStrategies/defaultJunctionTreeStrategy.h>
#include <agrum/graphs/triangulations/defaultTriangulation.h>
#include <agrum/graphs/binaryJoinTreeConverterDefault.h>
#include <agrum/BN/BayesNet.h>

class JTGenerator {
public:
  gum::JunctionTree junctionTree(const gum::UndiGraph& g) {
    // creating a default binary modalities for nodes
    gum::NodeProperty<gum::Size> mods=g.nodesProperty(static_cast<gum::Size>(2));

    gum::DefaultTriangulation triangulation(&g,&mods);
    gum::DefaultJunctionTreeStrategy strategy;
    strategy.setTriangulation(&triangulation);

    return strategy.junctionTree();
  }

  gum::JunctionTree junctionTree(const gum::BayesNet<double>& bn) {

    gum::NodeProperty<gum::Size> mods;
    for ( const auto node : bn.dag().nodes() )
      mods.insert( node, bn.variable( node ).domainSize() );
    gum::DefaultTriangulation triangulation(&bn.moralGraph(),&mods);
    gum::DefaultJunctionTreeStrategy strategy;
    strategy.setTriangulation(&triangulation);

    return strategy.junctionTree();
  }

  gum::JunctionTree binaryJoinTree(const gum::UndiGraph& g) {
    // creating a default binary modalities for nodes
    gum::NodeProperty<gum::Size> mods=g.nodesProperty(static_cast<gum::Size>(2));

    gum::DefaultTriangulation triangulation(&g,&mods);
    gum::DefaultJunctionTreeStrategy strategy;
    strategy.setTriangulation(&triangulation);

    gum::BinaryJoinTreeConverterDefault bjtc;
    gum::NodeSet emptyset;

    return bjtc.convert(strategy.junctionTree(),mods,emptyset);
  }

  gum::JunctionTree binaryJoinTree(const gum::BayesNet<double>& bn) {
    gum::NodeProperty<gum::Size> mods;
    for ( const auto node : bn.dag().nodes() )
      mods.insert( node, bn.variable( node ).domainSize() );
    gum::DefaultTriangulation triangulation(&bn.moralGraph(),&mods);
    gum::DefaultJunctionTreeStrategy strategy;
    strategy.setTriangulation(&triangulation);

    gum::BinaryJoinTreeConverterDefault bjtc;
    gum::NodeSet emptyset;

    return bjtc.convert(strategy.junctionTree(),mods,emptyset);
  }
};
