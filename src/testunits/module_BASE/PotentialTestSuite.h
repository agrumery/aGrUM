
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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


#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/tools/variables/discretizedVariable.h>
#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/tools/variables/rangeVariable.h>

#include <agrum/tools/multidim/ICIModels/multiDimLogit.h>
#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/multidim/instantiation.h>
#include <agrum/tools/multidim/potential.h>

namespace gum_tests {

    class PotentialTestSuite : public CxxTest::TestSuite {
    public:
        void setUp() {}

        void tearDown() {}

        void testCreation() {
            gum::Potential<double> p(new gum::MultiDimArray<double>());
            TS_ASSERT(p.empty());

            gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
                    c("c", "third var", 5), other_a("a", "first var", 2);
            TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c);
            TS_ASSERT_THROWS(p << a << a, gum::DuplicateElement);
            TS_ASSERT_THROWS(p << a << other_a, gum::DuplicateElement);
        }

        void testNormalisation() {
            gum::Potential<double> p(new gum::MultiDimArray<double>());

            gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
                    c("c", "third var", 5);

            p << a << b << c;

            gum::Size taille = p.domainSize();

            try {
                gum::Instantiation i(&p);

                p.fill((double) 0);

                int cpt = 1;
                for (i.setFirst(); !i.end(); ++i)
                    p.set(i, (double) (cpt++));

                p.normalize();

                i.setLast();
                TS_ASSERT_EQUALS(p[i], 2.0 / (taille + 1));

                i.setFirst();
                TS_ASSERT_EQUALS(p[i], 2.0 / (taille * (taille + 1)));

            } catch (gum::Exception &e) {GUM_SHOWERROR(e); }
        }

        void testDomainSizeChanges() {
            gum::Potential<double> p1(new gum::MultiDimArray<double>());
            gum::LabelizedVariable var1("var1", "first var", 2),
                    var2("var2", "second var", 2), var3("var3", "third var", 2);
            p1 << var1 << var2 << var3;

            TS_ASSERT_EQUALS(
                    p1.domainSize(),
                    (var1.domainSize() * var2.domainSize() * var3.domainSize()));
        }

        void testAddAnyNumber() {
            gum::Potential<double> proba(new gum::MultiDimArray<double>());
            gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
                    c("c", "third var", 5);
            proba << a << b << c;

            gum::Instantiation i(proba);

            for (i.setFirst(); !i.end(); ++i) TS_GUM_ASSERT_THROWS_NOTHING(proba.set(i, (double) -1));
        }

        void testCopyProba() {
            gum::Potential<double> m(new gum::MultiDimArray<double>());
            gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
                    c("c", "third var", 5);

            m << a << b << c;
            gum::Instantiation i(m);

            gum::Size cpt = 0;

            for (i.setFirst(); !i.end(); ++i) {
                m.set(i, double(cpt));
                cpt++;
            }

            gum::Potential<double> mm(new gum::MultiDimArray<double>());

            gum::LabelizedVariable x("x", "first var", 2), y("y", "second var", 4),
                    z("z", "third var", 5);
            mm << x << z;
            TS_ASSERT_THROWS_ANYTHING(mm.copyFrom(m));
            mm << y;
            TS_GUM_ASSERT_THROWS_NOTHING(mm.copyFrom(m));

            gum::Instantiation j(mm);
            i.chgVal(a, 0).chgVal(b, 2).chgVal(c, 2);
            j.chgVal(x, 0).chgVal(y, 2).chgVal(z, 0);
            TS_ASSERT_EQUALS(m[i], mm[j]);
        }

        void testRegressionCopy() {
            gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
                    c("c", "third var", 5);

            {
                gum::Potential<double> P1, P2;
                P1 << a << b << c;
                P2 << a << c;
                TS_GUM_ASSERT_THROWS_NOTHING(P2.copy(P1));
            }

            {
                gum::Potential<double> P1, P2;
                P1 << a << b << c;
                P2 << a << c;
                TS_GUM_ASSERT_THROWS_NOTHING(P1.copy(P2));
            }

            {
                gum::Potential<double> P1, P2;
                P1 << b;
                P2 << a << c;
                TS_GUM_ASSERT_THROWS_NOTHING(P1.copy(P2));
            }
        }

        void testEquality() {
            auto a = gum::LabelizedVariable("a", "afoo", 3);
            auto b = gum::LabelizedVariable("b", "bfoo", 3);
            auto c = gum::LabelizedVariable("c", "cfoo", 3);

            gum::Potential<int> p, q, r, t, u;
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

            TS_ASSERT(p != q);
            TS_ASSERT(p == r);
            TS_ASSERT(p != t);
            TS_ASSERT(p == u);
        }

        void testMinMax() {
            auto a = gum::LabelizedVariable("a", "afoo", 3);
            auto b = gum::LabelizedVariable("b", "bfoo", 3);

            gum::Potential<double> p;
            p << a << b;
            p.fillWith({1, 9, 3, 0, 5, 0, 7, 8, 9}).normalizeAsCPT();

            TS_ASSERT_EQUALS(p.min(), 0.0);
            TS_ASSERT_EQUALS(p.max(), 1.0);
            TS_ASSERT_EQUALS(p.minNonZero(), 1.0 / 13.0);
            TS_ASSERT_EQUALS(p.maxNonOne(), 9.0 / 13.0);

            p.fillWith({1, 1, 1, 1, 1, 1, 1, 1, 1});
            TS_ASSERT_EQUALS(p.maxNonOne(), 1);

            p.fillWith({0, 0, 0, 0, 0, 0, 0, 0, 0});
            TS_ASSERT_EQUALS(p.minNonZero(), 0);
        }

        void testIsNonZeroMap() {
            auto a = gum::LabelizedVariable("a", "afoo", 3);
            auto b = gum::LabelizedVariable("b", "bfoo", 3);

            gum::Potential<double> p, q;
            p << a << b;
            p.fillWith({1, 9, 3, 0, 5, 0, 7, 8, 9}).normalizeAsCPT();
            q << a << b;
            q.fillWith({1, 1, 1, 0, 1, 0, 1, 1, 1});

            TS_ASSERT_EQUALS(p.isNonZeroMap(), q);
        }

