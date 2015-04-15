/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
// =========================================================================
#include <vector>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
// =========================================================================
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
// =========================================================================
#include <agrum/multidim/multiDimDecisionDiagram.h>
#include <agrum/multidim/multiDimDecisionDiagramFactory.h>
#include <agrum/multidim/instantiation.h>
// =========================================================================
#include <agrum/variables/labelizedVariable.h>

#include <agrum/core/algorithms/approximationPolicy/exactPolicy.h>
#include <agrum/core/algorithms/approximationPolicy/linearApproximationPolicy.h>
// =========================================================================

namespace gum_tests {

  class MultiDimDecisionDiagramTestSuite : public CxxTest::TestSuite {

    private:
    void __fillFactory(gum::MultiDimDecisionDiagramFactoryBase<float> *factory,
                       gum::List<gum::NodeId> *idList) {

      gum::Sequence<const gum::DiscreteVariable *> varseq;
      varseq.insert(Cprimevar);
      varseq.insert(Cvar);
      varseq.insert(PLvar);
      varseq.insert(APUvar);
      varseq.insert(BPUvar);
      varseq.insert(ADRvar);
      varseq.insert(BDRvar);
      varseq.insert(BOvar);
      factory->setVariablesSequence(varseq);

      idList->insert(factory->addNonTerminalNode(Cprimevar)); // 0
      idList->insert(factory->addNonTerminalNode(Cvar));      // 1
      idList->insert(factory->addNonTerminalNode(PLvar));     // 2
      idList->insert(factory->addNonTerminalNode(APUvar));    // 3
      idList->insert(factory->addNonTerminalNode(BPUvar));    // 4
      idList->insert(factory->addNonTerminalNode(ADRvar));    // 5
      idList->insert(factory->addNonTerminalNode(BDRvar));    // 6
      idList->insert(factory->addNonTerminalNode(BOvar));     // 7

      idList->insert(factory->addNonTerminalNode(Cvar));   // 8
      idList->insert(factory->addNonTerminalNode(PLvar));  // 9
      idList->insert(factory->addNonTerminalNode(APUvar)); // 10
      idList->insert(factory->addNonTerminalNode(BPUvar)); // 11
      idList->insert(factory->addNonTerminalNode(ADRvar)); // 12
      idList->insert(factory->addNonTerminalNode(BDRvar)); // 13
      idList->insert(factory->addNonTerminalNode(BOvar));  // 14

      idList->insert(factory->addTerminalNode(0));  // 15
      idList->insert(factory->addTerminalNode(10)); // 16
      idList->insert(factory->addTerminalNode(9));  // 17
      idList->insert(factory->addTerminalNode(1));  // 18

      factory->addArc((*idList)[0], (*idList)[1], 1);
      factory->addArc((*idList)[0], (*idList)[8], 0);

      factory->addArc((*idList)[1], (*idList)[17], 1);
      factory->addArc((*idList)[1], (*idList)[2], 0);
      factory->addArc((*idList)[2], (*idList)[3], 1);
      factory->addArc((*idList)[2], (*idList)[5], 0);
      factory->addArc((*idList)[3], (*idList)[4], 1);
      factory->addArc((*idList)[3], (*idList)[5], 0);
      factory->addArc((*idList)[4], (*idList)[7], 1);
      factory->addArc((*idList)[4], (*idList)[15], 0);
      factory->addArc((*idList)[5], (*idList)[6], 1);
      factory->addArc((*idList)[5], (*idList)[15], 0);
      factory->addArc((*idList)[6], (*idList)[7], 1);
      factory->addArc((*idList)[6], (*idList)[15], 0);
      factory->addArc((*idList)[7], (*idList)[17], 1);
      factory->addArc((*idList)[7], (*idList)[15], 0);

      factory->addArc((*idList)[8], (*idList)[18], 1);
      factory->addArc((*idList)[8], (*idList)[9], 0);
      factory->addArc((*idList)[9], (*idList)[10], 1);
      factory->addArc((*idList)[9], (*idList)[12], 0);
      factory->addArc((*idList)[10], (*idList)[11], 1);
      factory->addArc((*idList)[10], (*idList)[12], 0);
      factory->addArc((*idList)[11], (*idList)[14], 1);
      factory->addArc((*idList)[11], (*idList)[16], 0);
      factory->addArc((*idList)[12], (*idList)[13], 1);
      factory->addArc((*idList)[12], (*idList)[16], 0);
      factory->addArc((*idList)[13], (*idList)[14], 1);
      factory->addArc((*idList)[13], (*idList)[16], 0);
      factory->addArc((*idList)[14], (*idList)[18], 1);
      factory->addArc((*idList)[14], (*idList)[16], 0);
    }

