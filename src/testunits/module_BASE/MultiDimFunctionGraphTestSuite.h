/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


// =========================================================================
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
// =========================================================================
#include "testsuite_utils.h"
#include <gumtest/AgrumTestSuite.h>
// =========================================================================
#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>
#include <agrum/tools/multidim/implementations/multiDimFunctionGraphGenerator.h>
#include <agrum/tools/multidim/implementations/multiDimFunctionGraphManager.h>
#include <agrum/tools/multidim/instantiation.h>
// =========================================================================
#include <agrum/tools/variables/labelizedVariable.h>
// =========================================================================

namespace gum_tests {

  class MultiDimFunctionGraphTestSuite: public CxxTest::TestSuite {
    private:
    void fillFactory__(gum::MultiDimFunctionGraph< double >* factory,
                       gum::List< gum::NodeId >*             idList) {
      try {
        factory->add(*Cprimevar);
        factory->add(*Cvar);
        factory->add(*PLvar);
        factory->add(*APUvar);
        factory->add(*BPUvar);
        factory->add(*ADRvar);
        factory->add(*BDRvar);
        factory->add(*BOvar);


        idList->insert(factory->manager()->addInternalNode(Cprimevar));   // 1
        factory->manager()->setRootNode((*idList)[0]);
        idList->insert(factory->manager()->addInternalNode(Cvar));     // 2
        idList->insert(factory->manager()->addInternalNode(PLvar));    // 3
        idList->insert(factory->manager()->addInternalNode(APUvar));   // 4
        idList->insert(factory->manager()->addInternalNode(BPUvar));   // 5
        idList->insert(factory->manager()->addInternalNode(ADRvar));   // 6
        idList->insert(factory->manager()->addInternalNode(BDRvar));   // 7
        idList->insert(factory->manager()->addInternalNode(BOvar));    // 8

        idList->insert(factory->manager()->addInternalNode(Cvar));     // 9
        idList->insert(factory->manager()->addInternalNode(PLvar));    // 10
        idList->insert(factory->manager()->addInternalNode(APUvar));   // 11
        idList->insert(factory->manager()->addInternalNode(BPUvar));   // 12
        idList->insert(factory->manager()->addInternalNode(ADRvar));   // 13
        idList->insert(factory->manager()->addInternalNode(BDRvar));   // 14
        idList->insert(factory->manager()->addInternalNode(BOvar));    // 15

        idList->insert(factory->manager()->addTerminalNode(0));    // 16
        idList->insert(factory->manager()->addTerminalNode(10));   // 17
        idList->insert(factory->manager()->addTerminalNode(9));    // 18
        idList->insert(factory->manager()->addTerminalNode(1));    // 19


        factory->manager()->setSon((*idList)[0], 1, (*idList)[1]);
        factory->manager()->setSon((*idList)[0], 0, (*idList)[8]);

        factory->manager()->setSon((*idList)[1], 1, (*idList)[17]);
        factory->manager()->setSon((*idList)[1], 0, (*idList)[2]);
        factory->manager()->setSon((*idList)[2], 1, (*idList)[3]);
        factory->manager()->setSon((*idList)[2], 0, (*idList)[5]);
        factory->manager()->setSon((*idList)[3], 1, (*idList)[4]);
        factory->manager()->setSon((*idList)[3], 0, (*idList)[5]);
        factory->manager()->setSon((*idList)[4], 1, (*idList)[7]);
        factory->manager()->setSon((*idList)[4], 0, (*idList)[15]);
        factory->manager()->setSon((*idList)[5], 1, (*idList)[6]);
        factory->manager()->setSon((*idList)[5], 0, (*idList)[15]);
        factory->manager()->setSon((*idList)[6], 1, (*idList)[7]);
        factory->manager()->setSon((*idList)[6], 0, (*idList)[15]);
        factory->manager()->setSon((*idList)[7], 1, (*idList)[17]);
        factory->manager()->setSon((*idList)[7], 0, (*idList)[15]);


        factory->manager()->setSon((*idList)[8], 1, (*idList)[18]);
        factory->manager()->setSon((*idList)[8], 0, (*idList)[9]);
        factory->manager()->setSon((*idList)[9], 1, (*idList)[10]);
        factory->manager()->setSon((*idList)[9], 0, (*idList)[12]);
        factory->manager()->setSon((*idList)[10], 1, (*idList)[11]);
        factory->manager()->setSon((*idList)[10], 0, (*idList)[12]);
        factory->manager()->setSon((*idList)[11], 1, (*idList)[14]);
        factory->manager()->setSon((*idList)[11], 0, (*idList)[16]);
        factory->manager()->setSon((*idList)[12], 1, (*idList)[13]);
        factory->manager()->setSon((*idList)[12], 0, (*idList)[16]);
        factory->manager()->setSon((*idList)[13], 1, (*idList)[14]);
        factory->manager()->setSon((*idList)[13], 0, (*idList)[16]);
        factory->manager()->setSon((*idList)[14], 1, (*idList)[18]);
        factory->manager()->setSon((*idList)[14], 0, (*idList)[16]);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    public:
    gum::LabelizedVariable *Cvar, *PLvar, *APUvar, *BPUvar, *ADRvar, *BDRvar,
       *BOvar, *Cprimevar;

    void setUp() {
      try {
        Cvar      = new gum::LabelizedVariable("C", "C", 2);
        PLvar     = new gum::LabelizedVariable("PL", "PL", 2);
        APUvar    = new gum::LabelizedVariable("APU", "APU", 2);
        BPUvar    = new gum::LabelizedVariable("BPU", "BPU", 2);
        ADRvar    = new gum::LabelizedVariable("ADR", "ADR", 2);
        BDRvar    = new gum::LabelizedVariable("BDR", "BDR", 2);
        BOvar     = new gum::LabelizedVariable("BO", "BO", 2);
        Cprimevar = new gum::LabelizedVariable("C'", "C'", 2);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }


    void tearDown() {
      try {
        delete Cvar;
        delete Cprimevar;
        delete PLvar;
        delete APUvar;
        delete BPUvar;
        delete ADRvar;
        delete BDRvar;
        delete BOvar;
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }


    /**
     * **********************************************************************************/
    /** Testing creation, filling and destrcution of a function graph */
    /**
     * **********************************************************************************/
    void test_Simple_Creation() {
      try {
        // *********************************************************************
        // Création of the Function Graph
        // *********************************************************************
        gum::MultiDimFunctionGraph< double >* functionGraph = NULL;

        TS_GUM_ASSERT_THROWS_NOTHING(
           functionGraph
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance());

        // *********************************************************************
        // Testing the insertion of nodes and arcs in the graph
        // The insertion are done so that no error should be rised
        // *********************************************************************
        gum::List< gum::NodeId > idList;
        TS_GUM_ASSERT_THROWS_NOTHING(fillFactory__(functionGraph, &idList));

        // *********************************************************************
        // Destruction of the multidim
        // *********************************************************************
        TS_GUM_ASSERT_THROWS_NOTHING(delete functionGraph);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }


    /**
     * **********************************************************************************/
    /** Testing access method in the graph */
    /**
     * **********************************************************************************/
    void test_MultiDimFunctionGraph_Diagram_Handlers_Methods() {
      try {
        // *********************************************************************
        // Création du multidim
        // *********************************************************************
        gum::MultiDimFunctionGraph< double >* functionGraph
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();
        gum::List< gum::NodeId > idList;
        fillFactory__(functionGraph, &idList);

        // *********************************************************************
        // Tests
        // *********************************************************************
        gum::LabelizedVariable* Banditovar
           = new gum::LabelizedVariable("Bandito", "Desperado", 2);

        // Test isTerminalNode
        TS_ASSERT_EQUALS(functionGraph->isTerminalNode(16), true);
        TS_ASSERT_EQUALS(functionGraph->isTerminalNode(6), false);
        TS_ASSERT_EQUALS(functionGraph->isTerminalNode(24), false);

        // Test isInternalNode
        TS_ASSERT_EQUALS(functionGraph->isInternalNode(6), true);
        TS_ASSERT_EQUALS(functionGraph->isInternalNode(16), false);
        TS_ASSERT_EQUALS(functionGraph->isInternalNode(24), false);

        // Test Root
        TS_ASSERT_EQUALS(functionGraph->root(), (gum::NodeId)1);

        // test terminal node value
        TS_GUM_ASSERT_THROWS_NOTHING(functionGraph->nodeValue(16));
        TS_ASSERT_EQUALS(functionGraph->nodeValue(16), 0);
        TS_ASSERT_THROWS(functionGraph->nodeValue(6), gum::InvalidArgument);
        TS_ASSERT_THROWS(functionGraph->nodeValue(24), gum::InvalidArgument);

        // Test internal node structure
        TS_GUM_ASSERT_THROWS_NOTHING(functionGraph->node(6));
        TS_ASSERT_THROWS(functionGraph->node(16), gum::InvalidArgument);
        TS_ASSERT_THROWS(functionGraph->node(24), gum::InvalidArgument);

        // Test var node List
        TS_GUM_ASSERT_THROWS_NOTHING(functionGraph->varNodeListe(Cvar));
        TS_ASSERT_THROWS(functionGraph->varNodeListe(Banditovar),
                         gum::InvalidArgument);

        // *********************************************************************
        // Cleaning
        // *********************************************************************
        TS_GUM_ASSERT_THROWS_NOTHING(delete functionGraph);
        delete Banditovar;
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }


    /**
     * **********************************************************************************/
    /** Testing export method */
    /**
     * **********************************************************************************/
    void test_toDot() {
      try {
        // *********************************************************************
        // Création du multidim
        // *********************************************************************
        gum::MultiDimFunctionGraph< double >* functionGraph
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();
        gum::List< gum::NodeId > idList;
        fillFactory__(functionGraph, &idList);

        // For comparison with what readers will return
        std::string   dotfile = GET_RESSOURCES_PATH("outputs/FunctionGraph.dot");
        std::ofstream output(dotfile.c_str(), std::ios::out | std::ios::trunc);

        if (!output.good()) {
          GUM_ERROR(gum::IOError, "Stream states flags are not all unset.")
        }

        output << functionGraph->toDot();

        output.flush();

        output.close();

        if (output.fail()) {
          GUM_ERROR(gum::IOError, "Writting in the ostream failed.")
        }

        delete functionGraph;
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }


    /**
     * *******************************************************************************/
    /**  Test on manager graph handling methods */
    /**
     * *******************************************************************************/
    void test_Manager_Graphical_Functions() {
      try {
        // *********************************************************************
        // Creation of the multidim
        // *********************************************************************
        gum::MultiDimFunctionGraph< double >* functionGraph
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();
        gum::List< gum::NodeId > idList;
        fillFactory__(functionGraph, &idList);


        // *********************************************************************
        // Testing insertion and removal nodes functions
        // *********************************************************************

        // Inserting a new terminal node with same value as an already inserted
        // one
        // (returned id must be the one of the already inserted)
        TS_ASSERT_EQUALS(functionGraph->manager()->addTerminalNode(10),
                         idList[16]);

        // Testing terminal node removal
        TS_GUM_ASSERT_THROWS_NOTHING(
           functionGraph->manager()->eraseNode(idList[15]));

        // Testing terminal node removal again
        TS_ASSERT_THROWS(functionGraph->manager()->eraseNode(idList[15]),
                         gum::NotFound);

        // Testing internal node removal
        TS_GUM_ASSERT_THROWS_NOTHING(
           functionGraph->manager()->eraseNode(idList[12]));

        // Testing internal node removal again
        TS_ASSERT_THROWS(functionGraph->manager()->eraseNode(idList[12]),
                         gum::NotFound);


        // *********************************************************************
        // Testing modifying arcs function
        // *********************************************************************

        // Testing arc modification where starting node does not exist
        // (must raise gum::NotFound)
        // (node 15 has been erased before)
        TS_ASSERT_THROWS(
           functionGraph->manager()->setSon(idList[15], 2, idList[1]),
           gum::NotFound);
        TS_ASSERT_THROWS(
           functionGraph->manager()->setSon(idList[1], 2, idList[15]),
           gum::NotFound);

        // Testing node modification where starting node is a terminal node
        // (must raise gum::InvalidNode
        // as terminal nodes are ... terminal)
        TS_ASSERT_THROWS(
           functionGraph->manager()->setSon(idList[16], 3, idList[1]),
           gum::InvalidNode);

        // Testing arc modification where variable modality is higher than its
        // domain size
        TS_ASSERT_THROWS(
           functionGraph->manager()->setSon(idList[8], 4, idList[18]),
           gum::InvalidArgument);

        // Testing arc modification that violates order constraint
        TS_ASSERT_THROWS(
           functionGraph->manager()->setSon(idList[10], 0, idList[1]),
           gum::OperationNotAllowed);
        // NB : idList 8 and 1 are bound to the same variable
        TS_ASSERT_THROWS(functionGraph->manager()->setSon(idList[8], 0, idList[1]),
                         gum::OperationNotAllowed);

        TS_GUM_ASSERT_THROWS_NOTHING(delete functionGraph);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }


    /**
     * **********************************************************************************/
    /**   Testing access and modification function on multidim */
    /**
     * **********************************************************************************/
    void test_MultiDimFunctionGraph_Accessors_Modifiers_Methods() {
      try {
        // *********************************************************************
        // Creation of multidim
        // *********************************************************************
        gum::MultiDimFunctionGraph< double >* functionGraph
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();
        gum::List< gum::NodeId > idList;
        fillFactory__(functionGraph, &idList);


        // *********************************************************************
        // Testing variablesSequence(), and if all variables are indeed present
        // *********************************************************************
        gum::Sequence< const gum::DiscreteVariable* > varSeq;
        TS_GUM_ASSERT_THROWS_NOTHING(varSeq = functionGraph->variablesSequence());
        TS_ASSERT_EQUALS(varSeq.exists(Cprimevar), true);
        TS_ASSERT_EQUALS(varSeq.exists(Cvar), true);
        TS_ASSERT_EQUALS(varSeq.exists(PLvar), true);
        TS_ASSERT_EQUALS(varSeq.exists(APUvar), true);
        TS_ASSERT_EQUALS(varSeq.exists(BPUvar), true);
        TS_ASSERT_EQUALS(varSeq.exists(ADRvar), true);
        TS_ASSERT_EQUALS(varSeq.exists(BDRvar), true);
        TS_ASSERT_EQUALS(varSeq.exists(BOvar), true);


        // *********************************************************************
        // Test name() and basename()
        // *********************************************************************
        TS_ASSERT_EQUALS(functionGraph->name(), "MultiDimFunctionGraph");

        TS_ASSERT_EQUALS(functionGraph->basename(), "MultiDimImplementation");


        // *********************************************************************
        // Test set(), fill() and populate()
        // *********************************************************************
        gum::Instantiation inst(*functionGraph);

        TS_ASSERT_THROWS(functionGraph->set(inst, 14.0), gum::OperationNotAllowed);

        TS_ASSERT_THROWS(functionGraph->fill(14.0), gum::OperationNotAllowed);

        std::vector< double > v;

        for (float i = 0; i < 128; i++)
          v.push_back(i);

        TS_ASSERT_THROWS(functionGraph->populate(v), gum::OperationNotAllowed);

        for (float i = 128; i < 256; i++)
          v.push_back(i);

        TS_ASSERT_THROWS(functionGraph->populate(v), gum::OperationNotAllowed);

        TS_ASSERT_THROWS(functionGraph->populate({1, 2, 3}),
                         gum::OperationNotAllowed);


        // *********************************************************************
        // Test get(), [], and value access
        // *********************************************************************
        TS_GUM_ASSERT_THROWS_NOTHING(functionGraph->get(inst));
        TS_GUM_ASSERT_THROWS_NOTHING((*functionGraph)[inst]);

        for (inst.setFirst(); !inst.end(); ++inst)
          if (inst.val(*Cvar) == 1) {
            if (inst.val(*Cprimevar) == 1) {
              TS_ASSERT_DELTA(functionGraph->get(inst), 9, 0.001);
            } else {
              TS_ASSERT_DELTA(functionGraph->get(inst), 1, 0.001);
            }
          } else {
            if (inst.val(*PLvar) == 1) {
              if (inst.val(*APUvar) == 1) {
                if (inst.val(*BPUvar) == 1) {
                  if (inst.val(*BOvar) == 1) {
                    if (inst.val(*Cprimevar) == 1) {
                      TS_ASSERT_DELTA(functionGraph->get(inst), 9, 0.001);
                    } else {
                      TS_ASSERT_DELTA(functionGraph->get(inst), 1, 0.001);
                    }

                  } else {
                    if (inst.val(*Cprimevar) == 1) {
                      TS_ASSERT_DELTA(functionGraph->get(inst), 0, 0.001);
                    } else {
                      TS_ASSERT_DELTA(functionGraph->get(inst), 10, 0.001);
                    }
                  }
                } else {
                  if (inst.val(*Cprimevar) == 1) {
                    TS_ASSERT_DELTA(functionGraph->get(inst), 0, 0.001);
                  } else {
                    TS_ASSERT_DELTA(functionGraph->get(inst), 10, 0.001);
                  }
                }
              } else {
                if (inst.val(*ADRvar) == 1) {
                  if (inst.val(*BDRvar) == 1) {
                    if (inst.val(*BOvar) == 1) {
                      if (inst.val(*Cprimevar) == 1) {
                        TS_ASSERT_DELTA(functionGraph->get(inst), 9, 0.001);
                      } else {
                        TS_ASSERT_DELTA(functionGraph->get(inst), 1, 0.001);
                      }

                    } else {
                      if (inst.val(*Cprimevar) == 1) {
                        TS_ASSERT_DELTA(functionGraph->get(inst), 0, 0.001);
                      } else {
                        TS_ASSERT_DELTA(functionGraph->get(inst), 10, 0.001);
                      }
                    }

                  } else {
                    if (inst.val(*Cprimevar) == 1) {
                      TS_ASSERT_DELTA(functionGraph->get(inst), 0, 0.001);
                    } else {
                      TS_ASSERT_DELTA(functionGraph->get(inst), 10, 0.001);
                    }
                  }

                } else {
                  if (inst.val(*Cprimevar) == 1) {
                    TS_ASSERT_DELTA(functionGraph->get(inst), 0, 0.001);
                  } else {
                    TS_ASSERT_DELTA(functionGraph->get(inst), 10, 0.001);
                  }
                }
              }
            } else {
              if (inst.val(*ADRvar) == 1) {
                if (inst.val(*BDRvar) == 1) {
                  if (inst.val(*BOvar) == 1) {
                    if (inst.val(*Cprimevar) == 1) {
                      TS_ASSERT_DELTA(functionGraph->get(inst), 9, 0.001);
                    } else {
                      TS_ASSERT_DELTA(functionGraph->get(inst), 1, 0.001);
                    }

                  } else {
                    if (inst.val(*Cprimevar) == 1) {
                      TS_ASSERT_DELTA(functionGraph->get(inst), 0, 0.001);
                    } else {
                      TS_ASSERT_DELTA(functionGraph->get(inst), 10, 0.001);
                    }
                  }

                } else {
                  if (inst.val(*Cprimevar) == 1) {
                    TS_ASSERT_DELTA(functionGraph->get(inst), 0, 0.001);
                  } else {
                    TS_ASSERT_DELTA(functionGraph->get(inst), 10, 0.001);
                  }
                }

              } else {
                if (inst.val(*Cprimevar) == 1) {
                  TS_ASSERT_DELTA(functionGraph->get(inst), 0, 0.001);
                } else {
                  TS_ASSERT_DELTA(functionGraph->get(inst), 10, 0.001);
                }
              }
            }
          }


        // *********************************************************************
        // Testing replace()
        // *********************************************************************
        gum::LabelizedVariable* lv
           = new gum::LabelizedVariable("Test", "Cornichon", 2);
        TS_ASSERT_THROWS(functionGraph->replace(*Cvar, *lv),
                         gum::OperationNotAllowed);
        delete lv;

        TS_GUM_ASSERT_THROWS_NOTHING(delete functionGraph);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }


    /**
     * **********************************************************************************/
    /**   Tests on Function Graph implementation of MultiDimImplementation */
    /**
     * **********************************************************************************/
    void test_MultiDimFunctionGraph_Implementation_Methods() {
      try {
        // *********************************************************************
        // Creation of the multidim
        // *********************************************************************
        gum::MultiDimFunctionGraph< double >* functionGraph
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();
        gum::List< gum::NodeId > idList;
        fillFactory__(functionGraph, &idList);
        gum::LabelizedVariable* lv
           = new gum::LabelizedVariable("Test", "Cornichon", 2);

        // *********************************************************************
        // Test nbrDim(), domainSize(), realSize()
        // *********************************************************************

        TS_ASSERT_EQUALS(functionGraph->nbrDim(), (gum::Idx)8);

        TS_ASSERT_EQUALS(functionGraph->domainSize(), (gum::Size)256);

        TS_ASSERT_EQUALS(functionGraph->realSize(), (gum::Size)15);   // 19);

        TS_GUM_ASSERT_THROWS_NOTHING(functionGraph->compressionRate());

        TS_GUM_ASSERT_THROWS_NOTHING(functionGraph->variable(0));

        TS_GUM_ASSERT_THROWS_NOTHING(functionGraph->pos(*Cvar));

        TS_ASSERT_EQUALS(functionGraph->contains(*Cprimevar), true);
        TS_ASSERT_EQUALS(functionGraph->contains(*lv), false);

        TS_ASSERT_EQUALS(functionGraph->empty(), false);

        // *********************************************************************
        // Test add(), erase()
        // *********************************************************************

        TS_GUM_ASSERT_THROWS_NOTHING(functionGraph->add(*lv));
        TS_ASSERT_EQUALS(functionGraph->contains(*lv), true);


        TS_GUM_ASSERT_THROWS_NOTHING(functionGraph->erase(*Cvar));
        TS_ASSERT_EQUALS(functionGraph->contains(*Cvar), false);


        TS_GUM_ASSERT_THROWS_NOTHING(delete functionGraph);
        delete lv;
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }


    /**
     * **********************************************************************************/
    /**   Tests on Copy Methods */
    /**
     * **********************************************************************************/
    void test_MultiDimFunctionGraph_Copy_Methods() {
      try {
        // *********************************************************************
        // Création du multidim
        // *********************************************************************
        gum::MultiDimFunctionGraph< double >* functionGraph
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();
        gum::List< gum::NodeId > idList;
        fillFactory__(functionGraph, &idList);


        // *********************************************************************
        // Creation of the multidim that we use to make the copy
        // *********************************************************************
        gum::MultiDimFunctionGraph< double >* functionGraph2
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();

        // *********************************************************************
        // Test on copy of other multidim (must raise OperationNotAllowed)
        // *********************************************************************
        gum::MultiDimArray< double >* array = new gum::MultiDimArray< double >();

        TS_ASSERT_THROWS(functionGraph2->copy(*array), gum::OperationNotAllowed)
        TS_ASSERT_THROWS(functionGraph2->copyFrom(*array),
                         gum::OperationNotAllowed)

        delete array;

        // *********************************************************************
        // Testing MultiDimFunctionGraph Duplication
        // *********************************************************************
        TS_ASSERT_THROWS(functionGraph2->copyFrom(*functionGraph),
                         gum::OperationNotAllowed);

        TS_GUM_ASSERT_THROWS_NOTHING(functionGraph2->copy(*functionGraph);)

        TS_GUM_ASSERT_THROWS_NOTHING(*functionGraph2 = *functionGraph;)

        // *********************************************************************
        // Testing CopyAndReassign method
        // *********************************************************************
        gum::LabelizedVariable *Bijouvar, *Caillouvar, *Chouvar, *Genouvar,
           *Hibouvar, *Joujouvar, *Pouvar, *Bijouprimevar;
        Bijouvar = new gum::LabelizedVariable("Bijou", "Un bijou, des bijoux", 2);
        Caillouvar
           = new gum::LabelizedVariable("Caillou", "Un caillou, des cailloux", 2);
        Chouvar  = new gum::LabelizedVariable("Chouvar", "Un chou, des choux", 2);
        Genouvar = new gum::LabelizedVariable("Genou", "Un genou, des genoux", 2);
        Hibouvar = new gum::LabelizedVariable("Hibou", "Un hibou, des hiboux", 2);
        Joujouvar
           = new gum::LabelizedVariable("Joujou", "Un joujou, des joujoux", 2);
        Pouvar = new gum::LabelizedVariable("Pouvar", "Un pou, des poux", 2);
        Bijouprimevar
           = new gum::LabelizedVariable("Bijou'", "Un bijou', des bijou'x", 2);

        gum::Bijection< const gum::DiscreteVariable*,
                        const gum::DiscreteVariable* >
           map;
        map.insert(Cvar, Bijouvar);
        map.insert(Cprimevar, Bijouprimevar);
        map.insert(PLvar, Caillouvar);
        map.insert(APUvar, Chouvar);
        map.insert(BPUvar, Genouvar);
        map.insert(ADRvar, Hibouvar);
        map.insert(BDRvar, Joujouvar);
        map.insert(BOvar, Pouvar);

        TS_GUM_ASSERT_THROWS_NOTHING(
           functionGraph2->copyAndReassign(*functionGraph, map););

        TS_GUM_ASSERT_THROWS_NOTHING(functionGraph2->clear(););

        map.eraseFirst(PLvar);
        gum::LabelizedVariable* failvar
           = new gum::LabelizedVariable("Fail", "Different domain size", 3);
        map.insert(PLvar, failvar);
        TS_ASSERT_THROWS(functionGraph2->copyAndReassign(*functionGraph, map),
                         gum::InvalidArgument);

        TS_GUM_ASSERT_THROWS_NOTHING(functionGraph2->clear(););
        delete failvar;

        delete Bijouvar;
        delete Bijouprimevar;
        delete Caillouvar;
        delete Chouvar;
        delete Genouvar;
        delete Hibouvar;
        delete Joujouvar;
        delete Pouvar;

        // *********************************************************************
        // Testing copyAndMultiplyByScalar method
        // *********************************************************************
        TS_GUM_ASSERT_THROWS_NOTHING(
           functionGraph2->copyAndMultiplyByScalar(*functionGraph, 2);)

        TS_GUM_ASSERT_THROWS_NOTHING(
           functionGraph2->copyAndMultiplyByScalar(*functionGraph, 0);)


        TS_GUM_ASSERT_THROWS_NOTHING(delete functionGraph);

        TS_GUM_ASSERT_THROWS_NOTHING(delete functionGraph2);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }


    /**
     * **********************************************************************************/
    /**   Tests on several miscelleanous methods */
    /**
     * **********************************************************************************/
    void test_MultiDimFunctionGraph_Various_Methods() {
      try {
        // *********************************************************************
        // Creation of severals multidims
        // *********************************************************************
        gum::MultiDimFunctionGraph< double >* dg1
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();
        gum::MultiDimFunctionGraph< double >* dg2
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();
        gum::MultiDimFunctionGraph< double >* dg3
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();
        gum::List< gum::NodeId > idList;
        fillFactory__(dg1, &idList);
        fillFactory__(dg2, &idList);
        fillFactory__(dg3, &idList);

        dg3->manager()->setSon(3, 0, 16);

        // *********************************************************************
        // test tostring() and tostring(gum::Instantiation*)
        // *********************************************************************
        //        TS_GUM_ASSERT_THROWS_NOTHING (dg1->toString());

        gum::Instantiation inst(*dg1);
        inst.setFirst();
        TS_GUM_ASSERT_THROWS_NOTHING(dg1->toString(&inst));


        // *********************************************************************
        // test == et !=
        // *********************************************************************
        TS_ASSERT_EQUALS(*dg1 == *dg2, true);
        TS_ASSERT_EQUALS(*dg1 == *dg3, false);

        TS_ASSERT_EQUALS(*dg1 != *dg2, false);
        TS_ASSERT_EQUALS(*dg1 != *dg3, true);


        TS_GUM_ASSERT_THROWS_NOTHING(delete dg1);
        TS_GUM_ASSERT_THROWS_NOTHING(delete dg2);
        TS_GUM_ASSERT_THROWS_NOTHING(delete dg3);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }


    /**
     * **********************************************************************************/
    /**   Tests on moveTo function and minimize */
    /**
     * **********************************************************************************/
    void test_MoveTo() {
      try {
        // *********************************************************************
        // Creation of the multidim
        // *********************************************************************
        gum::MultiDimFunctionGraph< double >* functionGraph
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();
        gum::List< gum::NodeId > idList;
        fillFactory__(functionGraph, &idList);

        // *********************************************************************
        // Testing moveTo function
        // *********************************************************************
        // TS_GUM_ASSERT_THROWS_NOTHING(functionGraph->manager()->moveTo(
        //  Cprimevar, functionGraph->variablesSequence().pos(BOvar)));
        functionGraph->manager()->moveTo(
           Cprimevar,
           functionGraph->variablesSequence().pos(BOvar));

        // For comparison with what readers will return
        std::string dotfile
           = GET_RESSOURCES_PATH("outputs/FunctionGraphMVBOvar.dot");
        std::ofstream output(dotfile.c_str(), std::ios::out | std::ios::trunc);
        if (!output.good()) {
          GUM_ERROR(gum::IOError, "Stream states flags are not all unset.")
        }
        output << functionGraph->toDot(true);
        output.flush();
        output.close();
        if (output.fail()) {
          GUM_ERROR(gum::IOError, "Writting in the ostream failed.")
        }

        gum::LabelizedVariable* phantomvar
           = new gum::LabelizedVariable("Phantom", "Not here, nor there", 2);
        functionGraph->add(*phantomvar);
        TS_GUM_ASSERT_THROWS_NOTHING(functionGraph->manager()->moveTo(
           Cprimevar,
           functionGraph->variablesSequence().pos(phantomvar)));

        delete functionGraph;
        delete phantomvar;
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    void xtest_Generator() {
      try {
        for (gum::Idx i = 0; i < 100; i++) {
          gum::Sequence< const gum::DiscreteVariable* >* varList
             = new gum::Sequence< const gum::DiscreteVariable* >();

          for (int j = 0; j < 10; j++) {
            std::stringstream varName;
            varName << "var" << j;
            varList->insert(
               new gum::LabelizedVariable(varName.str(), "", 2 + rand() % 4));
          }

          gum::MultiDimFunctionGraphGenerator gene(2, 5, *varList);

          gum::MultiDimFunctionGraph< double >* dg1 = nullptr;
          TS_GUM_ASSERT_THROWS_NOTHING(dg1 = gene.generate());

          gum::MultiDimFunctionGraph< double >* dg2 = gum::MultiDimFunctionGraph<
             double >::getReducedAndOrderedInstance();

          TS_GUM_ASSERT_THROWS_NOTHING(dg2->copy(*dg1));
          TS_GUM_ASSERT_THROWS_NOTHING(dg2->manager()->reduce());

          gum::Instantiation inst(dg1);
          for (inst.setFirst(); !inst.end(); ++inst) {
            TS_ASSERT_DELTA(dg1->get(inst), dg2->get(inst), 0.001);
          }


          dg2->manager()->clean();
          for (inst.setFirst(); !inst.end(); ++inst) {
            TS_ASSERT_DELTA(dg1->get(inst), dg2->get(inst), 0.001);
          }

          delete dg1;
          delete dg2;

          for (gum::SequenceIterator< const gum::DiscreteVariable* > ite
               = varList->begin();
               ite != varList->end();
               ++ite)
            delete *ite;
          delete varList;
        }
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }
  };
}   // namespace gum_tests
