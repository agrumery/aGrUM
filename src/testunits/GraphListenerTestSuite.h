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

#include <iostream>
#include <cxxtest/AgrumTestSuite.h>

#include <agrum/graphs/graphElements.h>

#include <agrum/graphs/DAG.h>
#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/mixedGraph.h>

#include <agrum/core/signal/listener.h>
#include <agrum/graphs/diGraphListener.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5


class GraphListenerTestSuite: public CxxTest::TestSuite {


    class Counter : public gum::Listener {
    private:
        int __nbrNode,__nbrArcs,__nbrEdges;
        bool __isOn;
    public:
        Counter() {
            __nbrNode=__nbrArcs=__nbrEdges=0;
            __isOn=false;
        }

        void whenNodeAdded( const void * ,gum::NodeId ) {
            __nbrNode++;
        }

        void whenNodeDeleted( const void *,gum::NodeId ) {
            __nbrNode--;
        }

        void whenArcAdded( const void *,gum::NodeId,gum::NodeId ) {
            __nbrArcs++;
        }

        void whenArcDeleted( const void *,gum::NodeId,gum::NodeId ) {
            __nbrArcs--;
        }

        void whenEdgeAdded( const void *,gum::NodeId f,gum::NodeId s ) {
            __nbrEdges++;
            if ( __isOn ) GUM_TRACE( f<<"--"<<s<<"("<<__nbrEdges<<")" );
        }

        void whenEdgeDeleted( const void *,gum::NodeId f,gum::NodeId s ) {
            __nbrEdges--;
            if ( __isOn ) GUM_TRACE( f<<"--"<<s<<"("<<__nbrEdges<<")" );
        }

        const int nodes() const {
            return __nbrNode;
        }

        const int arcs() const {
            return __nbrArcs;
        }

        const int edges() const {
            return __nbrEdges;
        }

        void on() {
            __isOn=true;
        }

        void off() {
            __isOn=false;
        }
    };

    class DiGraphCounter : public gum::DiGraphListener {
    private:
        int __nbrNode,__nbrArcs;
    public:
        DiGraphCounter(gum::DiGraph *g) : gum::DiGraphListener(g) {
            __nbrNode=__nbrArcs=0;
        }

        void whenNodeAdded( const void * ,gum::NodeId ) {
            __nbrNode++;
        }

        void whenNodeDeleted( const void *,gum::NodeId ) {
            __nbrNode--;
        }

        void whenArcAdded( const void *,gum::NodeId,gum::NodeId ) {
            __nbrArcs++;
        }

        void whenArcDeleted( const void *,gum::NodeId,gum::NodeId ) {
            __nbrArcs--;
        }
        const int nodes() const {
            return __nbrNode;
        }

        const int arcs() const {
            return __nbrArcs;
        }

    };

private:
    gum::NodeId id1;
    gum::NodeId id2;
    gum::NodeId id3;
    gum::NodeId id4;
    gum::NodeId id5;

    void buildDAG( gum::DAG& g ) {
        id1=g.insertNode();
        id2=g.insertNode();
        id3=g.insertNode();
        id4=g.insertNode();
        id5=g.insertNode();

        g.insertArc( id1,id3 );
        g.insertArc( id3,id5 );
        g.insertArc( id2,id4 );
        g.insertArc( id1,id4 );
        g.insertArc( id4,id5 );
        g.insertArc( id2,id5 );
    }

    void buildUndiGraph( gum::UndiGraph& g ) {
        id3=g.insertNode();
        id4=g.insertNode();
        id5=g.insertNode();
        id1=g.insertNode();
        id2=g.insertNode();

        g.insertEdge( id4,id5 );
        g.insertEdge( id2,id5 );
        g.insertEdge( id2,id4 );
        g.insertEdge( id1,id4 );
        g.insertEdge( id1,id3 );
        g.insertEdge( id3,id5 );
    }

