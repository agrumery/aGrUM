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

namespace gum_tests {
  class [[maybe_unused]] TensorTestSuite: public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    GUM_ACTIVE_TEST(Creation) {
      gum::Tensor< double > p(new gum::MultiDimArray< double >());
      TS_ASSERT(p.empty());

      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5), other_a("a", "first var", 2);
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c)
      TS_ASSERT_THROWS(p << a << a, const gum::DuplicateElement&)
      TS_ASSERT_THROWS(p << a << other_a, const gum::DuplicateElement&)
    }

    GUM_ACTIVE_TEST(CreationWithVars) {
      gum::LabelizedVariable a("a", "first var", 2);
      gum::LabelizedVariable b("b", "second var", 4);
      gum::LabelizedVariable c("c", "third var", 5);
      gum::LabelizedVariable other_a("a", "first var", 2);

      gum::Tensor< double > p({&a, &b, &c});
      TS_ASSERT_THROWS(p << a << a, const gum::DuplicateElement&)
      TS_ASSERT_THROWS(p << a << other_a, const gum::DuplicateElement&)
    }

    GUM_ACTIVE_TEST(Normalisation) {
      gum::Tensor< double > p(new gum::MultiDimArray< double >());

      gum::LabelizedVariable a("a", "first var", 2);
      gum::LabelizedVariable b("b", "second var", 4);
      gum::LabelizedVariable c("c", "third var", 5);

      p << a << b << c;

      gum::Size taille = p.domainSize();

      try {
        gum::Instantiation i(&p);

        p.fill((double)0);

        int cpt = 1;
        for (i.setFirst(); !i.end(); ++i)
          p.set(i, (double)(cpt++));

        p.normalize();

        i.setLast();
        TS_ASSERT_EQUALS(p[i], 2.0 / (taille + 1))

        i.setFirst();
        TS_ASSERT_EQUALS(p[i], 2.0 / (taille * (taille + 1)))
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    GUM_ACTIVE_TEST(DomainSizeChanges) {
      gum::Tensor< double >  p1(new gum::MultiDimArray< double >());
      gum::LabelizedVariable var1("var1", "first var", 2);
      gum::LabelizedVariable var2("var2", "second var", 2);
      gum::LabelizedVariable var3("var3", "third var", 2);
      p1 << var1 << var2 << var3;

      TS_ASSERT_EQUALS(p1.domainSize(),
                       (var1.domainSize() * var2.domainSize() * var3.domainSize()));
    }

    GUM_ACTIVE_TEST(AddAnyNumber) {
      gum::LabelizedVariable a("a", "first var", 2);
      gum::LabelizedVariable b("b", "second var", 4);
      gum::LabelizedVariable c("c", "third var", 5);

      gum::Tensor< double > proba({&a, &b, &c});
      gum::Instantiation    i(proba);

      for (i.setFirst(); !i.end(); ++i)
        TS_GUM_ASSERT_THROWS_NOTHING(proba.set(i, (double)-1))
    }

    GUM_ACTIVE_TEST(CopyProba) {
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
      TS_ASSERT_THROWS_ANYTHING(mm.copyFrom(m))
      mm << y;
      TS_GUM_ASSERT_THROWS_NOTHING(mm.copyFrom(m))

      gum::Instantiation j(mm);
      i.chgVal(a, 0).chgVal(b, 2).chgVal(c, 2);
      j.chgVal(x, 0).chgVal(y, 2).chgVal(z, 0);
      TS_ASSERT_EQUALS(m[i], mm[j])
    }

    GUM_ACTIVE_TEST(RegressionCopy) {
      gum::LabelizedVariable a("a", "first var", 2);
      gum::LabelizedVariable b("b", "second var", 4);
      gum::LabelizedVariable c("c", "third var", 5);
      {
        gum::Tensor< double > P1({&a, &b, &c});
        gum::Tensor< double > P2({&a, &c});

        TS_GUM_ASSERT_THROWS_NOTHING(P2.copy(P1))
      }
      {
        gum::Tensor< double > P1, P2;
        P1 << a << b << c;
        P2 << a << c;
        TS_GUM_ASSERT_THROWS_NOTHING(P1.copy(P2))
      }
      {
        gum::Tensor< double > P1, P2;
        P1 << b;
        P2 << a << c;
        TS_GUM_ASSERT_THROWS_NOTHING(P1.copy(P2))
      }
    }

    GUM_ACTIVE_TEST(Equality) {
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

      TS_ASSERT_DIFFERS(p, q)
      TS_ASSERT_EQUALS(p, r)
      TS_ASSERT_DIFFERS(p, t)
      TS_ASSERT_EQUALS(p, u)
    }

    GUM_ACTIVE_TEST(MinMax) {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({1, 9, 3, 0, 5, 0, 7, 8, 9}).normalizeAsCPT();

      TS_ASSERT_EQUALS(p.min(), 0.0)
      TS_ASSERT_EQUALS(p.max(), 1.0)
      TS_ASSERT_EQUALS(p.minNonZero(), 1.0 / 13.0)
      TS_ASSERT_EQUALS(p.maxNonOne(), 9.0 / 13.0)

      p.fillWith({1, 1, 1, 1, 1, 1, 1, 1, 1});
      TS_ASSERT_EQUALS(p.maxNonOne(), 1)

      p.fillWith({0, 0, 0, 0, 0, 0, 0, 0, 0});
      TS_ASSERT_EQUALS(p.minNonZero(), 0)
    }

    GUM_ACTIVE_TEST(IsNonZeroMap) {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< double > p, q;
      p << a << b;
      p.fillWith({1, 9, 3, 0, 5, 0, 7, 8, 9}).normalizeAsCPT();
      q << a << b;
      q.fillWith({1, 1, 1, 0, 1, 0, 1, 1, 1});

      TS_ASSERT_EQUALS(p.isNonZeroMap(), q)
    }

    GUM_ACTIVE_TEST(Operators) {
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

      TS_ASSERT_EQUALS(
          pA,
          (gum::Tensor< int >() << b << c << a).fillWith({5, 18, 7, 24, 10, 24, 14, 32}))

      auto pB = p1 + p2;
      TS_ASSERT_EQUALS(pB,
                       (gum::Tensor< int >() << b << c << a).fillWith({6, 9, 8, 11, 7, 10, 9, 12}))

      auto pC = p2 / p1;
      TS_ASSERT_EQUALS(pC, (gum::Tensor< int >() << b << a << c).fillWith({5, 2, 2, 1, 7, 2, 3, 2}))

      auto pD = p2 - p1;
      TS_ASSERT_EQUALS(pD, (gum::Tensor< int >() << b << a << c).fillWith({4, 3, 3, 2, 6, 5, 5, 4}))

      TS_ASSERT_EQUALS(
          ((p1 * p2) - (p2 / p1) + p1),
          (gum::Tensor< int >() << b << a << c).fillWith({1, 19, 10, 27, 1, 25, 13, 34}))

      p = p1 * p2;
      p -= (p2 / p1);
      p += p1;
      TS_ASSERT_EQUALS(
          p,
          (gum::Tensor< int >() << b << a << c).fillWith({1, 19, 10, 27, 1, 25, 13, 34}))
    }

    GUM_ACTIVE_TEST(MargOutFunctions) {
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

        TS_ASSERT_DIFFERS(p, q)

        gum::Tensor< double > r;
        r << c << d;
        r.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});
        TS_ASSERT_DIFFERS(q, r)
        r.normalize();
        TS_ASSERT_EQUALS(q, r)

        auto joint = p * q;

        auto margAB = joint.sumOut({&c, &d});
        TS_ASSERT_EQUALS(p, margAB)
        auto margCD = joint.sumOut({&b, &a});
        TS_ASSERT_EQUALS(q, margCD)

        p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});
        TS_ASSERT_EQUALS(p.prodOut({&a}), (gum::Tensor< double >() << b).fillWith({6, 120, 504}))
        TS_ASSERT_EQUALS(p.prodOut({&b}), (gum::Tensor< double >() << a).fillWith({28, 80, 162}))

        TS_ASSERT_EQUALS(p.maxOut({&a}), (gum::Tensor< double >() << b).fillWith({3, 6, 9}))
        TS_ASSERT_EQUALS(p.maxOut({&b}), (gum::Tensor< double >() << a).fillWith({7, 8, 9}))

        TS_ASSERT_EQUALS(p.minOut({&a}), (gum::Tensor< double >() << b).fillWith({1, 4, 7}))
        TS_ASSERT_EQUALS(p.minOut({&b}), (gum::Tensor< double >() << a).fillWith({1, 2, 3}))
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    GUM_ACTIVE_TEST(MarginalizingFunctions) {
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

      TS_ASSERT_EQUALS(joint.sumOut({&c, &d}), joint.sumIn({&a, &b}))
      TS_ASSERT_EQUALS(joint.sumOut({&c, &d}), joint.sumIn({&b, &a}))

      TS_ASSERT_EQUALS(joint.prodOut({&c, &d}), joint.prodIn({&a, &b}))

      TS_ASSERT_EQUALS(joint.minOut({&c, &d}), joint.minIn({&a, &b}))

      TS_ASSERT_EQUALS(joint.maxOut({&c, &d}), joint.maxIn({&a, &b}))
    }

    GUM_ACTIVE_TEST(DeprecatedMarginalizingFunctions) {
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

      TS_ASSERT_EQUALS(joint.sumOut({&c, &d}), joint.sumIn({&a, &b}))
      TS_ASSERT_EQUALS(joint.sumOut({&c, &d}), joint.sumIn({&b, &a}))

      TS_ASSERT_EQUALS(joint.prodOut({&c, &d}), joint.prodIn({&a, &b}))

      TS_ASSERT_EQUALS(joint.minOut({&c, &d}), joint.minIn({&a, &b}))

      TS_ASSERT_EQUALS(joint.maxOut({&c, &d}), joint.maxIn({&a, &b}))
    }

    GUM_ACTIVE_TEST(AbsTensor) {
      auto a = gum::LabelizedVariable("a", "afoo", 2);
      auto b = gum::LabelizedVariable("b", "bfoo", 2);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({0, 1, 2, 3});

      gum::Tensor< double > q;
      q << a << b;
      q.fillWith({0, 3, 0, 3});

      TS_ASSERT_EQUALS((p - q).abs(), (gum::Tensor< double >() << a << b).fillWith({0, 2, 2, 0}))
      TS_ASSERT_EQUALS((q - p).abs(), (gum::Tensor< double >() << a << b).fillWith({0, 2, 2, 0}))
      TS_ASSERT_EQUALS((q - p).abs().max(), 2)
      TS_ASSERT_EQUALS((q - p).abs().min(), 0)
    }

    GUM_ACTIVE_TEST(SqTensor) {
      auto a = gum::LabelizedVariable("a", "afoo", 2);
      auto b = gum::LabelizedVariable("b", "bfoo", 2);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({0, 1, 2, 3});

      gum::Tensor< double > q;
      q << b << a;
      q.fillWith({0, 0, 3, 3});

      TS_ASSERT_EQUALS((p - q).sq(), (gum::Tensor< double >() << a << b).fillWith({0, 4, 4, 0}))
      TS_ASSERT_EQUALS((q - p).sq(), (gum::Tensor< double >() << b << a).fillWith({0, 4, 4, 0}))
      TS_ASSERT_EQUALS((q - p).sq().max(), 4)
      TS_ASSERT_EQUALS((q - p).sq().min(), 0)
    }

    GUM_ACTIVE_TEST(EntropyTensor) {
      auto                  a = gum::LabelizedVariable("a", "afoo", 2);
      gum::Tensor< double > p;
      p.add(a);
      TS_ASSERT_EQUALS(p.fillWith({0, 1}).entropy(), 0.0)
      TS_ASSERT_EQUALS(p.fillWith({1, 0}).entropy(), 0.0)
      TS_ASSERT_EQUALS(p.fillWith({0.5, 0.5}).entropy(), 1.0)
    }

    GUM_ACTIVE_TEST(ReorganizeTensor) {
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
      TS_ASSERT_DIFFERS(joint1.toString(), joint2.toString())
      auto joint3 = (q * p).reorganize({&c, &d, &a, &b});
      TS_ASSERT_EQUALS(joint1.toString(), joint3.toString())

      gum::Instantiation inst;
      inst << a << b << c << d;
      for (inst.setFirst(); !inst.end(); ++inst) {
        TS_ASSERT_EQUALS(joint1.get(inst), joint2.get(inst))
        TS_ASSERT_EQUALS(joint1.get(inst), joint3.get(inst))
      }

      TS_ASSERT_THROWS(p.reorganize({&a, &b, &c}), const gum::InvalidArgument&)
      TS_ASSERT_THROWS(p.reorganize(std::vector< const gum::DiscreteVariable* >{&a, &d}),


                       const gum::InvalidArgument&


      )
      TS_ASSERT_THROWS(p.reorganize(std::vector< const gum::DiscreteVariable* >{&a}),


                       const gum::InvalidArgument&


      )
    }

    GUM_ACTIVE_TEST(ReorganizeTensorFromNames) {
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
      TS_ASSERT_DIFFERS(joint1.toString(), joint2.toString())
      auto joint3 = (q * p).reorganize({"c", "d", "a", "b"});
      TS_ASSERT_EQUALS(joint1.toString(), joint3.toString())

      gum::Instantiation inst;
      inst << a << b << c << d;
      for (inst.setFirst(); !inst.end(); ++inst) {
        TS_ASSERT_EQUALS(joint1.get(inst), joint2.get(inst))
        TS_ASSERT_EQUALS(joint1.get(inst), joint3.get(inst))
      }

      TS_ASSERT_THROWS(p.reorganize({"a", "b", "c"}), const gum::InvalidArgument&)
      TS_ASSERT_THROWS(p.reorganize({std::string("a"), "d"}), const gum::InvalidArgument&)
      TS_ASSERT_THROWS(p.reorganize({std::string("a")}), const gum::InvalidArgument&)
      TS_ASSERT_THROWS(p.reorganize({std::string("héhé")}), const gum::InvalidArgument&)
    }

    GUM_ACTIVE_TEST(PutFirst) {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);
      auto c = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      TS_ASSERT_DIFFERS(p.toString(), p.putFirst(&b).toString())
      TS_ASSERT_EQUALS(p.toString(), p.putFirst(&b).putFirst(&a).toString())
      TS_ASSERT_EQUALS(p.toString(), p.putFirst(&a).toString())

      TS_ASSERT_THROWS(p.putFirst(&c), const gum::InvalidArgument&)
    }

    GUM_ACTIVE_TEST(PutFirstWithName) {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);
      auto c = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      TS_ASSERT_DIFFERS(p.toString(), p.putFirst("b").toString())
      TS_ASSERT_EQUALS(p.toString(), p.putFirst("b").putFirst("a").toString())
      TS_ASSERT_EQUALS(p.toString(), p.putFirst("a").toString())

      TS_ASSERT_THROWS(p.putFirst("c"), const gum::InvalidArgument&)
    }

    GUM_ACTIVE_TEST(Extraction) {
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
      TS_ASSERT_EQUALS(pot.extract(I), p)
      I.chgVal(c, 2);
      gum::Tensor< double > r;
      r << a << b;
      r.fillWith({3, 6, 9, 12, 15, 18, 21, 24, 27});
      TS_ASSERT_EQUALS(pot.reorganize({&b, &c, &a}).extract(I), r)
    }

    GUM_ACTIVE_TEST(OperatorEqual) {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< double > p;
      {
        // empty tensors are equal
        gum::Tensor< double > q;
        TS_ASSERT_EQUALS(p, q)
      }
      p << a;
      p.fillWith({1, 2, 3});
      TS_ASSERT_EQUALS(p, p)
      {
        // same tensor
        gum::Tensor< double > q;
        q << a;
        q.fillWith({1, 2, 3});
        TS_ASSERT_EQUALS(p, q)
      }
      {
        // difference values
        gum::Tensor< double > q;
        q << a;
        q.fillWith({3, 6, 9});
        TS_ASSERT_DIFFERS(p, q)
      }
      {
        // same values, different variables
        gum::Tensor< double > q;
        q << b;
        q.fillWith({1, 2, 3});
        TS_ASSERT_DIFFERS(p, q)
      }
      {
        // different dimensions
        gum::Tensor< double > q;
        q << b << a;
        q.fillWith(1);
        TS_ASSERT_DIFFERS(p, q)
      }
    }

    GUM_ACTIVE_TEST(ScaleAndTranslate) {
      auto a = gum::LabelizedVariable("a", "afoo", 3);

      gum::Tensor< double > p;
      p << a;
      p.fillWith({1, 2, 3});
      gum::Tensor< double > p3;
      p3 << a;
      p3.fillWith({3, 6, 9});

      TS_GUM_ASSERT_THROWS_NOTHING(p.scale(3.0))
      TS_ASSERT_EQUALS(p, p3)
      TS_ASSERT_EQUALS(p, p3)

      p.fillWith({1, 2, 3});
      gum::Tensor< double > p2;
      p2 << a;
      p2.fillWith({2, 3, 4});

      TS_GUM_ASSERT_THROWS_NOTHING(p.translate(1.0))
      TS_ASSERT_EQUALS(p, p2)
      TS_ASSERT_EQUALS(p, p2)


      p.fillWith({1, 2, 3});
      gum::Tensor< double > p1;
      p1 << a;
      p1.fillWith({4, 7, 10});
      p.scale(3.0).translate(1.0);
      // TS_GUM_ASSERT_THROWS_NOTHING(p.scale(3.0).translate(1.0))
      TS_ASSERT_EQUALS(p, p1)
      TS_ASSERT_EQUALS(p, p1)
    }

    GUM_ACTIVE_TEST(NormalizeAsCPT) {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< double > p;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

      auto q = p / p.sumOut({&a});
      p.normalizeAsCPT();
      TS_ASSERT_EQUALS(p, q)
      TS_ASSERT_EQUALS(q, p)

      gum::Tensor< double > p2;
      p2 << a << b;
      p2.fill(0.0f);
      TS_ASSERT_THROWS(p2.normalizeAsCPT(), const gum::FatalError&)

      gum::Tensor< double > p3;
      p3 << a << b;
      p3.fillWith({1, 2, 3, 0, 0, 0, 7, 8, 9});
      TS_ASSERT_THROWS(p2.normalizeAsCPT(), const gum::FatalError&)

      gum::Tensor< double > p4;
      p4 << a;
      p4.fillWith({1, 3, 6});
      p4.normalizeAsCPT();
      gum::Tensor< double > witness;
      witness << a;
      witness.fillWith({0.1f, 0.3f, 0.6f});
      TS_ASSERT_EQUALS(p4, witness)
    }

    GUM_ACTIVE_TEST(EmptyTensor) {
      gum::Tensor< double > p;
      gum::Instantiation    inst(p);
      double                a     = 0;
      auto                  var_a = gum::LabelizedVariable("a", "afoo", 3);

      TS_GUM_ASSERT_THROWS_NOTHING(a = p[inst];)
      TS_ASSERT_EQUALS(a, 1.0f)

      TS_GUM_ASSERT_THROWS_NOTHING(p.set(inst, a + (double)3.0f);)
      TS_ASSERT_EQUALS(p[inst], 4.0f)

      TS_GUM_ASSERT_THROWS_NOTHING(p.populate({1.0f}))
      TS_ASSERT_EQUALS(p[inst], 1.0f)
      TS_GUM_ASSERT_THROWS_NOTHING(p.populate(std::vector< double >{2.0f}))
      TS_ASSERT_EQUALS(p[inst], 2.0f)
      TS_ASSERT_THROWS(p.populate(std::vector< double >{2.0f, 3.0f}), const gum::SizeError&)

      TS_GUM_ASSERT_THROWS_NOTHING(p.apply([](double x) { return x * 2.0f + 1.0f; });)
      TS_ASSERT_EQUALS(p[inst], 5.0f)

      a = 0.3f;
      TS_GUM_ASSERT_THROWS_NOTHING(a = p.reduce([](double x, double y) { return x + y; }, 0.0f);)
      TS_ASSERT_EQUALS(a, 0.0f)

      TS_GUM_ASSERT_THROWS_NOTHING(p.populate({33.0f}))
      TS_ASSERT_EQUALS(p.domainSize(), 1u)
      TS_ASSERT_EQUALS(p.get(gum::Instantiation()), 33.0f)


      TS_GUM_ASSERT_THROWS_NOTHING(p.set(inst, (double)3.0f);)
      TS_ASSERT_EQUALS(p.sum(), 3.0f)
      TS_ASSERT_EQUALS(p.product(), 3.0f)
      TS_ASSERT_EQUALS(p.max(), 3.0f)
      TS_ASSERT_EQUALS(p.min(), 3.0f)
      TS_ASSERT_EQUALS(p.minNonZero(), 3.0f)
      TS_ASSERT_EQUALS(p.maxNonOne(), 3.0f)

      TS_GUM_ASSERT_THROWS_NOTHING(p.set(inst, (double)0.0f);)
      TS_ASSERT_EQUALS(p.minNonZero(), 0)

      TS_GUM_ASSERT_THROWS_NOTHING(p.set(inst, (double)1.0f);)
      TS_ASSERT_EQUALS(p.maxNonOne(), 1.0f)

      TS_GUM_ASSERT_THROWS_NOTHING(p.fill(2.0f);)
      TS_ASSERT_EQUALS(p[inst], 2.0f)
      TS_GUM_ASSERT_THROWS_NOTHING(p.fillWith(3.0f);)
      TS_ASSERT_EQUALS(p[inst], 3.0f)
      TS_GUM_ASSERT_THROWS_NOTHING(p.fillWith({4.0f});)
      TS_ASSERT_EQUALS(p[inst], 4.0f)
      TS_GUM_ASSERT_THROWS_NOTHING(p.fillWith(std::vector< double >({5.0f}));)
      TS_ASSERT_EQUALS(p[inst], 5.0f)

      TS_ASSERT_EQUALS(p.entropy(), 0.0f)

      TS_ASSERT_EQUALS(p.fillWith(2.0f).sq()[inst], 4.0f)
      TS_ASSERT_EQUALS(p.fillWith(-2.0f).sq()[inst], 4.0f)
      TS_ASSERT_EQUALS(p.fillWith(2.0f).abs()[inst], 2.0f)
      TS_ASSERT_EQUALS(p.fillWith(-2.0f).abs()[inst], 2.0f)

      TS_ASSERT_EQUALS(p.fillWith(2.0f).normalize()[inst], 1.0f)
      TS_ASSERT_EQUALS(p.fillWith(0.0f).normalize()[inst], 0.0f)

      TS_ASSERT_EQUALS(p.fillWith(2.0f).scale(4)[inst], 8.0f)
      TS_ASSERT_EQUALS(p.fillWith(2.0f).translate(4)[inst], 6.0f)


      TS_ASSERT_EQUALS(p.fillWith(2.0f).sumOut({&var_a})[inst], 2.0f)
      TS_ASSERT_EQUALS(p.fillWith(3.0f).sumIn({&var_a})[inst], 3.0f)
      TS_ASSERT_EQUALS(p.fillWith(4.0f).prodOut({&var_a})[inst], 4.0f)
      TS_ASSERT_EQUALS(p.fillWith(5.0f).prodIn({&var_a})[inst], 5.0f)
      TS_ASSERT_EQUALS(p.fillWith(6.0f).maxIn({&var_a})[inst], 6.0f)
      TS_ASSERT_EQUALS(p.fillWith(7.0f).maxOut({&var_a})[inst], 7.0f)
      TS_ASSERT_EQUALS(p.fillWith(8.0f).minOut({&var_a})[inst], 8.0f)
      TS_ASSERT_EQUALS(p.fillWith(9.0f).minOut({&var_a})[inst], 9.0f)

      TS_ASSERT_EQUALS(p.fillWith(0.0f).isNonZeroMap()[inst], 0.0f)
      TS_ASSERT_EQUALS(p.fillWith(1.0f).isNonZeroMap()[inst], 1.0f)
      TS_ASSERT_EQUALS(p.fillWith(0.5f).isNonZeroMap()[inst], 1.0f)
      TS_ASSERT_EQUALS(p.fillWith(-0.5f).isNonZeroMap()[inst], 1.0f)
    }

    GUM_ACTIVE_TEST(OperationForEmptyTensor) {
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
      TS_ASSERT_EQUALS(p + q, res)
      tmp = p;
      tmp += q;
      TS_ASSERT_EQUALS(tmp, res)

      TS_ASSERT_EQUALS(q + p, res)
      tmp = q;
      tmp += p;
      TS_ASSERT_EQUALS(tmp, res)

      res.fillWith({2, 4, 6, 8, 10, 12, 14, 16, 18});
      TS_ASSERT_EQUALS(p * q, res)
      tmp = p;
      tmp *= q;
      TS_ASSERT_EQUALS(tmp, res)

      TS_ASSERT_EQUALS(q * p, res)
      tmp = q;
      tmp *= p;
      TS_ASSERT_EQUALS(tmp, res)

      res.fillWith({-1, 0, 1, 2, 3, 4, 5, 6, 7});
      TS_ASSERT_EQUALS(p - q, res)
      tmp = p;
      tmp -= q;
      TS_ASSERT_EQUALS(tmp, res)

      res.fillWith({1, 0, -1, -2, -3, -4, -5, -6, -7});
      TS_ASSERT_EQUALS(q - p, res)
      tmp = q;
      tmp -= p;
      TS_ASSERT_EQUALS(tmp, res)

      res.fillWith({1 / 2.0f,
                    2 / 2.0f,
                    3 / 2.0f,
                    4 / 2.0f,
                    5 / 2.0f,
                    6 / 2.0f,
                    7 / 2.0f,
                    8 / 2.0f,
                    9 / 2.0f});
      TS_ASSERT_EQUALS(p / q, res)
      tmp = p;
      tmp /= q;
      TS_ASSERT_EQUALS(tmp, res)

      res.fillWith({2.0f / 1,
                    2.0f / 2,
                    2.0f / 3,
                    2.0f / 4,
                    2.0f / 5,
                    2.0f / 6,
                    2.0f / 7,
                    2.0f / 8,
                    2.0f / 9});
      TS_ASSERT_EQUALS(q / p, res)
      tmp = q;
      tmp /= p;
      TS_ASSERT_EQUALS(tmp, res)
    }

    GUM_ACTIVE_TEST(OperationForTwoEmptyTensors) {
      gum::Tensor< double > p;
      p.fill(3);

      gum::Tensor< double > q;
      q.fill(2);

      gum::Tensor< double > res, tmp;
      res.fill(5);
      TS_ASSERT_EQUALS(p + q, res)
      tmp = p;
      tmp += q;
      TS_ASSERT_EQUALS(tmp, res)

      TS_ASSERT_EQUALS(q + p, res)
      tmp = q;
      tmp += p;
      TS_ASSERT_EQUALS(tmp, res)

      res.fillWith({6});
      TS_ASSERT_EQUALS(p * q, res)
      tmp = p;
      tmp *= q;
      TS_ASSERT_EQUALS(tmp, res)

      TS_ASSERT_EQUALS(q * p, res)
      tmp = q;
      tmp *= p;
      TS_ASSERT_EQUALS(tmp, res)

      res.fillWith({1});
      TS_ASSERT_EQUALS(p - q, res)
      tmp = p;
      tmp -= q;
      TS_ASSERT_EQUALS(tmp, res)

      res.fillWith({-1});
      TS_ASSERT_EQUALS(q - p, res)
      tmp = q;
      tmp -= p;
      TS_ASSERT_EQUALS(tmp, res)

      res.fillWith({3 / 2.0f});
      TS_ASSERT_EQUALS(p / q, res)
      tmp = p;
      tmp /= q;
      TS_ASSERT_EQUALS(tmp, res)

      res.fillWith({2.0f / 3});
      TS_ASSERT_LESS_THAN((q / p - res).abs().max(), 1e-7)
      tmp = q;
      tmp /= p;
      TS_ASSERT_LESS_THAN((tmp - res).abs().max(), 1e-7)
    }

    GUM_ACTIVE_TEST(LoopsForEmptyTensor) {
      gum::Tensor< double > p;
      gum::Instantiation    inst(p);
      p.fill(3);

      gum::Size cpt   = 0;
      double    total = 0.0;
      for (inst.setFirst(); !inst.end(); ++inst) {
        cpt++;
        total += p[inst];
      }
      TS_ASSERT_EQUALS(cpt, (gum::Size)1)
      TS_ASSERT_EQUALS(total, 3)

      cpt   = 0;
      total = 0.0;
      for (inst.setLast(); !inst.end(); --inst) {
        cpt++;
        total += p[inst];
      }
      TS_ASSERT_EQUALS(cpt, (gum::Size)1)
      TS_ASSERT_EQUALS(total, 3)
    }

    void /*test*/ EliminatationOffAllVariables() {
      auto a = gum::LabelizedVariable("a", "afoo", 3);
      auto b = gum::LabelizedVariable("b", "bfoo", 3);

      gum::Tensor< int > p;
      p << a << b;
      p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});
      TS_ASSERT_EQUALS(p.sumOut({&a, &b}).toString(), "[45]")
    }

    GUM_ACTIVE_TEST(KL) {
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

      TS_GUM_ASSERT_THROWS_NOTHING(res = p.KL(p))
      TS_ASSERT_EQUALS(res, 0.0f)

      TS_ASSERT_THROWS(res = p.KL(s), const gum::InvalidArgument&)
      TS_ASSERT_THROWS(res = s.KL(p), const gum::InvalidArgument&)

      TS_GUM_ASSERT_THROWS_NOTHING(res = p.KL(q))
      TS_ASSERT_DELTA(res, 0.0 + 1.0 * log2(1.0 / 0.5), TS_GUM_SMALL_ERROR)
      TS_ASSERT_THROWS(res = q.KL(p), const gum::FatalError&)

      TS_GUM_ASSERT_THROWS_NOTHING(res = p.KL(r))
      TS_ASSERT_DELTA(res, 0.0 + 1.0 * log2(1.0 / 0.3), TS_GUM_SMALL_ERROR)
      TS_ASSERT_THROWS(res = r.KL(p), const gum::FatalError&)

      TS_GUM_ASSERT_THROWS_NOTHING(res = q.KL(r))
      TS_ASSERT_DELTA(res, 0.5 * log2(0.5 / 0.7) + 0.5 * log2(0.5 / 0.3), TS_GUM_SMALL_ERROR)

      TS_GUM_ASSERT_THROWS_NOTHING(res = r.KL(q))
      TS_ASSERT_DELTA(res, 0.7 * log2(0.7 / 0.5) + 0.3 * log2(0.3 / 0.5), TS_GUM_SMALL_ERROR)
    }

    GUM_ACTIVE_TEST(TensorDraw) {
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
        TS_ASSERT_DELTA(s, int(0.2 * NBRITER), DELTA)

        p.add(w);
        TS_ASSERT_THROWS(auto x [[maybe_unused]] = p.draw(), const gum::FatalError&)

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
        TS_ASSERT_DELTA(s0, int(0.3 * NBRITER), DELTA)
        TS_ASSERT_DELTA(s1, int(0.6 * NBRITER), DELTA)
        TS_ASSERT_DELTA(s2, int(0.1 * NBRITER), DELTA)
      } catch (const gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    GUM_ACTIVE_TEST(VariableAccessor) {
      gum::LabelizedVariable v("v", "v", 2), w("w", "w", 3);
      gum::Tensor< double >  p;
      p.add(v);
      p.add(w);

      TS_ASSERT(p.variable(0) == p.variable("v"))
      TS_ASSERT(p.variable(1) == p.variable("w"))
      TS_ASSERT(p.variable(1) != p.variable("v"))
      TS_ASSERT(p.variable(0) != p.variable("w"))

      TS_ASSERT_THROWS(p.variable("ZZ"), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(FillWithTensorMethod) {
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

      TS_ASSERT_EQUALS(p.domainSize(), (gum::Size)6)
      TS_ASSERT(pp.domainSize() == (gum::Size)6)

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
          TS_ASSERT_EQUALS(vp, vpp)
        } catch (gum::Exception& e) { GUM_SHOWERROR(e); }

      // errors
      gum::Tensor< int > bad_p;
      bad_p.add(w);
      TS_ASSERT_THROWS(bad_p.fillWith(p), const gum::InvalidArgument&)

      gum::Tensor< int > bad_p2;
      bad_p2.add(vvv);
      bad_p2.add(www);
      TS_ASSERT_THROWS(bad_p2.fillWith(p), const gum::InvalidArgument&)

      gum::Tensor< int > bad_p3;
      bad_p3.add(w);
      bad_p3.add(z);
      TS_ASSERT_THROWS(bad_p3.fillWith(p), const gum::InvalidArgument&)

      gum::Tensor< int >     bad_p4;
      gum::LabelizedVariable badv("v", "v", 0);
      badv.addLabel("3").addLabel("1");
      bad_p4.add(w);
      bad_p4.add(badv);
      TS_ASSERT_THROWS(bad_p4.fillWith(p), const gum::InvalidArgument&)
    }

    GUM_ACTIVE_TEST(FillWithTensorAndMapMethod) {
      gum::LabelizedVariable v("v", "v", 2), w("w", "w", 3);
      gum::Tensor< int >     p;
      p.add(v);
      p.add(w);

      gum::LabelizedVariable vv("vv", "vv", 2), ww("ww", "ww", 3);
      gum::Tensor< int >     pp;
      pp.add(ww);
      pp.add(vv);

      TS_ASSERT_EQUALS(p.domainSize(), (gum::Size)6)
      TS_ASSERT_EQUALS(pp.domainSize(), (gum::Size)6)

      p.fillWith({1, 2, 3, 4, 5, 6});
      TS_GUM_ASSERT_THROWS_NOTHING(pp.fillWith(p, {"w", "v"}))
      TS_ASSERT_THROWS(pp.fillWith(p, {"v", "w"}), const gum::InvalidArgument&)
    }

    GUM_ACTIVE_TEST(ArgMaxMinFindAll) {
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

    GUM_ACTIVE_TEST(AddDummyVariables) {
      {
        gum::LabelizedVariable v("v", "v", 0);

        gum::Tensor< double > p;
        TS_ASSERT_EQUALS(v.domainSize(), (gum::Size)0)
        TS_ASSERT_THROWS(p.add(v), const gum::InvalidArgument&)

        v.addLabel("first");
        TS_ASSERT_EQUALS(v.domainSize(), (gum::Size)1)
        TS_GUM_ASSERT_THROWS_NOTHING(p.add(v))

        p = gum::Tensor< double >();
        v.addLabel("second");
        TS_ASSERT_EQUALS(v.domainSize(), (gum::Size)2)
        TS_GUM_ASSERT_THROWS_NOTHING(p.add(v))
      }

      {
        gum::RangeVariable v("v", "v");
        v.setMinVal(1);
        v.setMaxVal(0);
        gum::Tensor< double > p;
        TS_ASSERT_EQUALS(v.domainSize(), (gum::Size)0)
        TS_ASSERT_THROWS(p.add(v), const gum::InvalidArgument&)

        v.setMaxVal(1);
        TS_ASSERT_EQUALS(v.domainSize(), (gum::Size)1)
        TS_GUM_ASSERT_THROWS_NOTHING(p.add(v))

        p = gum::Tensor< double >();
        v.setMaxVal(2);
        TS_ASSERT_EQUALS(v.domainSize(), (gum::Size)2)
        TS_GUM_ASSERT_THROWS_NOTHING(p.add(v))
      }

      {
        gum::DiscretizedVariable< int > v("v", "v");
        gum::Tensor< double >           p;
        TS_ASSERT_EQUALS(v.domainSize(), (gum::Size)0)
        TS_ASSERT_THROWS(p.add(v), const gum::InvalidArgument&)

        v.addTick(1);
        TS_ASSERT_EQUALS(v.domainSize(), (gum::Size)0)
        TS_ASSERT_THROWS(p.add(v), const gum::InvalidArgument&)

        v.addTick(2);
        TS_ASSERT_EQUALS(v.domainSize(), (gum::Size)1)
        TS_GUM_ASSERT_THROWS_NOTHING(p.add(v))

        p = gum::Tensor< double >();
        v.addTick(3);
        TS_ASSERT_EQUALS(v.domainSize(), (gum::Size)2)
        TS_GUM_ASSERT_THROWS_NOTHING(p.add(v))
      }
    }

    GUM_ACTIVE_TEST(RandomTensor) {
      gum::LabelizedVariable u("u", "u", 4), v("v", "v", 2), w("w", "w", 3);
      gum::Tensor< double >  p;
      p.add(u);
      p.add(v);
      p.add(w);
      gum::Instantiation I(p);

      p.random();
      for (I.setFirst(); !I.end(); I.inc()) {
        TS_ASSERT_LESS_THAN_EQUALS(p[I], 1.0)
      }

      p.randomDistribution();
      double cum = 0.0;
      for (I.setFirst(); !I.end(); I.inc()) {
        TS_ASSERT_LESS_THAN_EQUALS(p[I], 1.0)
        cum += p[I];
      }
      TS_ASSERT_DELTA(cum, 1.0, 1e-6)

      p.randomCPT();
      const gum::DiscreteVariable& var = p.variable(0);
      for (I.setFirstNotVar(var); !I.end(); I.incNotVar(var)) {
        cum = 0.0;
        for (I.setFirstVar(var); !I.end(); I.incVar(var)) {
          TS_ASSERT_LESS_THAN_EQUALS(p[I], 1.0)
          cum += p[I];
        }
        TS_ASSERT_DELTA(cum, 1.0, 1e-6)
        I.unsetEnd();
      }

      p.fillWith(1.0).normalizeAsCPT();   // every thing should be 0.25
      for (I.setFirst(); !I.end(); I.inc()) {
        TS_ASSERT_DELTA(p[I], 0.25, 1e-6)
      }

      for (double alpha = 0.0; alpha <= 1; alpha += 0.1) {
        p.fillWith(1.0).normalizeAsCPT();   // every thing should be 0.25
        p.noising(alpha);
        double min = (1 - alpha) * 0.25 + alpha * 0.0;
        double max = (1 - alpha) * 0.25 + alpha * 1.0;
        for (I.setFirst(); !I.end(); I.inc()) {
          TS_ASSERT_LESS_THAN_EQUALS(min, p[I])
          TS_ASSERT_LESS_THAN_EQUALS(p[I], max)
        }
      }
    }

    GUM_ACTIVE_TEST(Equalities) {
      gum::LabelizedVariable u("u", "u", 4);
      gum::LabelizedVariable v("v", "v", 2);
      gum::LabelizedVariable w("w", "w", 3);
      gum::Tensor< double >  p;
      p.add(u);
      p.add(v);
      p.add(w);
      p.random();
      gum::Tensor< double > q(p);
      TS_ASSERT_EQUALS(p, q)
      gum::Instantiation i(q);
      i.setLast();
      q.set(i, 0);
      TS_ASSERT_DIFFERS(p, q)
      q.fillWith(p);
      TS_ASSERT_EQUALS(p, q)
      q.fillWith(1);
      TS_ASSERT_DIFFERS(p, q)
      q.fillWith(p);
      TS_ASSERT_EQUALS(p, q)
      gum::LabelizedVariable x("x", "Unknown", 5);
      q.add(x);
      TS_ASSERT_DIFFERS(p, q)
      q.erase("x");
      q.fillWith(p);
      TS_ASSERT_EQUALS(p, q)
      q.erase("u");
      TS_ASSERT_DIFFERS(p, q)
    }

    GUM_ACTIVE_TEST(ExpectedValue) {
      gum::LabelizedVariable u("u", "u", 4);
      gum::LabelizedVariable v("v", "v", 2);
      gum::LabelizedVariable w("w", "w", 3);
      gum::Tensor< double >  p;
      p.add(u);
      p.add(v);
      p.add(w);
      p.randomDistribution();

      TS_GUM_ASSERT_QUASI_EQUALS(
          p.expectedValue([](const gum::Instantiation&) -> auto { return 1.0; }),
          1.0)

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

      TS_GUM_ASSERT_QUASI_EQUALS(
          q.expectedValue([](const gum::Instantiation& i) -> auto { return double(i.val(0)); }),
          mean);
    }

    GUM_ACTIVE_TEST(Inverse) {
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
      TS_ASSERT_DELTA((p * q).min(), 1.0, 1e-7)
      TS_ASSERT_DELTA((p * q).max(), 1.0, 1e-7)
    }

    GUM_ACTIVE_TEST(MinNegatif) {
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

      TS_ASSERT_DELTA(p.min(), -150.0, 1e-7)
      TS_ASSERT_DELTA(p.max(), -150 + 4 * 2 * 3 - 1, 1e-7)
    }

    GUM_ACTIVE_TEST(Sgn) {
      gum::LabelizedVariable u("u", "u", 4);
      gum::LabelizedVariable v("v", "v", 2);
      gum::LabelizedVariable w("w", "w", 3);
      gum::Tensor< double >  p;
      p.add(u);
      p.add(v);
      p.add(w);
      p.random();

      auto r = p * 2;
      TS_ASSERT_EQUALS((r - p).sgn().min(), 1.0);
      TS_ASSERT_EQUALS((r - p).sgn().max(), 1.0);
      TS_ASSERT_EQUALS((p - r).sgn().min(), -1.0);
      TS_ASSERT_EQUALS((p - r).sgn().max(), -1.0);
      TS_ASSERT_EQUALS((p - p).sgn().min(), 0.0);
      TS_ASSERT_EQUALS((p - p).sgn().max(), 0.0);
    }

    GUM_ACTIVE_TEST(OperatorWithScalars) {
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
        TS_ASSERT_EQUALS(p * 2, q)
      }
      {
        gum::Tensor< double > q(p);
        q.scale(0.5);
        TS_ASSERT_EQUALS(p / 2, q)
      }
      {
        gum::Tensor< double > q(p);
        q.translate(1.5);
        TS_ASSERT_EQUALS(p + 1.5, q)
      }
      {
        gum::Tensor< double > q(p);
        q.translate(3.2);
        TS_ASSERT_EQUALS(p - 3.2, q)
      }
      {
        gum::Tensor< double > q(p);
        gum::Tensor< double > r(p);
        q.translate(4.33);
        r += 4.33;
        TS_ASSERT_EQUALS(r, q)
      }
      {
        gum::Tensor< double > q(p);
        gum::Tensor< double > r(p);
        q.translate(-4.33);
        r -= 4.33;
        TS_ASSERT_EQUALS(r, q)
      }
      {
        gum::Tensor< double > q(p);
        gum::Tensor< double > r(p);
        q.scale(4.33);
        r *= 4.33;
        TS_ASSERT_EQUALS(r, q)
      }
      {
        gum::Tensor< double > q(p);
        gum::Tensor< double > r(p);
        q.scale(1 / 4.33);
        r /= 4.33;
        TS_ASSERT_EQUALS(r, q)
      }
    }

    GUM_ACTIVE_TEST(DeterministicTensor_CreatesTensorWithCorrectValue) {
      gum::RangeVariable var("var", "Variable1", 1, 4);
      const gum::Idx     value = 2;

      auto tensor = gum::Tensor< double >::deterministicTensor(var, value);

      TS_ASSERT_EQUALS(tensor.sum(), 1.0);

      TS_ASSERT_EQUALS(tensor.max(), 1.0);
      const auto& [x, y] = tensor.argmax();
      TS_ASSERT_EQUALS(x.size(), gum::Size(1))
      const auto& i = *x.begin();
      TS_ASSERT_EQUALS(i.val(0), value);

      TS_ASSERT_EQUALS(tensor.min(), 0.0);
      TS_ASSERT_EQUALS(tensor.minNonZero(), 1.0);
      TS_ASSERT_EQUALS(tensor.maxNonOne(), 0.0);
    }

    GUM_ACTIVE_TEST(DeterministicTensor_CreatesTensorWithCorrectLabel) {
      const gum::RangeVariable var("var", "Variable1", 1, 4);
      const std::string        label = "2";

      const auto tensor = gum::Tensor< double >::deterministicTensor(var, label);

      TS_ASSERT_EQUALS(tensor.sum(), 1.0);

      TS_ASSERT_EQUALS(tensor.max(), 1.0);
      const auto& [x, y] = tensor.argmax();
      TS_ASSERT_EQUALS(x.size(), gum::Size(1))
      const auto& i = *x.begin();
      TS_ASSERT_EQUALS(i.val(0), gum::Idx(1));

      TS_ASSERT_EQUALS(tensor.min(), 0.0);
      TS_ASSERT_EQUALS(tensor.minNonZero(), 1.0);
      TS_ASSERT_EQUALS(tensor.maxNonOne(), 0.0);
    }

    GUM_ACTIVE_TEST(UniformTensor_CreatesUniformTensor) {
      const gum::RangeVariable var("var", "Variable1", 0, 3);

      const auto tensor = gum::Tensor< double >::uniformTensor(var);

      TS_ASSERT_EQUALS(tensor.sum(), 1.0);
      TS_ASSERT_EQUALS(tensor.max(), 0.25);
      TS_ASSERT_EQUALS(tensor.min(), 0.25);
    }

    private:
    static void _testval_for_set_(const gum::Tensor< int >&             p,
                                  int                                   val,
                                  const gum::Set< gum::Instantiation >& s,
                                  gum::Size                             expected_size) {
      gum::Instantiation ip(p);

      TS_ASSERT_EQUALS(s.size(), expected_size)
      for (ip.setFirst(); !ip.end(); ++ip) {
        if (s.contains(ip)) {
          TS_ASSERT_EQUALS(p[ip], val)
        } else {
          TS_ASSERT_DIFFERS(p[ip], val)
        }
      }
    }
  };
}   // namespace gum_tests
