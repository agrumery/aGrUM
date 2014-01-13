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
*  @note For deep understanding of the inherent algorithms put in motion here
*  a strong look at "From Influence Diagrams To Junction Trees, Frank Jensen, Finn V. Jensen, Soren L; Dittmer, 1994" is
*  highly recommended.
*  @note __triangulation->eliminationOrder() gives nodes in order in which they disappear, meaning that the first one
*  is the first one to be eliminated.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS


// to ease parsing by IDE
#include <agrum/ID/inference/defaultInfluenceDiagramInference.h>

namespace gum {
  /* **************************************************************************************************** **/
  /* **                                                                                                                                                    **/
  /* **                                                   Constructor & Destructor                                                         **/
  /* **                                                                                                                                                    **/
  /* **************************************************************************************************** **/


  // Default constructor.
  // @param inDiag The influence diagram on which we perform inferences

  template<typename GUM_SCALAR>
  DefaultInfluenceDiagramInference<GUM_SCALAR>::DefaultInfluenceDiagramInference ( const InfluenceDiagram<GUM_SCALAR>& infDiag ) :
    InfluenceDiagramInference<GUM_SCALAR> ( infDiag ),
    __triangulation ( 0 ),
    __inferencePotential ( 0 ),
    __inferenceUtility ( 0 ),
    __inferenceMade ( false ) {

    GUM_CONSTRUCTOR ( DefaultInfluenceDiagramInference );

    //Make modalities map
    NodeProperty<Size> __modalitiesMap;

    // the moral graph does not include utility nodes
    UndiGraph partialMoralGraph ( this->influenceDiagram().moralGraph() );

    for(const auto iter : this->influenceDiagram().nodes()) {
      if ( this->influenceDiagram().isUtilityNode ( iter ) ) {
        partialMoralGraph.eraseNode ( iter );
      } else {
        __modalitiesMap.insert ( iter,  this->influenceDiagram().variable ( iter ).domainSize() );
      }
    }

    const List<NodeSet>* partialTemporalOrder = & ( this->influenceDiagram().getPartialTemporalOrder() );

    //Make Junction Tree
    __triangulation = new PartialOrderedTriangulation ( &partialMoralGraph, &__modalitiesMap, partialTemporalOrder );
    __triangulation->junctionTree();
    __makeCliquePropertiesMap();
    __makeStrongJunctionTree();
  }


  // Default destructor.

  template<typename GUM_SCALAR>
  DefaultInfluenceDiagramInference<GUM_SCALAR>::~DefaultInfluenceDiagramInference() {

    GUM_DESTRUCTOR ( DefaultInfluenceDiagramInference );

    delete __triangulation;

    __cleanUp();

    for ( typename Property< CliqueProperties<GUM_SCALAR>* >::onNodes::iterator_safe cliquePropertyIter = __cliquePropertiesMap.beginSafe();
          cliquePropertyIter != __cliquePropertiesMap.endSafe(); ++cliquePropertyIter )
      delete cliquePropertyIter.val();

    for ( SetIteratorSafe< Potential<GUM_SCALAR>* > potentialDummiesIter = __potentialDummies.beginSafe();
          potentialDummiesIter != __potentialDummies.endSafe(); ++potentialDummiesIter )
      delete *potentialDummiesIter;

    for ( SetIteratorSafe< UtilityTable<GUM_SCALAR>* > utilityDummiesIter = __utilityDummies.beginSafe();
          utilityDummiesIter != __utilityDummies.endSafe(); ++utilityDummiesIter )
      delete *utilityDummiesIter;
  }



  /* **************************************************************************************************** **/
  /* **                                                                                                                                                    **/
  /* **                                                                  Inference                                                                   **/
  /* **                                                                                                                                                    **/
  /* **************************************************************************************************** **/


  // makeInference : Performs the inference

  template <typename GUM_SCALAR>
  void
  DefaultInfluenceDiagramInference<GUM_SCALAR>::makeInference() {

    __cleanUp();

    NodeId rootClique = __cliqueEliminationMap[0];
    const NodeSet& neighbours = __triangulation->junctionTree().neighbours ( rootClique );

    for ( NodeSetIterator childIterator = neighbours.beginSafe(); childIterator != neighbours.endSafe(); ++childIterator )
      __collectChild ( rootClique, *childIterator );

    NodeSet separator;

    __reduceClique ( __cliquePropertiesMap[ rootClique ], separator, __inferencePotential, __inferenceUtility );

    __inferenceMade = true;

  }


  // getMEU : Returns the maximum expected utility for this inference on this diagram

  template <typename GUM_SCALAR> INLINE
  GUM_SCALAR
  DefaultInfluenceDiagramInference<GUM_SCALAR>::getMEU() {

    if ( !__inferenceMade )
      GUM_ERROR ( OperationNotAllowed, "No inference have yet been made" );

    Instantiation res ( __inferenceUtility );
    return __inferenceUtility->get ( res );
  }


  // getBestDecisionChoice : Returns for given decision node the best decision to take under this infernece