    void buildMixedGraph( gum::MixedGraph& g ) {
        id3=g.insertNode();
        id4=g.insertNode();
        id5=g.insertNode();
        id1=g.insertNode();
        id2=g.insertNode();

        g.insertEdge( id4,id5 );
        g.insertEdge( id3,id5 );
        g.insertArc( id2,id5 );
        g.insertArc( id2,id4 );
        g.insertEdge( id1,id4 );
        g.insertArc( id1,id3 );
    }

public:
    void testDAG() {
        gum::DAG g1;
        gum::DAG g2;

        Counter c1;
        Counter c2;

// g1 has 2 listeners
// g2 has 1 listener
// c1 listens to 1 graph
// c2 listens to 2 graphs
        GUM_CONNECT( g1,onNodeAdded,c1,Counter::whenNodeAdded );
        GUM_CONNECT( g1,onNodeDeleted,c1,Counter::whenNodeDeleted );
        GUM_CONNECT( g1,onArcAdded,c1,Counter::whenArcAdded );
        GUM_CONNECT( g1,onArcDeleted,c1,Counter::whenArcDeleted );

        GUM_CONNECT( g1,onNodeAdded,c2,Counter::whenNodeAdded );
        GUM_CONNECT( g1,onNodeDeleted,c2,Counter::whenNodeDeleted );
        GUM_CONNECT( g1,onArcAdded,c2,Counter::whenArcAdded );
        GUM_CONNECT( g1,onArcDeleted,c2,Counter::whenArcDeleted );

        GUM_CONNECT( g2,onNodeAdded,c2,Counter::whenNodeAdded );
        GUM_CONNECT( g2,onNodeDeleted,c2,Counter::whenNodeDeleted );
        GUM_CONNECT( g2,onArcAdded,c2,Counter::whenArcAdded );
        GUM_CONNECT( g2,onArcDeleted,c2,Counter::whenArcDeleted );

        buildDAG( g2 ); // 5 nodes/6 arcs for g2

        g1.insertArc( g1.insertNode(),g1.insertNode() );
        buildDAG( g1 ); // 7 nodes/7 arcs for g1


        TS_ASSERT_EQUALS( c1.nodes(),7 );
        TS_ASSERT_EQUALS( c1.arcs(),7 );
        TS_ASSERT_EQUALS( c2.nodes(),7+5 );
        TS_ASSERT_EQUALS( c2.arcs(),7+6 );

        g1.eraseNode( id5 );// -1 nodes/-3 arcs for g1

        TS_ASSERT_EQUALS( c1.nodes(),6 );
        TS_ASSERT_EQUALS( c1.arcs(),4 );
        TS_ASSERT_EQUALS( c2.nodes(),6+5 );
        TS_ASSERT_EQUALS( c2.arcs(),4+6 );

        g1.eraseArc( id1,id3 ); // 6 nodes, 3 arcs

        TS_ASSERT_EQUALS( c1.nodes(),6 );
        TS_ASSERT_EQUALS( c1.arcs(),3 );
        TS_ASSERT_EQUALS( c2.nodes(),6+5 );
        TS_ASSERT_EQUALS( c2.arcs(),3+6 );

        g1.clear(); // 0 node, 0 arc

        TS_ASSERT_EQUALS( c1.nodes(),0 );
        TS_ASSERT_EQUALS( c1.arcs(),0 );
        TS_ASSERT_EQUALS( c2.nodes(),0+5 );
        TS_ASSERT_EQUALS( c2.arcs(),0+6 );

        g2.clearArcs(); // 5 nodes, 0 arc

        TS_ASSERT_EQUALS( c1.nodes(),0 );
        TS_ASSERT_EQUALS( c1.arcs(),0 );
        TS_ASSERT_EQUALS( c2.nodes(),0+5 );
        TS_ASSERT_EQUALS( c2.arcs(),0+0 );
    }

