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
* @brief Implementations of the classes defined in
* InfluenceDiagram/inference/defaultInfluenceDiagramInference.h.
*/
// ============================================================================
#ifndef DOXYGEN_SHOULD_SKIP_THIS
// ============================================================================

// to ease parsing by IDE
#include <agrum/influenceDiagram/inference/defaultInfluenceDiagramInference.h>

namespace gum {
// ============================================================================

// Default constructor
  template<typename T_DATA>
  DefaultInfluenceDiagramInference<T_DATA>::DefaultInfluenceDiagramInference ( const InfluenceDiagram<T_DATA> &infDiag ) :
      InfluenceDiagramInference<T_DATA> ( infDiag ),
      __triangulation ( 0 ),
      __inferencePotential ( 0 ),
      __inferenceUtility ( 0 ),
      __inferenceMade ( false ) {
    GUM_CONSTRUCTOR ( DefaultInfluenceDiagramInference );

    //Make modalities map
    typename Property<unsigned int>::onNodes __modalitiesMap;

    for ( DAG::NodeIterator iter = this->getInfluenceDiagram().beginNodes(); iter != this->getInfluenceDiagram().endNodes(); ++iter )
      __modalitiesMap.insert ( *iter,  this->getInfluenceDiagram().variable ( *iter ).domainSize() );

    const UndiGraph* moralGraph = & ( this->getInfluenceDiagram().moralGraph() );

    const List<NodeSet>* partialTemporalOrder = & ( this->getInfluenceDiagram().getPartialTemporalOrder() );

    //Make Junction Tree
    __triangulation = new PartialOrderedTriangulation ( moralGraph, &__modalitiesMap, partialTemporalOrder );

    __triangulation->junctionTree();

    __makeCliquePropertiesMap();

    __makeStrongJunctionTree();
  }

// Destructor
  template<typename T_DATA>
  DefaultInfluenceDiagramInference<T_DATA>::~DefaultInfluenceDiagramInference() {
    GUM_DESTRUCTOR ( DefaultInfluenceDiagramInference );

    delete __triangulation;

    __cleanUp();

    for ( typename Property< CliqueProperties<T_DATA>* >::onNodes::iterator cliquePropertyIter = __cliquePropertiesMap.begin();
          cliquePropertyIter != __cliquePropertiesMap.end(); ++cliquePropertyIter )
      delete *cliquePropertyIter;

    for ( SetIterator< Potential<T_DATA>* > potentialDummiesIter = __potentialDummies.begin();
          potentialDummiesIter != __potentialDummies.end(); ++potentialDummiesIter )
      delete *potentialDummiesIter;

    for ( SetIterator< UtilityTable<T_DATA>* > utilityDummiesIter = __utilityDummies.begin();
          utilityDummiesIter != __utilityDummies.end(); ++utilityDummiesIter )
      delete *utilityDummiesIter;
  }

  template <typename T_DATA>
  void
  DefaultInfluenceDiagramInference<T_DATA>::makeInference() {
    __cleanUp();

    NodeId rootClique = __cliqueEliminationMap[0];
    const NodeSet& neighbours = __triangulation->junctionTree().neighbours ( rootClique );

    for ( NodeSetIterator childIterator = neighbours.begin(); childIterator != neighbours.end(); ++childIterator )
      __collectChild ( rootClique, *childIterator );

    NodeSet separator;

    __reduceClique ( __cliquePropertiesMap[ rootClique ], separator, __inferencePotential, __inferenceUtility );

    __inferenceMade = true;
  }

  template <typename T_DATA> INLINE
  T_DATA
  DefaultInfluenceDiagramInference<T_DATA>::getMEU() {
    if ( !__inferenceMade )
      GUM_ERROR ( OperationNotAllowed, "No inference have yet been made" );

    Instantiation res ( __inferenceUtility );

    return __inferenceUtility->get ( res );
  }

  template <typename T_DATA> INLINE
  const Idx
  DefaultInfluenceDiagramInference<T_DATA>::getBestDecisionChoice ( NodeId decisionId ) {
    if ( !__inferenceMade )
      GUM_ERROR ( OperationNotAllowed, "No inference have yet been made" );

    if ( !this->getInfluenceDiagram().isDecisionNode ( decisionId ) )
      GUM_ERROR ( InvalidNode, "Node is not a decision node" );

    return __utakenDecisionMap[ decisionId ];
  }