        void testOperators() {
            auto a = gum::LabelizedVariable("a", "afoo");
            auto b = gum::LabelizedVariable("b", "bfoo");
            auto c = gum::LabelizedVariable("c", "cfoo");

            gum::Potential<int> p1;
            p1 << a << b;
            p1.fillWith({1, 2, 3, 4});

            gum::Potential<int> p2;
            p2 << b << c;
            p2.fillWith({5, 6, 7, 8});

            auto p = p1;

            // just checking memory allocation (what else ?)
            auto pA = p1 * p2;
            TS_ASSERT_EQUALS(pA.toString(),
                             "<b:0|c:0|a:0> :: 5 /"
                             "<b:1|c:0|a:0> :: 18 /"
                             "<b:0|c:1|a:0> :: 7 /"
                             "<b:1|c:1|a:0> :: 24 /"
                             "<b:0|c:0|a:1> :: 10 /"
                             "<b:1|c:0|a:1> :: 24 /"
                             "<b:0|c:1|a:1> :: 14 /"
                             "<b:1|c:1|a:1> :: 32");

            auto pB = p1 + p2;
            TS_ASSERT_EQUALS(pB.toString(),
                             "<b:0|c:0|a:0> :: 6 /"
                             "<b:1|c:0|a:0> :: 9 /"
                             "<b:0|c:1|a:0> :: 8 /"
                             "<b:1|c:1|a:0> :: 11 /"
                             "<b:0|c:0|a:1> :: 7 /"
                             "<b:1|c:0|a:1> :: 10 /"
                             "<b:0|c:1|a:1> :: 9 /"
                             "<b:1|c:1|a:1> :: 12");

            auto pC = p2 / p1;
            TS_ASSERT_EQUALS(pC.toString(),
                             "<b:0|a:0|c:0> :: 5 /"
                             "<b:1|a:0|c:0> :: 2 /"
                             "<b:0|a:1|c:0> :: 2 /"
                             "<b:1|a:1|c:0> :: 1 /"
                             "<b:0|a:0|c:1> :: 7 /"
                             "<b:1|a:0|c:1> :: 2 /"
                             "<b:0|a:1|c:1> :: 3 /"
                             "<b:1|a:1|c:1> :: 2");

            auto pD = p2 - p1;
            TS_ASSERT_EQUALS(pD.toString(),
                             "<b:0|a:0|c:0> :: 4 /"
                             "<b:1|a:0|c:0> :: 3 /"
                             "<b:0|a:1|c:0> :: 3 /"
                             "<b:1|a:1|c:0> :: 2 /"
                             "<b:0|a:0|c:1> :: 6 /"
                             "<b:1|a:0|c:1> :: 5 /"
                             "<b:0|a:1|c:1> :: 5 /"
                             "<b:1|a:1|c:1> :: 4");

            TS_ASSERT_EQUALS(((p1 * p2) - (p2 / p1) + p1).toString(),
                             "<b:0|a:0|c:0> :: 1 /"
                             "<b:1|a:0|c:0> :: 19 /"
                             "<b:0|a:1|c:0> :: 10 /"
                             "<b:1|a:1|c:0> :: 27 /"
                             "<b:0|a:0|c:1> :: 1 /"
                             "<b:1|a:0|c:1> :: 25 /"
                             "<b:0|a:1|c:1> :: 13 /"
                             "<b:1|a:1|c:1> :: 34");

            p = p1 * p2;
            p -= (p2 / p1);
            p += p1;
            TS_ASSERT_EQUALS(p.toString(),
                             "<b:0|a:0|c:0> :: 1 /"
                             "<b:1|a:0|c:0> :: 19 /"
                             "<b:0|a:1|c:0> :: 10 /"
                             "<b:1|a:1|c:0> :: 27 /"
                             "<b:0|a:0|c:1> :: 1 /"
                             "<b:1|a:0|c:1> :: 25 /"
                             "<b:0|a:1|c:1> :: 13 /"
                             "<b:1|a:1|c:1> :: 34");
        }

        void testMargOutFunctions() {
            try {
                auto a = gum::LabelizedVariable("a", "afoo", 3);
                auto b = gum::LabelizedVariable("b", "bfoo", 3);
                auto c = gum::LabelizedVariable("c", "cfoo", 3);
                auto d = gum::LabelizedVariable("d", "dfoo", 3);

                gum::Potential<double> p;
                p << a << b;
                p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});
                p.normalize();