  template <typename GUM_SCALAR> INLINE
  Idx
  DefaultInfluenceDiagramInference<GUM_SCALAR>::getBestDecisionChoice ( NodeId decisionId ) {

    if ( !__inferenceMade )
      GUM_ERROR ( OperationNotAllowed, "No inference have yet been made" );

    if ( !this->influenceDiagram().isDecisionNode ( decisionId ) )
      GUM_ERROR ( InvalidNode, "Node is not a decision node" );

    return __utakenDecisionMap[ decisionId ];

  }


  // displayResult : displays results obtained from inference

  template <typename GUM_SCALAR>
  void
  DefaultInfluenceDiagramInference<GUM_SCALAR>::displayResult ( std::ostream& stream ) {

    if ( !__inferenceMade )
      GUM_ERROR ( OperationNotAllowed, "No inference have yet been made" );

    stream << "Espérance d'utilité finale : " << std::endl << std::flush;
    stream << *__inferenceUtility << std::endl;

    stream << "Choix maximisant l'espérance d'utilité : " << std::endl << std::flush;

    for ( HashTableConstIteratorSafe< NodeId, Idx > utilityIter = __utakenDecisionMap.beginSafe(); utilityIter != __utakenDecisionMap.endSafe(); ++utilityIter )
      stream << "Decision " << this->influenceDiagram().variable ( utilityIter.key() ) << " : " << this->influenceDiagram().variable ( utilityIter.key() ).label ( utilityIter.val() ) << std::endl;

  }



  /* **************************************************************************************************** **/
  /* **                                                                                                                                                    **/
  /* **                                               Evidence management                                                                 **/
  /* **                                                                                                                                                    **/
  /* **************************************************************************************************** **/


  // insertEvidence : inserts new evidence in the graph

  template<typename GUM_SCALAR>
  void
  DefaultInfluenceDiagramInference<GUM_SCALAR>::insertEvidence ( const List<const Potential<GUM_SCALAR>*>& evidenceList ) {

    for ( ListConstIteratorSafe<const Potential<GUM_SCALAR>*> evidenceListIter = evidenceList.cbeginSafe(); evidenceListIter != evidenceList.cendSafe(); ++evidenceListIter )
      __cliquePropertiesMap[ __nodeToCliqueMap[ this->influenceDiagram().nodeId ( ( *evidenceListIter )->variable ( 0 ) ) ] ]->addEvidence ( **evidenceListIter );

  }


  // eraseEvidence : removes a given evidence from the graph

  template <typename GUM_SCALAR>
  void
  DefaultInfluenceDiagramInference<GUM_SCALAR>::eraseEvidence ( const Potential<GUM_SCALAR>* evidence ) {

    if ( not ( evidence->variablesSequence().size() != 1 ) )
      __cliquePropertiesMap[ __nodeToCliqueMap[ this->influenceDiagram().nodeId ( evidence->variable ( 0 ) ) ] ]->removeEvidence ( evidence->variable ( 0 ) );

  }


  // eraseAllEvidence : removes all evidence from the graph

  template <typename GUM_SCALAR>
  void
  DefaultInfluenceDiagramInference<GUM_SCALAR>::eraseAllEvidence() {

    for ( typename Property< CliqueProperties<GUM_SCALAR>* >::onNodes::iterator_safe nodeIter = __cliquePropertiesMap.beginSafe(); nodeIter != __cliquePropertiesMap.endSafe(); ++nodeIter )
      ( nodeIter.val() )->removeAllEvidence();

  }



  /* **************************************************************************************************** **/
  /* **                                                                                                                                                    **/
  /* **                                                                Getters                                                                        **/
  /* **                                                                                                                                                    **/
  /* **************************************************************************************************** **/


  // getTriangulation : returns the triangulation obtained for this influence diagram

  template <typename GUM_SCALAR> INLINE
  Triangulation&
  DefaultInfluenceDiagramInference<GUM_SCALAR>::getTriangulation() {

    return *__triangulation;

  }


  // __getSeparator :: returns the set of node contains in clique1 inter clique2

  template<typename GUM_SCALAR> INLINE
  const NodeSet&
  DefaultInfluenceDiagramInference<GUM_SCALAR>::__getSeparator ( NodeId clique_1, NodeId clique_2 ) {

    return __triangulation->junctionTree().separator ( clique_1, clique_2 );

  }


  // __getClique : for a given node in diagram, returns the clique id  of the first variable to disappear between node variable and its parents variables

  template<typename GUM_SCALAR>
  NodeId
  DefaultInfluenceDiagramInference<GUM_SCALAR>::__getClique ( const std::vector<NodeId>& eliminationOrder, NodeId id ) {


    //***********************************************************************************************************************************
    // First, we create a node set with node id and parents id
    NodeSet idSet;
    idSet.insert ( id );

    const NodeSet& parents = this->influenceDiagram().dag().parents ( id );

    for ( NodeSet::const_iterator_safe parentsIter = parents.beginSafe(); parentsIter != parents.endSafe(); ++parentsIter )
      idSet.insert ( *parentsIter );

    //***********************************************************************************************************************************

    //***********************************************************************************************************************************
    // Then, we search for the first one to be eliminated in elimination order
    for ( size_t i = 0; i < eliminationOrder.size(); ++i )
      if ( idSet.contains ( eliminationOrder[i] ) )
        return __triangulation->createdJunctionTreeClique ( eliminationOrder[i] );

    GUM_ERROR ( FatalError, "No clique found for node " << id );
    //***********************************************************************************************************************************

  }


