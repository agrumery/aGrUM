/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


// =========================================================================
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
// =========================================================================
#include <gumtest/AgrumTestSuite.h>

#include "utils.h"
// =========================================================================
#include <agrum/base/multidim/implementations/multiDimArray.h>
#include <agrum/base/multidim/implementations/multiDimFunctionGraph.h>
#include <agrum/base/multidim/implementations/multiDimFunctionGraphGenerator.h>
#include <agrum/base/multidim/implementations/multiDimFunctionGraphManager.h>
#include <agrum/base/multidim/instantiation.h>
// =========================================================================
#include <agrum/base/variables/labelizedVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  MultiDimFunctionGraph
#define GUM_CURRENT_MODULE GUMBASE

// =========================================================================

namespace gum_tests {

  struct MultiDimFunctionGraphTestSuite {
    private:
    void _fillFactory_(gum::MultiDimFunctionGraph< double >* factory,
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
        idList->insert(factory->manager()->addInternalNode(Cvar));        // 2
        idList->insert(factory->manager()->addInternalNode(PLvar));       // 3
        idList->insert(factory->manager()->addInternalNode(APUvar));      // 4
        idList->insert(factory->manager()->addInternalNode(BPUvar));      // 5
        idList->insert(factory->manager()->addInternalNode(ADRvar));      // 6
        idList->insert(factory->manager()->addInternalNode(BDRvar));      // 7
        idList->insert(factory->manager()->addInternalNode(BOvar));       // 8

        idList->insert(factory->manager()->addInternalNode(Cvar));        // 9
        idList->insert(factory->manager()->addInternalNode(PLvar));       // 10
        idList->insert(factory->manager()->addInternalNode(APUvar));      // 11
        idList->insert(factory->manager()->addInternalNode(BPUvar));      // 12
        idList->insert(factory->manager()->addInternalNode(ADRvar));      // 13
        idList->insert(factory->manager()->addInternalNode(BDRvar));      // 14
        idList->insert(factory->manager()->addInternalNode(BOvar));       // 15

        idList->insert(factory->manager()->addTerminalNode(0));           // 16
        idList->insert(factory->manager()->addTerminalNode(10));          // 17
        idList->insert(factory->manager()->addTerminalNode(9));           // 18
        idList->insert(factory->manager()->addTerminalNode(1));           // 19


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
    gum::LabelizedVariable *Cvar, *PLvar, *APUvar, *BPUvar, *ADRvar, *BDRvar, *BOvar, *Cprimevar;

    MultiDimFunctionGraphTestSuite() {
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

    ~MultiDimFunctionGraphTestSuite() {
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

        GUM_CHECK_ASSERT_THROWS_NOTHING(
            functionGraph = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance());

        // *********************************************************************
        // Testing the insertion of nodes and arcs in the graph
        // The insertion are done so that no error should be rised
        // *********************************************************************
        gum::List< gum::NodeId > idList;
        GUM_CHECK_ASSERT_THROWS_NOTHING(_fillFactory_(functionGraph, &idList));

        // *********************************************************************
        // Destruction of the multidim
        // *********************************************************************
        GUM_CHECK_ASSERT_THROWS_NOTHING(delete functionGraph);
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
        _fillFactory_(functionGraph, &idList);

        // *********************************************************************
        // Tests
        // *********************************************************************
        gum::LabelizedVariable* Banditovar = new gum::LabelizedVariable("Bandito", "Desperado", 2);

        // Test isTerminalNode
        CHECK((functionGraph->isTerminalNode(16)) == (true));
        CHECK((functionGraph->isTerminalNode(6)) == (false));
        CHECK((functionGraph->isTerminalNode(24)) == (false));

        // Test isInternalNode
        CHECK((functionGraph->isInternalNode(6)) == (true));
        CHECK((functionGraph->isInternalNode(16)) == (false));
        CHECK((functionGraph->isInternalNode(24)) == (false));

        // Test Root
        CHECK((functionGraph->root()) == ((gum::NodeId)1));

        // test terminal node value
        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph->nodeValue(16));
        CHECK((functionGraph->nodeValue(16)) == (0));
        CHECK_THROWS_AS(functionGraph->nodeValue(6), const gum::InvalidArgument&);
        CHECK_THROWS_AS(functionGraph->nodeValue(24), const gum::InvalidArgument&);

        // Test internal node structure
        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph->node(6));
        CHECK_THROWS_AS(functionGraph->node(16), const gum::InvalidArgument&);
        CHECK_THROWS_AS(functionGraph->node(24), const gum::InvalidArgument&);

        // Test var node List
        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph->varNodeListe(Cvar));
        CHECK_THROWS_AS(functionGraph->varNodeListe(Banditovar), const gum::InvalidArgument&);