                gum::Potential<double> q;
                q << c << d;
                q.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});
                q.normalize();

                TS_ASSERT(p != q);

                gum::Potential<double> r;
                r << c << d;
                r.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});
                TS_ASSERT(q != r);
                r.normalize();
                TS_ASSERT(q == r);

                auto joint = p * q;

                auto margAB = joint.margSumOut({&c, &d});
                TS_ASSERT((p == margAB));
                auto margCD = joint.margSumOut({&b, &a});
                TS_ASSERT((q == margCD));

                p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});
                TS_ASSERT_EQUALS(p.margProdOut({&a}).toString(),
                                 "<b:0> :: 6 /<b:1> :: 120 /<b:2> :: 504");
                TS_ASSERT_EQUALS(p.margProdOut({&b}).toString(),
                                 "<a:0> :: 28 /<a:1> :: 80 /<a:2> :: 162");

                TS_ASSERT_EQUALS(p.margMaxOut({&a}).toString(),
                                 "<b:0> :: 3 /<b:1> :: 6 /<b:2> :: 9");
                TS_ASSERT_EQUALS(p.margMaxOut({&b}).toString(),
                                 "<a:0> :: 7 /<a:1> :: 8 /<a:2> :: 9");

                TS_ASSERT_EQUALS(p.margMinOut({&a}).toString(),
                                 "<b:0> :: 1 /<b:1> :: 4 /<b:2> :: 7");
                TS_ASSERT_EQUALS(p.margMinOut({&b}).toString(),
                                 "<a:0> :: 1 /<a:1> :: 2 /<a:2> :: 3");
            } catch (gum::Exception &e) {GUM_SHOWERROR(e); }
        }

        void testMargInFunctions() {
            auto a = gum::LabelizedVariable("a", "afoo", 3);
            auto b = gum::LabelizedVariable("b", "bfoo", 3);
            auto c = gum::LabelizedVariable("c", "cfoo", 3);
            auto d = gum::LabelizedVariable("d", "dfoo", 3);

            gum::Potential<double> p;
            p << a << b;
            p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

            gum::Potential<double> q;
            q << c << d;
            q.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

            auto joint = p * q;

            TS_ASSERT((joint.margSumOut({&c, &d}) == joint.margSumIn({&a, &b})));
            TS_ASSERT((joint.margSumOut({&c, &d}) == joint.margSumIn({&b, &a})));

            TS_ASSERT((joint.margProdOut({&c, &d}) == joint.margProdIn({&a, &b})));

            TS_ASSERT((joint.margMinOut({&c, &d}) == joint.margMinIn({&a, &b})));

            TS_ASSERT((joint.margMaxOut({&c, &d}) == joint.margMaxIn({&a, &b})));
        }

        void testAbsPotential() {
            auto a = gum::LabelizedVariable("a", "afoo", 2);
            auto b = gum::LabelizedVariable("b", "bfoo", 2);

            gum::Potential<double> p;
            p << a << b;
            p.fillWith({0, 1, 2, 3});

            gum::Potential<double> q;
            q << a << b;
            q.fillWith({0, 3, 0, 3});

            TS_ASSERT_EQUALS((p - q).abs().toString(),
                             "<a:0|b:0> :: 0 /<a:1|b:0> :: 2 /"
                             "<a:0|b:1> :: 2 /<a:1|b:1> :: 0");
            TS_ASSERT_EQUALS((q - p).abs().toString(),
                             "<a:0|b:0> :: 0 /<a:1|b:0> :: 2 /"
                             "<a:0|b:1> :: 2 /<a:1|b:1> :: 0");
            TS_ASSERT_EQUALS((q - p).abs().max(), 2);
            TS_ASSERT_EQUALS((q - p).abs().min(), 0);
        }

        void testSqPotential() {
            auto a = gum::LabelizedVariable("a", "afoo", 2);
            auto b = gum::LabelizedVariable("b", "bfoo", 2);

            gum::Potential<double> p;
            p << a << b;
            p.fillWith({0, 1, 2, 3});

            gum::Potential<double> q;
            q << b << a;
            q.fillWith({0, 0, 3, 3});

            TS_ASSERT_EQUALS((p - q).sq().toString(),
                             "<a:0|b:0> :: 0 /<a:1|b:0> :: 4 /"
                             "<a:0|b:1> :: 4 /<a:1|b:1> :: 0");
            TS_ASSERT_EQUALS((q - p).sq().toString(),
                             "<b:0|a:0> :: 0 /<b:1|a:0> :: 4 /"
                             "<b:0|a:1> :: 4 /<b:1|a:1> :: 0");
            TS_ASSERT_EQUALS((q - p).sq().max(), 4);
            TS_ASSERT_EQUALS((q - p).sq().min(), 0);
        }

        void testEntropyPotential() {
            auto a = gum::LabelizedVariable("a", "afoo", 2);
            gum::Potential<double> p;
            p.add(a);
            TS_ASSERT_EQUALS(p.fillWith({0, 1}).entropy(), 0.0);
            TS_ASSERT_EQUALS(p.fillWith({1, 0}).entropy(), 0.0);
            TS_ASSERT_EQUALS(p.fillWith({0.5, 0.5}).entropy(), 1.0);
        }

        void testReorganizePotential() {
            auto a = gum::LabelizedVariable("a", "afoo", 3);
            auto b = gum::LabelizedVariable("b", "bfoo", 3);
            auto c = gum::LabelizedVariable("c", "cfoo", 3);
            auto d = gum::LabelizedVariable("d", "dfoo", 3);

            gum::Potential<double> p;
            p<<a<<b;
            p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

            gum::Potential<double> q;
            q<<c<<d;
            q.fillWith({4, 5, 6, 3, 2, 1, 4, 3, 2});

            auto joint1 = p * q;
            auto joint2 = q * p;
            TS_ASSERT_DIFFERS(joint1.toString(), joint2.toString());
            auto joint3 = (q * p).reorganize({&c, &d, &a, &b});
            TS_ASSERT_EQUALS(joint1.toString(), joint3.toString());

            gum::Instantiation inst;
            inst << a << b << c << d;
            for (inst.setFirst(); !inst.end(); ++inst) {
                TS_ASSERT_EQUALS(joint1.get(inst), joint2.get(inst));
                TS_ASSERT_EQUALS(joint1.get(inst), joint3.get(inst));
            }

            TS_ASSERT_THROWS(p.reorganize({&a, &b, &c}), gum::InvalidArgument);
            TS_ASSERT_THROWS(p.reorganize({&a, &d}), gum::InvalidArgument);
            TS_ASSERT_THROWS(p.reorganize({&a}), gum::InvalidArgument);
        }

        void testPutFirst() {
            auto a = gum::LabelizedVariable("a", "afoo", 3);
            auto b = gum::LabelizedVariable("b", "bfoo", 3);
            auto c = gum::LabelizedVariable("b", "bfoo", 3);

            gum::Potential<double> p;
            p << a << b;
            p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

            TS_ASSERT_DIFFERS(p.toString(), p.putFirst(&b).toString());
            TS_ASSERT_EQUALS(p.toString(), p.putFirst(&b).putFirst(&a).toString());
            TS_ASSERT_EQUALS(p.toString(), p.putFirst(&a).toString());

            TS_ASSERT_THROWS(p.putFirst(&c), gum::InvalidArgument);
        }

        void testExtraction() {
            auto a = gum::LabelizedVariable("a", "afoo", 3);
            auto b = gum::LabelizedVariable("b", "bfoo", 3);
            auto c = gum::LabelizedVariable("c", "cfoo", 3);

            gum::Potential<double> p;
            p << a << b;
            p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

            gum::Potential<double> q;
            q << c;
            q.fillWith({1, 2, 3});

            auto pot = q * p;

            gum::Instantiation I;
            I << c;
            I.chgVal(c, 0);
            TS_ASSERT(pot.extract(I) == p);
            I.chgVal(c, 2);
            gum::Potential<double> r;
            r << a << b;
            r.fillWith({3, 6, 9, 12, 15, 18, 21, 24, 27});
            TS_ASSERT(pot.reorganize({&b, &c, &a}).extract(I) == r);
        }

        void testOperatorEqual() {
            auto a = gum::LabelizedVariable("a", "afoo", 3);
            auto b = gum::LabelizedVariable("b", "bfoo", 3);

            gum::Potential<double> p;
            {   // empty potentials are equal
                gum::Potential<double> q;
                TS_ASSERT_EQUALS(p, q);
            }
            p << a;
            p.fillWith({1, 2, 3});
            TS_ASSERT_EQUALS(p, p);

            {   // same potential
                gum::Potential<double> q;
                q << a;
                q.fillWith({1, 2, 3});
                TS_ASSERT_EQUALS(p, q);
            }

            {   // difference values
                gum::Potential<double> q;
                q << a;
                q.fillWith({3, 6, 9});
                TS_ASSERT_DIFFERS(p, q);
            }

            {   // same values, different variables
                gum::Potential<double> q;
                q << b;
                q.fillWith({1, 2, 3});
                TS_ASSERT_DIFFERS(p, q);
            }

            {   // different dimensions
                gum::Potential<double> q;
                q << b << a;
                q.fillWith(1);
                TS_ASSERT_DIFFERS(p, q);
            }
        }

        void testScaleAndTranslate() {
            auto a = gum::LabelizedVariable("a", "afoo", 3);

            gum::Potential<double> p;
            p << a;
            p.fillWith({1, 2, 3});
            gum::Potential<double> p3;
            p3 << a;
            p3.fillWith({3, 6, 9});

            TS_GUM_ASSERT_THROWS_NOTHING(p.scale(3.0));
            TS_ASSERT(p == p3);
            TS_ASSERT_EQUALS(p, p3);

            p.fillWith({1, 2, 3});
            gum::Potential<double> p2;
            p2 << a;
            p2.fillWith({2, 3, 4});

            TS_GUM_ASSERT_THROWS_NOTHING(p.translate(1.0));
            TS_ASSERT(p == p2);
            TS_ASSERT_EQUALS(p, p2);


            p.fillWith({1, 2, 3});
            gum::Potential<double> p1;
            p1 << a;
            p1.fillWith({4, 7, 10});
            p.scale(3.0).translate(1.0);
            // TS_GUM_ASSERT_THROWS_NOTHING(p.scale(3.0).translate(1.0));
            TS_ASSERT(p == p1);
            TS_ASSERT_EQUALS(p, p1);
        }

        void testNormalizeAsCPT() {
            auto a = gum::LabelizedVariable("a", "afoo", 3);
            auto b = gum::LabelizedVariable("b", "bfoo", 3);

            gum::Potential<double> p;
            p << a << b;
            p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

            auto q = p / p.margSumOut({&a});
            p.normalizeAsCPT();
            TS_ASSERT_EQUALS(p, q);
            TS_ASSERT_EQUALS(q, p);

            gum::Potential<double> p2;
            p2 << a << b;
            p2.fill(0.0f);
            TS_ASSERT_THROWS(p2.normalizeAsCPT(), gum::FatalError);

            gum::Potential<double> p3;
            p3 << a << b;
            p3.fillWith({1, 2, 3, 0, 0, 0, 7, 8, 9});
            TS_ASSERT_THROWS(p2.normalizeAsCPT(), gum::FatalError);

            gum::Potential<double> p4;
            p4 << a;
            p4.fillWith({1, 3, 6});
            p4.normalizeAsCPT();
            gum::Potential<double> witness;
            witness << a;
            witness.fillWith({0.1f, 0.3f, 0.6f});
            TS_ASSERT_EQUALS(p4, witness);
        }

        void testEmptyPotential() {
            gum::Potential<double> p;
            gum::Instantiation inst(p);
            double a;
            auto var_a = gum::LabelizedVariable("a", "afoo", 3);

            TS_GUM_ASSERT_THROWS_NOTHING(a = p[inst];);
            TS_ASSERT_EQUALS(a, 1.0f)

            TS_GUM_ASSERT_THROWS_NOTHING(p.set(inst, a + (double) 3.0f););
            TS_ASSERT_EQUALS(p[inst], 4.0f);

            TS_GUM_ASSERT_THROWS_NOTHING(p.populate({1.0f}));
            TS_ASSERT_EQUALS(p[inst], 1.0f);
            TS_GUM_ASSERT_THROWS_NOTHING(p.populate(std::vector<double>{2.0f}));
            TS_ASSERT_EQUALS(p[inst], 2.0f);
            TS_ASSERT_THROWS(p.populate(std::vector<double>{2.0f, 3.0f}),
                             gum::SizeError)

            TS_GUM_ASSERT_THROWS_NOTHING(
                    p.apply([](double x) { return x * 2.0f + 1.0f; }););
            TS_ASSERT_EQUALS(p[inst], 5.0f);

            a = 0.3f;
            TS_GUM_ASSERT_THROWS_NOTHING(
                    a = p.reduce([](double x, double y) { return x + y; }, 0.0f););
            TS_ASSERT_EQUALS(a, 0.0f);

            TS_GUM_ASSERT_THROWS_NOTHING(p.populate({33.0f}));
            TS_ASSERT_EQUALS(p.toString(), "<> :: 33");


            TS_GUM_ASSERT_THROWS_NOTHING(p.set(inst, (double) 3.0f););
            TS_ASSERT_EQUALS(p.sum(), 3.0f);
            TS_ASSERT_EQUALS(p.product(), 3.0f);
            TS_ASSERT_EQUALS(p.max(), 3.0f);
            TS_ASSERT_EQUALS(p.min(), 3.0f);
            TS_ASSERT_EQUALS(p.minNonZero(), 3.0f);
            TS_ASSERT_EQUALS(p.maxNonOne(), 3.0f);

            TS_GUM_ASSERT_THROWS_NOTHING(p.set(inst, (double) 0.0f););
            TS_ASSERT_EQUALS(p.minNonZero(), 0);

            TS_GUM_ASSERT_THROWS_NOTHING(p.set(inst, (double) 1.0f););
            TS_ASSERT_EQUALS(p.maxNonOne(), 1.0f);

            TS_GUM_ASSERT_THROWS_NOTHING(p.fill(2.0f););
            TS_ASSERT_EQUALS(p[inst], 2.0f);
            TS_GUM_ASSERT_THROWS_NOTHING(p.fillWith(3.0f););
            TS_ASSERT_EQUALS(p[inst], 3.0f);
            TS_GUM_ASSERT_THROWS_NOTHING(p.fillWith({4.0f}););
            TS_ASSERT_EQUALS(p[inst], 4.0f);
            TS_GUM_ASSERT_THROWS_NOTHING(p.fillWith(std::vector<double>({5.0f})););
            TS_ASSERT_EQUALS(p[inst], 5.0f);

            TS_ASSERT_EQUALS(p.entropy(), 0.0f);

            TS_ASSERT_EQUALS(p.fillWith(2.0f).sq()[inst], 4.0f);
            TS_ASSERT_EQUALS(p.fillWith(-2.0f).sq()[inst], 4.0f);
            TS_ASSERT_EQUALS(p.fillWith(2.0f).abs()[inst], 2.0f);
            TS_ASSERT_EQUALS(p.fillWith(-2.0f).abs()[inst], 2.0f);

            TS_ASSERT_EQUALS(p.fillWith(2.0f).normalize()[inst], 1.0f);
            TS_ASSERT_EQUALS(p.fillWith(0.0f).normalize()[inst], 0.0f);

            TS_ASSERT_EQUALS(p.fillWith(2.0f).scale(4)[inst], 8.0f);
            TS_ASSERT_EQUALS(p.fillWith(2.0f).translate(4)[inst], 6.0f);


            TS_ASSERT_EQUALS(p.fillWith(2.0f).margSumOut({&var_a})[inst], 2.0f);
            TS_ASSERT_EQUALS(p.fillWith(3.0f).margSumIn({&var_a})[inst], 3.0f);
            TS_ASSERT_EQUALS(p.fillWith(4.0f).margProdOut({&var_a})[inst], 4.0f);
            TS_ASSERT_EQUALS(p.fillWith(5.0f).margProdIn({&var_a})[inst], 5.0f);
            TS_ASSERT_EQUALS(p.fillWith(6.0f).margMaxIn({&var_a})[inst], 6.0f);
            TS_ASSERT_EQUALS(p.fillWith(7.0f).margMaxOut({&var_a})[inst], 7.0f);
            TS_ASSERT_EQUALS(p.fillWith(8.0f).margMinOut({&var_a})[inst], 8.0f);
            TS_ASSERT_EQUALS(p.fillWith(9.0f).margMinOut({&var_a})[inst], 9.0f);

            TS_ASSERT_EQUALS(p.fillWith(0.0f).isNonZeroMap()[inst], 0.0f);
            TS_ASSERT_EQUALS(p.fillWith(1.0f).isNonZeroMap()[inst], 1.0f);
            TS_ASSERT_EQUALS(p.fillWith(0.5f).isNonZeroMap()[inst], 1.0f);
            TS_ASSERT_EQUALS(p.fillWith(-0.5f).isNonZeroMap()[inst], 1.0f);
        }

        void testOperationForEmptyPotential() {
            auto a = gum::LabelizedVariable("a", "afoo", 3);
            auto b = gum::LabelizedVariable("b", "bfoo", 3);

            gum::Potential<double> res;
            res << a << b;

            gum::Potential<double> p;
            p << a << b;
            p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});

            gum::Potential<double> q;
            q.fill(2);

            gum::Potential<double> tmp;

            res.fillWith({3, 4, 5, 6, 7, 8, 9, 10, 11});
            TS_ASSERT_EQUALS(p + q, res);
            tmp = p;
            tmp += q;
            TS_ASSERT_EQUALS(tmp, res);

            TS_ASSERT_EQUALS(q + p, res);
            tmp = q;
            tmp += p;
            TS_ASSERT_EQUALS(tmp, res);

            res.fillWith({2, 4, 6, 8, 10, 12, 14, 16, 18});
            TS_ASSERT_EQUALS(p * q, res);
            tmp = p;
            tmp *= q;
            TS_ASSERT_EQUALS(tmp, res);

            TS_ASSERT_EQUALS(q * p, res);
            tmp = q;
            tmp *= p;
            TS_ASSERT_EQUALS(tmp, res);

            res.fillWith({-1, 0, 1, 2, 3, 4, 5, 6, 7});
            TS_ASSERT_EQUALS(p - q, res);
            tmp = p;
            tmp -= q;
            TS_ASSERT_EQUALS(tmp, res);

            res.fillWith({1, 0, -1, -2, -3, -4, -5, -6, -7});
            TS_ASSERT_EQUALS(q - p, res);
            tmp = q;
            tmp -= p;
            TS_ASSERT_EQUALS(tmp, res);

            res.fillWith({1 / 2.0f,
                          2 / 2.0f,
                          3 / 2.0f,
                          4 / 2.0f,
                          5 / 2.0f,
                          6 / 2.0f,
                          7 / 2.0f,
                          8 / 2.0f,
                          9 / 2.0f});
            TS_ASSERT_EQUALS(p / q, res);
            tmp = p;
            tmp /= q;
            TS_ASSERT_EQUALS(tmp, res);

            res.fillWith({2.0f / 1,
                          2.0f / 2,
                          2.0f / 3,
                          2.0f / 4,
                          2.0f / 5,
                          2.0f / 6,
                          2.0f / 7,
                          2.0f / 8,
                          2.0f / 9});
            TS_ASSERT_EQUALS(q / p, res);
            tmp = q;
            tmp /= p;
            TS_ASSERT_EQUALS(tmp, res);
        }

        void testOperationForTwoEmptyPotentials() {
            gum::Potential<double> p;
            p.fill(3);

            gum::Potential<double> q;
            q.fill(2);

            gum::Potential<double> res, tmp;
            res.fill(5);
            TS_ASSERT_EQUALS(p + q, res);
            tmp = p;
            tmp += q;
            TS_ASSERT_EQUALS(tmp, res);

            TS_ASSERT_EQUALS(q + p, res);
            tmp = q;
            tmp += p;
            TS_ASSERT_EQUALS(tmp, res);

            res.fillWith({6});
            TS_ASSERT_EQUALS(p * q, res);
            tmp = p;
            tmp *= q;
            TS_ASSERT_EQUALS(tmp, res);

            TS_ASSERT_EQUALS(q * p, res);
            tmp = q;
            tmp *= p;
            TS_ASSERT_EQUALS(tmp, res);

            res.fillWith({1});
            TS_ASSERT_EQUALS(p - q, res);
            tmp = p;
            tmp -= q;
            TS_ASSERT_EQUALS(tmp, res);

            res.fillWith({-1});
            TS_ASSERT_EQUALS(q - p, res);
            tmp = q;
            tmp -= p;
            TS_ASSERT_EQUALS(tmp, res);

            res.fillWith({3 / 2.0f});
            TS_ASSERT_EQUALS(p / q, res);
            tmp = p;
            tmp /= q;
            TS_ASSERT_EQUALS(tmp, res);

            res.fillWith({2.0f / 3});
            TS_ASSERT_LESS_THAN((q / p - res).abs().max(), 1e-7);
            tmp = q;
            tmp /= p;
            TS_ASSERT_LESS_THAN((tmp - res).abs().max(), 1e-7);
        }


        void testLoopsForEmptyPotential() {
            gum::Potential<double> p;
            gum::Instantiation inst(p);
            p.fill(3);

            gum::Size cpt = 0;
            double total = 0.0;
            for (inst.setFirst(); !inst.end(); ++inst) {
                cpt++;
                total += p[inst];
            }
            TS_ASSERT_EQUALS(cpt, gum::Size(1));
            TS_ASSERT_EQUALS(total, 3);

            cpt = 0;
            total = 0.0;
            for (inst.setLast(); !inst.end(); --inst) {
                cpt++;
                total += p[inst];
            }
            TS_ASSERT_EQUALS(cpt, gum::Size(1));
            TS_ASSERT_EQUALS(total, 3);
        }

        void /*test*/ EliminatationOffAllVariables() {
            auto a = gum::LabelizedVariable("a", "afoo", 3);
            auto b = gum::LabelizedVariable("b", "bfoo", 3);

            gum::Potential<int> p;
            p << a << b;
            p.fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9});
            TS_ASSERT_EQUALS(p.margSumOut({&a, &b}).toString(), "<> :: 45");
        }

        void testKL() {
            gum::LabelizedVariable v("v", "v", 2), w("w", "w", 2);
            gum::Potential<double> p, q, r, s;
            p.add(v);
            p.fillWith({0.0f, 1.0f});
            q.add(v);
            q.fillWith({0.5f, 0.5f});
            r.add(v);
            r.fillWith({0.7f, 0.3f});
            s.add(v);
            s.add(w);
            s.fillWith({0.0f, 1.0f, 0.2f, 0.8f});

            double res;

            TS_GUM_ASSERT_THROWS_NOTHING(res = p.KL(p));
            TS_ASSERT_EQUALS(res, 0.0f);

            TS_ASSERT_THROWS(res = p.KL(s), gum::InvalidArgument);
            TS_ASSERT_THROWS(res = s.KL(p), gum::InvalidArgument);

            TS_GUM_ASSERT_THROWS_NOTHING(res = p.KL(q));
            TS_ASSERT_DELTA(res, 0.0 + 1.0 * log2(1.0 / 0.5), 1e-5);
            TS_ASSERT_THROWS(res = q.KL(p), gum::FatalError);

            TS_GUM_ASSERT_THROWS_NOTHING(res = p.KL(r));
            TS_ASSERT_DELTA(res, 0.0 + 1.0 * log2(1.0 / 0.3), 1e-5);
            TS_ASSERT_THROWS(res = r.KL(p), gum::FatalError);

            TS_GUM_ASSERT_THROWS_NOTHING(res = q.KL(r));
            TS_ASSERT_DELTA(res, 0.5 * log2(0.5 / 0.7) + 0.5 * log2(0.5 / 0.3), 1e-5);

            TS_GUM_ASSERT_THROWS_NOTHING(res = r.KL(q));
            TS_ASSERT_DELTA(res, 0.7 * log2(0.7 / 0.5) + 0.3 * log2(0.3 / 0.5), 1e-5);
        }

        void testPotentialDraw() {
            try {
                constexpr int DELTA = 200;
                constexpr int NBRITER = 10000;
                gum::LabelizedVariable v("v", "v", 2), w("w", "w", 3);

                gum::Potential<double> p;
                p.add(v);
                p.fillWith({0.2f, 0.6f});
                int s = 0;
                for (int i = 0; i < NBRITER; i++) {
                    if (p.draw() == 0) s++;
                }
                TS_ASSERT_DELTA(s, int(0.2 * NBRITER), DELTA);

                p.add(w);
                TS_ASSERT_THROWS(p.draw(), gum::FatalError)

                gum::Potential<double> q;
                q.add(w);
                q.fillWith({0.3f, 0.6f, 0.1f});
                int s0 = 0, s1 = 0, s2 = 0;
                for (int i = 0; i < NBRITER; i++) {
                    auto d = q.draw();
                    if (d == 0) s0++;
                    if (d == 1) s1++;
                    if (d == 2) s2++;
                }
                TS_ASSERT_DELTA(s0, int(0.3 * NBRITER), DELTA);
                TS_ASSERT_DELTA(s1, int(0.6 * NBRITER), DELTA);
                TS_ASSERT_DELTA(s2, int(0.1 * NBRITER), DELTA);
            } catch (const gum::Exception &e) {GUM_SHOWERROR(e); }
        }

        void testVariableAccessor() {
            gum::LabelizedVariable v("v", "v", 2), w("w", "w", 3);
            gum::Potential<double> p;
            p.add(v);
            p.add(w);

            TS_ASSERT(p.variable(0) == p.variable("v"));
            TS_ASSERT(p.variable(1) == p.variable("w"));
            TS_ASSERT(p.variable(1) != p.variable("v"));
            TS_ASSERT(p.variable(0) != p.variable("w"));

            TS_ASSERT_THROWS(p.variable("ZZ"), gum::NotFound);
        }

        void testFillWithPotentialMethod() {
            gum::LabelizedVariable v("v", "v", 2), w("w", "w", 3);
            gum::LabelizedVariable z("z", "z", 2);
            gum::LabelizedVariable vv("v", "v", 2), ww("w", "w", 3);
            gum::LabelizedVariable vvv("v", "v", 3), www("w", "w", 2);

            gum::Potential<int> p;
            p.add(v);
            p.add(w);

            gum::Potential<int> pp;
            pp.add(ww);
            pp.add(vv);

            TS_ASSERT_EQUALS(p.domainSize(), gum::Size(6));
            TS_ASSERT_EQUALS(pp.domainSize(), gum::Size(6));

            p.fillWith({1, 2, 3, 4, 5, 6});
            pp.fillWith(p);
            gum::Instantiation Ip(p);
            gum::Instantiation Ipp;
            Ipp.add(vv);
            Ipp.add(ww);
            Ipp.setFirst();
            for (Ip.setFirst(); !Ip.end(); ++Ip, ++Ipp)
                try {
                    auto vp = p[Ip];
                    auto vpp = pp[Ipp];
                    TS_ASSERT_EQUALS(vp, vpp);
                } catch (gum::Exception &e) {GUM_SHOWERROR(e); }

            // errors
            gum::Potential<int> bad_p;
            bad_p.add(w);
            TS_ASSERT_THROWS(bad_p.fillWith(p), gum::InvalidArgument);

            gum::Potential<int> bad_p2;
            bad_p2.add(vvv);
            bad_p2.add(www);
            TS_ASSERT_THROWS(bad_p2.fillWith(p), gum::InvalidArgument);

            gum::Potential<int> bad_p3;
            bad_p3.add(w);
            bad_p3.add(z);
            TS_ASSERT_THROWS(bad_p3.fillWith(p), gum::InvalidArgument);

            gum::Potential<int> bad_p4;
            gum::LabelizedVariable badv("v", "v", 0);
            badv.addLabel("3").addLabel("1");
            bad_p4.add(w);
            bad_p4.add(badv);
            TS_ASSERT_THROWS(bad_p4.fillWith(p), gum::InvalidArgument);
        }

        void testFillWithPotentialAndMapMethod() {
            gum::LabelizedVariable v("v", "v", 2), w("w", "w", 3);
            gum::Potential<int> p;
            p.add(v);
            p.add(w);

            gum::LabelizedVariable vv("vv", "vv", 2), ww("ww", "ww", 3);
            gum::Potential<int> pp;
            pp.add(ww);
            pp.add(vv);

            TS_ASSERT_EQUALS(p.domainSize(), gum::Size(6));
            TS_ASSERT_EQUALS(pp.domainSize(), gum::Size(6));

            p.fillWith({1, 2, 3, 4, 5, 6});
            TS_GUM_ASSERT_THROWS_NOTHING(pp.fillWith(p, {"w", "v"}));
            TS_ASSERT_THROWS(pp.fillWith(p, {"v", "w"}), gum::InvalidArgument);
        }

    private:
        void __testval_for_set(const gum::Potential<int> &p,
                               int val,
                               const gum::Set<gum::Instantiation> s,
                               gum::Size expected_size) {
            gum::Instantiation ip(p);

            TS_ASSERT_EQUALS(s.size(), expected_size);
            for (ip.setFirst(); !ip.end(); ++ip) {
                if (s.contains(ip)) {
                    TS_ASSERT_EQUALS(p[ip], val);
                } else {
                    TS_ASSERT_DIFFERS(p[ip], val);
                }
            }
        }

    public:
        void testArgMaxMinFindAll() {
            gum::LabelizedVariable v("v", "v", 2), w("w", "w", 3);
            gum::Potential<int> p;
            __testval_for_set(p, 4, p.findAll(4), 0);

            p.add(v);
            p.add(w);
            p.fillWith({1, 3, 2, 4, 1, 4});

            __testval_for_set(p, 3, p.findAll(3), 1);
            __testval_for_set(p, 10, p.findAll(10), 0);
            __testval_for_set(p, 4, p.argmax(), 2);
            __testval_for_set(p, 1, p.argmin(), 2);
        }

        void testAddDummyVariables() {
            {
                gum::LabelizedVariable v("v", "v", 0);

                gum::Potential<double> p;
                TS_ASSERT_EQUALS(v.domainSize(), gum::Size(0));
                TS_ASSERT_THROWS(p.add(v), gum::InvalidArgument);

                v.addLabel("first");
                TS_ASSERT_EQUALS(v.domainSize(), gum::Size(1));
                TS_GUM_ASSERT_THROWS_NOTHING(p.add(v));

                p = gum::Potential<double>();
                v.addLabel("second");
                TS_ASSERT_EQUALS(v.domainSize(), gum::Size(2));
                TS_GUM_ASSERT_THROWS_NOTHING(p.add(v));
            }

            {
                gum::RangeVariable v("v", "v");
                v.setMinVal(1);
                v.setMaxVal(0);
                gum::Potential<double> p;
                TS_ASSERT_EQUALS(v.domainSize(), gum::Size(0));
                TS_ASSERT_THROWS(p.add(v), gum::InvalidArgument);

                v.setMaxVal(1);
                TS_ASSERT_EQUALS(v.domainSize(), gum::Size(1));
                TS_GUM_ASSERT_THROWS_NOTHING(p.add(v));

                p = gum::Potential<double>();
                v.setMaxVal(2);
                TS_ASSERT_EQUALS(v.domainSize(), gum::Size(2));
                TS_GUM_ASSERT_THROWS_NOTHING(p.add(v));
            }

            {
                gum::DiscretizedVariable<int> v("v", "v");
                gum::Potential<double> p;
                TS_ASSERT_EQUALS(v.domainSize(), gum::Size(0));
                TS_ASSERT_THROWS(p.add(v), gum::InvalidArgument);

                v.addTick(1);
                TS_ASSERT_EQUALS(v.domainSize(), gum::Size(0));
                TS_ASSERT_THROWS(p.add(v), gum::InvalidArgument);

                v.addTick(2);
                TS_ASSERT_EQUALS(v.domainSize(), gum::Size(1));
                TS_GUM_ASSERT_THROWS_NOTHING(p.add(v));

                p = gum::Potential<double>();
                v.addTick(3);
                TS_ASSERT_EQUALS(v.domainSize(), gum::Size(2));
                TS_GUM_ASSERT_THROWS_NOTHING(p.add(v));
            }
        }

        void testRandomPotential() {
            gum::LabelizedVariable u("u", "u", 4), v("v", "v", 2), w("w", "w", 3);
            gum::Potential<double> p;
            p.add(u);
            p.add(v);
            p.add(w);
            gum::Instantiation I(p);

            p.random();
            for (I.setFirst(); !I.end(); I.inc()) {
                TS_ASSERT_LESS_THAN_EQUALS(p[I], 1.0);
            }

            p.randomDistribution();
            double cum = 0.0;
            for (I.setFirst(); !I.end(); I.inc()) {
                TS_ASSERT_LESS_THAN_EQUALS(p[I], 1.0);
                cum += p[I];
            }
            TS_ASSERT_DELTA(cum, 1.0, 1e-6);

            p.randomCPT();
            const gum::DiscreteVariable &var = p.variable(0);
            for (I.setFirstNotVar(var); !I.end(); I.incNotVar(var)) {
                cum = 0.0;
                for (I.setFirstVar(var); !I.end(); I.incVar(var)) {
                    TS_ASSERT_LESS_THAN_EQUALS(p[I], 1.0);
                    cum += p[I];
                }
                TS_ASSERT_DELTA(cum, 1.0, 1e-6);
                I.unsetEnd();
            }

            p.fillWith(1.0).normalizeAsCPT();   // every thing should be 0.25
            for (I.setFirst(); !I.end(); I.inc()) {
                TS_ASSERT_DELTA(p[I], 0.25, 1e-6);
            }

            for (double alpha = 0.0; alpha <= 1; alpha += 0.1) {
                p.fillWith(1.0).normalizeAsCPT();   // every thing should be 0.25
                p.noising(alpha);
                double min = (1 - alpha) * 0.25 + alpha * 0.0;
                double max = (1 - alpha) * 0.25 + alpha * 1.0;
                for (I.setFirst(); !I.end(); I.inc()) {
                    TS_ASSERT_LESS_THAN_EQUALS(min, p[I]);
                    TS_ASSERT_LESS_THAN_EQUALS(p[I], max);
                }
            }
        }

        void testEqualities() {
            gum::LabelizedVariable u("u", "u", 4), v("v", "v", 2), w("w", "w", 3);
            gum::Potential<double> p;
            p.add(u);
            p.add(v);
            p.add(w);
            p.random();
            gum::Potential<double> q(p);
            TS_ASSERT_EQUALS(p, q);
            gum::Instantiation i(q);
            i.setLast();
            q.set(i, 0);
            TS_ASSERT_DIFFERS(p, q);
            q.fillWith(p);
            TS_ASSERT_EQUALS(p, q);
            q.fillWith(1);
            TS_ASSERT_DIFFERS(p, q);
            q.fillWith(p);
            TS_ASSERT_EQUALS(p, q);
            gum::LabelizedVariable x("x", "Unknown", 5);
            q.add(x);
            TS_ASSERT_DIFFERS(p, q);
            q.erase("x");
            q.fillWith(p);
            TS_ASSERT_EQUALS(p, q);
            q.erase("u");
            TS_ASSERT_DIFFERS(p, q);
        }


        void testInverse() {
            gum::LabelizedVariable u("u", "u", 4), v("v", "v", 2), w("w", "w", 3);
            gum::Potential<double> p;
            p.add(u);
            p.add(v);
            p.add(w);
            p.random();
            gum::Potential<double> q(p);
            q.inverse();
            TS_ASSERT_DELTA((p * q).min(), 1.0, 1e-7);
            TS_ASSERT_DELTA((p * q).max(), 1.0, 1e-7);
        }

        void testOperatorWithScalars() {
            gum::LabelizedVariable u("u", "u", 4), v("v", "v", 2), w("w", "w", 3);
            gum::Potential<double> p;
            p.add(u);
            p.add(v);
            p.add(w);
            p.random();
            {
                gum::Potential<double> q(p);
                q.scale(2);
                TS_ASSERT_EQUALS(p * 2, q);
            }
            {
                gum::Potential<double> q(p);
                q.scale(0.5);
                TS_ASSERT_EQUALS(p / 2, q);
            }
            {
                gum::Potential<double> q(p);
                q.translate(1.5);
                TS_ASSERT_EQUALS(p + 1.5, q);
            }
            {
                gum::Potential<double> q(p);
                q.translate(3.2);
                TS_ASSERT_EQUALS(p - 3.2, q);
            }
            {
                gum::Potential<double> q(p);
                gum::Potential<double> r(p);
                q.translate(4.33);
                r += 4.33;
                TS_ASSERT_EQUALS(r, q);
            }
            {
                gum::Potential<double> q(p);
                gum::Potential<double> r(p);
                q.translate(-4.33);
                r -= 4.33;
                TS_ASSERT_EQUALS(r, q);
            }
            {
                gum::Potential<double> q(p);
                gum::Potential<double> r(p);
                q.scale(4.33);
                r *= 4.33;
                TS_ASSERT_EQUALS(r, q);
            }
            {
                gum::Potential<double> q(p);
                gum::Potential<double> r(p);
                q.scale(1 / 4.33);
                r /= 4.33;
                TS_ASSERT_EQUALS(r, q);
            }
        }
    };
}   // namespace gum_tests