  // __makeCliquePropertiesMap : Builds up clique data structure for the inference

  template<typename GUM_SCALAR>
  void
  DefaultInfluenceDiagramInference<GUM_SCALAR>::__makeCliquePropertiesMap() {

    const std::vector<NodeId> elim = __triangulation->eliminationOrder();

    // Those two sets will contains cliques id for cliques which doesn't have a potential or a utility table at all
    NodeSet potentialsCliquesSet, utilitiesCliqueSet;

    //***********************************************************************************************************************************
    // First pass to create the clique's table
    for ( const auto cli : __triangulation->junctionTree().nodes() ) {
      __cliquePropertiesMap.insert ( cli, new CliqueProperties<GUM_SCALAR>() );

      potentialsCliquesSet.insert ( cli );
      utilitiesCliqueSet.insert ( cli );

      // Insertion in clique properties of the variables contains in the clique
      for ( NodeSetIterator cliqueNodesIter = __triangulation->junctionTree().clique ( cli ).beginSafe();
            cliqueNodesIter != __triangulation->junctionTree().clique ( cli ).endSafe(); ++cliqueNodesIter )
        __cliquePropertiesMap[cli]->addVariable ( this->influenceDiagram().variable ( *cliqueNodesIter ) );

      // Creation of clique own elimination order (based on the general one)
      __cliquePropertiesMap[cli]->makeEliminationOrder ( elim, this->influenceDiagram() );
    }

    //***********************************************************************************************************************************

    //***********************************************************************************************************************************
    // Second pass to add the potentials into good cliques
    for ( size_t i = 0; i < elim.size(); i++ ) {

      // Récupération de la bonne clique
      NodeId cliqueId = __getClique ( elim, elim[i] );
      __nodeToCliqueMap.insert ( elim[i], cliqueId );

      // Ajout de la cpt si le noeud est un noeud chance
      if ( this->influenceDiagram().isChanceNode ( elim[i] ) ) {
        __cliquePropertiesMap[cliqueId]->addPotential ( this->influenceDiagram().cpt ( elim[i] ) );
        potentialsCliquesSet.erase ( cliqueId );
      }
    }

    //***********************************************************************************************************************************

    //***********************************************************************************************************************************
    // Third pass to fill empty cliques with "one" matrices for potentials and "zero" matrices for utilities.
    for ( NodeSetIterator potentialCliqueIter = potentialsCliquesSet.beginSafe(); potentialCliqueIter != potentialsCliquesSet.endSafe(); ++potentialCliqueIter )
      __cliquePropertiesMap[ *potentialCliqueIter ]->addPotential ( *__makeDummyPotential ( *potentialCliqueIter ) );

    //***********************************************************************************************************************************

    //***********************************************************************************************************************************
    // Fourth pass to adress utility table to the good clique
    // We go trought all diagram's nodes in search of utility nodes since they do not appear in elimination order
    for(const auto nodesIter : this->influenceDiagram().nodes()) 
      if ( this->influenceDiagram().isUtilityNode ( nodesIter ) ) {
        // Récupération de la bonne clique
        NodeId cliqueId = __getClique ( elim, nodesIter );
        __cliquePropertiesMap[cliqueId]->addUtility ( this->influenceDiagram().utility ( nodesIter ) );
        utilitiesCliqueSet.erase ( cliqueId );
      }

    //***********************************************************************************************************************************

    //***********************************************************************************************************************************
    // Fifth pass to fill empty cliques with "zero" matrices for utilities.
    for ( NodeSetIterator utilityCliqueIter = utilitiesCliqueSet.beginSafe(); utilityCliqueIter != utilitiesCliqueSet.endSafe(); ++utilityCliqueIter )
      __cliquePropertiesMap[ *utilityCliqueIter ]->addUtility ( *__makeDummyUtility ( *utilityCliqueIter ) );

    //***********************************************************************************************************************************

  }


  // __makeStrongJunctionTree : actually, the main interest of this function is to find the strong root from which make the recursive
  // __collectChild call.