  template <typename T_DATA>
  void
  DefaultInfluenceDiagramInference<T_DATA>::displayResult ( std::ostream& stream ) {
    if ( !__inferenceMade )
      GUM_ERROR ( OperationNotAllowed, "No inference have yet been made" );

    stream << "Espérance d'utilité finale : " << std::endl << std::flush;

    stream << *__inferenceUtility << std::endl;

    stream << "Choix maximisant l'espérance d'utilité : " << std::endl << std::flush;

    for ( HashTableConstIterator< NodeId, Idx > utilityIter = __utakenDecisionMap.begin();
          utilityIter != __utakenDecisionMap.end(); ++utilityIter ) {
      stream << "Decision " << this->getInfluenceDiagram().variable ( utilityIter.key() ) << " : " << this->getInfluenceDiagram().variable ( utilityIter.key() ).label ( *utilityIter ) << std::endl << std::flush;
    }
  }

// Returns the Triangulation used by this class.
  template <typename T_DATA> INLINE
  Triangulation&
  DefaultInfluenceDiagramInference<T_DATA>::getTriangulation() {
    return *__triangulation;
  }

// insert new evidence in the graph
  template<typename T_DATA>
  void
  DefaultInfluenceDiagramInference<T_DATA>::insertEvidence ( const List<const Potential<T_DATA>*>& evidenceList ) {
    for ( ListConstIterator<const Potential<T_DATA>*> evidenceListIter = evidenceList.begin();
          evidenceListIter != evidenceList.end(); ++evidenceListIter )
      __cliquePropertiesMap[ __nodeToCliqueMap[ this->getInfluenceDiagram().nodeId ( ( *evidenceListIter )->variable ( 0 ) ) ] ]->addEvidence ( **evidenceListIter );
  }

// remove a given evidence from the graph
  template <typename T_DATA>
  void
  DefaultInfluenceDiagramInference<T_DATA>::eraseEvidence ( const Potential<T_DATA>* evidence ) {
    if ( not ( evidence->variablesSequence().size() != 1 ) )
      __cliquePropertiesMap[ __nodeToCliqueMap[ this->getInfluenceDiagram().nodeId ( evidence->variable ( 0 ) ) ] ]->removeEvidence ( evidence->variable ( 0 ) );
  }

// remove all evidence from the graph
  template <typename T_DATA>
  void
  DefaultInfluenceDiagramInference<T_DATA>::eraseAllEvidence() {
    for ( typename Property< CliqueProperties<T_DATA>* >::onNodes::iterator nodeIter = __cliquePropertiesMap.begin(); nodeIter != __cliquePropertiesMap.end(); ++nodeIter )
      ( *nodeIter )->removeAllEvidence();
  }

// @return Returns the list of neighbours of a given clique
  template<typename T_DATA> INLINE
  const EdgeSet&
  DefaultInfluenceDiagramInference<T_DATA>::__getNeighbours ( NodeId cliqueId ) {
    return __triangulation->junctionTree().neighbours ( cliqueId );
  }

// @return Returns a separator given two adjacent cliques
  template<typename T_DATA> INLINE
  const NodeSet&
  DefaultInfluenceDiagramInference<T_DATA>::__getSeparator ( NodeId clique_1, NodeId clique_2 ) {
    return __triangulation->junctionTree().separator ( clique_1, clique_2 );
  }


// @return Returns the clique in which the node's potential must be stored
  template<typename T_DATA>
  NodeId
  DefaultInfluenceDiagramInference<T_DATA>::__getClique ( const std::vector<NodeId> &eliminationOrder, NodeId id ) {
    NodeSet idSet;
    idSet.insert ( id );

    const NodeSet& parents = this->getInfluenceDiagram().dag().parents ( id );

    for ( NodeSet::const_iterator parentsIter = parents.begin(); parentsIter != parents.end(); ++parentsIter )
      idSet.insert ( *parentsIter );

    for ( size_t i = 0; i < eliminationOrder.size(); ++i )
      if ( idSet.contains ( eliminationOrder[i] ) )
        return __triangulation->createdJunctionTreeClique ( eliminationOrder[i] );

    std::stringstream msg;

    msg << "No clique found for node " << id;

    GUM_ERROR ( FatalError, msg.str() );
  }

// Builds the cliques tables
  template<typename T_DATA>
  void
  DefaultInfluenceDiagramInference<T_DATA>::__makeCliquePropertiesMap() {
    const std::vector<NodeId> elim = __triangulation->eliminationOrder();

    NodeSet potentialsCliquesSet, utilitiesCliqueSet;

    // First pass to create the clique's table

    for ( CliqueGraph::NodeIterator cliqueIter = __triangulation->junctionTree().beginNodes();
          cliqueIter != __triangulation->junctionTree().endNodes(); ++cliqueIter ) {
      __cliquePropertiesMap.insert ( *cliqueIter, new CliqueProperties<T_DATA>() );
      potentialsCliquesSet.insert ( *cliqueIter );
      utilitiesCliqueSet.insert ( *cliqueIter );

      for ( NodeSetIterator cliqueNodesIter = __triangulation->junctionTree().clique ( *cliqueIter ).begin();
            cliqueNodesIter != __triangulation->junctionTree().clique ( *cliqueIter ).end(); ++cliqueNodesIter )
        __cliquePropertiesMap[*cliqueIter]->addVariable ( this->getInfluenceDiagram().variable ( *cliqueNodesIter ) );

      __cliquePropertiesMap[*cliqueIter]->makeEliminationOrder ( elim, this->getInfluenceDiagram() );
    }

    // Second pass to add the potentials and utilities into good cliques
    for ( size_t i = 0; i < elim.size(); i++ ) {
      // Récupération de la bonne clique
      NodeId cliqueId = __getClique ( elim, elim[i] );
      __nodeToCliqueMap.insert ( elim[i], cliqueId );

      // Ajout de la cpt si le noeud est un noeud chance

      if ( this->getInfluenceDiagram().isChanceNode ( elim[i] ) ) {
        __cliquePropertiesMap[cliqueId]->addPotential ( this->getInfluenceDiagram().cpt ( elim[i] ) );
        potentialsCliquesSet.erase ( cliqueId );
      }
    }

    // Third pass to fill empty cliques with "one" matrices for potentials and "zero" matrices for utilities.
    for ( NodeSetIterator potentialCliqueIter = potentialsCliquesSet.begin();
          potentialCliqueIter != potentialsCliquesSet.end(); ++potentialCliqueIter )
      __cliquePropertiesMap[ *potentialCliqueIter ]->addPotential ( *__makeDummyPotential ( *potentialCliqueIter ) );

    // Fourth pass to adress utility table to the good clique
    // We go trought all diagram's nodes in search of utility nodes
    for ( NodeGraphPartIterator  nodesIter = this->getInfluenceDiagram().dag().beginNodes();
          nodesIter != this->getInfluenceDiagram().dag().endNodes(); ++nodesIter ) {
      if ( this->getInfluenceDiagram().isUtilityNode ( *nodesIter ) ) {
        // Récupération de la bonne clique
        NodeId cliqueId = __getClique ( elim, *nodesIter );
        __cliquePropertiesMap[cliqueId]->addUtility ( this->getInfluenceDiagram().utility ( *nodesIter ) );
        utilitiesCliqueSet.erase ( cliqueId );
      }
    }

    for ( NodeSetIterator utilityCliqueIter = utilitiesCliqueSet.begin();
          utilityCliqueIter != utilitiesCliqueSet.end(); ++utilityCliqueIter )
      __cliquePropertiesMap[ *utilityCliqueIter ]->addUtility ( *__makeDummyUtility ( *utilityCliqueIter ) );
  }

// Makes the strong junction tree to make easier inferences
  template<typename T_DATA>
  void
  DefaultInfluenceDiagramInference<T_DATA>::__makeStrongJunctionTree() {
    UndiGraph triaGraph = __triangulation->triangulatedGraph();
    CliqueGraph junTree = __triangulation->junctionTree();
    const std::vector<NodeId> elim = __triangulation->eliminationOrder();

    // Pour chaque clique

    for ( CliqueGraph::NodeIterator cliqueIter = junTree.beginNodes(); cliqueIter != junTree.endNodes(); ++cliqueIter ) {
      Sequence<NodeId> eliminationOrder = __cliquePropertiesMap[ *cliqueIter ]->cliqueEliminationOrder();
      SequenceIterator<NodeId> cliqueNodesIter =  eliminationOrder.begin();
      bool ValidIndex = false;

      // On parcours chaque noeud de la clique par ordre d'élimination, ...

      while ( cliqueNodesIter != eliminationOrder.end() && !ValidIndex ) {
        // ...pour chacun de ces noeuds on regarde ses voisins, ...
        const NodeSet& neighbours = triaGraph.neighbours ( *cliqueNodesIter );

        for ( NodeSetIterator nodeNeighboorsIter = neighbours.begin();nodeNeighboorsIter != neighbours.end(); ++ nodeNeighboorsIter ) {
          // ... si un de ces voisin n'appartient pas à la clique ...
          if ( ! eliminationOrder.exists ( *nodeNeighboorsIter ) && __IsValidNeighBoor ( *cliqueNodesIter, *nodeNeighboorsIter ) ) {
            SequenceIterator<NodeId> cliqueRemainingNodesIter = cliqueNodesIter;
            ++cliqueRemainingNodesIter;
            bool neighboorsAgreed = true;

            // ... alors on regarde si les autres noeuds de la clique non observés ont ce voisin en commun ...

            while ( cliqueRemainingNodesIter != eliminationOrder.end() && neighboorsAgreed ) {
              neighboorsAgreed = false;
              // ... en vérifiant que ce voisin est dans leur voisinage.
              const NodeSet& remainsNodes = triaGraph.neighbours ( *cliqueRemainingNodesIter );

              for ( NodeSetIterator remainNodeNeighboorsIter = remainsNodes.begin();remainNodeNeighboorsIter != remainsNodes.end(); ++ remainNodeNeighboorsIter ) {
                if ( *remainNodeNeighboorsIter == *nodeNeighboorsIter ) {
                  neighboorsAgreed = true;
                  break;
                }
              }

              ++cliqueRemainingNodesIter;
            }

            if ( neighboorsAgreed )
              ValidIndex = true;
          }

          if ( ValidIndex )
            break;
        }

        if ( !ValidIndex )
          ++cliqueNodesIter;
      }

      // Si on a trouvé un noeud,
      if ( ValidIndex ) {
        // On recherche sa position dans la séquence d'élimination globale
        size_t i = 0;

        while ( i < elim.size() ) {
          if ( *cliqueNodesIter == elim[i] ) {
            try {
              __cliqueEliminationMap.insert ( elim.size() - i, *cliqueIter );
            } catch ( gum::Exception e ) {

            }

            break;
          }

          i++;
        }
      } else {
        __cliqueEliminationMap.insert ( 0, *cliqueIter );
      }
    }
  }

/// Returns true if neighboor is eliminated after current node
  template <typename T_DATA>
  bool
  DefaultInfluenceDiagramInference<T_DATA>::__IsValidNeighBoor ( NodeId currentNode, NodeId neighboorNode ) {
    const std::vector<NodeId> elim = __triangulation->eliminationOrder();
    int i = 0;

    while ( elim[i] != currentNode && elim[i] != neighboorNode )
      i++;

    if ( elim[i] == currentNode )
      return true;

    return false;
  }