    public:
    gum::LabelizedVariable *Cvar, *PLvar, *APUvar, *BPUvar, *ADRvar, *BDRvar, *BOvar,
        *Cprimevar;

    void setUp() {
      Cvar = new gum::LabelizedVariable("C", "C", 2);
      PLvar = new gum::LabelizedVariable("PL", "PL", 2);
      APUvar = new gum::LabelizedVariable("APU", "APU", 2);
      BPUvar = new gum::LabelizedVariable("BPU", "BPU", 2);
      ADRvar = new gum::LabelizedVariable("ADR", "ADR", 2);
      BDRvar = new gum::LabelizedVariable("BDR", "BDR", 2);
      BOvar = new gum::LabelizedVariable("BO", "BO", 2);
      Cprimevar = new gum::LabelizedVariable("C'", "C'", 2);
    }

    void tearDown() {
      delete Cvar;
      delete Cprimevar;
      delete PLvar;
      delete APUvar;
      delete BPUvar;
      delete ADRvar;
      delete BDRvar;
      delete BOvar;
    }

    /**
     * *************************************************************************************/
    /**  Test sur la construction, le remplissage et la destruction d'une factory  */
    /**
     * *************************************************************************************/
    void testFactorySimpleCreation() {

      // *********************************************************************
      // Création du multidim
      // *********************************************************************
      gum::MultiDimDecisionDiagramFactory<float> *factory = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          factory = new gum::MultiDimDecisionDiagramFactory<float>());

      // *********************************************************************
      // Remplissage du multidim ( donc accessoirement des fonctions d'insertion
      // de noeuds et d'arcs sans risques de levage d'exception)
      // *********************************************************************
      gum::List<gum::NodeId> idList;
      TS_GUM_ASSERT_THROWS_NOTHING(__fillFactory(factory, &idList));

      // *********************************************************************
      // Destruction du multidim
      // *********************************************************************
      TS_GUM_ASSERT_THROWS_NOTHING(delete factory);
    }