  template<typename GUM_SCALAR>
  void
  DefaultInfluenceDiagramInference<GUM_SCALAR>::__makeStrongJunctionTree() {

    // Pour chaque clique
    for ( const auto cli : __triangulation->junctionTree().nodes() ) {

      Sequence<NodeId> eliminationOrder = __cliquePropertiesMap[ cli]->cliqueEliminationOrder();
      SequenceIteratorSafe<NodeId> cliqueNodesIter =  eliminationOrder.beginSafe();
      bool validIndex = false;

      // On parcours chaque noeud de la clique par ordre d'élimination, ...
      while ( cliqueNodesIter != eliminationOrder.endSafe() && !validIndex ) {

        SequenceIteratorSafe<NodeId> cliqueRemainingNodesIter = cliqueNodesIter;
        ++ cliqueRemainingNodesIter;

        if ( cliqueRemainingNodesIter  != eliminationOrder.endSafe() ) {

          NodeSet suspectedNodes ( __triangulation->triangulatedGraph().neighbours ( *cliqueRemainingNodesIter ) );

          while ( cliqueRemainingNodesIter != eliminationOrder.endSafe() && ! suspectedNodes.empty() ) {
            NodeSet possiblesNodes ( suspectedNodes );

            for ( NodeSetIterator possibleNodesIter = possiblesNodes.beginSafe(); possibleNodesIter != possiblesNodes.endSafe(); ++possibleNodesIter )
              if ( !__triangulation->triangulatedGraph().neighbours ( *cliqueRemainingNodesIter ).exists ( *possibleNodesIter ) )
                suspectedNodes.erase ( *possibleNodesIter );

            ++cliqueRemainingNodesIter;
          }

          if ( ! suspectedNodes.empty() )
            for ( NodeSetIterator suspectedNodesIter = suspectedNodes.beginSafe(); suspectedNodesIter != suspectedNodes.endSafe(); ++suspectedNodesIter )
              if ( ! eliminationOrder.exists ( *suspectedNodesIter ) && __IsEliminatedAfter ( *suspectedNodesIter, *cliqueNodesIter ) ) {
                validIndex = true;
                break;
              }
        }

        if ( !validIndex )
          ++cliqueNodesIter;
      }

      if ( validIndex ) {
        Idx index = 1;

        for ( std::vector<NodeId>::const_iterator eliminationOrderIter = __triangulation->eliminationOrder().begin();
              eliminationOrderIter != __triangulation->eliminationOrder().end() && *eliminationOrderIter != *cliqueNodesIter; ++eliminationOrderIter, ++index );

        __cliqueEliminationMap.insert ( __triangulation->eliminationOrder().size() - index, cli );

      } else
        try {
          __cliqueEliminationMap.insert ( 0, cli );
        } catch ( Exception e ) {
          throw ( e );
        }
    }

  }




  // __IsEliminatedAfter :   checks if  observed  node is eliminated after current node.

  template <typename GUM_SCALAR>
  bool
  DefaultInfluenceDiagramInference<GUM_SCALAR>::__IsEliminatedAfter ( NodeId observedNode, NodeId currentNode ) {

    for ( std::vector<NodeId>::const_iterator eliminationOrderIter = __triangulation->eliminationOrder().begin();
          eliminationOrderIter != __triangulation->eliminationOrder().end(); ++eliminationOrderIter ) {
      if ( *eliminationOrderIter == currentNode )
        return true;

      if ( *eliminationOrderIter == observedNode )
        return false;
    }

    return false;
  }




  // displayStrongJunctionTree : displays the junction tree obtained for this influence diagram

  template <typename GUM_SCALAR>
  void
  DefaultInfluenceDiagramInference<GUM_SCALAR>::displayStrongJunctionTree ( std::ostream& stream ) {
    stream << std::endl << "Strong junction tree map : " << std::endl;

    for ( HashTableConstIteratorSafe< size_t, NodeId > strongJunctionTreeIter = __cliqueEliminationMap.beginSafe();
          strongJunctionTreeIter != __cliqueEliminationMap.endSafe(); ++strongJunctionTreeIter )
      stream << "Clique  : " << __triangulation->junctionTree().clique ( strongJunctionTreeIter.val() ) << " - Index : " << strongJunctionTreeIter.key() << std::endl;
  }




  //  __cleanUp : removes all data computed for previous inference

  template <typename GUM_SCALAR>
  void
  DefaultInfluenceDiagramInference<GUM_SCALAR>::__cleanUp() {
    if ( __inferencePotential != 0 ) {
      delete __inferencePotential;
      __inferencePotential = 0;
    }

    if ( __inferenceUtility != 0 ) {
      delete __inferenceUtility;
      __inferenceUtility = 0;
    }

    for ( typename Property< CliqueProperties<GUM_SCALAR>* >::onNodes::iterator_safe cliquePropertyIter = __cliquePropertiesMap.beginSafe();
          cliquePropertyIter != __cliquePropertiesMap.endSafe(); ++cliquePropertyIter )
      ( cliquePropertyIter.val() )->cleanFromInference();

    __utakenDecisionMap.clear();

    __inferenceMade = false;
  }




  //  __collectChild : or when the parents collects message from its child

  template <typename GUM_SCALAR>
  void
  DefaultInfluenceDiagramInference<GUM_SCALAR>::__collectChild ( NodeId parent, NodeId child ) {

    const NodeSet& neighbours = __triangulation->junctionTree().neighbours ( child );

    for ( NodeSetIterator childIterator = neighbours.beginSafe(); childIterator != neighbours.endSafe(); ++childIterator )
      if ( *childIterator != parent )
        __collectChild ( child, *childIterator );

    __absorbClique ( child, parent );

  }


