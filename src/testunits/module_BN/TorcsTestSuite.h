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
#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/core/hashTable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>

namespace gum_tests {

  class TorcsTestSuite : public CxxTest::TestSuite {
    public:
    gum::BayesNet<float> *bn;

    gum::LabelizedVariable *node1;
    gum::LabelizedVariable *node2;
    gum::LabelizedVariable *node3;
    gum::LabelizedVariable *node4;
    gum::LabelizedVariable *node5;

    gum::List<gum::Potential<float> *> *evidence;

    void setUp() {
      gum::HashTable<gum::LabelizedVariable *, gum::Id> idMap;
      bn = new gum::BayesNet<float>();

      node1 = new gum::LabelizedVariable("TgR", "", 72);
      node2 = new gum::LabelizedVariable("DirV", "", 72);
      node3 = new gum::LabelizedVariable("Angle 1", "", 72);
      node4 = new gum::LabelizedVariable("DistM", "", 100);
      node5 = new gum::LabelizedVariable("Angle 2", "", 72);

      gum::Id tempId;

      idMap.insert(node1, bn->add(*node1));
      idMap.insert(node2, bn->add(*node2));
      idMap.insert(node3, bn->add(*node3));
      idMap.insert(node4, bn->add(*node4));
      idMap.insert(node5, bn->add(*node5));

      bn->addArc(idMap[node1], idMap[node3]);
      bn->addArc(idMap[node2], idMap[node3]);
      bn->addArc(idMap[node3], idMap[node5]);
      bn->addArc(idMap[node4], idMap[node5]);

      evidence = new gum::List<gum::Potential<float> *>();

      gum::Potential<float> *e1 =
          new gum::Potential<float>(new gum::MultiDimArray<float>());
      (*e1) << bn->variable(idMap[node1]);
      e1->fill((float)0);
      gum::Instantiation inst1(*e1);
      inst1.chgVal(bn->variable(idMap[node1]), 4);
      e1->set(inst1, (float)1);

      gum::Potential<float> *e2 =
          new gum::Potential<float>(new gum::MultiDimArray<float>());
      (*e2) << bn->variable(idMap[node2]);
      e2->fill((float)0);
      gum::Instantiation inst2(*e2);
      inst2.chgVal(bn->variable(idMap[node2]), 4);
      e2->set(inst1, (float)1);

      gum::Potential<float> *e3 =
          new gum::Potential<float>(new gum::MultiDimArray<float>());
      (*e3) << bn->variable(idMap[node3]);
      e3->fill((float)0);
      gum::Instantiation inst3(*e3);
      inst3.chgVal(bn->variable(idMap[node3]), 4);
      e3->set(inst1, (float)1);

      evidence->insert(e1);
      evidence->insert(e2);
      evidence->insert(e3);
    }

    void tearDown() {
      delete bn;

      delete node1;
      delete node2;
      delete node3;
      delete node4;
      delete node5;

      delete (*evidence)[0];
      delete (*evidence)[1];
      delete (*evidence)[2];
      delete evidence;
    }

    void testInference() {
      gum::LazyPropagation<float> inf(*bn);

      try {
        inf.makeInference();
      } catch (gum::Exception e) {
        // std::cerr << std::endl << e.getContent() << std::endl;
        throw;
      }
    }

    private:
    // Uncomment this to have some outputs.
    void printProba(const gum::Potential<double> &) {
      // gum::Instantiation inst(p);

      // for (inst.setFirst(); !inst.end(); ++inst)
      //{
      //  std::cerr << inst<<" : " <<p[inst] << std::endl;
      //}
      // std::cerr << std::endl;
    }
  };
}
