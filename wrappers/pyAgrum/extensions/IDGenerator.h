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
 * @brief This file contains definition for a IDGenerator
 *
 * @author Pierre-Henri WUILLEMIN
 */

#include <agrum/ID/generator/influenceDiagramGenerator.h>

class IDGenerator {
  public:
    gum::InfluenceDiagram<double> generate ( gum::Size nbrNodes = 10, double arcDensity = 0.2, double chanceNodeDensity = 0.8, double utilityNodeDensity = 0.1, int max_modality = 2 ) {
      gum::InfluenceDiagramGenerator<double> gen;
      gum::InfluenceDiagram<double>* id = gen.generateID ( nbrNodes, arcDensity, chanceNodeDensity, utilityNodeDensity, max_modality );
      return *id;
    }
};