  //  __absorbClique : Performs a clique absorption by another one

  template<typename GUM_SCALAR>
  void
  DefaultInfluenceDiagramInference<GUM_SCALAR>::__absorbClique ( NodeId absorbedCliqueId, NodeId absorbingCliqueId ) {

    // Recuperation of absorbed clique properties
    CliqueProperties<GUM_SCALAR>* absorbedClique =  __cliquePropertiesMap[ absorbedCliqueId ];

    // Get the nodes making the separtion between the absorbed clique and the absorbing one
    NodeSet separator = __getSeparator ( absorbedCliqueId, absorbingCliqueId );

    Potential<GUM_SCALAR>* potentialMarginal = 0;
    UtilityTable<GUM_SCALAR>* utilityMarginal = 0;

    // First  we reduce absorbed clique by eleminating clique variables which aren't in the separator.
    // The aim of this operation is to obtained by marginalization a potential and a utility over the variable presents
    // in the separator (and only those one ). Those tables represent the "messages" sent by child
    // clique to its parent.
    __reduceClique ( absorbedClique, separator, potentialMarginal, utilityMarginal );

    // Then those tables are add in parents clique property.
    // For the potential, we just add it
    __cliquePropertiesMap[ absorbingCliqueId ]->addPotential ( *potentialMarginal, true );

    // For  the utility table, we need first to divide it by the potential
    Instantiation utilityMarginalInst ( utilityMarginal );

    for ( utilityMarginalInst.setFirst(); not utilityMarginalInst.end(); utilityMarginalInst.inc() ) {

      GUM_SCALAR uVal = ( GUM_SCALAR ) 0;

      if ( potentialMarginal->get ( utilityMarginalInst ) != ( GUM_SCALAR ) 0 )
        uVal = utilityMarginal->get ( utilityMarginalInst ) / potentialMarginal->get ( utilityMarginalInst );

      utilityMarginal->set ( utilityMarginalInst, uVal );
    }

    // And then we can add the utility table.
    __cliquePropertiesMap[ absorbingCliqueId ]->addUtility ( *utilityMarginal, true );
  }


  // __reduceClique : Performs a clique reduction