    void testUndiGraph() {
        gum::UndiGraph g1;
        gum::UndiGraph g2;

        Counter c1;
        Counter c2;

// g1 has 2 listeners
// g2 has 1 listener
// c1 listens to 1 graph
// c2 listens to 2 graphs
        GUM_CONNECT( g1,onNodeAdded,c1,Counter::whenNodeAdded );
        GUM_CONNECT( g1,onNodeDeleted,c1,Counter::whenNodeDeleted );
        GUM_CONNECT( g1,onEdgeAdded,c1,Counter::whenEdgeAdded );
        GUM_CONNECT( g1,onEdgeDeleted,c1,Counter::whenEdgeDeleted );

        GUM_CONNECT( g1,onNodeAdded,c2,Counter::whenNodeAdded );
        GUM_CONNECT( g1,onNodeDeleted,c2,Counter::whenNodeDeleted );
        GUM_CONNECT( g1,onEdgeAdded,c2,Counter::whenEdgeAdded );
        GUM_CONNECT( g1,onEdgeDeleted,c2,Counter::whenEdgeDeleted );

        GUM_CONNECT( g2,onNodeAdded,c2,Counter::whenNodeAdded );
        GUM_CONNECT( g2,onNodeDeleted,c2,Counter::whenNodeDeleted );
        GUM_CONNECT( g2,onEdgeAdded,c2,Counter::whenEdgeAdded );
        GUM_CONNECT( g2,onEdgeDeleted,c2,Counter::whenEdgeDeleted );

        buildUndiGraph( g2 ); // 5 nodes/6 edges for g2

        g1.insertEdge( g1.insertNode(),g1.insertNode() );
        buildUndiGraph( g1 ); // 7 nodes/7 edges for g1


        TS_ASSERT_EQUALS( c1.nodes(),7 );
        TS_ASSERT_EQUALS( c1.edges(),7 );
        TS_ASSERT_EQUALS( c2.nodes(),7+5 );
        TS_ASSERT_EQUALS( c2.edges(),7+6 );

        g1.eraseNode( id5 );// -1 nodes/-3 edges for g1

        TS_ASSERT_EQUALS( c1.nodes(),6 );
        TS_ASSERT_EQUALS( c1.edges(),4 );
        TS_ASSERT_EQUALS( c2.nodes(),6+5 );
        TS_ASSERT_EQUALS( c2.edges(),4+6 );

        g1.eraseEdge( id1,id3 ); // 6 nodes, 3 edges

        TS_ASSERT_EQUALS( c1.nodes(),6 );
        TS_ASSERT_EQUALS( c1.edges(),3 );
        TS_ASSERT_EQUALS( c2.nodes(),6+5 );
        TS_ASSERT_EQUALS( c2.edges(),3+6 );

        g1.clear(); // 0 node, 0 arc

        TS_ASSERT_EQUALS( c1.nodes(),0 );
        TS_ASSERT_EQUALS( c1.edges(),0 );
        TS_ASSERT_EQUALS( c2.nodes(),0+5 );
        TS_ASSERT_EQUALS( c2.edges(),0+6 );

        g2.clearEdges(); // 5 nodes, 0 arc

        TS_ASSERT_EQUALS( c1.nodes(),0 );
        TS_ASSERT_EQUALS( c1.edges(),0 );
        TS_ASSERT_EQUALS( c2.nodes(),0+5 );
        TS_ASSERT_EQUALS( c2.edges(),0+0 );
    }

