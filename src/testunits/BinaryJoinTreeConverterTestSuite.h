/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <vector>
#include <iostream>

#include <cxxtest/AgrumTestSuite.h>
#include <agrum/graphs/binaryJoinTreeConverterDefault.h>

namespace gum_tests {

  class BinaryJoinTreeConverterTestSuite: public CxxTest::TestSuite {
    public:

      void test1() {
        gum::CliqueGraph graph;
        std::vector<NodeSet> cliques( 11 );

        cliques[0]  << 1 << 2;
        cliques[1]  << 3 << 4 << 5;
        cliques[2]  << 6 << 7 << 8;
        cliques[3]  << 9 << 10;
        cliques[4]  << 8 << 11 <<12 << 3;
        cliques[5]  << 2 << 3 << 7 << 8 << 10 << 11 << 12;
        cliques[6]  << 10 << 15;
        cliques[7]  << 13 << 17;
        cliques[8]  << 2 << 7 << 10 << 11 << 13;
        cliques[9]  << 7 << 14;
        cliques[10] << 14 << 16;

        for( unsigned int i = 0; i <= 10; ++i ) {
          graph.insertNode( i, cliques[i] );
        }

        graph.insertEdge( 0,5 );
        graph.insertEdge( 1,5 );
        graph.insertEdge( 2,5 );
        graph.insertEdge( 3,5 );
        graph.insertEdge( 4,5 );
        graph.insertEdge( 5,8 );
        graph.insertEdge( 6,8 );
        graph.insertEdge( 7,8 );
        graph.insertEdge( 8,9 );
        graph.insertEdge( 9,10 );

        Property<unsigned int>::onNodes domain_sizes;

        for( unsigned int i = 1; i <= 17; ++i ) {
          domain_sizes.insert( i, 3 );
        }

        gum::BinaryJoinTreeConverterDefault converter;
        NodeSet roots;
        roots << 9;

        try {
          gum::CliqueGraph binTree = converter.convert( graph, domain_sizes, roots );
        } catch( gum::Exception& e ) {
          GUM_SHOWERROR( e );
        }
      }
  };

} /* namespace gum_tests */

