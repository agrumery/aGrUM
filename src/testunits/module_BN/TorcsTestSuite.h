/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/hashTable.h>
#include <agrum/base/multidim/implementations/multiDimArray.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>

namespace gum_tests {

  class GUM_TEST_SUITE(Torcs) {
    public:
    gum::BayesNet< double >* bn;

    gum::LabelizedVariable* node1;
    gum::LabelizedVariable* node2;
    gum::LabelizedVariable* node3;
    gum::LabelizedVariable* node4;
    gum::LabelizedVariable* node5;

    gum::List< gum::Tensor< double >* >* evidence;

    void setUp() {
      gum::HashTable< gum::LabelizedVariable*, gum::NodeId > idMap;
      bn = new gum::BayesNet< double >();

      node1 = new gum::LabelizedVariable("TgR", "", 72);
      node2 = new gum::LabelizedVariable("DirV", "", 72);
      node3 = new gum::LabelizedVariable("Angle 1", "", 72);
      node4 = new gum::LabelizedVariable("DistM", "", 100);
      node5 = new gum::LabelizedVariable("Angle 2", "", 72);

      idMap.insert(node1, bn->add(*node1));
      idMap.insert(node2, bn->add(*node2));
      idMap.insert(node3, bn->add(*node3));
      idMap.insert(node4, bn->add(*node4));
      idMap.insert(node5, bn->add(*node5));

      bn->addArc(idMap[node1], idMap[node3]);
      bn->addArc(idMap[node2], idMap[node3]);
      bn->addArc(idMap[node3], idMap[node5]);
      bn->addArc(idMap[node4], idMap[node5]);

      evidence                  = new gum::List< gum::Tensor< double >* >();
      gum::Tensor< double >* e1 = new gum::Tensor< double >(new gum::MultiDimArray< double >());
      gum::Tensor< double >* e2 = new gum::Tensor< double >(new gum::MultiDimArray< double >());
      gum::Tensor< double >* e3 = new gum::Tensor< double >(new gum::MultiDimArray< double >());

      try {
        (*e1) << bn->variable(idMap[node1]);
        e1->fill(static_cast< float >(0));
        gum::Instantiation inst1(*e1);
        inst1.chgVal(bn->variable(idMap[node1]), 4);
        e1->set(inst1, static_cast< float >(1));
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }

      try {
        (*e2) << bn->variable(idMap[node2]);
        e2->fill(static_cast< float >(0));
        gum::Instantiation inst2(*e2);
        inst2.chgVal(bn->variable(idMap[node2]), 4);
        e2->set(inst2, static_cast< float >(1));
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }

      try {
        (*e3) << bn->variable(idMap[node3]);
        e3->fill(static_cast< float >(0));
        gum::Instantiation inst3(*e3);
        inst3.chgVal(bn->variable(idMap[node3]), 4);
        e3->set(inst3, static_cast< float >(1));
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }

      try {
        evidence->insert(e1);
        evidence->insert(e2);
        evidence->insert(e3);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
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

    GUM_ACTIVE_TEST(Inference) {
      gum::LazyPropagation< double > inf(bn);

      try {
        inf.makeInference();
      } catch (const gum::Exception& e) {
        GUM_UNUSED(e);
        TS_ASSERT(false);
      }
    }

    private:
    // Uncomment this to have some outputs.
    void printProba(const gum::Tensor< double >&) {
      // gum::Instantiation inst(p);

      // for (inst.setFirst(); !inst.end(); ++inst)
      //{
      //  std::cerr << inst<<" : " <<p[inst] << std::endl;
      //}
      // std::cerr << std::endl;
    }
  };
}   // namespace gum_tests
