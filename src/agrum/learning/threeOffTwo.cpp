/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
/** @file
 * @brief The greedy hill learning algorithm (for directed graphs)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/core/hashTable.h>
#include <agrum/core/heap.h>
#include <agrum/core/timer.h>
#include <agrum/graphs/mixedGraph.h>
#include <agrum/learning/paramUtils/DAG2BNLearner.h>
#include <agrum/learning/scores_and_tests/correctedMutualInformation.h>
#include <agrum/learning/threeOffTwo.h>
//#include <agrum/graphs/graphElements.h>


namespace gum {

  namespace learning {

    /// default constructor
    ThreeOffTwo::ThreeOffTwo() {
      /*
    setEpsilon( 0 );
    disableMinEpsilonRate();
    disableMaxIter();
    disableMaxTime();*/
      GUM_CONSTRUCTOR(ThreeOffTwo);
    }

    /// default constructor with maxLog
    ThreeOffTwo::ThreeOffTwo(int maxLog)
        : __maxLog(maxLog) {
      /*
    setEpsilon( 0 );
    disableMinEpsilonRate();
    disableMaxIter();
    disableMaxTime();*/
      GUM_CONSTRUCTOR(ThreeOffTwo);
    }
    /*
        /// copy constructor
        ThreeOffTwo::ThreeOffTwo( const ThreeOffTwo& from )
            : ApproximationScheme( from ) {
          GUM_CONS_CPY( ThreeOffTwo );
        }

        /// move constructor
        ThreeOffTwo::ThreeOffTwo( ThreeOffTwo&& from )
            : ApproximationScheme( std::move( from ) ) {
          GUM_CONS_MOV( ThreeOffTwo );
        }
    */
    /// destructor
    ThreeOffTwo::~ThreeOffTwo() { GUM_DESTRUCTOR(ThreeOffTwo); }
    /*
        /// copy operator
        ThreeOffTwo& ThreeOffTwo::
        operator=( const ThreeOffTwo& from ) {
          ApproximationScheme::operator=( from );
          return *this;
        }

        /// move operator
        ThreeOffTwo& ThreeOffTwo::
        operator=( ThreeOffTwo&& from ) {
          ApproximationScheme::operator=( std::move( from ) );
          return *this;
        }
    */

    bool GreaterPairOn2nd::operator()(
      const std::pair< std::tuple< Idx, Idx, Idx, std::vector< Idx > >*, double >&
        e1,
      const std::pair< std::tuple< Idx, Idx, Idx, std::vector< Idx > >*, double >&
        e2) const {
      return e1.second > e2.second;
    }

    bool GreaterAbsPairOn2nd::
    operator()(const std::pair< std::tuple< Idx, Idx, Idx >*, double >& e1,
               const std::pair< std::tuple< Idx, Idx, Idx >*, double >& e2) const {
      return std::abs(e1.second) > std::abs(e2.second);
    }

    bool GreaterTupleOnLast::
    operator()(const std::tuple< std::tuple< Idx, Idx, Idx >*, double, double, double >& e1,
               const std::tuple< std::tuple< Idx, Idx, Idx >*, double, double, double >& e2) const {
      double p1xz = std::get< 2 >(e1);
      double p1yz = std::get< 3 >(e1);
      double p2xz = std::get< 2 >(e2);
      double p2yz = std::get< 3 >(e2);
      return std::max(p1xz, p1yz) > std::max(p2xz, p2yz);
    }
    
    /// learns the structure of a MixedGraph
    MixedGraph ThreeOffTwo::learnMixedStructure(CorrectedMutualInformation<>& I,
                                                MixedGraph graph) {
      _timer.reset();
      _current_step = 0;

      // clear the vector of latent arcs to be sure
      __latent_couples.clear();

      /// the heap of ranks, with the score, and the Idxs of x, y and z.
      Heap< std::pair< std::tuple< Idx, Idx, Idx, std::vector< Idx > >*, double >,
            GreaterPairOn2nd >
        _rank;

      /// the variables separation sets
      HashTable< std::pair< Idx, Idx >, std::vector< Idx > > sep_set;

      // //std::cout << "INITIATION" << std::endl;
      _initiation(I, graph, sep_set, _rank);
      /*std::cout << graph << std::endl;*/
              
      // //std::cout << "ITERATION" << std::endl;
      _iteration(I, graph, sep_set, _rank);
      /*std::cout << graph << std::endl;*/
              
      if (__usemiic){
        _orientation_miic(I, graph, sep_set);
      } else {
        _orientation(I, graph, sep_set);
      }
      
      /*std::cout << graph << std::endl;*/
      return graph;
    }

    /*
     * PHASE 1 : INITIATION
     *
     * We go over all edges and test if the variables are independent. If they
     * are,
     * the edge is deleted. If not, the best contributor is found.
     */
    void ThreeOffTwo::_initiation(
      CorrectedMutualInformation<>& I, 
      MixedGraph& graph, 
      HashTable< std::pair< Idx, Idx >, std::vector< Idx > >& sep_set, 
      Heap< std::pair< std::tuple< Idx, Idx, Idx, std::vector< Idx > >*, double >,
            GreaterPairOn2nd >& _rank){
      Idx     x, y;
      EdgeSet edges = graph.edges();
      Size    steps_init = edges.size();

      for (const Edge& edge : edges) {
        x = edge.first();
        y = edge.second();
        /*std::cout << "Edge " << x << '-' << y << std::endl;*/
        double Ixy = I.score(x, y);

        /*std::cout << "  Ixy " << Ixy << std::endl;*/

        if (Ixy <= 0) {  //< K
          /*std::cout << "  Erase edge " << std::endl;*/
          graph.eraseEdge(edge);
          // attention, move ça fait des trucs étranges avec l'objet de départ
          // sep_set.insert( std::make_pair(x, y), std::move( __empty_set ) );
          sep_set.insert(std::make_pair(x, y), __empty_set);
        } else {
          // tps=time.step();
          _findBestContributor(x, y, __empty_set, graph, I, _rank);
          // tps = time.step()-tps;
          // //std::cout << "      best contrib:" << tps << "s" << std::endl;
        }

        ++_current_step;
        if (onProgress.hasListener()) {
          GUM_EMIT3(
            onProgress, (_current_step * 33) / steps_init, 0., _timer.step());
        }
      }
    }
      
    /*
     * PHASE 2 : ITERATION
     *
     * As long as we find important nodes for edges, we go over them to see if
     * we can assess the independence of the variables.
    */
    void ThreeOffTwo::_iteration(
      CorrectedMutualInformation<>& I, 
      MixedGraph& graph, 
      HashTable< std::pair< Idx, Idx >, std::vector< Idx > >& sep_set, 
      Heap< std::pair< std::tuple< Idx, Idx, Idx, std::vector< Idx > >*, double >,
            GreaterPairOn2nd >& _rank){
          
      // if no triples to further examine pass
      std::pair< std::tuple< Idx, Idx, Idx, std::vector< Idx > >*, double > best;
      if (!_rank.empty()) {
        best = _rank.pop();
      } else {
        auto tup =
          new std::tuple< Idx, Idx, Idx, std::vector< Idx > >{0, 0, 0, {}};
        best.first = tup;
        best.second = 0;
      }
      
      Size    steps_init = _current_step;
      Size steps_iter = _rank.size();
      
      while (best.second > 0.5) {
        Idx                x, y, z;
        std::vector< Idx > ui;
        x = std::get< 0 >(*best.first);
        y = std::get< 1 >(*best.first);
        z = std::get< 2 >(*best.first);
        ui = std::get< 3 >(*best.first);

        ui.push_back(z);
        /*std::cout << "Edge " << x << "-" << y << " with ui "<< ui << std::endl;*/
        double Ixy_ui = I.score(x, y, ui);
        /*std::cout << "  Ixy_ui = " << Ixy_ui << std::endl;*/
        if (Ixy_ui <= 0) {
          /*std::cout << "  Erase edge " << std::endl;*/
          graph.eraseEdge(Edge(x, y));
          sep_set.insert(std::make_pair(x, y), std::move(ui));
        } else {
          /*std::cout << "  Find next best contrib" << std::endl;*/
          _findBestContributor(x, y, ui, graph, I, _rank);
        }

        delete best.first;
        best = _rank.pop();

        ++_current_step;
        if (onProgress.hasListener()) {
          GUM_EMIT3(onProgress,
                    (_current_step * 66) / (steps_init + steps_iter),
                    0.,
                    _timer.step());
        }
      }
      _current_step = steps_init + steps_iter;
      if (onProgress.hasListener()) {
        GUM_EMIT3(onProgress, 66, 0., _timer.step());
      }
      _current_step = steps_init + steps_iter;
      
    }
      
    /// Orientation phase
    /*
    * PHASE 3 : ORIENTATION
    */
    void ThreeOffTwo::_orientation(
      CorrectedMutualInformation<>& I, 
      MixedGraph& graph, 
      const HashTable< std::pair< Idx, Idx >, std::vector< Idx > >& sep_set ){
          
      // //std::cout << "ORIENTATION" << std::endl;

      std::vector< std::pair< std::tuple< Idx, Idx, Idx >*, double > > triples =
        _getUnshieldedTriples(graph, I, sep_set);
      /*std::cout << "Triples found" << std::endl;*/
      Size steps_orient = triples.size();
      Size past_steps = _current_step;

      Idx i = 0;
      // list of elements that we shouldnt read again, ie elements that are
      // eligible to
      // rule 0 after the first time they are tested, and elements on which rule 1
      // has been applied
      while (i < triples.size()) {
        // if i not in do_not_reread
          std::pair< std::tuple< Idx, Idx, Idx >*, double > triple = triples[i];
          Idx x, y, z;
          x = std::get< 0 >(*triple.first);
          y = std::get< 1 >(*triple.first);
          z = std::get< 2 >(*triple.first);

          std::vector< Idx > ui;
          std::pair< Idx, Idx > key = {x, y};
          std::pair< Idx, Idx > rev_key = {y, x};
          if (sep_set.exists(key)) {
            ui = sep_set[key];
          } else if (sep_set.exists(rev_key)) {
            ui = sep_set[rev_key];
          }
          double Ixyz_ui = triple.second;
          /*std::cout << "Triple " << x << y << z << std::endl;*/
          /*std::cout << "Ixyz " << Ixyz_ui << std::endl;*/

      	  //try Rule 0
      	  if ( Ixyz_ui < 0 ){
      	    //if ( z not in Sep[x,y])
            if ( std::find(ui.begin(), ui.end(), z) == ui.end() ){
              //if what we want to add already exists : pass
              /*std::cout << "Rule 0 " << std::endl;*/
              if (( graph.existsArc( x, z ) || graph.existsArc( z, x ) ) &&
                  ( graph.existsArc( y, z ) || graph.existsArc( z, y ) ) ){
            	++i;
                if (graph.existsArc( z, x ) && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( z, x )) == __latent_couples.end()
                        && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( x, z )) == __latent_couples.end()){
                  __latent_couples.push_back( Arc( z, x ) );
                }
                if (graph.existsArc( z, y ) && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( z, y )) == __latent_couples.end()
                        && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( y, z )) == __latent_couples.end()){
                  __latent_couples.push_back( Arc( z, y ) );
                }
              } else{
              	i=0;
                graph.eraseEdge( Edge( x, z ) );
                graph.eraseEdge( Edge( y, z ) );
                //checking for cycles
                try{
                    std::vector<NodeId> path = graph.directedPath( z, x );
                    //if we find a cycle, we force the competing edge
                    graph.addArc( z, x );
                    if ( std::find(__latent_couples.begin(), __latent_couples.end(), Arc( z, x )) == __latent_couples.end() ){
                      __latent_couples.push_back( Arc( z, x ) );
                    }
                } catch ( gum::NotFound ){
                  graph.addArc( x, z );
                }
                try{
                    std::vector<NodeId> path = graph.directedPath( z, y );
                    //if we find a cycle, we force the competing edge
                    graph.addArc( z, y );
                    if ( std::find(__latent_couples.begin(), __latent_couples.end(), Arc( z, y )) == __latent_couples.end() ){
                      __latent_couples.push_back( Arc( z, y ) );
                    }
                } catch ( gum::NotFound ){
                  graph.addArc( y, z );
                }
              }
            } else {
              ++i;
            }
      	  }else {//try Rule 1
            bool reset{false};
            if ( graph.existsArc( x, z ) && !graph.existsArc( z, y )  && !graph.existsArc( y, z ) ){
              reset=true;
              graph.eraseEdge( Edge( z, y ) );
              /*std::cout << "Rule 1 " << std::endl;*/
              try{
                  std::vector<NodeId> path = graph.directedPath( y, z );
                  //if we find a cycle, we force the competing edge
                  graph.addArc( y, z );
                if ( std::find(__latent_couples.begin(), __latent_couples.end(), Arc( y, z )) == __latent_couples.end() ){
                  __latent_couples.push_back( Arc( y, z ) );
                }
              } catch ( gum::NotFound ){
                graph.addArc( z, y );
              }
            }
            if ( graph.existsArc( y, z ) && !graph.existsArc( z, x )  && !graph.existsArc( x, z )  ){
              reset=true;
              /*std::cout << "Rule 1 " << std::endl;*/
              graph.eraseEdge( Edge( z, x ) );
              try{
                  std::vector<NodeId> path = graph.directedPath( x, z );
                  //if we find a cycle, we force the competing edge
                  graph.addArc( x, z );
                if ( std::find(__latent_couples.begin(), __latent_couples.end(), Arc( x, z )) == __latent_couples.end() ){
                  __latent_couples.push_back( Arc( x, z ) );
                }
              } catch ( gum::NotFound ){
                graph.addArc( z, x );
              }
            }

            if ( reset ) {
              i=0;
            } else {
              ++i;
            }
      	  }//if rule 0 or rule 1

        if (onProgress.hasListener()) {
          GUM_EMIT3(onProgress,
                    ((_current_step + i) * 100) /
                      (past_steps + steps_orient),
                    0.,
                    _timer.step());
        }
      }  // while

      //erasing the the double headed arcs
      for ( const Arc& arc : __latent_couples ){
    	graph.eraseArc( Arc( arc.head(), arc.tail() ) );
      }
      
    }
    
    void ThreeOffTwo::_orientation_latents(
      CorrectedMutualInformation<>& I, 
      MixedGraph& graph, 
      const HashTable< std::pair< Idx, Idx >, std::vector< Idx > >& sep_set ){
          
      //std::cout << "ORIENTATION" << std::endl;

      std::vector< std::pair< std::tuple< Idx, Idx, Idx >*, double > > triples =
        _getUnshieldedTriples(graph, I, sep_set);
      /*std::cout << "Triples found" << std::endl;*/
      Size steps_orient = triples.size();
      Size past_steps = _current_step;

      Idx i = 0;
      // list of elements that we shouldnt read again, ie elements that are
      // eligible to
      // rule 0 after the first time they are tested, and elements on which rule 1
      // has been applied
      while (i < triples.size()) {
        // if i not in do_not_reread
          std::pair< std::tuple< Idx, Idx, Idx >*, double > triple = triples[i];
          Idx x, y, z;
          x = std::get< 0 >(*triple.first);
          y = std::get< 1 >(*triple.first);
          z = std::get< 2 >(*triple.first);

          std::vector< Idx > ui;
          std::pair< Idx, Idx > key = {x, y};
          std::pair< Idx, Idx > rev_key = {y, x};
          if (sep_set.exists(key)) {
            ui = sep_set[key];
          } else if (sep_set.exists(rev_key)) {
            ui = sep_set[rev_key];
          }
          double Ixyz_ui = triple.second;
          /*std::cout << "Triple " << x << y << z << std::endl;*/
          /*std::cout << "Ixyz " << Ixyz_ui << std::endl;*/

         //try Rule 0
         if ( Ixyz_ui < 0 ){
           //if ( z not in Sep[x,y])
            if ( std::find(ui.begin(), ui.end(), z) == ui.end() ){
              //if what we want to add already exists : pass
              /*std::cout << "Rule 0 " << std::endl;*/
              if (( graph.existsArc( x, z ) || graph.existsArc( z, x ) ) &&
                  ( graph.existsArc( y, z ) || graph.existsArc( z, y ) ) ){
             ++i;
              } else{
                i=0;
                graph.eraseEdge( Edge( x, z ) );
                graph.eraseEdge( Edge( y, z ) );
                //checking for cycles
                if ( graph.existsArc( z, x ) ){
                  graph.eraseArc( Arc( z, x ) );
                  try{
                      std::vector<NodeId> path = graph.directedPath( z, x );
                      //if we find a cycle, we force the competing edge
                      __latent_couples.push_back( Arc( z, x ) );
                  } catch ( gum::NotFound ){
                    graph.addArc( x, z );
                  }
                  graph.addArc( z, x );
                } else {
                  try{
                      std::vector<NodeId> path = graph.directedPath( z, x );
                      //if we find a cycle, we force the competing edge
                      graph.addArc( z, x );
                      __latent_couples.push_back( Arc( z, x ) );
                  } catch ( gum::NotFound ){
                    graph.addArc( x, z );
                  }
                }
                if ( graph.existsArc( z, y ) ){
                  graph.eraseArc( Arc( z, y ) );
                  try{
                      std::vector<NodeId> path = graph.directedPath( z, y );
                      //if we find a cycle, we force the competing edge
                      __latent_couples.push_back( Arc( z, y ) );
                  } catch ( gum::NotFound ){
                    graph.addArc( y, z );
                  }
                  graph.addArc( z, y );
                } else {
                  try{
                      std::vector<NodeId> path = graph.directedPath( z, y );
                      //if we find a cycle, we force the competing edge
                      graph.addArc( z, y );
                      __latent_couples.push_back( Arc( z, y ) );

                  } catch ( gum::NotFound ){
                    graph.addArc( y, z );
                  }
                }
             if ( graph.existsArc( z, x ) && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( z, x )) == __latent_couples.end()
                     && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( x, z )) == __latent_couples.end() ){
               __latent_couples.push_back( Arc( z, x ) );
             }
             if ( graph.existsArc( z, y ) && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( z, y )) == __latent_couples.end()
                     && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( y, z )) == __latent_couples.end() ){
               __latent_couples.push_back( Arc( z, y ) );
             }
              }
            } else {
              ++i;
            }
         }else {//try Rule 1
            bool reset{false};
            if ( graph.existsArc( x, z ) && !graph.existsArc( z, y )  && !graph.existsArc( y, z ) ){
              reset=true;
              graph.eraseEdge( Edge( z, y ) );
              /*std::cout << "Rule 1 " << std::endl;*/
              try{
                  std::vector<NodeId> path = graph.directedPath( y, z );
                  //if we find a cycle, we force the competing edge
                  graph.addArc( y, z );
                  __latent_couples.push_back( Arc( y, z ) );
              } catch ( gum::NotFound ){
                graph.addArc( z, y );
              }
            }
            if ( graph.existsArc( y, z ) && !graph.existsArc( z, x )  && !graph.existsArc( x, z )  ){
              reset=true;
              /*std::cout << "Rule 1 " << std::endl;*/
              graph.eraseEdge( Edge( z, x ) );
              try{
                  std::vector<NodeId> path = graph.directedPath( x, z );
                  //if we find a cycle, we force the competing edge
                  graph.addArc( x, z );
                  __latent_couples.push_back( Arc( x, z ) );
              } catch ( gum::NotFound ){
                graph.addArc( z, x );
              }
            }

            if ( reset ) {
              i=0;
            } else {
              ++i;
            }
         }//if rule 0 or rule 1
        if (onProgress.hasListener()) {
          GUM_EMIT3(onProgress,
                    ((_current_step + i) * 100) /
                      (past_steps + steps_orient),
                    0.,
                    _timer.step());
        }
      }  // while

      //erasing the the double headed arcs
      for ( const Arc& arc : __latent_couples ){
        graph.eraseArc( Arc( arc.head(), arc.tail() ) );
      }
    }
    
    
    void ThreeOffTwo::_orientation_miic(
      CorrectedMutualInformation<>& I, 
      MixedGraph& graph, 
      const HashTable< std::pair< Idx, Idx >, std::vector< Idx > >& sep_set ){
      //std::cout << "Coucou, c'est MIIC ! " << std::endl;
          
      //structure to store the orientations marks -, o, or >, 
      //Considers the head of the edge first node -* second node
      HashTable< std::pair< Idx, Idx >, char > marks;
      
      std::vector< std::tuple< std::tuple< Idx, Idx, Idx >*, double, double, double > > proba_triples =
        _getUnshieldedTriplesMIIC(graph, I, sep_set, marks);
      
      Size steps_orient = proba_triples.size();
      Size past_steps = _current_step;
      
      //std::cout << "Etapes à réaliser :" << steps_orient << std::endl;
      std::tuple< std::tuple< Idx, Idx, Idx >*, double, double, double > best;
      if (steps_orient > 0){ 
        best = proba_triples[0];
      }
      //std::cout << "Et c'est parti ! " << std::endl;
      
      while (!proba_triples.empty() && std::max(std::get< 2 >(best), std::get< 3 >(best)) > 0.5) {
        Idx                x, y, z;
        x = std::get< 0 >(*std::get< 0 >(best));
        y = std::get< 1 >(*std::get< 0 >(best));
        z = std::get< 2 >(*std::get< 0 >(best));
        //std::cout << "Triplet " << x << y << z << std::endl;
        
        const double i3 = std::get< 1 >(best);

        if (i3 <= 0) {
            std::cout << "V-structure " << std::endl;
          if ( marks[{x, z}] == 'o' && marks[{y, z}] == 'o' ){
            graph.eraseEdge(Edge( x, z ));
            graph.eraseEdge(Edge( y, z ));
            graph.addArc( x, z );
            graph.addArc( y, z );
            marks[{x, z}] = '>';
            marks[{y, z}] = '>';
            if ( graph.existsArc( z, x ) && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( z, x )) == __latent_couples.end()
                     && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( x, z )) == __latent_couples.end() ){
               __latent_couples.push_back( Arc( z, x ) );
               //std::cout << x << "<-" << z << std::endl;
            }
            if ( graph.existsArc( z, y ) && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( z, y )) == __latent_couples.end()
                     && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( y, z )) == __latent_couples.end() ){
               __latent_couples.push_back( Arc( z, y ) );
               //std::cout << y << "<-" << z << std::endl;
            }
          } else if ( marks[{x, z}] == '>' && marks[{y, z}] == 'o' ){
            graph.eraseEdge(Edge( y, z ));
            graph.addArc( y, z );
            marks[{y, z}] = '>';
            if ( graph.existsArc( z, y ) && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( z, y )) == __latent_couples.end()
                    && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( y, z )) == __latent_couples.end() ){
              __latent_couples.push_back( Arc( z, y ) );
               //std::cout << y << "<-" << z << std::endl;
            }
          } else if ( marks[{y, z}] == '>' && marks[{x, z}] == 'o' ){
            graph.eraseEdge(Edge( x, z ));
            graph.addArc( x, z );
            marks[{x, z}] = '>';
            if ( graph.existsArc( z, x ) && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( z, x )) == __latent_couples.end()
                    && std::find(__latent_couples.begin(), __latent_couples.end(), Arc( x, z )) == __latent_couples.end() ){
              __latent_couples.push_back( Arc( z, x ) );
               //std::cout << x << "<-" << z << std::endl;
            }
          }
          
        } else {
          if ( marks[{x, z}] == '>' && marks[{y, z}] == 'o' ){
      //std::cout << "propage z->y " << std::endl;
            graph.eraseEdge(Edge( z, y ));
            if ( !__existsDirectedPath(graph, y, z)){
              graph.addArc( z, y );
              marks[{z, y}] = '>';
              marks[{y, z}] = '-';
            } else {
              graph.addArc( y, z );
              marks[{z, y}] = '-';
              marks[{y, z}] = '>';
              __latent_couples.push_back( Arc( y, z ) );
            }
          } else if ( marks[{y, z}] == '>' && marks[{x, z}] == 'o' ){
      //std::cout << "propage z->x " << std::endl;
            graph.eraseEdge(Edge( z, x ));
            if ( !__existsDirectedPath(graph, x, z)){
              graph.addArc( z, x );
              marks[{z, x}] = '>';
              marks[{x, z}] = '-';
            } else{
              graph.addArc( x, z );
              marks[{z, x}] = '-';
              marks[{x, z}] = '>';
              __latent_couples.push_back( Arc( x, z ) );
            }
          }
        }

        delete std::get< 0 >(best);
        proba_triples.erase(proba_triples.begin());
        //actualisation of the list of triples 
        proba_triples = _updateProbaTriples( graph, proba_triples );
      //std::cout << "c'est reparti pour un tour ! " << std::endl;
        
        best = proba_triples[0];

        ++_current_step;
        if (onProgress.hasListener()) {
          GUM_EMIT3(onProgress,
                    (_current_step * 100) / (steps_orient + past_steps),
                    0.,
                    _timer.step());
        }
      }//while
      
      //erasing the the double headed arcs
      for ( auto iter=__latent_couples.rbegin(); iter != __latent_couples.rend(); ++iter ){
        graph.eraseArc( Arc( iter->head(), iter->tail() ) );
        try{
          std::vector<NodeId> path = graph.directedPath( iter->head(), iter->tail() );
          //if we find a cycle, we force the competing edge
          graph.addArc( iter->head(), iter->tail()  );
          graph.eraseArc( Arc( iter->tail(), iter->head() ) );
          *iter = Arc( iter->head(), iter->tail() );
        } catch ( gum::NotFound ){
        }
      }
      /*
      //erasing the the double headed arcs
      for ( const Arc& arc : __latent_couples ){
        graph.eraseArc( Arc( arc.head(), arc.tail() ) );
      }
      */
      //std::cout << "Finiiii <3 " << std::endl;
      //std::cout << graph << std::endl;
      std::cout << graph.toDot() << std::endl;
      if (onProgress.hasListener()) {
        GUM_EMIT3(onProgress,
                 100,
                 0.,
                 _timer.step());
      }
    }
    
    
    /// finds the best contributor node for a pair given a conditioning set
    void ThreeOffTwo::_findBestContributor(
      Idx                           x,
      Idx                           y,
      const std::vector< Idx >&     ui,
      const MixedGraph&             graph,
      CorrectedMutualInformation<>& I,
      Heap< std::pair< std::tuple< Idx, Idx, Idx, std::vector< Idx > >*, double >,
            GreaterPairOn2nd >& _rank) {
      double maxP = -1.0;
      Idx    maxZ;

      // compute N
      __N = I.N();
      const double Ixy_ui = I.score(x, y, ui);

      for (const Idx z : graph) {
        // if z!=x and z!=y and z not in ui
        if (z != x && z != y && std::find(ui.begin(), ui.end(), z) == ui.end()) {
          /*std::cout << "    Trying z = " << z << std::endl;*/
          double Pnv;
          double Pb;
          // Computing Pnv
          const double Ixyz_ui = I.score(x, y, z, ui);
          /*std::cout << "    Ixyz_ui = " << Ixyz_ui << std::endl;*/
          double calc_expo1 = -Ixyz_ui * __N;
          // if exponentials are too high or to low, crop them at |__maxLog|
          if (calc_expo1 > __maxLog) {
            Pnv = 0.0;
          } else if (calc_expo1 < -__maxLog) {
            Pnv = 1.0;
          } else {
            Pnv = 1 / (1 + std::exp(calc_expo1));
          }


          /*std::cout << "    Pnv = " << Pnv << std::endl;*/
          // Computing Pb
          const double Ixz_ui = I.score(x, z, ui);
          const double Iyz_ui = I.score(y, z, ui);

          calc_expo1 = -(Ixz_ui - Ixy_ui) * __N;
          double calc_expo2 = -(Iyz_ui - Ixy_ui) * __N;

          // if exponentials are too high or to low, crop them at __maxLog
          if (calc_expo1 > __maxLog || calc_expo2 > __maxLog) {
            Pb = 0.0;
          } else if (calc_expo1 < -__maxLog && calc_expo2 < -__maxLog) {
            Pb = 1.0;
          } else {
            double expo1, expo2;
            if (calc_expo1 < -__maxLog) {
              expo1 = 0.0;
            } else {
              expo1 = std::exp(calc_expo1);
            }
            if (calc_expo2 < -__maxLog) {
              expo2 = 0.0;
            } else {
              expo2 = std::exp(calc_expo2);
            }
            Pb = 1 / (1 + expo1 + expo2);
          }


          /*std::cout << "    Pb = " << Pb << std::endl;*/
          // Getting max(min(Pnv, pb))
          if (std::min(Pnv, Pb) > maxP) {
            maxP = std::min(Pnv, Pb);
            maxZ = z;
          }
        }  // if z not in (x, y)
      }    // for z in graph.nodes
      // storing best z in _rank
      std::pair< std::tuple< Idx, Idx, Idx, std::vector< Idx > >*, double > final;
      auto tup =
        new std::tuple< Idx, Idx, Idx, std::vector< Idx > >{x, y, maxZ, ui};
      final.first = tup;
      final.second = maxP;
      _rank.insert(final);
    }

    /// gets the list of unshielded triples in the graph in decreasing value of
    ///|I'(x, y, z|{ui})|
    /*@param graph graph in which to find the triples
     *
     */
    std::vector< std::pair< std::tuple< Idx, Idx, Idx >*, double > >
    ThreeOffTwo::_getUnshieldedTriples(
      const MixedGraph&             graph,
      CorrectedMutualInformation<>& I,
      const HashTable< std::pair< Idx, Idx >, std::vector< Idx > >& sep_set) {
      std::vector< std::pair< std::tuple< Idx, Idx, Idx >*, double > > triples;
      for (Idx z : graph) {
        for (Idx x : graph.neighbours(z)) {
          for (Idx y : graph.neighbours(z)) {
            if (y < x && !graph.existsEdge(x, y)) {
              /*std::cout << "    Triple found !" << std::endl;*/

              std::vector< Idx > ui;
              std::pair< Idx, Idx > key = {x, y};
              std::pair< Idx, Idx > rev_key = {y, x};
              if (sep_set.exists(key)) {
                ui = sep_set[key];
              } else if (sep_set.exists(rev_key)) {
                ui = sep_set[rev_key];
              }
              /*std::cout << "    separation set : " << ui << std::endl;*/
              // remove z from ui if it's present
              const auto iter_z_place = std::find(ui.begin(), ui.end(), z);
              if (iter_z_place != ui.end()) {
                /*std::cout << "z in ui" << std::endl;*/
                ui.erase(iter_z_place);
              }

              double Ixyz_ui = I.score(x, y, z, ui);
              std::pair< std::tuple< Idx, Idx, Idx >*, double > triple;
              auto tup = new std::tuple< Idx, Idx, Idx >{x, y, z};
              // std::tuple<Idx, Idx, Idx> tup = {x, y, z};
              triple.first = tup;
              triple.second = Ixyz_ui;
              triples.push_back(triple);
            }
          }
        }
      }
      std::sort(triples.begin(), triples.end(), GreaterAbsPairOn2nd());
      return triples;
    }

    std::vector< std::tuple< std::tuple< Idx, Idx, Idx >*, double, double, double > >
    ThreeOffTwo::_getUnshieldedTriplesMIIC(
      const MixedGraph&             graph,
      CorrectedMutualInformation<>& I,
      const HashTable< std::pair< Idx, Idx >, std::vector< Idx > >& sep_set, 
      HashTable< std::pair< Idx, Idx >, char >& marks) {
      std::vector< std::tuple< std::tuple< Idx, Idx, Idx >*, double, double, double > > triples;
      for (Idx z : graph) {
        for (Idx x : graph.neighbours(z)) {
          for (Idx y : graph.neighbours(z)) {
            if (y < x && !graph.existsEdge(x, y)) {
              /*std::cout << "    Triple found !" << std::endl;*/

              std::vector< Idx > ui;
              std::pair< Idx, Idx > key = {x, y};
              std::pair< Idx, Idx > rev_key = {y, x};
              if (sep_set.exists(key)) {
                ui = sep_set[key];
              } else if (sep_set.exists(rev_key)) {
                ui = sep_set[rev_key];
              }
              /*std::cout << "    separation set : " << ui << std::endl;*/
              // remove z from ui if it's present
              const auto iter_z_place = std::find(ui.begin(), ui.end(), z);
              if (iter_z_place != ui.end()) {
                /*std::cout << "z in ui" << std::endl;*/
                ui.erase(iter_z_place);
              }

              const double Ixyz_ui = I.score(x, y, z, ui);
              auto tup = new std::tuple< Idx, Idx, Idx >{x, y, z};
              std::tuple< std::tuple< Idx, Idx, Idx >*, double, double, double > 
                triple{tup, Ixyz_ui, 0.5, 0.5};
              triples.push_back(triple);
              if ( !marks.exists({x, z}) ){
                marks.insert({x, z}, 'o');
              }
              if ( !marks.exists({z, x}) ){
                marks.insert({z, x}, 'o');
              }
              if ( !marks.exists({y, z}) ){
                marks.insert({y, z}, 'o');
              }
              if ( !marks.exists({z, y}) ){
                marks.insert({z, y}, 'o');
              }
            }
          }
        }
      }
      triples = _updateProbaTriples( graph, triples );
      std::sort(triples.begin(), triples.end(), GreaterTupleOnLast());
      return triples;
    }
    
    std::vector< std::tuple< std::tuple< Idx, Idx, Idx >*, double, double, double > > 
    ThreeOffTwo::_updateProbaTriples( 
      const MixedGraph&             graph,
      std::vector< std::tuple< std::tuple< Idx, Idx, Idx >*, double, double, double > > proba_triples ){
      
      for (auto &triple : proba_triples){
          
        Idx x, y, z;
        x = std::get< 0 >(*std::get< 0 >(triple));
        y = std::get< 1 >(*std::get< 0 >(triple));
        z = std::get< 2 >(*std::get< 0 >(triple));
        const double Ixyz = std::get< 1 >(triple);
        double Pxz = std::get< 2 >(triple);
        double Pyz = std::get< 3 >(triple);
        //std::cout << "  Triplet " << x << y << z << std::endl;
        //std::cout << "  I " << Ixyz << " Pxz " << Pxz << " Pyz " << Pyz << std::endl;

        
        if ( Ixyz <= 0 ){
            const double expo = std::exp( Ixyz * __N );
            const double P0 = ( 1 + expo ) / ( 1 + 3 * expo );
            //distinguish betweeen the initialization and the update process
            if ( Pxz == Pyz && Pyz == 0.5 ){
                std::get< 2 >(triple) = P0;
                std::get< 3 >(triple) = P0;
                //std::cout << "  Pxz=Pyz=" << P0 << std::endl;
            } else {
                if ( graph.existsArc( x, z ) && Pxz >= P0 ){
                    std::get< 3 >(triple) = Pxz * ( 1 / ( 1 + expo ) - 0.5 ) + 0.5;
                //std::cout << "  Pyz=" << std::get< 3 >(triple) << std::endl;
                }else if ( graph.existsArc( y, z ) && Pyz >= P0 ){
                    std::get< 2 >(triple) = Pyz * ( 1 / ( 1 + expo ) - 0.5 ) + 0.5;
                //std::cout << "  Pxz=" << std::get< 2 >(triple) << std::endl;
                } 
            }
        } else {
            const double expo = std::exp( - Ixyz * __N );
            if ( graph.existsArc( x, z ) && Pxz >= 0.5 ){
                std::get< 3 >(triple) = Pxz * ( 1 / ( 1 + expo ) - 0.5 ) + 0.5;
                //std::cout << "  Pyz=" << std::get< 3 >(triple) << std::endl;
            } else if ( graph.existsArc( y, z ) && Pyz >= 0.5 ){
                std::get< 2 >(triple) = Pyz * ( 1 / ( 1 + expo ) - 0.5 ) + 0.5;
                //std::cout << "  Pxz=" << std::get< 2 >(triple) << std::endl;
            } 
        }
      }
      std::sort(proba_triples.begin(), proba_triples.end(), GreaterTupleOnLast());
      return proba_triples;
    }
    /// learns the structure of an Bayesian network, ie a DAG, from an Essential
    /// graph.
    DAG ThreeOffTwo::learnStructure(CorrectedMutualInformation<>& I,
                                    MixedGraph                    initialGraph) {

      MixedGraph essentialGraph = learnMixedStructure(I, initialGraph);

      // Second, orientate remaining edges
      const Sequence< NodeId > order = essentialGraph.topologicalOrder();
      /*std::cout << order << std::endl;*/
      /*std::cout << essentialGraph << std::endl;*/
      // first, propagate existing orientations
      for (NodeId x : order) {
        ///*std::cout << "  " << x << std::endl;*/
        if (!essentialGraph.parents(x).empty()) {
          _propagatesHead(essentialGraph, x);
        }
      }
      // then decide the orientation by the topological order and propagate them
      for (NodeId x : order) {
        if (!essentialGraph.neighbours(x).empty()) {
          _propagatesHead(essentialGraph, x);
        }
      }

      /*std::cout << essentialGraph << std::endl;*/

      // turn the mixed graph into a dag
      DAG dag;
      for (auto node : essentialGraph) {
        dag.addNode(node);
      }
      for (const Arc& arc : essentialGraph.arcs()) {
        dag.addArc(arc.tail(), arc.head());
      }

      return dag;
    }

    /// Propagates the orientation from a node to its neighbours
    void ThreeOffTwo::_propagatesHead(MixedGraph& graph, NodeId node) {
      ///*std::cout << "  populate " << node << std::endl;*/
      const auto neighbours = graph.neighbours(node);
      ///*std::cout << "  neighbouring " << neighbours << std::endl;*/
      for (auto& neighbour : neighbours) {
        // only propagate if it doesn't create a circle
        try {
          std::vector< NodeId > test = graph.directedPath(neighbour, node);
          graph.eraseEdge(Edge(neighbour, node));
          graph.addArc(neighbour, node);
        } catch (gum::NotFound) {
          graph.eraseEdge(Edge(neighbour, node));
          graph.addArc(node, neighbour);
          _propagatesHead(graph, neighbour);
        }
      }
    }

    /// get the list of arcs hiding latent variables
    const std::vector< Arc > ThreeOffTwo::latentVariables() const {
      return __latent_couples;
    }

    /// learns the structure and the parameters of a BN
    template < typename GUM_SCALAR,
               typename GRAPH_CHANGES_SELECTOR,
               typename PARAM_ESTIMATOR,
               typename CELL_TRANSLATORS >
    BayesNet< GUM_SCALAR >
    ThreeOffTwo::learnBN(GRAPH_CHANGES_SELECTOR&           selector,
                         PARAM_ESTIMATOR&                  estimator,
                         const std::vector< std::string >& names,
                         const std::vector< Idx >&         modal,
                         const CELL_TRANSLATORS&           translator,
                         DAG                               initial_dag) {
      return DAG2BNLearner::
        createBN< GUM_SCALAR, PARAM_ESTIMATOR, CELL_TRANSLATORS >(
          estimator,
          learnStructure(selector, modal, initial_dag),
          names,
          modal,
          translator);
    }

    void ThreeOffTwo::orientMIIC(){
        this->__usemiic = true;
    }
    void ThreeOffTwo::orient3off2(){
        this->__usemiic = false;
    }
    
    const bool ThreeOffTwo::__existsDirectedPath( const MixedGraph& graph, 
                                            const NodeId      n1,
                                            const NodeId      n2 ) const {
     // not recursive version => use a FIFO for simulating the recursion
     List<NodeId> nodeFIFO;
     nodeFIFO.pushBack( n2 );
 
     // mark[node] = successor if visited, else mark[node] does not exist
     NodeProperty<NodeId> mark;
     mark.insert( n2, n2 );
 
     NodeId current;
 
     while ( !nodeFIFO.empty() ) {
       current = nodeFIFO.front();
       nodeFIFO.popFront();
 
       // check the parents
 
       for ( const auto new_one : graph.parents( current ) ) {
         if ( mark.exists( new_one ) )  // if this node is already marked, do not
           continue;                    // check it again
 
         mark.insert( new_one, current );
 
         if ( graph.existsArc( current, new_one ) )//if there is a double arc, pass
           continue;
         
         if ( new_one == n1 ) {
           return true;
         }
 
         nodeFIFO.pushBack( new_one );
       }
     }
 
    return false;
   }
 
  } /* namespace learning */

} /* namespace gum */