    void testMixedGraph() {
        gum::MixedGraph g1;
        gum::MixedGraph g2;

        Counter c1;
        Counter c2;

// g1 has 2 listeners
// g2 has 1 listener
// c1 listens to 1 graph
// c2 listens to 2 graphs
        GUM_CONNECT( g1,onNodeAdded,c1,Counter::whenNodeAdded );
        GUM_CONNECT( g1,onNodeDeleted,c1,Counter::whenNodeDeleted );
        GUM_CONNECT( g1,onEdgeAdded,c1,Counter::whenEdgeAdded );
        GUM_CONNECT( g1,onEdgeDeleted,c1,Counter::whenEdgeDeleted );
        GUM_CONNECT( g1,onArcAdded,c1,Counter::whenArcAdded );
        GUM_CONNECT( g1,onArcDeleted,c1,Counter::whenArcDeleted );

        GUM_CONNECT( g1,onNodeAdded,c2,Counter::whenNodeAdded );
        GUM_CONNECT( g1,onNodeDeleted,c2,Counter::whenNodeDeleted );
        GUM_CONNECT( g1,onEdgeAdded,c2,Counter::whenEdgeAdded );
        GUM_CONNECT( g1,onEdgeDeleted,c2,Counter::whenEdgeDeleted );
        GUM_CONNECT( g1,onArcAdded,c2,Counter::whenArcAdded );
        GUM_CONNECT( g1,onArcDeleted,c2,Counter::whenArcDeleted );

        GUM_CONNECT( g2,onNodeAdded,c2,Counter::whenNodeAdded );
        GUM_CONNECT( g2,onNodeDeleted,c2,Counter::whenNodeDeleted );
        GUM_CONNECT( g2,onEdgeAdded,c2,Counter::whenEdgeAdded );
        GUM_CONNECT( g2,onEdgeDeleted,c2,Counter::whenEdgeDeleted );
        GUM_CONNECT( g2,onArcAdded,c2,Counter::whenArcAdded );
        GUM_CONNECT( g2,onArcDeleted,c2,Counter::whenArcDeleted );

        buildMixedGraph( g2 ); // 5 nodes/3 edges /3 arcs for g2

        g1.insertEdge( g1.insertNode(),g1.insertNode() );
        buildMixedGraph( g1 ); // 7 nodes/4 edges / 3 arcs for g1


        TS_ASSERT_EQUALS( c1.nodes(),7 );
        TS_ASSERT_EQUALS( c1.edges(),4 );
        TS_ASSERT_EQUALS( c1.arcs(),3 );
        TS_ASSERT_EQUALS( c2.nodes(),7+5 );
        TS_ASSERT_EQUALS( c2.edges(),4+3 );
        TS_ASSERT_EQUALS( c2.arcs(),3+3 );

        g1.eraseNode( id5 );// -1 nodes/-2 edges / -1 arcs for g1

        TS_ASSERT_EQUALS( c1.nodes(),6 );
        TS_ASSERT_EQUALS( c1.edges(),2 );
        TS_ASSERT_EQUALS( c1.arcs(),2 );
        TS_ASSERT_EQUALS( c2.nodes(),6+5 );
        TS_ASSERT_EQUALS( c2.edges(),2+3 );
        TS_ASSERT_EQUALS( c2.arcs(),2+3 );

        g1.eraseEdge( id1,id3 ); // THIS EDGE DOES NOT EXISTS !!!! => 6 nodes, 2 edges

        TS_ASSERT_EQUALS( c1.nodes(),6 );
        TS_ASSERT_EQUALS( c1.edges(),2);
        TS_ASSERT_EQUALS( c2.nodes(),6+5 );
        TS_ASSERT_EQUALS( c2.edges(),2+3 );

        g1.clear(); // 0 node, 0 arc

        TS_ASSERT_EQUALS( c1.nodes(),0 );
        TS_ASSERT_EQUALS( c1.edges(),0 );
        TS_ASSERT_EQUALS( c1.arcs(),0 );
        TS_ASSERT_EQUALS( c2.nodes(),0+5 );
        TS_ASSERT_EQUALS( c2.edges(),0+3 );
        TS_ASSERT_EQUALS( c2.arcs(),0+3 );

        g2.clearEdges(); // 5 nodes, 0 arc

        TS_ASSERT_EQUALS( c1.nodes(),0 );
        TS_ASSERT_EQUALS( c1.edges(),0 );
        TS_ASSERT_EQUALS( c1.arcs(),0 );
        TS_ASSERT_EQUALS( c2.nodes(),0+5 );
        TS_ASSERT_EQUALS( c2.edges(),0+0 );
        TS_ASSERT_EQUALS( c2.arcs(),0+3 );

        g2.clearArcs(); // 5 nodes, 0 arc

        TS_ASSERT_EQUALS( c1.nodes(),0 );
        TS_ASSERT_EQUALS( c1.edges(),0 );
        TS_ASSERT_EQUALS( c1.arcs(),0 );
        TS_ASSERT_EQUALS( c2.nodes(),0+5 );
        TS_ASSERT_EQUALS( c2.edges(),0+0 );
        TS_ASSERT_EQUALS( c2.arcs(),0+0 );
    }

    void testDAGWithGraphListener() {
        gum::DAG g;

        DiGraphCounter c(&g);

        buildDAG( g ); // 5 nodes/6 arcs for g

        TS_ASSERT_EQUALS( c.nodes(),5 );
        TS_ASSERT_EQUALS( c.arcs(),6 );

        g.eraseNode( id5 );// -1 nodes/-3 arcs for g

        TS_ASSERT_EQUALS( c.nodes(),4 );
        TS_ASSERT_EQUALS( c.arcs(),3 );

        g.eraseArc( id1,id3 ); // -1 arcs

        TS_ASSERT_EQUALS( c.nodes(),4 );
        TS_ASSERT_EQUALS( c.arcs(),2 );

        g.clear(); // 0 node, 0 arc

        TS_ASSERT_EQUALS( c.nodes(),0 );
        TS_ASSERT_EQUALS( c.arcs(),0 );
    }
};

// kate: indent-mode cstyle; space-indent on; indent-width 0;