  template <typename T_DATA>
  void
  DefaultInfluenceDiagramInference<T_DATA>::displayStrongJunctionTree ( std::ostream &stream ) {
    for ( HashTableConstIterator< size_t, NodeId > strongJunctionTreeIter = __cliqueEliminationMap.begin();
          strongJunctionTreeIter != __cliqueEliminationMap.end(); ++strongJunctionTreeIter ) {
      stream << "Clique " << __triangulation->junctionTree().clique ( *strongJunctionTreeIter ) << " : " << strongJunctionTreeIter.key() << std::endl << std::flush;

    }
  }

  template <typename T_DATA>
  void
  DefaultInfluenceDiagramInference<T_DATA>::__cleanUp() {
    if ( __inferencePotential != 0 ) {
      delete __inferencePotential;
      __inferencePotential = 0;
    }

    if ( __inferenceUtility != 0 ) {
      delete __inferenceUtility;
      __inferenceUtility = 0;
    }

    for ( typename Property< CliqueProperties<T_DATA>* >::onNodes::iterator cliquePropertyIter = __cliquePropertiesMap.begin();
          cliquePropertyIter != __cliquePropertiesMap.end(); ++cliquePropertyIter )
      ( * cliquePropertyIter )->cleanFromInference();

    __utakenDecisionMap.clear();

    __inferenceMade = false;
  }