        // *********************************************************************
        // Cleaning
        // *********************************************************************
        GUM_CHECK_ASSERT_THROWS_NOTHING(delete functionGraph);
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
        _fillFactory_(functionGraph, &idList);

        // For comparison with what readers will return
        std::string   dotfile = GET_RESSOURCES_PATH("outputs/FunctionGraph.dot");
        std::ofstream output(dotfile.c_str(), std::ios::out | std::ios::trunc);

        if (!output.good()) { GUM_ERROR(gum::IOError, "Stream states flags are not all unset.") }

        output << functionGraph->toDot();

        output.flush();

        output.close();

        if (output.fail()) { GUM_ERROR(gum::IOError, "Writting in the ostream failed.") }

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
        _fillFactory_(functionGraph, &idList);


        // *********************************************************************
        // Testing insertion and removal nodes functions
        // *********************************************************************

        // Inserting a new terminal node with same value as an already inserted
        // one
        // (returned id must be the one of the already inserted)
        CHECK((functionGraph->manager()->addTerminalNode(10)) == (idList[16]));

        // Testing terminal node removal
        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph->manager()->eraseNode(idList[15]));

        // Testing terminal node removal again
        CHECK_THROWS_AS(functionGraph->manager()->eraseNode(idList[15]), const gum::NotFound&);