    /**
     * *************************************************************************************/
    /**  Test sur des modifications apportées au remplissage du factory     */
    /**
     * *************************************************************************************/
    void testFactoryGraphicalFunctions() {

      // *********************************************************************
      // Création du multidim
      // *********************************************************************
      gum::MultiDimDecisionDiagramFactory<float> *factory =
          new gum::MultiDimDecisionDiagramFactory<float>();
      gum::List<gum::NodeId> idList;
      __fillFactory(factory, &idList);

      // *********************************************************************
      // Test sur les fonctions d'ajout et retrait de noeud
      // *********************************************************************

      // Ajout d'un noeud terminal ayant la même valeur qu'un autre noeud terminal
      // (la factory doit renvoyer l'id de ce node)
      TS_ASSERT_EQUALS(factory->addTerminalNode(10), idList[16]);
      // factory->showProperties();

      // Test de retrait d'un noeud non terminal
      TS_GUM_ASSERT_THROWS_NOTHING(factory->eraseNode(idList[12]));
      // factory->showProperties();

      // Test de retrait d'un noeud terminal
      TS_GUM_ASSERT_THROWS_NOTHING(factory->eraseNode(idList[15]));
      // factory->showProperties();

      // Test de retrait du même noeud, pour s'assurer du levage d'exception
      TS_ASSERT_THROWS(factory->eraseNode(idList[15]), gum::NotFound);

      // *********************************************************************
      // Test sur les fonctions d'ajout et retrait d'arc
      // *********************************************************************

      // Test d'ajout d'un arc ayant pour départ un noeud inexistant (doit lever
      // l'exception gum::NotFound)
      // (le noeud 15 a été détruit un peu plus haut)
      TS_ASSERT_THROWS(factory->addArc(idList[15], idList[1], 2), gum::NotFound);

      // Test d'ajout d'un arc ayant pour départ un noeud terminal (doit lever
      // l'exception gum::InvalidNode
      // vu que les noeuds terminaux sont ... terminaux)
      TS_ASSERT_THROWS(factory->addArc(idList[16], idList[1], 3), gum::InvalidNode);

      // Test d'ajout d'un arc entre 2 noeuds déjà reliés par un autre arc de valeur
      // différente(doit lever aucune exception)
      // TS_GUM_ASSERT_THROWS_NOTHING ( factory->addArc ( idList[8], idList[18], 0 )
      // );

      // Test d'ajout d'un arc entre 2 noeuds déjà reliés par un autre arc de valeur
      // égale(doit lever aucune exception)
      TS_ASSERT_THROWS(factory->addArc(idList[8], idList[18], 1),
                       gum::DuplicateElement);

      // Test d'ajout d'un arc qui viole l'ordre sur les variables (doit lever
      // l'exception gum::OperationNotAllowed)
      TS_ASSERT_THROWS(factory->addArc(idList[8], idList[1], 5),
                       gum::OperationNotAllowed);

      // Test de retrait d'un arc existant (ne doit lever aucune exception)
      TS_GUM_ASSERT_THROWS_NOTHING(factory->eraseArc(idList[3], idList[4]));
      // factory->showProperties();

      // Test de retrait d'un arc avec valeur spécifiée
      // TS_GUM_ASSERT_THROWS_NOTHING ( factory->eraseSpecificArc ( idList[8],
      // idList[18], 4 ) );
      // factory->showProperties();

      // Test de retrait d'un arc après une tentative ayant eu pour but d'ajouter un
      // autre arc reliant les deux noeuds
      TS_GUM_ASSERT_THROWS_NOTHING(factory->eraseArc(idList[8], idList[18]));

      // Test de retrait de retrait de l'arc qui violait la contrainte d'ordre et
      // donc n'existe pas
      TS_ASSERT_THROWS(factory->eraseArc(idList[8], idList[1]), gum::InvalidArc);

      // Test de retrait d'un arc dont un des noeuds n'existe plus (doit lever
      // l'exception InvalidArc)
      TS_ASSERT_THROWS(factory->eraseArc(idList[15], idList[1]), gum::InvalidArc);
      TS_ASSERT_THROWS(factory->eraseArc(idList[16], idList[1]), gum::InvalidArc);

      // *********************************************************************
      // Test sur les fonctions d'ajout et retrait d'arc par defaut
      // *********************************************************************

      // Test d'ajout d'un arc par défaut sans risque
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addDefaultArc(idList[5], idList[7]));
      // factory->showProperties();

      // Test d'ajout d'un arc par défaut ayant pour départ un noeud inexistant (doit
      // lever l'exception gum::InvalidNode)
      // (le noeud 15 a été détruit un peu plus haut
      TS_ASSERT_THROWS(factory->addDefaultArc(idList[15], idList[1]), gum::NotFound);

      // Test d'ajout d'un arc par défaut ayant pour départ un noeud terminal (doit
      // lever l'exception gum::OperationNotAllowed
      // vu que les noeuds terminaux sont ... terminaux)
      TS_ASSERT_THROWS(factory->addDefaultArc(idList[16], idList[1]),
                       gum::InvalidNode);