  template <typename T_DATA>
  void
  DefaultInfluenceDiagramInference<T_DATA>::__collectChild ( NodeId parent, NodeId child ) {
    const NodeSet& neighbours = __triangulation->junctionTree().neighbours ( child );

    for ( NodeSetIterator childIterator = neighbours.begin();childIterator != neighbours.end(); ++childIterator ) {
      if ( *childIterator != parent )
        __collectChild ( child, *childIterator );
    }

    __absorbClique ( child, parent );

  }

// Clique Absorbtion algorithm
  template<typename T_DATA>
  void
  DefaultInfluenceDiagramInference<T_DATA>::__absorbClique ( NodeId absorbedCliqueId, NodeId absorbingCliqueId ) {
    /*std::cout << std::endl << std::endl;
    std::cout << "Clique " << __triangulation->junctionTree().clique( absorbedCliqueId ) << " est absorbé par " << __triangulation->junctionTree().clique( absorbingCliqueId ) << std::endl;
    std::fflush(stdout);*/
    // Recuperation of clique properties
    CliqueProperties<T_DATA>* absorbedClique =  __cliquePropertiesMap[ absorbedCliqueId ];

    // Get the nodes making the separtion between the absorbed clique and the absorbing one
    NodeSet separator = __getSeparator ( absorbedCliqueId, absorbingCliqueId );
    /*std::cout << "Separator : " << separator << std::endl;
    std::fflush(stdout);*/

    Potential<T_DATA>* potentialMarginal = 0;
    UtilityTable<T_DATA>* utilityMarginal = 0;

    // ==================================================================================================================
    // To realize the absorption, we have to eliminate each variable contained in the clique wich is not on the separator
    // ==================================================================================================================
    __reduceClique ( absorbedClique, separator, potentialMarginal, utilityMarginal );

    __cliquePropertiesMap[ absorbingCliqueId ]->addPotential ( *potentialMarginal, true );

    Instantiation utilityMarginalInst ( utilityMarginal );

    for ( utilityMarginalInst.setFirst(); not utilityMarginalInst.end(); utilityMarginalInst.inc() ) {
      T_DATA uVal = ( T_DATA ) 0;

      if ( potentialMarginal->get ( utilityMarginalInst ) != ( T_DATA ) 0 )
        uVal = utilityMarginal->get ( utilityMarginalInst ) / potentialMarginal->get ( utilityMarginalInst );
    }

    __cliquePropertiesMap[ absorbingCliqueId ]->addUtility ( *utilityMarginal, true );
  }

// Reduces a clqiue
  template <typename T_DATA> INLINE
  void
  DefaultInfluenceDiagramInference<T_DATA>::__reduceClique ( CliqueProperties<T_DATA>* absorbedClique, NodeSet& separator,
      Potential<T_DATA>*& potentialMarginal, UtilityTable<T_DATA>*& utilityMarginal ) {

    Instantiation cliqueInstance ( absorbedClique->cliqueInstantiation() );
    Sequence<const DiscreteVariable*> cliqueVarList ( absorbedClique->cliqueVariables() );

    for ( SequenceIterator<NodeId> eliminationOrderIter = absorbedClique->cliqueEliminationOrder().begin();
          eliminationOrderIter != absorbedClique->cliqueEliminationOrder().end(); ++eliminationOrderIter ) {
      /*std::cout << "Node elimination : " << *eliminationOrderIter << std::endl;
      std::fflush(stdout);*/
      if ( ! separator.contains ( *eliminationOrderIter ) ) {
        //=================================================================================

        // First we create the potentials that will result from elimination of variable
        Potential<T_DATA>* potentialTemp = new Potential<T_DATA>();
        UtilityTable<T_DATA>* utilityTemp = new UtilityTable<T_DATA>();

        // This potential will have all none yet eliminated variable except the current one
        const DiscreteVariable* currentVar = & ( this->getInfluenceDiagram().variable ( *eliminationOrderIter ) );
        SequenceIterator<const DiscreteVariable*> eraseVarIter = cliqueVarList.begin();

        while ( eraseVarIter != cliqueVarList.end() ) {
          if ( **eraseVarIter == *currentVar )
            break;

          ++eraseVarIter;
        }

        cliqueVarList.erase ( eraseVarIter );

        for ( SequenceIterator<const DiscreteVariable*> cliqueVarListIter = cliqueVarList.begin();
              cliqueVarListIter != cliqueVarList.end(); ++cliqueVarListIter ) {
          /*std::cout << "Ajout de var :" << **cliqueVarListIter << std::endl;
          std::fflush(stdout);*/

          potentialTemp->add ( **cliqueVarListIter );
          utilityTemp->add ( **cliqueVarListIter );
        }

        // And we add an instanciation to that potential
        Instantiation potentialInst ( potentialTemp );

        //=====================================================================================

        // Then we fill the new potential by marginalizing the ancient one
        for ( potentialInst.setFirst(); not potentialInst.end(); potentialInst.inc() ) {
          T_DATA potentialValue = ( T_DATA ) 0;
          T_DATA utilityValue = ( T_DATA ) 0;
          cliqueInstance.chgValIn ( potentialInst );

          for ( cliqueInstance.setFirstOut ( potentialInst ); not cliqueInstance.end(); cliqueInstance.incOut ( potentialInst ) ) {
            // Récupération de la valeur de la cpt de la clique pour cette instance
            T_DATA currentPotential = ( T_DATA ) 1;

            if ( potentialMarginal == 0 ) {
              // If there's no ancient potential it means that we haven't yet compute him
              for ( HashTableConstIterator<const Potential<T_DATA>*, Instantiation*> potentialIter = absorbedClique->potentialBucket().begin();
                    potentialIter != absorbedClique->potentialBucket().end(); ++potentialIter ) {
                ( *potentialIter )->chgValIn ( cliqueInstance );
                currentPotential *= potentialIter.key()->get ( **potentialIter );
              }
            } else {
              Instantiation potentialMarginalInst ( potentialMarginal );
              potentialMarginalInst.chgValIn ( cliqueInstance );
              currentPotential = potentialMarginal->get ( potentialMarginalInst );
            }

            // Récupération de la valeur d'utilité de la clique pour cette instance
            T_DATA currentUtility = ( T_DATA ) 0;

            if ( utilityMarginal == 0 ) {
              for ( HashTableConstIterator<const UtilityTable<T_DATA>*, Instantiation*> utilityIter = absorbedClique->utilityBucket().begin();
                    utilityIter != absorbedClique->utilityBucket().end(); ++utilityIter ) {
                ( *utilityIter )->chgValIn ( cliqueInstance );
                currentUtility += utilityIter.key()->get ( **utilityIter );
              }

              currentUtility *= currentPotential;
            } else {
              Instantiation utilityMarginalInst ( utilityMarginal );
              utilityMarginalInst.chgValIn ( cliqueInstance );
              currentUtility = utilityMarginal->get ( utilityMarginalInst );
            }

            // Marginalization
            if ( this->getInfluenceDiagram().isDecisionNode ( *eliminationOrderIter ) ) {
              if ( potentialValue < currentPotential ) {
                potentialValue = currentPotential;
              }

              if ( utilityValue < currentUtility ) {
                utilityValue = currentUtility;

                if ( __utakenDecisionMap.exists ( *eliminationOrderIter ) )
                  __utakenDecisionMap.erase ( *eliminationOrderIter );

                __utakenDecisionMap.insert ( *eliminationOrderIter,
                                             cliqueInstance.val ( this->getInfluenceDiagram().variable ( *eliminationOrderIter ) ) );
              }
            } else {
              potentialValue += currentPotential;
              utilityValue += currentUtility;
            }

          }

          potentialTemp->set ( potentialInst, potentialValue );

          utilityTemp->set ( potentialInst, utilityValue );
        }

        cliqueInstance.erase ( this->getInfluenceDiagram().variable ( *eliminationOrderIter ) );

        /*std::cout << "Potential : " << *potentialTemp << std::endl;
        std::fflush(stdout);
        std::cout << "Utility : " << *utilityTemp << std::endl;
        std::fflush(stdout);*/

        if ( potentialMarginal != 0 )
          delete potentialMarginal;

        potentialMarginal = potentialTemp;

        if ( utilityMarginal != 0 )
          delete utilityMarginal;

        utilityMarginal = utilityTemp;
      }
    }
  }

// @param cliqueId The clique for which the dummy bucket is made.
// @return A pointer over the dummy bucket.
  template <typename T_DATA> INLINE
  Potential<T_DATA>*
  DefaultInfluenceDiagramInference<T_DATA>::__makeDummyPotential ( NodeId cliqueId ) {
    Potential<T_DATA>* pot = new Potential<T_DATA> ( new MultiDimSparse<T_DATA> ( ( T_DATA ) 1 ) );
    __potentialDummies.insert ( pot );

    for ( NodeSet::const_iterator cliqueNodesIter = __triangulation->junctionTree().clique ( cliqueId ).begin();
          cliqueNodesIter != __triangulation->junctionTree().clique ( cliqueId ).end(); ++cliqueNodesIter )
      pot->add ( this->getInfluenceDiagram().variable ( *cliqueNodesIter ) );

    return pot;
  }

// @param cliqueId The clique for which the dummy bucket is made.
// @return A pointer over the dummy bucket.
  template <typename T_DATA> INLINE
  UtilityTable<T_DATA>*
  DefaultInfluenceDiagramInference<T_DATA>::__makeDummyUtility ( NodeId cliqueId ) {
    UtilityTable<T_DATA>* ut = new UtilityTable<T_DATA> ( new MultiDimSparse<T_DATA> ( ( T_DATA ) 0 ) );
    __utilityDummies.insert ( ut );

    for ( NodeSet::const_iterator cliqueNodesIter = __triangulation->junctionTree().clique ( cliqueId ).begin();
          cliqueNodesIter != __triangulation->junctionTree().clique ( cliqueId ).end(); ++cliqueNodesIter )
      ut->add ( this->getInfluenceDiagram().variable ( *cliqueNodesIter ) );

    return ut;
  }

// ============================================================================
//                                CLIQUEPROP
// ============================================================================

// Default constructor.
// @param id the id of this clique on which this properties holds.
  template <typename T_DATA>
  CliqueProperties<T_DATA>::CliqueProperties() {
    GUM_CONSTRUCTOR ( CliqueProperties );
  }

// Destructor.
  template <typename T_DATA>
  CliqueProperties<T_DATA>::~CliqueProperties() {
    GUM_DESTRUCTOR ( CliqueProperties );

    cleanFromInference();

    removeAllEvidence();

    for ( HashTableIterator<const Potential<T_DATA>*, Instantiation*> potentialIter = __potentialBucket.begin();
          potentialIter != __potentialBucket.end(); ++potentialIter )
      delete *potentialIter;

    for ( HashTableIterator<const UtilityTable<T_DATA>*, Instantiation*> utilityIter = __utilityBucket.begin();
          utilityIter != __utilityBucket.end(); ++utilityIter )
      delete *utilityIter;
  }

// Add a variable to this clique
// @param v The added variable.
  template <typename T_DATA>
  void
  CliqueProperties<T_DATA>::addVariable ( const DiscreteVariable& v ) {
    try {
      __allVarsInst.add ( v );
    } catch ( DuplicateElement& e ) {
      // Nothing to do then!
    }
  }