        // Testing internal node removal
        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph->manager()->eraseNode(idList[12]));

        // Testing internal node removal again
        CHECK_THROWS_AS(functionGraph->manager()->eraseNode(idList[12]), const gum::NotFound&);


        // *********************************************************************
        // Testing modifying arcs function
        // *********************************************************************

        // Testing arc modification where starting node does not exist
        // (must raise gum::NotFound)
        // (node 15 has been erased before)
        CHECK_THROWS_AS(functionGraph->manager()->setSon(idList[15], 2, idList[1]),
                        const gum::NotFound&);
        CHECK_THROWS_AS(functionGraph->manager()->setSon(idList[1], 2, idList[15]),
                        const gum::NotFound&);

        // Testing node modification where starting node is a terminal node
        // (must raise gum::InvalidNode
        // as terminal nodes are ... terminal)
        CHECK_THROWS_AS(functionGraph->manager()->setSon(idList[16], 3, idList[1]),
                        const gum::InvalidNode&);

        // Testing arc modification where variable modality is higher than its
        // domain size
        CHECK_THROWS_AS(functionGraph->manager()->setSon(idList[8], 4, idList[18]),
                        const gum::InvalidArgument&);

        // Testing arc modification that violates order constraint
        CHECK_THROWS_AS(functionGraph->manager()->setSon(idList[10], 0, idList[1]),
                        const gum::OperationNotAllowed&);
        // NB : idList 8 and 1 are bound to the same variable
        CHECK_THROWS_AS(functionGraph->manager()->setSon(idList[8], 0, idList[1]),
                        const gum::OperationNotAllowed&);

        GUM_CHECK_ASSERT_THROWS_NOTHING(delete functionGraph);
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
        _fillFactory_(functionGraph, &idList);


        // *********************************************************************
        // Testing variablesSequence(), and if all variables are indeed present
        // *********************************************************************
        gum::Sequence< const gum::DiscreteVariable* > varSeq;
        GUM_CHECK_ASSERT_THROWS_NOTHING(varSeq = functionGraph->variablesSequence());
        CHECK((varSeq.exists(Cprimevar)) == (true));
        CHECK((varSeq.exists(Cvar)) == (true));
        CHECK((varSeq.exists(PLvar)) == (true));
        CHECK((varSeq.exists(APUvar)) == (true));
        CHECK((varSeq.exists(BPUvar)) == (true));
        CHECK((varSeq.exists(ADRvar)) == (true));
        CHECK((varSeq.exists(BDRvar)) == (true));
        CHECK((varSeq.exists(BOvar)) == (true));


        // *********************************************************************
        // Test name() and basename()
        // *********************************************************************
        CHECK((functionGraph->name()) == ("MultiDimFunctionGraph"));

        CHECK((functionGraph->basename()) == ("MultiDimImplementation"));


        // *********************************************************************
        // Test set(), fill() and populate()
        // *********************************************************************
        gum::Instantiation inst(*functionGraph);

        CHECK_THROWS_AS(functionGraph->set(inst, 14.0), const gum::OperationNotAllowed&);

        CHECK_THROWS_AS(functionGraph->fill(14.0), const gum::OperationNotAllowed&);

        std::vector< double > v;

        for (float i = 0; i < 128; i++)
          v.push_back(i);

        CHECK_THROWS_AS(functionGraph->populate(v), const gum::OperationNotAllowed&);

        for (float i = 128; i < 256; i++)
          v.push_back(i);

        CHECK_THROWS_AS(functionGraph->populate(v), const gum::OperationNotAllowed&);

        CHECK_THROWS_AS(functionGraph->populate({1, 2, 3}), const gum::OperationNotAllowed&);


        // *********************************************************************
        // Test get(), [], and value access
        // *********************************************************************
        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph->get(inst));
        GUM_CHECK_ASSERT_THROWS_NOTHING((*functionGraph)[inst]);

        for (inst.setFirst(); !inst.end(); ++inst)
          if (inst.val(*Cvar) == 1) {
            if (inst.val(*Cprimevar) == 1) {
              CHECK((functionGraph->get(inst)) == doctest::Approx(9).epsilon(0.001));
            } else {
              CHECK((functionGraph->get(inst)) == doctest::Approx(1).epsilon(0.001));
            }
          } else {
            if (inst.val(*PLvar) == 1) {
              if (inst.val(*APUvar) == 1) {
                if (inst.val(*BPUvar) == 1) {
                  if (inst.val(*BOvar) == 1) {
                    if (inst.val(*Cprimevar) == 1) {
                      CHECK((functionGraph->get(inst)) == doctest::Approx(9).epsilon(0.001));
                    } else {
                      CHECK((functionGraph->get(inst)) == doctest::Approx(1).epsilon(0.001));
                    }

                  } else {
                    if (inst.val(*Cprimevar) == 1) {
                      CHECK((functionGraph->get(inst)) == doctest::Approx(0).epsilon(0.001));
                    } else {
                      CHECK((functionGraph->get(inst)) == doctest::Approx(10).epsilon(0.001));
                    }
                  }
                } else {
                  if (inst.val(*Cprimevar) == 1) {
                    CHECK((functionGraph->get(inst)) == doctest::Approx(0).epsilon(0.001));
                  } else {
                    CHECK((functionGraph->get(inst)) == doctest::Approx(10).epsilon(0.001));
                  }
                }
              } else {
                if (inst.val(*ADRvar) == 1) {
                  if (inst.val(*BDRvar) == 1) {
                    if (inst.val(*BOvar) == 1) {
                      if (inst.val(*Cprimevar) == 1) {
                        CHECK((functionGraph->get(inst)) == doctest::Approx(9).epsilon(0.001));
                      } else {
                        CHECK((functionGraph->get(inst)) == doctest::Approx(1).epsilon(0.001));
                      }

                    } else {
                      if (inst.val(*Cprimevar) == 1) {
                        CHECK((functionGraph->get(inst)) == doctest::Approx(0).epsilon(0.001));
                      } else {
                        CHECK((functionGraph->get(inst)) == doctest::Approx(10).epsilon(0.001));
                      }
                    }

                  } else {
                    if (inst.val(*Cprimevar) == 1) {
                      CHECK((functionGraph->get(inst)) == doctest::Approx(0).epsilon(0.001));
                    } else {
                      CHECK((functionGraph->get(inst)) == doctest::Approx(10).epsilon(0.001));
                    }
                  }

                } else {
                  if (inst.val(*Cprimevar) == 1) {
                    CHECK((functionGraph->get(inst)) == doctest::Approx(0).epsilon(0.001));
                  } else {
                    CHECK((functionGraph->get(inst)) == doctest::Approx(10).epsilon(0.001));
                  }
                }
              }
            } else {
              if (inst.val(*ADRvar) == 1) {
                if (inst.val(*BDRvar) == 1) {
                  if (inst.val(*BOvar) == 1) {
                    if (inst.val(*Cprimevar) == 1) {
                      CHECK((functionGraph->get(inst)) == doctest::Approx(9).epsilon(0.001));
                    } else {
                      CHECK((functionGraph->get(inst)) == doctest::Approx(1).epsilon(0.001));
                    }

                  } else {
                    if (inst.val(*Cprimevar) == 1) {
                      CHECK((functionGraph->get(inst)) == doctest::Approx(0).epsilon(0.001));
                    } else {
                      CHECK((functionGraph->get(inst)) == doctest::Approx(10).epsilon(0.001));
                    }
                  }

                } else {
                  if (inst.val(*Cprimevar) == 1) {
                    CHECK((functionGraph->get(inst)) == doctest::Approx(0).epsilon(0.001));
                  } else {
                    CHECK((functionGraph->get(inst)) == doctest::Approx(10).epsilon(0.001));
                  }
                }

              } else {
                if (inst.val(*Cprimevar) == 1) {
                  CHECK((functionGraph->get(inst)) == doctest::Approx(0).epsilon(0.001));
                } else {
                  CHECK((functionGraph->get(inst)) == doctest::Approx(10).epsilon(0.001));
                }
              }
            }
          }


        // *********************************************************************
        // Testing replace()
        // *********************************************************************
        gum::LabelizedVariable* lv = new gum::LabelizedVariable("Test", "Cornichon", 2);
        CHECK_THROWS_AS(functionGraph->replace(*Cvar, *lv), const gum::OperationNotAllowed&);
        delete lv;

        GUM_CHECK_ASSERT_THROWS_NOTHING(delete functionGraph);
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
        _fillFactory_(functionGraph, &idList);
        gum::LabelizedVariable* lv = new gum::LabelizedVariable("Test", "Cornichon", 2);

        // *********************************************************************
        // Test nbrDim(), domainSize(), realSize()
        // *********************************************************************

        CHECK((functionGraph->nbrDim()) == (static_cast< gum::Idx >(8)));

        CHECK((functionGraph->domainSize()) == (static_cast< gum::Size >(256)));

        CHECK((functionGraph->realSize()) == (static_cast< gum::Size >(15)));   // 19)

        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph->compressionRate());

        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph->variable(0));

        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph->pos(*Cvar));

        CHECK((functionGraph->contains(*Cprimevar)) == (true));
        CHECK((functionGraph->contains(*lv)) == (false));

        CHECK((functionGraph->empty()) == (false));

        // *********************************************************************
        // Test add(), erase()
        // *********************************************************************

        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph->add(*lv));
        CHECK((functionGraph->contains(*lv)) == (true));


        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph->erase(*Cvar));
        CHECK((functionGraph->contains(*Cvar)) == (false));


        GUM_CHECK_ASSERT_THROWS_NOTHING(delete functionGraph);
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
        _fillFactory_(functionGraph, &idList);


        // *********************************************************************
        // Creation of the multidim that we use to make the copy
        // *********************************************************************
        gum::MultiDimFunctionGraph< double >* functionGraph2
            = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();

        // *********************************************************************
        // Test on copy of other multidim (must raise OperationNotAllowed)
        // *********************************************************************
        gum::MultiDimArray< double >* array = new gum::MultiDimArray< double >();

        CHECK_THROWS_AS(functionGraph2->copy(*array), const gum::OperationNotAllowed&);
        CHECK_THROWS_AS(functionGraph2->copyFrom(*array), const gum::OperationNotAllowed&);

        delete array;

        // *********************************************************************
        // Testing MultiDimFunctionGraph Duplication
        // *********************************************************************
        CHECK_THROWS_AS(functionGraph2->copyFrom(*functionGraph), const gum::OperationNotAllowed&);

        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph2->copy(*functionGraph););

        GUM_CHECK_ASSERT_THROWS_NOTHING(*functionGraph2 = *functionGraph;);

        // *********************************************************************
        // Testing CopyAndReassign method
        // *********************************************************************
        gum::LabelizedVariable *Bijouvar, *Caillouvar, *Chouvar, *Genouvar, *Hibouvar, *Joujouvar,
            *Pouvar, *Bijouprimevar;
        Bijouvar      = new gum::LabelizedVariable("Bijou", "Un bijou, des bijoux", 2);
        Caillouvar    = new gum::LabelizedVariable("Caillou", "Un caillou, des cailloux", 2);
        Chouvar       = new gum::LabelizedVariable("Chouvar", "Un chou, des choux", 2);
        Genouvar      = new gum::LabelizedVariable("Genou", "Un genou, des genoux", 2);
        Hibouvar      = new gum::LabelizedVariable("Hibou", "Un hibou, des hiboux", 2);
        Joujouvar     = new gum::LabelizedVariable("Joujou", "Un joujou, des joujoux", 2);
        Pouvar        = new gum::LabelizedVariable("Pouvar", "Un pou, des poux", 2);
        Bijouprimevar = new gum::LabelizedVariable("Bijou'", "Un bijou', des bijou'x", 2);

        gum::Bijection< const gum::DiscreteVariable*, const gum::DiscreteVariable* > map;
        map.insert(Cvar, Bijouvar);
        map.insert(Cprimevar, Bijouprimevar);
        map.insert(PLvar, Caillouvar);
        map.insert(APUvar, Chouvar);
        map.insert(BPUvar, Genouvar);
        map.insert(ADRvar, Hibouvar);
        map.insert(BDRvar, Joujouvar);
        map.insert(BOvar, Pouvar);

        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph2->copyAndReassign(*functionGraph, map););

        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph2->clear(););

        map.eraseFirst(PLvar);
        gum::LabelizedVariable* failvar
            = new gum::LabelizedVariable("Fail", "Different domain size", 3);
        map.insert(PLvar, failvar);
        CHECK_THROWS_AS(functionGraph2->copyAndReassign(*functionGraph, map),
                        const gum::InvalidArgument&);

        GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph2->clear(););
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
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            functionGraph2->copyAndMultiplyByScalar(*functionGraph, 2););

        GUM_CHECK_ASSERT_THROWS_NOTHING(
            functionGraph2->copyAndMultiplyByScalar(*functionGraph, 0););


        GUM_CHECK_ASSERT_THROWS_NOTHING(delete functionGraph);

        GUM_CHECK_ASSERT_THROWS_NOTHING(delete functionGraph2);
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
        _fillFactory_(dg1, &idList);
        _fillFactory_(dg2, &idList);
        _fillFactory_(dg3, &idList);

        dg3->manager()->setSon(3, 0, 16);

        // *********************************************************************
        // test tostring() and tostring(gum::Instantiation*)
        // *********************************************************************
        //        GUM_CHECK_ASSERT_THROWS_NOTHING (dg1->toString());

        gum::Instantiation inst(*dg1);
        inst.setFirst();
        GUM_CHECK_ASSERT_THROWS_NOTHING(dg1->toString(&inst));


        // *********************************************************************
        // test == et !=
        // *********************************************************************
        CHECK((*dg1 == *dg2) == (true));
        CHECK((*dg1 == *dg3) == (false));

        CHECK((*dg1 != *dg2) == (false));
        CHECK((*dg1 != *dg3) == (true));


        GUM_CHECK_ASSERT_THROWS_NOTHING(delete dg1);
        GUM_CHECK_ASSERT_THROWS_NOTHING(delete dg2);
        GUM_CHECK_ASSERT_THROWS_NOTHING(delete dg3);
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
        _fillFactory_(functionGraph, &idList);

        // *********************************************************************
        // Testing moveTo function
        // *********************************************************************
        // GUM_CHECK_ASSERT_THROWS_NOTHING(functionGraph->manager()->moveTo(
        //  Cprimevar, functionGraph->variablesSequence().pos(BOvar)));
        functionGraph->manager()->moveTo(Cprimevar, functionGraph->variablesSequence().pos(BOvar));

        // For comparison with what readers will return
        std::string   dotfile = GET_RESSOURCES_PATH("outputs/FunctionGraphMVBOvar.dot");
        std::ofstream output(dotfile.c_str(), std::ios::out | std::ios::trunc);
        if (!output.good()) { GUM_ERROR(gum::IOError, "Stream states flags are not all unset.") }
        output << functionGraph->toDot(true);
        output.flush();
        output.close();
        if (output.fail()) { GUM_ERROR(gum::IOError, "Writting in the ostream failed.") }

        gum::LabelizedVariable* phantomvar
            = new gum::LabelizedVariable("Phantom", "Not here, nor there", 2);
        functionGraph->add(*phantomvar);
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            functionGraph->manager()->moveTo(Cprimevar,
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
            varList->insert(new gum::LabelizedVariable(varName.str(), "", 2 + rand() % 4));
          }

          gum::MultiDimFunctionGraphGenerator gene(2, 5, *varList);

          gum::MultiDimFunctionGraph< double >* dg1 = nullptr;
          GUM_CHECK_ASSERT_THROWS_NOTHING(dg1 = gene.generate());

          gum::MultiDimFunctionGraph< double >* dg2
              = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();

          GUM_CHECK_ASSERT_THROWS_NOTHING(dg2->copy(*dg1));
          GUM_CHECK_ASSERT_THROWS_NOTHING(dg2->manager()->reduce());

          gum::Instantiation inst(dg1);
          for (inst.setFirst(); !inst.end(); ++inst) {
            CHECK((dg1->get(inst)) == doctest::Approx(dg2->get(inst)).epsilon(0.001));
          }


          dg2->manager()->clean();
          for (inst.setFirst(); !inst.end(); ++inst) {
            CHECK((dg1->get(inst)) == doctest::Approx(dg2->get(inst)).epsilon(0.001));
          }

          delete dg1;
          delete dg2;

          for (gum::SequenceIterator< const gum::DiscreteVariable* > ite = varList->begin();
               ite != varList->end();
               ++ite)
            delete *ite;
          delete varList;
        }
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }
  };

  GUM_TEST_ACTIF(_Simple_Creation)
  GUM_TEST_ACTIF(_MultiDimFunctionGraph_Diagram_Handlers_Methods)
  GUM_TEST_ACTIF(_toDot)
  GUM_TEST_ACTIF(_Manager_Graphical_Functions)
  GUM_TEST_ACTIF(_MultiDimFunctionGraph_Accessors_Modifiers_Methods)
  GUM_TEST_ACTIF(_MultiDimFunctionGraph_Implementation_Methods)
  GUM_TEST_ACTIF(_MultiDimFunctionGraph_Copy_Methods)
  GUM_TEST_ACTIF(_MultiDimFunctionGraph_Various_Methods)
  GUM_TEST_ACTIF(_MoveTo)
}   // namespace gum_tests
