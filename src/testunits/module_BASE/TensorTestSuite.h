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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/exceptions.h>
#include <agrum/base/multidim/ICIModels/multiDimLogit.h>
#include <agrum/base/multidim/implementations/multiDimArray.h>
#include <agrum/base/multidim/instantiation.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/discretizedVariable.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/base/variables/rangeVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Tensor
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {
  struct TensorTestSuite {
    public:
    static void testCreation() {
      gum::Tensor< double > p(new gum::MultiDimArray< double >());
      CHECK(p.empty());

      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5), other_a("a", "first var", 2);
      GUM_CHECK_ASSERT_THROWS_NOTHING(p << a << b << c);
      CHECK_THROWS_AS(p << a << a, const gum::DuplicateElement&);
      CHECK_THROWS_AS(p << a << other_a, const gum::DuplicateElement&);
    }   // namespace gum_tests

    static void testCreationWithVars() {
      gum::LabelizedVariable a("a", "first var", 2);
      gum::LabelizedVariable b("b", "second var", 4);
      gum::LabelizedVariable c("c", "third var", 5);
      gum::LabelizedVariable other_a("a", "first var", 2);

      gum::Tensor< double > p({&a, &b, &c});
      CHECK_THROWS_AS(p << a << a, const gum::DuplicateElement&);
      CHECK_THROWS_AS(p << a << other_a, const gum::DuplicateElement&);
    }

    static void testNormalisation() {
      gum::Tensor< double > p(new gum::MultiDimArray< double >());

      gum::LabelizedVariable a("a", "first var", 2);
      gum::LabelizedVariable b("b", "second var", 4);
      gum::LabelizedVariable c("c", "third var", 5);

      p << a << b << c;

      gum::Size taille = p.domainSize();

      try {
        gum::Instantiation i(&p);

        p.fill(static_cast< double >(0));

        int cpt = 1;
        for (i.setFirst(); !i.end(); ++i)
          p.set(i, (double)(cpt++));

        p.normalize();

        i.setLast();
        CHECK((p[i]) == (2.0 / (taille + 1)));

        i.setFirst();
        CHECK((p[i]) == (2.0 / (taille * (taille + 1))));
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    static void testDomainSizeChanges() {
      gum::Tensor< double >  p1(new gum::MultiDimArray< double >());
      gum::LabelizedVariable var1("var1", "first var", 2);
      gum::LabelizedVariable var2("var2", "second var", 2);
      gum::LabelizedVariable var3("var3", "third var", 2);
      p1 << var1 << var2 << var3;

      CHECK((p1.domainSize()) == ((var1.domainSize() * var2.domainSize() * var3.domainSize())));
    }

    static void testAddAnyNumber() {
      gum::LabelizedVariable a("a", "first var", 2);
      gum::LabelizedVariable b("b", "second var", 4);
      gum::LabelizedVariable c("c", "third var", 5);

      gum::Tensor< double > proba({&a, &b, &c});
      gum::Instantiation    i(proba);

      for (i.setFirst(); !i.end(); ++i)
        GUM_CHECK_ASSERT_THROWS_NOTHING(proba.set(i, (double)-1));
    }

    static void testCopyProba() {
      gum::Tensor< double >  m(new gum::MultiDimArray< double >());
      gum::LabelizedVariable a("a", "first var", 2);
      gum::LabelizedVariable b("b", "second var", 4);
      gum::LabelizedVariable c("c", "third var", 5);

      m << a << b << c;
      gum::Instantiation i(m);

      gum::Size cpt = 0;

      for (i.setFirst(); !i.end(); ++i) {
        m.set(i, double(cpt));
        cpt++;
      }

      gum::Tensor< double > mm(new gum::MultiDimArray< double >());

      gum::LabelizedVariable x("x", "first var", 2);
      gum::LabelizedVariable y("y", "second var", 4);
      gum::LabelizedVariable z("z", "third var", 5);
      mm << x << z;
      CHECK_THROWS(mm.copyFrom(m));
      mm << y;
      GUM_CHECK_ASSERT_THROWS_NOTHING(mm.copyFrom(m));

      gum::Instantiation j(mm);
      i.chgVal(a, 0).chgVal(b, 2).chgVal(c, 2);
      j.chgVal(x, 0).chgVal(y, 2).chgVal(z, 0);
      CHECK((m[i]) == (mm[j]));
    }

    static void testRegressionCopy() {
      gum::LabelizedVariable a("a", "first var", 2);
      gum::LabelizedVariable b("b", "second var", 4);
      gum::LabelizedVariable c("c", "third var", 5);
      {
        gum::Tensor< double > P1({&a, &b, &c});
        gum::Tensor< double > P2({&a, &c});

        GUM_CHECK_ASSERT_THROWS_NOTHING(P2.copy(P1));
      }
      {
        gum::Tensor< double > P1, P2;
        P1 << a << b << c;
        P2 << a << c;
        GUM_CHECK_ASSERT_THROWS_NOTHING(P1.copy(P2));
      }
      {
        gum::Tensor< double > P1, P2;
        P1 << b;
        P2 << a << c;
        GUM_CHECK_ASSERT_THROWS_NOTHING(P1.copy(P2));
      }
    }

    static void testEquality() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);
      auto c = gum::LabelizedVariable("c", "cfoo", 3);

      gum::Tensor< int > p, q, r, t, u;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      q << b << c;   // different dims
      q.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      r << a << b;   // same dims, same data
      r.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      t << a << b;   // same dims,different data
      t.fillWith({1, 2, 3, 4, 0, 6, 7, 8, 9});

      u << b << a;   // same dims, same data, different order
      u.fillWith({1, 4, 7, 2, 5, 8, 3, 6, 9});

      CHECK((p) != (q));
      CHECK((p) == (r));
      CHECK((p) != (t));
      CHECK((p) == (u));
    }

    static void testMinMax() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({1, 9, 3, 0, 5, 0, 7, 8, 9}).normalizeAsCPT();

      CHECK((p.min()) == (0.0));
      CHECK((p.max()) == (1.0));
      CHECK((p.minNonZero()) == (1.0 / 13.0));
      CHECK((p.maxNonOne()) == (9.0 / 13.0));

      p.fillWith({1, 1, 1, 1, 1, 1, 1, 1, 1});
      CHECK((p.maxNonOne()) == (1));

      p.fillWith({0, 0, 0, 0, 0, 0, 0, 0, 0});
      CHECK((p.minNonZero()) == (0));
    }

    static void testIsNonZeroMap() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< double > p, q;
      p << a << b;
      p.fillWith({1, 9, 3, 0, 5, 0, 7, 8, 9}).normalizeAsCPT();
      q << a << b;
      q.fillWith({1, 1, 1, 0, 1, 0, 1, 1, 1});

      CHECK((p.isNonZeroMap()) == (q));
    }

    static void testOperators() {
      auto a = gum::LabelizedVariable("a", "afoo");
      auto b = gum::LabelizedVariable("b", "bfoo");
      auto c = gum::LabelizedVariable("c", "cfoo");

      gum::Tensor< int > p1;
      p1 << a << b;
      p1.fillWith({1, 2, 3, 4});

      gum::Tensor< int > p2;
      p2 << b << c;
      p2.fillWith({5, 6, 7, 8});

      auto p = p1;

      // just checking memory allocation (what else ?)
      auto pA = p1 * p2;

      CHECK((pA)
            == ((gum::Tensor< int >() << b << c << a).fillWith({5, 18, 7, 24, 10, 24, 14, 32})));

      auto pB = p1 + p2;
      CHECK((pB) == ((gum::Tensor< int >() << b << c << a).fillWith({6, 9, 8, 11, 7, 10, 9, 12})));

      auto pC = p2 / p1;
      CHECK((pC) == ((gum::Tensor< int >() << b << a << c).fillWith({5, 2, 2, 1, 7, 2, 3, 2})));

      auto pD = p2 - p1;
      CHECK((pD) == ((gum::Tensor< int >() << b << a << c).fillWith({4, 3, 3, 2, 6, 5, 5, 4})));

      CHECK((((p1 * p2) - (p2 / p1) + p1))
            == ((gum::Tensor< int >() << b << a << c).fillWith({1, 19, 10, 27, 1, 25, 13, 34})));

      p = p1 * p2;
      p -= (p2 / p1);
      p += p1;
      CHECK((p)
            == ((gum::Tensor< int >() << b << a << c).fillWith({1, 19, 10, 27, 1, 25, 13, 34})));
    }

    static void testMargOutFunctions() {
      try {
        auto a = gum::LabelizedVariable("a", "afoo", 3);
        auto b = gum::LabelizedVariable("b", "bfoo", 3);
        auto c = gum::LabelizedVariable("c", "cfoo", 3);
        auto d = gum::LabelizedVariable("d", "dfoo", 3);

        gum::Tensor< double > p;
        p << a << b;
        p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});
        p.normalize();

        gum::Tensor< double > q;
        q << c << d;
        q.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});
        q.normalize();

        CHECK((p) != (q));

        gum::Tensor< double > r;
        r << c << d;
        r.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});
        CHECK((q) != (r));
        r.normalize();
        CHECK((q) == (r));

        auto joint = p * q;

        auto margAB = joint.sumOut({&c, &d});
        CHECK((p) == (margAB));
        auto margCD = joint.sumOut({&b, &a});
        CHECK((q) == (margCD));

        p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});
        CHECK((p.prodOut({&a})) == ((gum::Tensor< double >() << b).fillWith({6, 120, 504})));
        CHECK((p.prodOut({&b})) == ((gum::Tensor< double >() << a).fillWith({28, 80, 162})));

        CHECK((p.maxOut({&a})) == ((gum::Tensor< double >() << b).fillWith({3, 6, 9})));
        CHECK((p.maxOut({&b})) == ((gum::Tensor< double >() << a).fillWith({7, 8, 9})));

        CHECK((p.minOut({&a})) == ((gum::Tensor< double >() << b).fillWith({1, 4, 7})));
        CHECK((p.minOut({&b})) == ((gum::Tensor< double >() << a).fillWith({1, 2, 3})));
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    static void testMarginalizingFunctions() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);
      auto c = gum::LabelizedVariable("c", "cfoo", 3);
      auto d = gum::LabelizedVariable("d", "dfoo", 3);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      gum::Tensor< double > q;
      q << c << d;
      q.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      auto joint = p * q;

      CHECK((joint.sumOut({&c, &d})) == (joint.sumIn({&a, &b})));
      CHECK((joint.sumOut({&c, &d})) == (joint.sumIn({&b, &a})));

      CHECK((joint.prodOut({&c, &d})) == (joint.prodIn({&a, &b})));

      CHECK((joint.minOut({&c, &d})) == (joint.minIn({&a, &b})));

      CHECK((joint.maxOut({&c, &d})) == (joint.maxIn({&a, &b})));
    }

    static void testDeprecatedMarginalizingFunctions() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);
      auto c = gum::LabelizedVariable("c", "cfoo", 3);
      auto d = gum::LabelizedVariable("d", "dfoo", 3);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      gum::Tensor< double > q;
      q << c << d;
      q.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      auto joint = p * q;

      CHECK((joint.sumOut({&c, &d})) == (joint.sumIn({&a, &b})));
      CHECK((joint.sumOut({&c, &d})) == (joint.sumIn({&b, &a})));

      CHECK((joint.prodOut({&c, &d})) == (joint.prodIn({&a, &b})));

      CHECK((joint.minOut({&c, &d})) == (joint.minIn({&a, &b})));

      CHECK((joint.maxOut({&c, &d})) == (joint.maxIn({&a, &b})));
    }

    static void testAbsTensor() {
      auto a = gum::LabelizedVariable("a", "afoo", 2);
      auto b = gum::LabelizedVariable("b", "bfoo", 2);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({0, 1, 2, 3});

      gum::Tensor< double > q;
      q << a << b;
      q.fillWith({0, 3, 0, 3});

      CHECK(((p - q).abs()) == ((gum::Tensor< double >() << a << b).fillWith({0, 2, 2, 0})));
      CHECK(((q - p).abs()) == ((gum::Tensor< double >() << a << b).fillWith({0, 2, 2, 0})));
      CHECK(((q - p).abs().max()) == (2));
      CHECK(((q - p).abs().min()) == (0));
    }

    static void testSqTensor() {
      auto a = gum::LabelizedVariable("a", "afoo", 2);
      auto b = gum::LabelizedVariable("b", "bfoo", 2);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({0, 1, 2, 3});

      gum::Tensor< double > q;
      q << b << a;
      q.fillWith({0, 0, 3, 3});

      CHECK(((p - q).sq()) == ((gum::Tensor< double >() << a << b).fillWith({0, 4, 4, 0})));
      CHECK(((q - p).sq()) == ((gum::Tensor< double >() << b << a).fillWith({0, 4, 4, 0})));
      CHECK(((q - p).sq().max()) == (4));
      CHECK(((q - p).sq().min()) == (0));
    }

    static void testEntropyTensor() {
      auto                  a = gum::LabelizedVariable("a", "afoo", 2);
      gum::Tensor< double > p;
      p.add(a);
      CHECK((p.fillWith({0, 1}).entropy()) == (0.0));
      CHECK((p.fillWith({1, 0}).entropy()) == (0.0));
      CHECK((p.fillWith({0.5, 0.5}).entropy()) == (1.0));
    }

    static void testReorganizeTensor() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);
      auto c = gum::LabelizedVariable("c", "cfoo", 3);
      auto d = gum::LabelizedVariable("d", "dfoo", 3);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      gum::Tensor< double > q;
      q << c << d;
      q.fillWith({4, 5, 6, 3, 2, 1, 4, 3, 2});

      auto joint1 = p * q;
      auto joint2 = q * p;
      CHECK((joint1.toString()) != (joint2.toString()));
      auto joint3 = (q * p).reorganize({&c, &d, &a, &b});
      CHECK((joint1.toString()) == (joint3.toString()));

      gum::Instantiation inst;
      inst << a << b << c << d;
      for (inst.setFirst(); !inst.end(); ++inst) {
        CHECK((joint1.get(inst)) == (joint2.get(inst)));
        CHECK((joint1.get(inst)) == (joint3.get(inst)));
      }

      CHECK_THROWS_AS(p.reorganize({&a, &b, &c}), const gum::InvalidArgument&);
      CHECK_THROWS_AS(p.reorganize(std::vector< const gum::DiscreteVariable* >{&a, &d}),


                      const gum::InvalidArgument&


      );
      CHECK_THROWS_AS(p.reorganize(std::vector< const gum::DiscreteVariable* >{&a}),


                      const gum::InvalidArgument&


      );
    }

    static void testReorganizeTensorFromNames() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);
      auto c = gum::LabelizedVariable("c", "cfoo", 3);
      auto d = gum::LabelizedVariable("d", "dfoo", 3);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      gum::Tensor< double > q;
      q << c << d;
      q.fillWith({4, 5, 6, 3, 2, 1, 4, 3, 2});

      auto joint1 = p * q;
      auto joint2 = q * p;
      CHECK((joint1.toString()) != (joint2.toString()));
      auto joint3 = (q * p).reorganize({"c", "d", "a", "b"});
      CHECK((joint1.toString()) == (joint3.toString()));

      gum::Instantiation inst;
      inst << a << b << c << d;
      for (inst.setFirst(); !inst.end(); ++inst) {
        CHECK((joint1.get(inst)) == (joint2.get(inst)));
        CHECK((joint1.get(inst)) == (joint3.get(inst)));
      }

      CHECK_THROWS_AS(p.reorganize({"a", "b", "c"}), const gum::InvalidArgument&);
      CHECK_THROWS_AS(p.reorganize({std::string("a"), "d"}), const gum::InvalidArgument&);
      CHECK_THROWS_AS(p.reorganize({std::string("a")}), const gum::InvalidArgument&);
      CHECK_THROWS_AS(p.reorganize({std::string("héhé")}), const gum::InvalidArgument&);
    }

    static void testPutFirst() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);
      auto c = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      CHECK((p.toString()) != (p.putFirst(&b).toString()));
      CHECK((p.toString()) == (p.putFirst(&b).putFirst(&a).toString()));
      CHECK((p.toString()) == (p.putFirst(&a).toString()));

      CHECK_THROWS_AS(p.putFirst(&c), const gum::InvalidArgument&);
    }

    static void testPutFirstWithName() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);
      auto c = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      CHECK((p.toString()) != (p.putFirst("b").toString()));
      CHECK((p.toString()) == (p.putFirst("b").putFirst("a").toString()));
      CHECK((p.toString()) == (p.putFirst("a").toString()));

      CHECK_THROWS_AS(p.putFirst("c"), const gum::InvalidArgument&);
    }

    static void testExtraction() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);
      auto c = gum::LabelizedVariable("c", "cfoo", 3);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      gum::Tensor< double > q;
      q << c;
      q.fillWith({1, 2, 3});

      auto pot = q * p;

      gum::Instantiation I;
      I << c;
      I.chgVal(c, 0);
      CHECK((pot.extract(I)) == (p));
      I.chgVal(c, 2);
      gum::Tensor< double > r;
      r << a << b;
      r.fillWith({3, 6, 9, 12, 15, 18, 21, 24, 27});
      CHECK((pot.reorganize({&b, &c, &a}).extract(I)) == (r));
    }

    static void testOperatorEqual() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< double > p;
      {
        // empty tensors are equal
        gum::Tensor< double > q;
        CHECK((p) == (q));
      }
      p << a;
      p.fillWith({1, 2, 3});
      CHECK((p) == (p));
      {
        // same tensor
        gum::Tensor< double > q;
        q << a;
        q.fillWith({1, 2, 3});
        CHECK((p) == (q));
      }
      {
        // difference values
        gum::Tensor< double > q;
        q << a;
        q.fillWith({3, 6, 9});
        CHECK((p) != (q));
      }
      {
        // same values, different variables
        gum::Tensor< double > q;
        q << b;
        q.fillWith({1, 2, 3});
        CHECK((p) != (q));
      }
      {
        // different dimensions
        gum::Tensor< double > q;
        q << b << a;
        q.fillWith(1);
        CHECK((p) != (q));
      }
    }

    static void testScaleAndTranslate() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);

      gum::Tensor< double > p;
      p << a;
      p.fillWith({1, 2, 3});
      gum::Tensor< double > p3;
      p3 << a;
      p3.fillWith({3, 6, 9});

      GUM_CHECK_ASSERT_THROWS_NOTHING(p.scale(3.0));
      CHECK((p) == (p3));
      CHECK((p) == (p3));

      p.fillWith({1, 2, 3});
      gum::Tensor< double > p2;
      p2 << a;
      p2.fillWith({2, 3, 4});

      GUM_CHECK_ASSERT_THROWS_NOTHING(p.translate(1.0));
      CHECK((p) == (p2));
      CHECK((p) == (p2));


      p.fillWith({1, 2, 3});
      gum::Tensor< double > p1;
      p1 << a;
      p1.fillWith({4, 7, 10});
      p.scale(3.0).translate(1.0);
      // GUM_CHECK_ASSERT_THROWS_NOTHING(p.scale(3.0).translate(1.0));
      CHECK((p) == (p1));
      CHECK((p) == (p1));
    }

    static void testNormalizeAsCPT() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      auto q = p / p.sumOut({&a});
      p.normalizeAsCPT();
      CHECK((p) == (q));
      CHECK((q) == (p));

      gum::Tensor< double > p2;
      p2 << a << b;
      p2.fill(0.0f);
      CHECK_THROWS_AS(p2.normalizeAsCPT(), const gum::FatalError&);

      gum::Tensor< double > p3;
      p3 << a << b;
      p3.fillWith({1, 2, 3, 0, 0, 0, 7, 8, 9});
      CHECK_THROWS_AS(p2.normalizeAsCPT(), const gum::FatalError&);

      gum::Tensor< double > p4;
      p4 << a;
      p4.fillWith({1, 3, 6});
      p4.normalizeAsCPT();
      gum::Tensor< double > witness;
      witness << a;
      witness.fillWith({0.1f, 0.3f, 0.6f});
      CHECK((p4) == (witness));
    }

    static void testEmptyTensor() {
      gum::Tensor< double > p;
      gum::Instantiation    inst(p);
      double                a     = 0;
      auto                  var_a = gum::LabelizedVariable("a", "afoo", 3);

      GUM_CHECK_ASSERT_THROWS_NOTHING(a = p[inst];);
      CHECK((a) == (1.0f));

      GUM_CHECK_ASSERT_THROWS_NOTHING(p.set(inst, a + static_cast< double >(3.0f)););
      CHECK((p[inst]) == (4.0f));

      GUM_CHECK_ASSERT_THROWS_NOTHING(p.populate({1.0f}));
      CHECK((p[inst]) == (1.0f));
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.populate(std::vector< double >{2.0f}));
      CHECK((p[inst]) == (2.0f));
      CHECK_THROWS_AS(p.populate(std::vector< double >{2.0f, 3.0f}), const gum::SizeError&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(p.apply([](double x) { return x * 2.0f + 1.0f; }););
      CHECK((p[inst]) == (5.0f));

      a = 0.3f;
      GUM_CHECK_ASSERT_THROWS_NOTHING(a
                                      = p.reduce([](double x, double y) { return x + y; }, 0.0f););
      CHECK((a) == (0.0f));

      GUM_CHECK_ASSERT_THROWS_NOTHING(p.populate({33.0f}));
      CHECK((p.domainSize()) == (1u));
      CHECK((p.get(gum::Instantiation())) == (33.0f));


      GUM_CHECK_ASSERT_THROWS_NOTHING(p.set(inst, static_cast< double >(3.0f)););
      CHECK((p.sum()) == (3.0f));
      CHECK((p.product()) == (3.0f));
      CHECK((p.max()) == (3.0f));
      CHECK((p.min()) == (3.0f));
      CHECK((p.minNonZero()) == (3.0f));
      CHECK((p.maxNonOne()) == (3.0f));

      GUM_CHECK_ASSERT_THROWS_NOTHING(p.set(inst, static_cast< double >(0.0f)););
      CHECK((p.minNonZero()) == (0));

      GUM_CHECK_ASSERT_THROWS_NOTHING(p.set(inst, static_cast< double >(1.0f)););
      CHECK((p.maxNonOne()) == (1.0f));

      GUM_CHECK_ASSERT_THROWS_NOTHING(p.fill(2.0f););
      CHECK((p[inst]) == (2.0f));
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.fillWith(3.0f););
      CHECK((p[inst]) == (3.0f));
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.fillWith({4.0f}););
      CHECK((p[inst]) == (4.0f));
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.fillWith(std::vector< double >({5.0f})););
      CHECK((p[inst]) == (5.0f));

      CHECK((p.entropy()) == (0.0f));

      CHECK((p.fillWith(2.0f).sq()[inst]) == (4.0f));
      CHECK((p.fillWith(-2.0f).sq()[inst]) == (4.0f));
      CHECK((p.fillWith(2.0f).abs()[inst]) == (2.0f));
      CHECK((p.fillWith(-2.0f).abs()[inst]) == (2.0f));

      CHECK((p.fillWith(2.0f).normalize()[inst]) == (1.0f));
      CHECK((p.fillWith(0.0f).normalize()[inst]) == (0.0f));

      CHECK((p.fillWith(2.0f).scale(4)[inst]) == (8.0f));
      CHECK((p.fillWith(2.0f).translate(4)[inst]) == (6.0f));


      CHECK((p.fillWith(2.0f).sumOut({&var_a})[inst]) == (2.0f));
      CHECK((p.fillWith(3.0f).sumIn({&var_a})[inst]) == (3.0f));
      CHECK((p.fillWith(4.0f).prodOut({&var_a})[inst]) == (4.0f));
      CHECK((p.fillWith(5.0f).prodIn({&var_a})[inst]) == (5.0f));
      CHECK((p.fillWith(6.0f).maxIn({&var_a})[inst]) == (6.0f));
      CHECK((p.fillWith(7.0f).maxOut({&var_a})[inst]) == (7.0f));
      CHECK((p.fillWith(8.0f).minOut({&var_a})[inst]) == (8.0f));
      CHECK((p.fillWith(9.0f).minOut({&var_a})[inst]) == (9.0f));

      CHECK((p.fillWith(0.0f).isNonZeroMap()[inst]) == (0.0f));
      CHECK((p.fillWith(1.0f).isNonZeroMap()[inst]) == (1.0f));
      CHECK((p.fillWith(0.5f).isNonZeroMap()[inst]) == (1.0f));
      CHECK((p.fillWith(-0.5f).isNonZeroMap()[inst]) == (1.0f));
    }

    static void testOperationForEmptyTensor() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< double > res;
      res << a << b;

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      gum::Tensor< double > q;
      q.fill(2);

      gum::Tensor< double > tmp;

      res.fillWith({3, 4, 5, 6, 7, 8, 9, 10, 11});
      CHECK((p + q) == (res));
      tmp = p;
      tmp += q;
      CHECK((tmp) == (res));

      CHECK((q + p) == (res));
      tmp = q;
      tmp += p;
      CHECK((tmp) == (res));

      res.fillWith({2, 4, 6, 8, 10, 12, 14, 16, 18});
      CHECK((p * q) == (res));
      tmp = p;
      tmp *= q;
      CHECK((tmp) == (res));

      CHECK((q * p) == (res));
      tmp = q;
      tmp *= p;
      CHECK((tmp) == (res));

      res.fillWith({-1, 0, 1, 2, 3, 4, 5, 6, 7});
      CHECK((p - q) == (res));
      tmp = p;
      tmp -= q;
      CHECK((tmp) == (res));

      res.fillWith({1, 0, -1, -2, -3, -4, -5, -6, -7});
      CHECK((q - p) == (res));
      tmp = q;
      tmp -= p;
      CHECK((tmp) == (res));

      res.fillWith({1 / 2.0f,
                    2 / 2.0f,
                    3 / 2.0f,
                    4 / 2.0f,
                    5 / 2.0f,
                    6 / 2.0f,
                    7 / 2.0f,
                    8 / 2.0f,
                    9 / 2.0f});
      CHECK((p / q) == (res));
      tmp = p;
      tmp /= q;
      CHECK((tmp) == (res));

      res.fillWith({2.0f / 1,
                    2.0f / 2,
                    2.0f / 3,
                    2.0f / 4,
                    2.0f / 5,
                    2.0f / 6,
                    2.0f / 7,
                    2.0f / 8,
                    2.0f / 9});
      CHECK((q / p) == (res));
      tmp = q;
      tmp /= p;
      CHECK((tmp) == (res));
    }

    static void testOperationForTwoEmptyTensors() {
      gum::Tensor< double > p;
      p.fill(3);

      gum::Tensor< double > q;
      q.fill(2);

      gum::Tensor< double > res, tmp;
      res.fill(5);
      CHECK((p + q) == (res));
      tmp = p;
      tmp += q;
      CHECK((tmp) == (res));

      CHECK((q + p) == (res));
      tmp = q;
      tmp += p;
      CHECK((tmp) == (res));

      res.fillWith({6});
      CHECK((p * q) == (res));
      tmp = p;
      tmp *= q;
      CHECK((tmp) == (res));

      CHECK((q * p) == (res));
      tmp = q;
      tmp *= p;
      CHECK((tmp) == (res));

      res.fillWith({1});
      CHECK((p - q) == (res));
      tmp = p;
      tmp -= q;
      CHECK((tmp) == (res));

      res.fillWith({-1});
      CHECK((q - p) == (res));
      tmp = q;
      tmp -= p;
      CHECK((tmp) == (res));

      res.fillWith({3 / 2.0f});
      CHECK((p / q) == (res));
      tmp = p;
      tmp /= q;
      CHECK((tmp) == (res));

      res.fillWith({2.0f / 3});
      CHECK(((q / p - res).abs().max()) < (1e-7));
      tmp = q;
      tmp /= p;
      CHECK(((tmp - res).abs().max()) < (1e-7));
    }

    static void testLoopsForEmptyTensor() {
      gum::Tensor< double > p;
      gum::Instantiation    inst(p);
      p.fill(3);

      gum::Size cpt   = 0;
      double    total = 0.0;
      for (inst.setFirst(); !inst.end(); ++inst) {
        cpt++;
        total += p[inst];
      }
      CHECK((cpt) == (static_cast< gum::Size >(1)));
      CHECK((total) == (3));

      cpt   = 0;
      total = 0.0;
      for (inst.setLast(); !inst.end(); --inst) {
        cpt++;
        total += p[inst];
      }
      CHECK((cpt) == (static_cast< gum::Size >(1)));
      CHECK((total) == (3));
    }

    void /*test*/ EliminatationOffAllVariables() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< int > p;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});
      CHECK((p.sumOut({&a, &b}).toString()) == ("[45]"));
    }

    static void testKL() {
      gum::LabelizedVariable v("v", "v", 2), w("w", "w", 2);
      gum::Tensor< double >  p, q, r, s;
      p.add(v);
      p.fillWith({0.0f, 1.0f});
      q.add(v);
      q.fillWith({0.5f, 0.5f});
      r.add(v);
      r.fillWith({0.7f, 0.3f});
      s.add(v);
      s.add(w);
      s.fillWith({0.0f, 1.0f, 0.2f, 0.8f});

      double res = -1.0f;

      GUM_CHECK_ASSERT_THROWS_NOTHING(res = p.KL(p));
      CHECK((res) == (0.0f));

      CHECK_THROWS_AS(res = p.KL(s), const gum::InvalidArgument&);
      CHECK_THROWS_AS(res = s.KL(p), const gum::InvalidArgument&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(res = p.KL(q));
      CHECK((res) == doctest::Approx(0.0 + 1.0 * log2(1.0 / 0.5)).epsilon(GUM_SMALL_ERROR));
      CHECK_THROWS_AS(res = q.KL(p), const gum::FatalError&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(res = p.KL(r));
      CHECK((res) == doctest::Approx(0.0 + 1.0 * log2(1.0 / 0.3)).epsilon(GUM_SMALL_ERROR));
      CHECK_THROWS_AS(res = r.KL(p), const gum::FatalError&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(res = q.KL(r));
      CHECK((res)
            == doctest::Approx(0.5 * log2(0.5 / 0.7) + 0.5 * log2(0.5 / 0.3))
                   .epsilon(GUM_SMALL_ERROR));

      GUM_CHECK_ASSERT_THROWS_NOTHING(res = r.KL(q));
      CHECK((res)
            == doctest::Approx(0.7 * log2(0.7 / 0.5) + 0.3 * log2(0.3 / 0.5))
                   .epsilon(GUM_SMALL_ERROR));
    }

    static void testTensorDraw() {
      try {
        constexpr int          DELTA   = 200;
        constexpr int          NBRITER = 10000;
        gum::LabelizedVariable v("v", "v", 2), w("w", "w", 3);

        gum::Tensor< double > p;
        p.add(v);
        p.fillWith({0.2f, 0.6f});
        int s = 0;
        for (int i = 0; i < NBRITER; i++) {
          if (p.draw() == 0) s++;
        }
        CHECK((s) == doctest::Approx(int(0.2 * NBRITER)).epsilon(DELTA));

        p.add(w);
        CHECK_THROWS_AS(auto x [[maybe_unused]] = p.draw(), const gum::FatalError&);

        gum::Tensor< double > q;
        q.add(w);
        q.fillWith({0.3f, 0.6f, 0.1f});
        int s0 = 0, s1 = 0, s2 = 0;
        for (int i = 0; i < NBRITER; i++) {
          auto d = q.draw();
          if (d == 0) s0++;
          if (d == 1) s1++;
          if (d == 2) s2++;
        }
        CHECK((s0) == doctest::Approx(int(0.3 * NBRITER)).epsilon(DELTA));
        CHECK((s1) == doctest::Approx(int(0.6 * NBRITER)).epsilon(DELTA));
        CHECK((s2) == doctest::Approx(int(0.1 * NBRITER)).epsilon(DELTA));
      } catch (const gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    static void testVariableAccessor() {
      gum::LabelizedVariable v("v", "v", 2), w("w", "w", 3);
      gum::Tensor< double >  p;
      p.add(v);
      p.add(w);

      CHECK(p.variable(0) == p.variable("v"));
      CHECK(p.variable(1) == p.variable("w"));
      CHECK(p.variable(1) != p.variable("v"));
      CHECK(p.variable(0) != p.variable("w"));

      CHECK_THROWS_AS(p.variable("ZZ"), const gum::NotFound&);
    }

    static void testFillWithTensorMethod() {
      gum::LabelizedVariable v("v", "v", 2), w("w", "w", 3);
      gum::LabelizedVariable z("z", "z", 2);
      gum::LabelizedVariable vv("v", "v", 2), ww("w", "w", 3);
      gum::LabelizedVariable vvv("v", "v", 3), www("w", "w", 2);

      gum::Tensor< int > p;
      p.add(v);
      p.add(w);

      gum::Tensor< int > pp;
      pp.add(ww);
      pp.add(vv);

      CHECK((p.domainSize()) == (static_cast< gum::Size >(6)));
      CHECK(pp.domainSize() == static_cast< gum::Size >(6));

      p.fillWith({1, 2, 3, 4, 5, 6});
      pp.fillWith(p);
      gum::Instantiation Ip(p);
      gum::Instantiation Ipp;
      Ipp.add(vv);
      Ipp.add(ww);
      Ipp.setFirst();
      for (Ip.setFirst(); !Ip.end(); ++Ip, ++Ipp)
        try {
          auto vp  = p[Ip];
          auto vpp = pp[Ipp];
          CHECK((vp) == (vpp));
        } catch (gum::Exception& e) { GUM_SHOWERROR(e); }

      // errors
      gum::Tensor< int > bad_p;
      bad_p.add(w);
      CHECK_THROWS_AS(bad_p.fillWith(p), const gum::InvalidArgument&);

      gum::Tensor< int > bad_p2;
      bad_p2.add(vvv);
      bad_p2.add(www);
      CHECK_THROWS_AS(bad_p2.fillWith(p), const gum::InvalidArgument&);

      gum::Tensor< int > bad_p3;
      bad_p3.add(w);
      bad_p3.add(z);
      CHECK_THROWS_AS(bad_p3.fillWith(p), const gum::InvalidArgument&);

      gum::Tensor< int >     bad_p4;
      gum::LabelizedVariable badv("v", "v", 0);
      badv.addLabel("3").addLabel("1");
      bad_p4.add(w);
      bad_p4.add(badv);
      CHECK_THROWS_AS(bad_p4.fillWith(p), const gum::InvalidArgument&);
    }

    static void testFillWithTensorAndMapMethod() {
      gum::LabelizedVariable v("v", "v", 2), w("w", "w", 3);
      gum::Tensor< int >     p;
      p.add(v);
      p.add(w);

      gum::LabelizedVariable vv("vv", "vv", 2), ww("ww", "ww", 3);
      gum::Tensor< int >     pp;
      pp.add(ww);
      pp.add(vv);

      CHECK((p.domainSize()) == (static_cast< gum::Size >(6)));
      CHECK((pp.domainSize()) == (static_cast< gum::Size >(6)));

      p.fillWith({1, 2, 3, 4, 5, 6});
      GUM_CHECK_ASSERT_THROWS_NOTHING(pp.fillWith(p, {"w", "v"}));
      CHECK_THROWS_AS(pp.fillWith(p, {"v", "w"}), const gum::InvalidArgument&);
    }

    static void testArgMaxMinFindAll() {
      gum::LabelizedVariable v("v", "v", 2), w("w", "w", 3);
      gum::Tensor< int >     p;
      _testval_for_set_(p, 4, p.findAll(4), 0);

      p.add(v);
      p.add(w);
      p.fillWith({1, 3, 2, 4, 1, 4});

      _testval_for_set_(p, 3, p.findAll(3), 1);
      _testval_for_set_(p, 10, p.findAll(10), 0);

      const auto [amax, vmax] = p.argmax();
      _testval_for_set_(p, 4, amax, 2);

      const auto [amin, vmin] = p.argmin();
      _testval_for_set_(p, 1, amin, 2);
    }

    static void testAddDummyVariables() {
      {
        gum::LabelizedVariable v("v", "v", 0);

        gum::Tensor< double > p;
        CHECK((v.domainSize()) == (static_cast< gum::Size >(0)));
        CHECK_THROWS_AS(p.add(v), const gum::InvalidArgument&);

        v.addLabel("first");
        CHECK((v.domainSize()) == (static_cast< gum::Size >(1)));
        GUM_CHECK_ASSERT_THROWS_NOTHING(p.add(v));

        p = gum::Tensor< double >();
        v.addLabel("second");
        CHECK((v.domainSize()) == (static_cast< gum::Size >(2)));
        GUM_CHECK_ASSERT_THROWS_NOTHING(p.add(v));
      }

      {
        gum::RangeVariable v("v", "v");
        v.setMinVal(1);
        v.setMaxVal(0);
        gum::Tensor< double > p;
        CHECK((v.domainSize()) == (static_cast< gum::Size >(0)));
        CHECK_THROWS_AS(p.add(v), const gum::InvalidArgument&);

        v.setMaxVal(1);
        CHECK((v.domainSize()) == (static_cast< gum::Size >(1)));
        GUM_CHECK_ASSERT_THROWS_NOTHING(p.add(v));

        p = gum::Tensor< double >();
        v.setMaxVal(2);
        CHECK((v.domainSize()) == (static_cast< gum::Size >(2)));
        GUM_CHECK_ASSERT_THROWS_NOTHING(p.add(v));
      }

      {
        gum::DiscretizedVariable< int > v("v", "v");
        gum::Tensor< double >           p;
        CHECK((v.domainSize()) == (static_cast< gum::Size >(0)));
        CHECK_THROWS_AS(p.add(v), const gum::InvalidArgument&);

        v.addTick(1);
        CHECK((v.domainSize()) == (static_cast< gum::Size >(0)));
        CHECK_THROWS_AS(p.add(v), const gum::InvalidArgument&);

        v.addTick(2);
        CHECK((v.domainSize()) == (static_cast< gum::Size >(1)));
        GUM_CHECK_ASSERT_THROWS_NOTHING(p.add(v));

        p = gum::Tensor< double >();
        v.addTick(3);
        CHECK((v.domainSize()) == (static_cast< gum::Size >(2)));
        GUM_CHECK_ASSERT_THROWS_NOTHING(p.add(v));
      }
    }

    static void testRandomTensor() {
      gum::LabelizedVariable u("u", "u", 4), v("v", "v", 2), w("w", "w", 3);
      gum::Tensor< double >  p;
      p.add(u);
      p.add(v);
      p.add(w);
      gum::Instantiation I(p);

      p.random();
      for (I.setFirst(); !I.end(); I.inc()) {
        CHECK((p[I]) <= (1.0));
      }

      p.randomDistribution();
      double cum = 0.0;
      for (I.setFirst(); !I.end(); I.inc()) {
        CHECK((p[I]) <= (1.0));
        cum += p[I];
      }
      CHECK((cum) == doctest::Approx(1.0).epsilon(1e-6));

      p.randomCPT();
      const gum::DiscreteVariable& var = p.variable(0);
      for (I.setFirstNotVar(var); !I.end(); I.incNotVar(var)) {
        cum = 0.0;
        for (I.setFirstVar(var); !I.end(); I.incVar(var)) {
          CHECK((p[I]) <= (1.0));
          cum += p[I];
        }
        CHECK((cum) == doctest::Approx(1.0).epsilon(1e-6));
        I.unsetEnd();
      }

      p.fillWith(1.0).normalizeAsCPT();   // every thing should be 0.25
      for (I.setFirst(); !I.end(); I.inc()) {
        CHECK((p[I]) == doctest::Approx(0.25).epsilon(1e-6));
      }

      for (double alpha = 0.0; alpha <= 1; alpha += 0.1) {
        p.fillWith(1.0).normalizeAsCPT();   // every thing should be 0.25
        p.noising(alpha);
        double min = (1 - alpha) * 0.25 + alpha * 0.0;
        double max = (1 - alpha) * 0.25 + alpha * 1.0;
        for (I.setFirst(); !I.end(); I.inc()) {
          CHECK((min) <= (p[I]));
          CHECK((p[I]) <= (max));
        }
      }
    }

    static void testEqualities() {
      gum::LabelizedVariable u("u", "u", 4);
      gum::LabelizedVariable v("v", "v", 2);
      gum::LabelizedVariable w("w", "w", 3);
      gum::Tensor< double >  p;
      p.add(u);
      p.add(v);
      p.add(w);
      p.random();
      gum::Tensor< double > q(p);
      CHECK((p) == (q));
      gum::Instantiation i(q);
      i.setLast();
      q.set(i, 0);
      CHECK((p) != (q));
      q.fillWith(p);
      CHECK((p) == (q));
      q.fillWith(1);
      CHECK((p) != (q));
      q.fillWith(p);
      CHECK((p) == (q));
      gum::LabelizedVariable x("x", "Unknown", 5);
      q.add(x);
      CHECK((p) != (q));
      q.erase("x");
      q.fillWith(p);
      CHECK((p) == (q));
      q.erase("u");
      CHECK((p) != (q));
    }

    static void testExpectedValue() {
      gum::LabelizedVariable u("u", "u", 4);
      gum::LabelizedVariable v("v", "v", 2);
      gum::LabelizedVariable w("w", "w", 3);
      gum::Tensor< double >  p;
      p.add(u);
      p.add(v);
      p.add(w);
      p.randomDistribution();

      CHECK(fabs((p.expectedValue([](const gum::Instantiation&) -> auto { return 1.0; })) - (1.0))
            < 1e-10);

      gum::Tensor< double > q;
      q.add(u);
      q.randomDistribution();

      // maean by hand
      auto   I    = gum::Instantiation(q);
      double mean = q[I] * I.val(0);
      for (gum::Idx i = 1; i < u.domainSize(); i++) {
        I.inc();
        mean += q[I] * I.val(0);
      }

      CHECK(fabs((q.expectedValue(
                     [](const gum::Instantiation& i) -> auto { return double(i.val(0)); }))
                 - (mean))
            < 1e-10);
    }

    static void testInverse() {
      gum::LabelizedVariable u("u", "u", 4);
      gum::LabelizedVariable v("v", "v", 2);
      gum::LabelizedVariable w("w", "w", 3);
      gum::Tensor< double >  p;
      p.add(u);
      p.add(v);
      p.add(w);
      p.random();
      gum::Tensor< double > q(p);
      q.inverse();
      CHECK(((p * q).min()) == doctest::Approx(1.0).epsilon(1e-7));
      CHECK(((p * q).max()) == doctest::Approx(1.0).epsilon(1e-7));
    }

    static void testMinNegatif() {
      gum::LabelizedVariable u("u", "u", 4);
      gum::LabelizedVariable v("v", "v", 2);
      gum::LabelizedVariable w("w", "w", 3);
      gum::Tensor< double >  p;
      p.add(u);
      p.add(v);
      p.add(w);

      gum::Instantiation I(p);
      double             f = -150.0;
      for (I.setFirst(); !I.end(); I.inc())
        p.set(I, f++);

      CHECK((p.min()) == doctest::Approx(-150.0).epsilon(1e-7));
      CHECK((p.max()) == doctest::Approx(-150 + 4 * 2 * 3 - 1).epsilon(1e-7));
    }

    static void testSgn() {
      gum::LabelizedVariable u("u", "u", 4);
      gum::LabelizedVariable v("v", "v", 2);
      gum::LabelizedVariable w("w", "w", 3);
      gum::Tensor< double >  p;
      p.add(u);
      p.add(v);
      p.add(w);
      p.random();

      auto r = p * 2;
      CHECK(((r - p).sgn().min()) == (1.0));
      CHECK(((r - p).sgn().max()) == (1.0));
      CHECK(((p - r).sgn().min()) == (-1.0));
      CHECK(((p - r).sgn().max()) == (-1.0));
      CHECK(((p - p).sgn().min()) == (0.0));
      CHECK(((p - p).sgn().max()) == (0.0));
    }

    static void testOperatorWithScalars() {
      gum::LabelizedVariable u("u", "u", 4);
      gum::LabelizedVariable v("v", "v", 2);
      gum::LabelizedVariable w("w", "w", 3);
      gum::Tensor< double >  p;
      p.add(u);
      p.add(v);
      p.add(w);
      p.random();
      {
        gum::Tensor< double > q(p);
        q.scale(2);
        CHECK((p * 2) == (q));
      }
      {
        gum::Tensor< double > q(p);
        q.scale(0.5);
        CHECK((p / 2) == (q));
      }
      {
        gum::Tensor< double > q(p);
        q.translate(1.5);
        CHECK((p + 1.5) == (q));
      }
      {
        gum::Tensor< double > q(p);
        q.translate(3.2);
        CHECK((p - 3.2) == (q));
      }
      {
        gum::Tensor< double > q(p);
        gum::Tensor< double > r(p);
        q.translate(4.33);
        r += 4.33;
        CHECK((r) == (q));
      }
      {
        gum::Tensor< double > q(p);
        gum::Tensor< double > r(p);
        q.translate(-4.33);
        r -= 4.33;
        CHECK((r) == (q));
      }
      {
        gum::Tensor< double > q(p);
        gum::Tensor< double > r(p);
        q.scale(4.33);
        r *= 4.33;
        CHECK((r) == (q));
      }
      {
        gum::Tensor< double > q(p);
        gum::Tensor< double > r(p);
        q.scale(1 / 4.33);
        r /= 4.33;
        CHECK((r) == (q));
      }
    }

    static void testDeterministicTensor_CreatesTensorWithCorrectValue() {
      gum::RangeVariable var("var", "Variable1", 1, 4);
      const gum::Idx     value = 2;

      auto tensor = gum::Tensor< double >::deterministicTensor(var, value);

      CHECK((tensor.sum()) == (1.0));

      CHECK((tensor.max()) == (1.0));
      const auto& [x, y] = tensor.argmax();
      CHECK((x.size()) == (gum::Size(1)));
      const auto& i = *x.begin();
      CHECK((i.val(0)) == (value));

      CHECK((tensor.min()) == (0.0));
      CHECK((tensor.minNonZero()) == (1.0));
      CHECK((tensor.maxNonOne()) == (0.0));
    }

    static void testDeterministicTensor_CreatesTensorWithCorrectLabel() {
      const gum::RangeVariable var("var", "Variable1", 1, 4);
      const std::string        label = "2";

      const auto tensor = gum::Tensor< double >::deterministicTensor(var, label);

      CHECK((tensor.sum()) == (1.0));

      CHECK((tensor.max()) == (1.0));
      const auto& [x, y] = tensor.argmax();
      CHECK((x.size()) == (gum::Size(1)));
      const auto& i = *x.begin();
      CHECK((i.val(0)) == (gum::Idx(1)));

      CHECK((tensor.min()) == (0.0));
      CHECK((tensor.minNonZero()) == (1.0));
      CHECK((tensor.maxNonOne()) == (0.0));
    }

    static void testUniformTensor_CreatesUniformTensor() {
      const gum::RangeVariable var("var", "Variable1", 0, 3);

      const auto tensor = gum::Tensor< double >::uniformTensor(var);

      CHECK((tensor.sum()) == (1.0));
      CHECK((tensor.max()) == (0.25));
      CHECK((tensor.min()) == (0.25));
    }

    private:
    static void _testval_for_set_(const gum::Tensor< int >&             p,
                                  int                                   val,
                                  const gum::Set< gum::Instantiation >& s,
                                  gum::Size                             expected_size) {
      gum::Instantiation ip(p);

      CHECK((s.size()) == (expected_size));
      for (ip.setFirst(); !ip.end(); ++ip) {
        if (s.contains(ip)) {
          CHECK((p[ip]) == (val));
        } else {
          CHECK((p[ip]) != (val));
        }
      }
    }
  };

  GUM_TEST_ACTIF(Creation)
  GUM_TEST_ACTIF(CreationWithVars)
  GUM_TEST_ACTIF(Normalisation)
  GUM_TEST_ACTIF(DomainSizeChanges)
  GUM_TEST_ACTIF(AddAnyNumber)
  GUM_TEST_ACTIF(CopyProba)
  GUM_TEST_ACTIF(RegressionCopy)
  GUM_TEST_ACTIF(Equality)
  GUM_TEST_ACTIF(MinMax)
  GUM_TEST_ACTIF(IsNonZeroMap)
  GUM_TEST_ACTIF(Operators)
  GUM_TEST_ACTIF(MargOutFunctions)
  GUM_TEST_ACTIF(MarginalizingFunctions)
  GUM_TEST_ACTIF(DeprecatedMarginalizingFunctions)
  GUM_TEST_ACTIF(AbsTensor)
  GUM_TEST_ACTIF(SqTensor)
  GUM_TEST_ACTIF(EntropyTensor)
  GUM_TEST_ACTIF(ReorganizeTensor)
  GUM_TEST_ACTIF(ReorganizeTensorFromNames)
  GUM_TEST_ACTIF(PutFirst)
  GUM_TEST_ACTIF(PutFirstWithName)
  GUM_TEST_ACTIF(Extraction)
  GUM_TEST_ACTIF(OperatorEqual)
  GUM_TEST_ACTIF(ScaleAndTranslate)
  GUM_TEST_ACTIF(NormalizeAsCPT)
  GUM_TEST_ACTIF(EmptyTensor)
  GUM_TEST_ACTIF(OperationForEmptyTensor)
  GUM_TEST_ACTIF(OperationForTwoEmptyTensors)
  GUM_TEST_ACTIF(LoopsForEmptyTensor)
  GUM_TEST_ACTIF(KL)
  GUM_TEST_ACTIF(TensorDraw)
  GUM_TEST_ACTIF(VariableAccessor)
  GUM_TEST_ACTIF(FillWithTensorMethod)
  GUM_TEST_ACTIF(FillWithTensorAndMapMethod)
  GUM_TEST_ACTIF(ArgMaxMinFindAll)
  GUM_TEST_ACTIF(AddDummyVariables)
  GUM_TEST_ACTIF(RandomTensor)
  GUM_TEST_ACTIF(Equalities)
  GUM_TEST_ACTIF(ExpectedValue)
  GUM_TEST_ACTIF(Inverse)
  GUM_TEST_ACTIF(MinNegatif)
  GUM_TEST_ACTIF(Sgn)
  GUM_TEST_ACTIF(OperatorWithScalars)
  GUM_TEST_ACTIF(DeterministicTensor_CreatesTensorWithCorrectValue)
  GUM_TEST_ACTIF(DeterministicTensor_CreatesTensorWithCorrectLabel)
  GUM_TEST_ACTIF(UniformTensor_CreatesUniformTensor)
}   // namespace gum_tests