  template <typename T_DATA>
  void
  CliqueProperties<T_DATA>::makeEliminationOrder ( const std::vector<NodeId>& elim, const InfluenceDiagram<T_DATA>& infDiag ) {
    for ( size_t i = 0; i < elim.size(); i++ ) {
      if ( __allVarsInst.contains ( infDiag.variable ( elim[i] ) ) )
        __eliminationOrder.insert ( elim[i] );
    }
  }

// Add a Potential to this clique
// @param cpt The added CPT.
  template <typename T_DATA>
  void
  CliqueProperties<T_DATA>::addPotential ( const Potential<T_DATA>& cpt, bool removable ) {
    __potentialBucket.insert ( &cpt, new Instantiation ( cpt ) );

    if ( removable ) __removablePotentialList.insert ( &cpt );

    for ( Sequence< const DiscreteVariable * >::iterator iter = cpt.begin(); iter != cpt.end(); ++iter ) {
      if ( removable && !__allVarsInst.contains ( **iter ) )
        __removableVarList.insert ( *iter );

      addVariable ( **iter );
    }
  }

// Add a Potential to this clique
// @param cpt The added CPT.
  template <typename T_DATA>
  void
  CliqueProperties<T_DATA>::addUtility ( const UtilityTable<T_DATA>& ut, bool removable ) {
    try {
      __utilityBucket.insert ( &ut, new Instantiation ( ut ) );

      if ( removable ) __removableUtilityList.insert ( &ut );

      for ( Sequence< const DiscreteVariable * >::iterator iter = ut.begin(); iter != ut.end(); ++iter ) {
        if ( removable && !__allVarsInst.contains ( **iter ) )
          __removableVarList.insert ( *iter );

        addVariable ( **iter );
      }
    } catch ( DuplicateElement& e ) {
      // Nothing to do then!
    }
  }

// Cleans the clique after an inference
  template <typename T_DATA>
  void
  CliqueProperties<T_DATA>::cleanFromInference() {
    for ( ListIterator<const DiscreteVariable*> removedVarIter =  __removableVarList.begin(); removedVarIter !=  __removableVarList.end(); ++removedVarIter ) {
      __allVarsInst.erase ( **removedVarIter );
      __removableVarList.erase ( removedVarIter );
    }

    for ( ListIterator<const Potential<T_DATA>* > removedPotIter =  __removablePotentialList.begin();
          removedPotIter !=  __removablePotentialList.end(); ++removedPotIter ) {
      delete __potentialBucket[ *removedPotIter ];
      __potentialBucket.erase ( *removedPotIter );
      delete *removedPotIter;
      __removablePotentialList.erase ( removedPotIter );
    }

    for ( ListIterator<const UtilityTable<T_DATA>* > removedUtIter =  __removableUtilityList.begin();
          removedUtIter !=  __removableUtilityList.end(); ++removedUtIter ) {
      delete __utilityBucket[ *removedUtIter ];
      __utilityBucket.erase ( *removedUtIter );
      delete *removedUtIter;
      __removableUtilityList.erase ( removedUtIter );
    }
  }


// Add an evidence on a variable in this clique.
// @throw NotFound Raised if the evidence is on a variable not present
//                 in this clique.
  template <typename T_DATA>
  void
  CliqueProperties<T_DATA>::addEvidence ( const Potential<T_DATA>& evidence ) {
    // To avoid interference
    cleanFromInference();

    if ( evidence.variablesSequence().size() != 1 ) {
      std::stringstream msg;
      msg << ": expected evidence on 1 variable, found on ";
      msg  << evidence.variablesSequence().size();
      GUM_ERROR ( OperationNotAllowed, msg.str() );
    }

    if ( ! __allVarsInst.contains ( evidence.variablesSequence().atPos ( 0 ) ) ) {
      std::stringstream msg;
      msg << ": " << evidence.variablesSequence().atPos ( 0 )->name();
      msg << " not found in clique ";
      GUM_ERROR ( NotFound, msg.str() );
    }

    __evidences.insert ( evidence.variablesSequence().atPos ( 0 ) , &evidence );

    __potentialBucket.insert ( &evidence, new Instantiation ( evidence ) );
  }

// Removes all the evidences containing v
  template <typename T_DATA> INLINE
  void
  CliqueProperties<T_DATA>::removeEvidence ( const DiscreteVariable& v ) {
    const Potential<T_DATA>* evi = __evidences[ &v ];

    delete __potentialBucket[ evi ];
    __potentialBucket.erase ( evi );
    __evidences.erase ( &v );
  }

// Remove all the evidences
  template <typename T_DATA> INLINE
  void
  CliqueProperties<T_DATA>::removeAllEvidence() {
    for ( HashTableIterator< const DiscreteVariable*, const Potential<T_DATA>*  > evidencesIter = __evidences.begin();
          evidencesIter != __evidences.end(); ++evidencesIter )
      removeEvidence ( * ( evidencesIter.key() ) );

    __evidences.clear();
  }

// @return Returns the mapping of evidences on the variables in this clique.
  template <typename T_DATA> INLINE
  const HashTable<const DiscreteVariable*, const Potential<T_DATA>* >&
  CliqueProperties<T_DATA>::evidences() const {
    return __evidences;
  }

// @return Returns the bucket of this Clique
  template <typename T_DATA> INLINE
  const HashTable<const Potential<T_DATA>*, Instantiation* >&
  CliqueProperties<T_DATA>::potentialBucket() {
    return __potentialBucket;
  }

// @return Returns the bucket of this Clique
  template <typename T_DATA> INLINE
  const HashTable<const UtilityTable<T_DATA>*, Instantiation* >&
  CliqueProperties<T_DATA>::utilityBucket() {
    return __utilityBucket;
  }

// @return returns the elimination sequence for this clique
  template <typename T_DATA> INLINE
  const Sequence<NodeId>&
  CliqueProperties<T_DATA>::cliqueEliminationOrder() {
    return __eliminationOrder;
  }

// @return returns List containing all variables contained in this clique
  template <typename T_DATA> INLINE
  const Sequence< const DiscreteVariable*>&
  CliqueProperties<T_DATA>::cliqueVariables() {
    return __allVarsInst.variablesSequence();
  }

// @return returns instanciation on variable within this clique
  template <typename T_DATA> INLINE
  Instantiation&
  CliqueProperties<T_DATA>::cliqueInstantiation() {
    return __allVarsInst;
  }

// ============================================================================
} /* namespace gum */

// ============================================================================
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
// ============================================================================
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;    replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