  // This operations consists in eliminating each variable of the clique wich are not in the separtor between this clique and its parent clique.
  // Variable elimination is done by performing the correct marging operation on clique potential. That operation is determined by the nature
  // of currently eleminated variable : if its a chance variable, we sum over its modalities, if its a decision node we maximise over its modalities.
  template <typename GUM_SCALAR> INLINE
  void
  DefaultInfluenceDiagramInference<GUM_SCALAR>::__reduceClique ( CliqueProperties<GUM_SCALAR>* absorbedClique, NodeSet& separator,
      Potential<GUM_SCALAR>*& potentialMarginal, UtilityTable<GUM_SCALAR>*& utilityMarginal ) {

    Instantiation cliqueInstance ( absorbedClique->cliqueInstantiation() );
    Sequence< const DiscreteVariable* > cliqueRemainVarList ( cliqueInstance.variablesSequence() );

    // So for each variable of that clique ...
    for ( SequenceIteratorSafe<NodeId> eliminationOrderIter = absorbedClique->cliqueEliminationOrder().beginSafe();
          eliminationOrderIter != absorbedClique->cliqueEliminationOrder().endSafe(); ++eliminationOrderIter ) {

      // if it's not on separtor with its parent
      if ( ! separator.contains ( *eliminationOrderIter ) ) {

        //=================================================================================
        // Initialisation Operations

        // First we create the tables that will result from variable elimination
        Potential<GUM_SCALAR>* newPotential = new Potential<GUM_SCALAR>();
        UtilityTable<GUM_SCALAR>* newUtility = new UtilityTable<GUM_SCALAR>();

        // Then we need to add all not yet eliminated variables of the clique in ours new table
        cliqueRemainVarList .erase ( & ( this->influenceDiagram().variable ( *eliminationOrderIter ) ) );

        for ( SequenceIteratorSafe<const DiscreteVariable*> cliqueVarListIter = cliqueRemainVarList.beginSafe();
              cliqueVarListIter != cliqueRemainVarList.endSafe(); ++cliqueVarListIter ) {
          newPotential->add ( **cliqueVarListIter );
          newUtility->add ( **cliqueVarListIter );
        }

        // And finally, before doing marginalizing operations,
        // we add an instanciation to those tables to ease marginalizing operations.
        // Note that we only need one tablel instance because the other one 's got
        // exactly sames variables
        Instantiation newInstantiation ( newPotential );

        //=====================================================================================
        // Marginalization

        // Then we fill the new potentials over all their values by marginalizing the previous one
        for ( newInstantiation.setFirst(); not newInstantiation.end(); newInstantiation.inc() ) {

          // Various initialization
          GUM_SCALAR potentialValue = ( GUM_SCALAR ) 0;
          GUM_SCALAR utilityValue = ( GUM_SCALAR ) 0;

          if ( this->influenceDiagram().isDecisionNode ( *eliminationOrderIter ) )
            utilityValue = -1 * ( std::numeric_limits< GUM_SCALAR>::max() );

          // Then we compute value for current newInstanciation
          cliqueInstance.setVals ( newInstantiation );

          for ( cliqueInstance.setFirstOut ( newInstantiation ); not cliqueInstance.end(); cliqueInstance.incOut ( newInstantiation ) ) {

            //********************************************************************************************************
            // Récupération de la valeur de la cpt de la clique pour cette instance
            GUM_SCALAR currentPotential = ( GUM_SCALAR ) 1;

            if ( potentialMarginal == 0 ) {
              // If there's no ancient potential it means that we haven't yet compute him
              for ( HashTableConstIteratorSafe<const Potential<GUM_SCALAR>*, Instantiation*> potentialIter = absorbedClique->potentialBucket().beginSafe();
                    potentialIter != absorbedClique->potentialBucket().endSafe(); ++potentialIter ) {
                ( potentialIter.val() )->setVals ( cliqueInstance );
                currentPotential *= potentialIter.key()->get ( *( potentialIter.val() ) );
              }
            } else {
              Instantiation potentialMarginalInst ( potentialMarginal );
              potentialMarginalInst.setVals ( cliqueInstance );
              currentPotential = potentialMarginal->get ( potentialMarginalInst );
            }

            //********************************************************************************************************


            //********************************************************************************************************
            // Récupération de la valeur d'utilité de la clique pour cette instance
            GUM_SCALAR currentUtility = ( GUM_SCALAR ) 0;

            if ( utilityMarginal == 0 ) {
              for ( HashTableConstIteratorSafe<const UtilityTable<GUM_SCALAR>*, Instantiation*> utilityIter = absorbedClique->utilityBucket().beginSafe();
                    utilityIter != absorbedClique->utilityBucket().endSafe(); ++utilityIter ) {
                ( utilityIter.val() )->setVals ( cliqueInstance );
                currentUtility += utilityIter.key()->get ( *( utilityIter.val() ) );
              }

              currentUtility *= currentPotential;
            } else {
              Instantiation utilityMarginalInst ( utilityMarginal );
              utilityMarginalInst.setVals ( cliqueInstance );
              currentUtility = utilityMarginal->get ( utilityMarginalInst );
            }

            //********************************************************************************************************

            //********************************************************************************************************
            // Marginalization
            if ( this->influenceDiagram().isDecisionNode ( *eliminationOrderIter ) ) {
              if ( potentialValue < currentPotential ) {
                potentialValue = currentPotential;
              }

              if ( utilityValue < currentUtility ) {
                utilityValue = currentUtility;

                if ( __utakenDecisionMap.exists ( *eliminationOrderIter ) )
                  __utakenDecisionMap.erase ( *eliminationOrderIter );

                __utakenDecisionMap.insert ( *eliminationOrderIter,
                                             cliqueInstance.val ( this->influenceDiagram().variable ( *eliminationOrderIter ) ) );
              }
            } else {
              potentialValue += currentPotential;
              utilityValue += currentUtility;
            }

            //********************************************************************************************************
          }

          // And finally we update  the potentials with computed value for newInstanciation
          newPotential->set ( newInstantiation, potentialValue );
          newUtility->set ( newInstantiation, utilityValue );
        }

        //=====================================================================================
        // Updates of tables
        if ( potentialMarginal != 0 )
          delete potentialMarginal;

        potentialMarginal = newPotential;

        if ( utilityMarginal != 0 )
          delete utilityMarginal;

        utilityMarginal = newUtility;

        //=====================================================================================
        // Then we removed variable from clique list of variable  ...
        cliqueInstance.erase ( this->influenceDiagram().variable ( *eliminationOrderIter ) );
      }
    }
  }




  //  __makeDummyPotential : creates a generic potential

  template <typename GUM_SCALAR> INLINE
  Potential<GUM_SCALAR>*
  DefaultInfluenceDiagramInference<GUM_SCALAR>::__makeDummyPotential ( NodeId cliqueId ) {

    Potential<GUM_SCALAR>* pot = new Potential<GUM_SCALAR> ( new MultiDimSparse<GUM_SCALAR> ( ( GUM_SCALAR ) 1 ) );
    __potentialDummies.insert ( pot );

    for ( NodeSet::const_iterator_safe cliqueNodesIter = __triangulation->junctionTree().clique ( cliqueId ).beginSafe();
          cliqueNodesIter != __triangulation->junctionTree().clique ( cliqueId ).endSafe(); ++cliqueNodesIter )
      pot->add ( this->influenceDiagram().variable ( *cliqueNodesIter ) );

    pot->normalize();
    return pot;
  }


  //  __makeDummyUtility : creates a generic utility

