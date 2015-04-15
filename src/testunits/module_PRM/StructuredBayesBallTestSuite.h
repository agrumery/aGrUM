/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr                                                  *
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
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/inference/BayesBall.h>

#include <agrum/PRM/inference/SVE.h>
#include <agrum/PRM/inference/structuredBayesBall.h>

#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum_tests {
  class StructuredBayesBallTestSuite : public CxxTest::TestSuite {
    private:
    gum::prm::PRM<double> *prm;
    gum::prm::PRMInference<double> *prm_inf;
    gum::prm::System<double> *sys;
    gum::prm::PRM<double> *small;
    gum::prm::PRMInference<double> *small_inf;
    gum::prm::System<double> *small_sys;

    public:
    void setUp() {
      {
        gum::prm::o3prm::O3prmReader<double> reader;
        reader.readFile("../../../src/testunits/ressources/o3prm/inference.o3prm");
        prm = reader.prm();
        sys = &(prm->system("aSys"));
        prm_inf = new gum::prm::SVE<double>(*prm, *sys);
      }
      {
        gum::prm::o3prm::O3prmReader<double> reader;
        reader.readFile(
            "../../../src/testunits/ressources/o3prm/printers_systems.o3prm");
        small = reader.prm();
        small_sys = &(small->system("smallSys"));
        small_inf = new gum::prm::SVE<double>(*small, *small_sys);
      }
      // std::cerr << std::endl;
    }

    void tearDown() {
      delete prm;
      delete prm_inf;
      delete small;
      delete small_inf;
    }

    void testConstructors() {
      gum::prm::StructuredBayesBall<double> *bb = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(
          bb = new gum::prm::StructuredBayesBall<double>(*prm_inf));
      TS_GUM_ASSERT_THROWS_NOTHING(delete bb);
      TS_GUM_ASSERT_THROWS_NOTHING(
          bb = new gum::prm::StructuredBayesBall<double>(*small_inf));
      TS_GUM_ASSERT_THROWS_NOTHING(delete bb);
    }

    /// Checking that when a root is queried and there is no evidence, the
    /// requisite nodes set contains only the root node.
    void testRootsNoObs() {
      gum::prm::StructuredBayesBall<double> *bb = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(
          bb = new gum::prm::StructuredBayesBall<double>(*prm_inf));

      for (auto i = sys->begin(); i != sys->end(); ++i) {
        for (auto a = (*(i.val())).begin(); a != (*(i.val())).end(); ++a) {
          if ((*(i.val())).type().dag().parents((*(a.val())).id()).empty()) {
            TS_GUM_ASSERT_THROWS_NOTHING(bb->compute(i.val(), (*(a.val())).id()));

            for (auto j = sys->begin(); j != sys->end(); ++j) {
              if ((j.val()) != (i.val())) {
                TS_ASSERT(not bb->exists(j.val()));
              } else if (bb->exists(j.val())) {
                TS_ASSERT_EQUALS(bb->requisiteNodes(j.val()).size(), (gum::Size)1);
                TS_ASSERT(bb->requisiteNodes(j.val()).contains((*(a.val())).id()));
              } else {
                TS_ASSERT(false);
              }
            }
          }
        }
      }

      TS_GUM_ASSERT_THROWS_NOTHING(delete bb);
    }

    /// Checking that when a root is queried and there is no evidence, the
    /// requisite nodes set contains only the root node.
    void testRootsNoObsSmall() {
      gum::prm::StructuredBayesBall<double> *bb = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(
          bb = new gum::prm::StructuredBayesBall<double>(*small_inf));

      for (auto i = small_sys->begin(); i != small_sys->end(); ++i) {
        for (auto a = (*(i.val())).begin(); a != (*(i.val())).end(); ++a) {
          if ((*(i.val())).type().dag().parents((*(a.val())).id()).empty()) {
            TS_GUM_ASSERT_THROWS_NOTHING(bb->compute(i.val(), (*(a.val())).id()));

            for (gum::prm::System<double>::iterator j = small_sys->begin();
                 j != small_sys->end(); ++j) {
              if ((j.val()) != (i.val())) {
                TS_ASSERT(not bb->exists(j.val()));
              } else if (bb->exists(j.val())) {
                TS_ASSERT_EQUALS(bb->requisiteNodes(j.val()).size(), (gum::Size)1);
                TS_ASSERT(bb->requisiteNodes(j.val()).contains((*(a.val())).id()));
              } else {
                TS_ASSERT(false);
              }
            }
          }
        }
      }

      TS_GUM_ASSERT_THROWS_NOTHING(delete bb);
    }

    /// Checking that when a root is queried and there is evidence on each leaf node,
    /// the
    /// requisite nodes set contains all nodes
    //       void /*test*/RootsObs() { // do not know what to do for now ...
    //         gum::prm::StructuredBayesBall<double>* bb = 0;
    //         TS_GUM_ASSERT_THROWS_NOTHING(bb = new
    //         gum::prm::StructuredBayesBall<double>(*prm_inf));
    //         for (gum::prm::System<double>::iterator i = sys->begin(); i !=
    //         sys->end(); ++i) {
    //           for (gum::prm::Instance::iterator a = (**i).begin(); a !=
    //           (**i).end(); ++a) {
    //             if ( (**i).type().dag().children((**a).id()).empty() and (not
    //             (**i).hasRefAttr((**a).id())) ) {
    //               gum::prm::PRMInference<double>::Chain chain = std::make_pair(*i,
    //               *a);
    //               Potential<prm_float> e;
    //               e.add((**a).type().variable());
    //               e.fill((prm_float) 0.0);
    //               Instantiation inst(e);
    //               inst.setFirst();
    //               e.set(inst, (prm_float)1.0);
    //               prm_inf->addEvidence(chain, e);
    //               TS_ASSERT(bb->__isHardEvidence(*i, (**a).id()));
    //             }
    //           }
    //         }
    //         for (gum::prm::System<double>::iterator i = sys->begin(); i !=
    //         sys->end(); ++i) {
    //           for (gum::prm::Instance::iterator a = (**i).begin(); a !=
    //           (**i).end(); ++a) {
    //             if ((**i).type().dag().parents((**a).id()).empty()) {
    //               TS_GUM_ASSERT_THROWS_NOTHING(bb->compute(*i, (**a).id()));
    //               std::vector<gum::prm::Instance*> stack;
    //               stack.push_back(*i);
    //               gum::Size count = 0;
    //               while (not stack.empty()) {
    //                 gum::prm::Instance* inst = stack.back();
    //                 stack.pop_back();
    //                 TS_ASSERT(bb->exists(inst));
    //                 if (not bb->exists(inst)) {
    //                   GUM_TRACE_VAR(inst->name());
    //                   std::string dot = ".";
    //                   GUM_TRACE((**i).name() + dot + (**a).safeName());
    //                 }
    //                 count += inst->size();
    //                 for (gum::prm::Instance::InvRefIterator iter =
    //                 inst->beginInvRef(); iter != inst->endInvRef(); ++iter) {
    //                   typedef std::vector< std::pair<gum::prm::Instance*,
    //                   std::string> >::iterator Iter;
    //                   for (Iter jter = (**iter).begin(); jter != (**iter).end();
    //                   ++jter) {
    //                     stack.push_back(jter->first);
    //                   }
    //                 }
    //               }
    //               TS_ASSERT(bb->requisiteNodes(*i).size() <= count);
    //               // std::cerr << std::endl;
    //             }
    //           }
    //         }
    //         TS_GUM_ASSERT_THROWS_NOTHING(delete bb);
    //       }
  };

} // namespace gum_tests