      // Test d'ajout d'un arc par défaut entre 2 noeuds déjà reliés par un autre arc
      // (doit rien lever)
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addDefaultArc(idList[0], idList[1]));

      // Test d'ajout d'un arc par défaut entre 2 noeuds déjà reliés par un autre arc
      // par defaut(doit lever DuplicateElement)
      TS_ASSERT_THROWS(factory->addDefaultArc(idList[0], idList[3]),
                       gum::DuplicateElement);

      // Test d'ajout d'un arc par défaut qui viole l'ordre sur les variables (doit
      // lever l'exception InvalidArc)
      TS_ASSERT_THROWS(factory->addDefaultArc(idList[8], idList[1]),
                       gum::OperationNotAllowed);

      TS_GUM_ASSERT_THROWS_NOTHING(delete factory);
    }

    /**
     * *************************************************************************************/
    /**       Test sur d'appel du getMultiDimDecisionDiagram()      */
    /**
     * *************************************************************************************/
    void testFactoryGetMultiDimDecisionDiagram() {

      // *********************************************************************
      // Création du multidim
      // *********************************************************************
      gum::MultiDimDecisionDiagramFactoryBase<float> *factory =
          new gum::MultiDimDecisionDiagramFactory<float>();
      gum::List<gum::NodeId> idList;
      __fillFactory(factory, &idList);
      factory->addDefaultArc(idList[2], idList[15]);

      // *********************************************************************
      // Appel au multidim
      // *********************************************************************
      gum::MultiDimDecisionDiagramBase<float> *container = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(container =
                                       factory->getMultiDimDecisionDiagram());

      // *********************************************************************
      // Verification de la complétion automatique des arcs par défaut (en mode
      // texte)
      // *********************************************************************
      // factory->showProperties();

      // *********************************************************************
      // Destruction du multidim
      // *********************************************************************
      TS_GUM_ASSERT_THROWS_NOTHING(delete factory);
      TS_GUM_ASSERT_THROWS_NOTHING(delete container);
    }

    /**
     * *************************************************************************************/
    /**   Test des fonctions d'accès et de modification du multidim     */
    /**
     * *************************************************************************************/
    void testMultiDimDecisionDiagramAccessorsModifiersMethods() {

      // *********************************************************************
      // Création du multidim
      // *********************************************************************
      gum::MultiDimDecisionDiagramFactory<float> *factory =
          new gum::MultiDimDecisionDiagramFactory<float>();
      gum::List<gum::NodeId> idList;
      __fillFactory(factory, &idList);

      gum::MultiDimDecisionDiagramBase<float> *container = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(container =
                                       factory->getMultiDimDecisionDiagram());

      TS_GUM_ASSERT_THROWS_NOTHING(delete factory);

      // *********************************************************************
      // Test variablesSequence(), et si toutes les variables sont
      // accesoirement bien présentent
      // *********************************************************************
      gum::Sequence<const gum::DiscreteVariable *> varSeq;
      TS_GUM_ASSERT_THROWS_NOTHING(varSeq = container->variablesSequence());
      TS_ASSERT_EQUALS(varSeq.exists(Cprimevar), true);
      TS_ASSERT_EQUALS(varSeq.exists(Cvar), true);
      TS_ASSERT_EQUALS(varSeq.exists(PLvar), true);
      TS_ASSERT_EQUALS(varSeq.exists(APUvar), true);
      TS_ASSERT_EQUALS(varSeq.exists(BPUvar), true);
      TS_ASSERT_EQUALS(varSeq.exists(ADRvar), true);
      TS_ASSERT_EQUALS(varSeq.exists(BDRvar), true);
      TS_ASSERT_EQUALS(varSeq.exists(BOvar), true);

      // *********************************************************************
      // Test name() et basename()
      // *********************************************************************
      TS_ASSERT_EQUALS(container->name(), "MultiDimDecisionDiagram");

      TS_ASSERT_EQUALS(container->basename(), "MultiDimImplementation");

      // *********************************************************************
      // Test set(), fill() et fillWith()
      // *********************************************************************
      gum::Instantiation inst(*container);

      TS_ASSERT_THROWS(container->set(inst, 14.0), gum::OperationNotAllowed);

      TS_ASSERT_THROWS(container->fill(14.0), gum::OperationNotAllowed);

      std::vector<float> v;

      for (float i = 0; i < 128; i++)
        v.push_back(i);

      TS_ASSERT_THROWS(container->fillWith(v), gum::SizeError);

      for (float i = 128; i < 256; i++)
        v.push_back(i);

      TS_ASSERT_THROWS(container->fillWith(v), gum::OperationNotAllowed);

      // *********************************************************************
      // Test get(), [], et l'accès aux valeurs
      // *********************************************************************
      TS_GUM_ASSERT_THROWS_NOTHING(container->get(inst));
      TS_GUM_ASSERT_THROWS_NOTHING((*container)[inst]);

      for (inst.setFirst(); !inst.end(); ++inst)
        if (inst.val(*Cvar) == 1) {
          if (inst.val(*Cprimevar) == 1) {
            TS_ASSERT_DELTA(container->get(inst), 9, 0.001);
          } else {
            TS_ASSERT_DELTA(container->get(inst), 1, 0.001);
          }
        } else {

          if (inst.val(*PLvar) == 1) {

            if (inst.val(*APUvar) == 1) {

              if (inst.val(*BPUvar) == 1) {

                if (inst.val(*BOvar) == 1) {

                  if (inst.val(*Cprimevar) == 1) {
                    TS_ASSERT_DELTA(container->get(inst), 9, 0.001);
                  } else {
                    TS_ASSERT_DELTA(container->get(inst), 1, 0.001);
                  }

                } else {

                  if (inst.val(*Cprimevar) == 1) {
                    TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
                  } else {
                    TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
                  }
                }
              } else {

                if (inst.val(*Cprimevar) == 1) {
                  TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
                } else {
                  TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
                }
              }
            } else {

              if (inst.val(*ADRvar) == 1) {

                if (inst.val(*BDRvar) == 1) {

                  if (inst.val(*BOvar) == 1) {

                    if (inst.val(*Cprimevar) == 1) {
                      TS_ASSERT_DELTA(container->get(inst), 9, 0.001);
                    } else {
                      TS_ASSERT_DELTA(container->get(inst), 1, 0.001);
                    }

                  } else {

                    if (inst.val(*Cprimevar) == 1) {
                      TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
                    } else {
                      TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
                    }
                  }

                } else {

                  if (inst.val(*Cprimevar) == 1) {
                    TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
                  } else {
                    TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
                  }
                }

              } else {

                if (inst.val(*Cprimevar) == 1) {
                  TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
                } else {
                  TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
                }
              }
            }
          } else {

            if (inst.val(*ADRvar) == 1) {

              if (inst.val(*BDRvar) == 1) {

                if (inst.val(*BOvar) == 1) {

                  if (inst.val(*Cprimevar) == 1) {
                    TS_ASSERT_DELTA(container->get(inst), 9, 0.001);
                  } else {
                    TS_ASSERT_DELTA(container->get(inst), 1, 0.001);
                  }

                } else {

                  if (inst.val(*Cprimevar) == 1) {
                    TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
                  } else {
                    TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
                  }
                }

              } else {

                if (inst.val(*Cprimevar) == 1) {
                  TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
                } else {
                  TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
                }
              }

            } else {

              if (inst.val(*Cprimevar) == 1) {
                TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
              } else {
                TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
              }
            }
          }
        }

      // *********************************************************************
      // Test swap()
      // *********************************************************************
      gum::LabelizedVariable *lv =
          new gum::LabelizedVariable("Test", "Cornichon", 2);
      TS_ASSERT_THROWS(container->swap(*Cvar, *lv), gum::OperationNotAllowed);
      delete lv;

      TS_GUM_ASSERT_THROWS_NOTHING(delete container);
    }

    void testMultiDimDecisionDiagramImplementationMethods() {

      // *********************************************************************
      // Création du multidim
      // *********************************************************************
      gum::MultiDimDecisionDiagramFactory<float> *factory =
          new gum::MultiDimDecisionDiagramFactory<float>();
      gum::List<gum::NodeId> idList;
      __fillFactory(factory, &idList);

      gum::MultiDimDecisionDiagramBase<float> *container = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(container =
                                       factory->getMultiDimDecisionDiagram());

      TS_GUM_ASSERT_THROWS_NOTHING(delete factory);

      // *********************************************************************
      // Test add(), erase()
      // *********************************************************************
      gum::LabelizedVariable *lv =
          new gum::LabelizedVariable("Test", "Cornichon", 2);

      TS_ASSERT_THROWS(container->add(*lv), gum::OperationNotAllowed);

      TS_ASSERT_THROWS(container->erase(*Cprimevar), gum::OperationNotAllowed);

      // *********************************************************************
      // Test nbrDim(), domainSize(), realSize()
      // *********************************************************************

      TS_ASSERT_EQUALS(container->nbrDim(), (gum::Idx)8);

      TS_ASSERT_EQUALS(container->domainSize(), (gum::Size)256);

      TS_ASSERT_EQUALS(container->realSize(), (gum::Size)19);

      TS_GUM_ASSERT_THROWS_NOTHING(container->compressionRate());

      TS_GUM_ASSERT_THROWS_NOTHING(container->variable(0));

      TS_GUM_ASSERT_THROWS_NOTHING(container->pos(*Cvar));

      TS_ASSERT_EQUALS(container->contains(*Cprimevar), true);
      TS_ASSERT_EQUALS(container->contains(*lv), false);

      TS_ASSERT_EQUALS(container->empty(), false);

      delete lv;

      TS_GUM_ASSERT_THROWS_NOTHING(delete container);
    }

    void testMultiDimDecisionDiagramCopyMethods() {

      // *********************************************************************
      // Création du multidim
      // *********************************************************************
      gum::MultiDimDecisionDiagramFactory<float> *factory =
          new gum::MultiDimDecisionDiagramFactory<float>();
      gum::List<gum::NodeId> idList;
      __fillFactory(factory, &idList);

      gum::MultiDimDecisionDiagramBase<float> *container = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(container =
                                       factory->getMultiDimDecisionDiagram());

      gum::MultiDimDecisionDiagramBase<float> *container2 = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(container2 =
                                       factory->getMultiDimDecisionDiagram());

      TS_GUM_ASSERT_THROWS_NOTHING(delete factory);

      TS_ASSERT_THROWS(container->copy(*container2), gum::OperationNotAllowed);

      TS_ASSERT_THROWS(container->copyFrom(*container2), gum::OperationNotAllowed);

      TS_GUM_ASSERT_THROWS_NOTHING(delete container);
      TS_GUM_ASSERT_THROWS_NOTHING(delete container2);
    }

    void testMultiDimDecisionDiagramDiagramHandlersMethods() {

      // *********************************************************************
      // Création du multidim
      // *********************************************************************
      gum::MultiDimDecisionDiagramFactory<float> *factory =
          new gum::MultiDimDecisionDiagramFactory<float>();
      gum::List<gum::NodeId> idList;
      __fillFactory(factory, &idList);

      gum::MultiDimDecisionDiagramBase<float> *container = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(container =
                                       factory->getMultiDimDecisionDiagram());

      delete factory;

      // *********************************************************************
      // Tests
      // *********************************************************************
      gum::LabelizedVariable *Banditovar =
          new gum::LabelizedVariable("Bandito", "Desperado", 2);

      // Test is terminal node
      TS_ASSERT_EQUALS(container->isTerminalNode(16), true);
      TS_ASSERT_EQUALS(container->isTerminalNode(6), false);

      // Test Root
      TS_ASSERT_EQUALS(container->root(), (gum::Idx)1);

      // test node value
      TS_GUM_ASSERT_THROWS_NOTHING(container->nodeValue(16));
      TS_ASSERT_EQUALS(container->nodeValue(16), 0);
      TS_ASSERT_THROWS(container->nodeValue(6), gum::InvalidNode);

      // Test node variable
      TS_GUM_ASSERT_THROWS_NOTHING(container->nodeVariable(6));
      TS_ASSERT_EQUALS(container->nodeVariable(6)->name(), "ADR");
      TS_ASSERT_THROWS(container->nodeVariable(16), gum::InvalidNode);

      // Test node variable
      TS_GUM_ASSERT_THROWS_NOTHING(container->variableNodes(Cprimevar));
      //~ TS_ASSERT_EQUALS( container->variableNodes( Banditovar ), nullptr );

      // Test node sons
      TS_GUM_ASSERT_THROWS_NOTHING(container->nodeSons(6));
      TS_ASSERT_THROWS(container->nodeSons(16), gum::InvalidNode);

      // Test has node default sons
      TS_ASSERT_EQUALS(container->hasNodeDefaultSon(6), false);
      TS_ASSERT_THROWS(container->hasNodeDefaultSon(16), gum::InvalidNode);

      // Test has node default sons
      TS_ASSERT_THROWS(container->nodeDefaultSon(6), gum::NotFound);
      TS_ASSERT_THROWS(container->nodeDefaultSon(16), gum::InvalidNode);

      // Test is in diagram variable
      TS_GUM_ASSERT_THROWS_NOTHING(container->isInDiagramVariable(Cprimevar));
      //~ TS_ASSERT_EQUALS( container->isInDiagramVariable( Banditovar ), nullptr );

      // *********************************************************************
      // Cleaning
      // *********************************************************************
      TS_GUM_ASSERT_THROWS_NOTHING(delete container);
      delete Banditovar;
    }

    void testMultiDimDecisionDiagramVariousMethods() {

      // *********************************************************************
      // Création du multidim
      // *********************************************************************
      gum::MultiDimDecisionDiagramFactory<float> *factory =
          new gum::MultiDimDecisionDiagramFactory<float>();
      gum::List<gum::NodeId> idList;
      __fillFactory(factory, &idList);

      gum::MultiDimDecisionDiagramBase<float> *container = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(container =
                                       factory->getMultiDimDecisionDiagram());

      gum::MultiDimDecisionDiagramBase<float> *container2 = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(container2 =
                                       factory->getMultiDimDecisionDiagram());

      factory->eraseArc(3, 6);
      factory->addArc(3, 16, 0);

      gum::MultiDimDecisionDiagramBase<float> *container3 = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(container3 =
                                       factory->getMultiDimDecisionDiagram());

      // *********************************************************************
      // test tostring() et tostring(gum::Instantiation*)
      // *********************************************************************
      TS_GUM_ASSERT_THROWS_NOTHING(container->toString());

      gum::Instantiation inst(*container);
      inst.setFirst();
      TS_GUM_ASSERT_THROWS_NOTHING(container->toString(&inst));

      // *********************************************************************
      // test == et !=
      // *********************************************************************
      TS_ASSERT_EQUALS(*container == *container2, true);
      TS_ASSERT_EQUALS(*container == *container3, false);

      TS_ASSERT_EQUALS(*container != *container2, false);
      TS_ASSERT_EQUALS(*container != *container3, true);

      TS_GUM_ASSERT_THROWS_NOTHING(delete factory);

      TS_GUM_ASSERT_THROWS_NOTHING(delete container);
      TS_GUM_ASSERT_THROWS_NOTHING(delete container2);
      TS_GUM_ASSERT_THROWS_NOTHING(delete container3);
    }

    void testToDot() {

      // *********************************************************************
      // Création du multidim
      // *********************************************************************
      gum::MultiDimDecisionDiagramFactory<float> *factory =
          new gum::MultiDimDecisionDiagramFactory<float>();
      gum::List<gum::NodeId> idList;
      __fillFactory(factory, &idList);

      gum::MultiDimDecisionDiagramBase<float> *container = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(container =
                                       factory->getMultiDimDecisionDiagram());

      // For comparison with what readers will return
      std::string dotfile = GET_PATH_STR("DecisionDiagram.dot");
      std::ofstream output(dotfile.c_str(), std::ios::out | std::ios::trunc);

      if (!output.good()) {
        GUM_ERROR(gum::IOError, "Stream states flags are not all unset.");
      }

      output << container->toDot();

      output.flush();

      output.close();

      if (output.fail()) {
        GUM_ERROR(gum::IOError, "Writting in the ostream failed.");
      }

      delete factory;
      delete container;
    }

    void testLinearApproximation() {

      // *********************************************************************
      // Création du multidim
      // *********************************************************************
      gum::MultiDimDecisionDiagramFactory<float, gum::LinearApproximationPolicy> *
          factory = new gum::MultiDimDecisionDiagramFactory<
              float, gum::LinearApproximationPolicy>();

      factory->setLowLimit(0);
      factory->setHighLimit(10);
      factory->setEpsilon(5);

      gum::List<gum::NodeId> idList;
      __fillFactory(factory, &idList);

      gum::MultiDimDecisionDiagramBase<float> *container = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(container =
                                       factory->getMultiDimDecisionDiagram());

      delete factory;

      gum::Instantiation inst(*container);

      for (inst.setFirst(); !inst.end(); ++inst)
        if (inst.val(*Cvar) == 1) {
          if (inst.val(*Cprimevar) == 1) {
            TS_ASSERT_DELTA(container->get(inst), 7.5, 0.001);
          } else {
            TS_ASSERT_DELTA(container->get(inst), 2.5, 0.001);
          }
        } else {

          if (inst.val(*PLvar) == 1) {

            if (inst.val(*APUvar) == 1) {

              if (inst.val(*BPUvar) == 1) {

                if (inst.val(*BOvar) == 1) {

                  if (inst.val(*Cprimevar) == 1) {
                    TS_ASSERT_DELTA(container->get(inst), 7.5, 0.001);
                  } else {
                    TS_ASSERT_DELTA(container->get(inst), 2.5, 0.001);
                  }

                } else {

                  if (inst.val(*Cprimevar) == 1) {
                    TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
                  } else {
                    TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
                  }
                }
              } else {

                if (inst.val(*Cprimevar) == 1) {
                  TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
                } else {
                  TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
                }
              }
            } else {

              if (inst.val(*ADRvar) == 1) {

                if (inst.val(*BDRvar) == 1) {

                  if (inst.val(*BOvar) == 1) {

                    if (inst.val(*Cprimevar) == 1) {
                      TS_ASSERT_DELTA(container->get(inst), 7.5, 0.001);
                    } else {
                      TS_ASSERT_DELTA(container->get(inst), 2.5, 0.001);
                    }

                  } else {

                    if (inst.val(*Cprimevar) == 1) {
                      TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
                    } else {
                      TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
                    }
                  }

                } else {

                  if (inst.val(*Cprimevar) == 1) {
                    TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
                  } else {
                    TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
                  }
                }

              } else {

                if (inst.val(*Cprimevar) == 1) {
                  TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
                } else {
                  TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
                }
              }
            }
          } else {

            if (inst.val(*ADRvar) == 1) {

              if (inst.val(*BDRvar) == 1) {

                if (inst.val(*BOvar) == 1) {

                  if (inst.val(*Cprimevar) == 1) {
                    TS_ASSERT_DELTA(container->get(inst), 7.5, 0.001);
                  } else {
                    TS_ASSERT_DELTA(container->get(inst), 2.5, 0.001);
                  }

                } else {

                  if (inst.val(*Cprimevar) == 1) {
                    TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
                  } else {
                    TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
                  }
                }

              } else {

                if (inst.val(*Cprimevar) == 1) {
                  TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
                } else {
                  TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
                }
              }

            } else {

              if (inst.val(*Cprimevar) == 1) {
                TS_ASSERT_DELTA(container->get(inst), 0, 0.001);
              } else {
                TS_ASSERT_DELTA(container->get(inst), 10, 0.001);
              }
            }
          }
        }

      delete container;
    }
  };
}