  template <typename GUM_SCALAR> INLINE
  UtilityTable<GUM_SCALAR>*
  DefaultInfluenceDiagramInference<GUM_SCALAR>::__makeDummyUtility ( NodeId cliqueId ) {
    UtilityTable<GUM_SCALAR>* ut = new UtilityTable<GUM_SCALAR> ( new MultiDimSparse<GUM_SCALAR> ( ( GUM_SCALAR ) 0 ) );
    __utilityDummies.insert ( ut );

    for ( NodeSet::const_iterator_safe cliqueNodesIter = __triangulation->junctionTree().clique ( cliqueId ).beginSafe();
          cliqueNodesIter != __triangulation->junctionTree().clique ( cliqueId ).endSafe(); ++cliqueNodesIter )
      ut->add ( this->influenceDiagram().variable ( *cliqueNodesIter ) );

    return ut;
  }






  // Default constructor

  template <typename GUM_SCALAR>
  CliqueProperties<GUM_SCALAR>::CliqueProperties() {
    GUM_CONSTRUCTOR ( CliqueProperties );
  }


  // Default destructor

  template <typename GUM_SCALAR>
  CliqueProperties<GUM_SCALAR>::~CliqueProperties() {
    GUM_DESTRUCTOR ( CliqueProperties );

    cleanFromInference();

    removeAllEvidence();

    for ( HashTableIteratorSafe<const Potential<GUM_SCALAR>*, Instantiation*> potentialIter = __potentialBucket.beginSafe();
          potentialIter != __potentialBucket.endSafe(); ++potentialIter )
      delete potentialIter.val();

    for ( HashTableIteratorSafe<const UtilityTable<GUM_SCALAR>*, Instantiation*> utilityIter = __utilityBucket.beginSafe();
          utilityIter != __utilityBucket.endSafe(); ++utilityIter )
      delete utilityIter.val();
  }




  // addVariable : adds a variable to the clique

  template <typename GUM_SCALAR>
  void
  CliqueProperties<GUM_SCALAR>::addVariable ( const DiscreteVariable& v ) {
    try {
      __allVarsInst.add ( v );
    } catch ( DuplicateElement& e ) {
      // Nothing to do then!
    }
  }


  // cliqueVariables : returns List containing all variables contained in this clique

  // @return returns List containing all variables contained in this clique
  template <typename GUM_SCALAR> INLINE
  const Sequence< const DiscreteVariable*>&
  CliqueProperties<GUM_SCALAR>::cliqueVariables() {
    return __allVarsInst.variablesSequence();
  }


  // cliqueInstantiation : returns instanciation on variable within this clique

  // @return returns instanciation on variable within this clique
  template <typename GUM_SCALAR> INLINE
  Instantiation&
  CliqueProperties<GUM_SCALAR>::cliqueInstantiation() {
    return __allVarsInst;
  }


  // addPotential : adds a potential to this clique

  // The removable boolean inidcates if this potential can be cleaned off after an inference or not
  template <typename GUM_SCALAR>
  void
  CliqueProperties<GUM_SCALAR>::addPotential ( const Potential<GUM_SCALAR>& cpt, bool removable ) {
    __potentialBucket.insert ( &cpt, new Instantiation ( cpt ) );

    if ( removable ) __removablePotentialList.insert ( &cpt );

    for ( Sequence< const DiscreteVariable* >::iterator_safe iter = cpt.begin (); iter != cpt.end (); ++iter ) {
      if ( removable && !__allVarsInst.contains ( **iter ) )
        try {
          __removableVarList.insert ( *iter );
        } catch ( DuplicateElement& e ) {
          // Nothing to do then!
        }

      addVariable ( **iter );
    }
  }


  // potentialBucket : Returns the potential bucket of this Clique

  template <typename GUM_SCALAR> INLINE
  const HashTable<const Potential<GUM_SCALAR>*, Instantiation* >&
  CliqueProperties<GUM_SCALAR>::potentialBucket() {
    return __potentialBucket;
  }



  // addUtility : adds a utility table to this clique

  // The removable boolean inidcates if this utilityTable can be cleaned off after an inference or not
  template <typename GUM_SCALAR>
  void
  CliqueProperties<GUM_SCALAR>::addUtility ( const UtilityTable<GUM_SCALAR>& ut, bool removable ) {
    __utilityBucket.insert ( &ut, new Instantiation ( ut ) );

    if ( removable ) __removableUtilityList.insert ( &ut );

    for ( Sequence< const DiscreteVariable* >::iterator_safe iter = ut.begin (); iter != ut.end (); ++iter ) {
      if ( removable && !__allVarsInst.contains ( **iter ) )
        try {
          __removableVarList.insert ( *iter );
        } catch ( DuplicateElement& e ) {
          // Nothing to do then!
        }

      addVariable ( **iter );
    }
  }


  //  utilityBucket : Returns the utiluty table bucket of this Clique

  template <typename GUM_SCALAR> INLINE
  const HashTable<const UtilityTable<GUM_SCALAR>*, Instantiation* >&
  CliqueProperties<GUM_SCALAR>::utilityBucket() {
    return __utilityBucket;
  }



  // cleanFromInference : performs a cleaning of the clique after an inference

  // This is done by removing the "message" given by a child clique to its parents, meaning the potentials and utility tables obtained by
  // variable elemination in the clique.
  template <typename GUM_SCALAR>
  void
  CliqueProperties<GUM_SCALAR>::cleanFromInference() {

    // Removed added variables during inference (normally, the __removableVarList is empty, but we never know )
    for ( ListIteratorSafe<const DiscreteVariable*> removedVarIter =  __removableVarList.beginSafe(); removedVarIter !=  __removableVarList.endSafe(); ++removedVarIter ) {
      __allVarsInst.erase ( **removedVarIter );
      __removableVarList.erase ( removedVarIter );
    }

    // Removed added potentials during inference
    for ( ListIteratorSafe<const Potential<GUM_SCALAR>* > removedPotIter =  __removablePotentialList.beginSafe();
          removedPotIter !=  __removablePotentialList.endSafe(); ++removedPotIter ) {
      delete __potentialBucket[ *removedPotIter ];
      __potentialBucket.erase ( *removedPotIter );
      delete *removedPotIter;
    }

    __removablePotentialList.clear();

    // Removed added utility tables during inference
    for ( ListIteratorSafe<const UtilityTable<GUM_SCALAR>* > removedUtIter =  __removableUtilityList.beginSafe();
          removedUtIter !=  __removableUtilityList.endSafe(); ++removedUtIter ) {
      delete __utilityBucket[ *removedUtIter ];
      __utilityBucket.erase ( *removedUtIter );
      delete *removedUtIter;
    }

    __removableUtilityList.clear();
  }




  // makeEliminationOrder : creates an elimination order in the clique compatible with global elimination order

  // The global elimination order is given by elim.
  template <typename GUM_SCALAR>
  void
  CliqueProperties<GUM_SCALAR>::makeEliminationOrder ( const std::vector<NodeId>& elim, const InfluenceDiagram<GUM_SCALAR>& infDiag ) {
    for ( size_t i = 0; i < elim.size(); i++ ) {
      if ( __allVarsInst.contains ( infDiag.variable ( elim[i] ) ) )
        __eliminationOrder.insert ( elim[i] );
    }
  }


  // cliqueEliminationOrder : returns the elimination sequence for this clique

  template <typename GUM_SCALAR> INLINE
  const Sequence<NodeId>&
  CliqueProperties<GUM_SCALAR>::cliqueEliminationOrder() {
    return __eliminationOrder;
  }




  // addEvidence : add evidence over one variable present in the clique

  // @throw OperationNotAllowed if evidence has more than one variable.
  // @throw NotFound Raised if the evidence is on a variable not present in this clique.
  // @throw DuplicateElement, if another evidence over this variable exists for this clique
  template <typename GUM_SCALAR>
  void
  CliqueProperties<GUM_SCALAR>::addEvidence ( const Potential<GUM_SCALAR>& evidence ) {

    // To avoid interference
    cleanFromInference();

    // First, we check if evidence is over only one variable.
    if ( evidence.variablesSequence().size() != 1 ) {
      GUM_ERROR ( OperationNotAllowed, "expected evidence on 1 variable, found on " << evidence.variablesSequence().size() );
    }

    // Then we assure us that that variable is in the clique
    if ( ! __allVarsInst.contains ( evidence.variablesSequence().atPos ( 0 ) ) ) {
      GUM_ERROR ( NotFound, evidence.variablesSequence().atPos ( 0 )->name() << " not found in clique " );
    }

    __evidences.insert ( evidence.variablesSequence().atPos ( 0 ) , &evidence );

    __potentialBucket.insert ( &evidence, new Instantiation ( evidence ) );
  }


  // evidences : Returns the mapping of evidences on variables in this clique

  template <typename GUM_SCALAR> INLINE
  const HashTable<const DiscreteVariable*, const Potential<GUM_SCALAR>* >&
  CliqueProperties<GUM_SCALAR>::evidences() const {
    return __evidences;
  }


  // removeEvidence : Removes the evidence over v

  template <typename GUM_SCALAR> INLINE
  void
  CliqueProperties<GUM_SCALAR>::removeEvidence ( const DiscreteVariable& v ) {
    const Potential<GUM_SCALAR>* evi = __evidences[ &v ];

    delete __potentialBucket[ evi ];
    __potentialBucket.erase ( evi );
    __evidences.erase ( &v );
  }


  // removeAllEvidence : Removes all the evidences

  template <typename GUM_SCALAR> INLINE
  void
  CliqueProperties<GUM_SCALAR>::removeAllEvidence() {
    for ( HashTableIteratorSafe< const DiscreteVariable*, const Potential<GUM_SCALAR>*  > evidencesIter = __evidences.beginSafe();
          evidencesIter != __evidences.endSafe(); )
      removeEvidence ( * ( evidencesIter.key() ) );

    __evidences.clear();
  }




} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 
