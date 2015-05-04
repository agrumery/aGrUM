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
#include <sstream>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/multidim/multiDimBucket.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/instantiation.h>

namespace gum_tests {

  class MultiDimBucketTestSuite : public CxxTest::TestSuite {
    private:
    std::vector<gum::LabelizedVariable *> *__variables;
    std::vector<gum::Potential<double> *> *__potentials;

    void __fillBucket(gum::MultiDimBucket<double> *bucket) {
      for (size_t i = 0; i < 5; ++i) {
        bucket->add(__potentials->at(i));
      }
    }

    // Product must have variables
    void __makeProduct(gum::Potential<double> &product) {
      gum::Potential<double> temp;

      for (size_t i = 0; i < 5; ++i) {
        temp.multiplicateBy(*(__potentials->at(i)));
      }

      product.marginalize(temp);
    }

    public:
    void setUp() {
      gum::SimpleCPTGenerator<double> cptGenerator;
      __variables = new std::vector<gum::LabelizedVariable *>();

      for (gum::Size i = 0; i < 10; ++i) {
        std::stringstream sBuff;
        sBuff << "var_" << i;
        __variables->push_back(
            new gum::LabelizedVariable(sBuff.str(), "A binary variable", 2));
      }

      __potentials = new std::vector<gum::Potential<double> *>();

      for (gum::Size i = 0; i < 5; ++i) {
        __potentials->push_back(
            new gum::Potential<double>(new gum::MultiDimArray<double>()));
      }

      // Creating a table of 2 elements
      __potentials->at(0)->add(*(__variables->at(0)));

      cptGenerator.generateCPT(__potentials->at(0)->pos(*(__variables->at(0))),
                               *(__potentials->at(0)));

      // Creating a table of 2 elements
      __potentials->at(1)->add(*(__variables->at(1)));

      cptGenerator.generateCPT(__potentials->at(1)->pos(*(__variables->at(1))),
                               *(__potentials->at(1)));

      // Creating a table of 2^4=16 elements
      for (size_t i = 2; i < 6; ++i) {
        __potentials->at(2)->add(*(__variables->at(i)));
      }

      cptGenerator.generateCPT(__potentials->at(2)->pos(*(__variables->at(2))),
                               *(__potentials->at(2)));

      // Creatinh a table of 2^4=16 elements

      for (size_t i = 4; i < 8; ++i) {
        __potentials->at(3)->add(*(__variables->at(i)));
      }

      cptGenerator.generateCPT(__potentials->at(3)->pos(*(__variables->at(4))),
                               *(__potentials->at(3)));

      // Creatinh a table of 2^4=16 elements

      for (size_t i = 6; i < 10; ++i) {
        __potentials->at(4)->add(*(__variables->at(i)));
      }

      cptGenerator.generateCPT(__potentials->at(4)->pos(*(__variables->at(6))),
                               *(__potentials->at(4)));
    }

    void tearDown() {
      while (__potentials->size() > 0) {
        delete __potentials->back();
        __potentials->pop_back();
      }

      while (__variables->size() > 0) {
        delete __variables->back();
        __variables->pop_back();
      }

      delete __variables;

      delete __potentials;
    }

    void testCreation() {
      gum::MultiDimBucket<double> *bucket = 0;

      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>());
      TS_ASSERT_THROWS_NOTHING(delete bucket);
    }

    void testAddEraseTables() {
      gum::MultiDimBucket<double> *bucket = 0;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>());

      if (bucket != 0) {
        TS_ASSERT(bucket->isBucketEmpty());

        for (size_t i = 0; i < 5; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add(__potentials->at(i)));
        }

        TS_ASSERT(!bucket->isBucketEmpty());

        TS_ASSERT_EQUALS(bucket->bucketSize(), (gum::Size)5);
        TS_ASSERT_THROWS_NOTHING(bucket->erase(__potentials->at(4)));
        TS_ASSERT_EQUALS(bucket->bucketSize(), (gum::Size)4);

        for (size_t i = 5; i > 0; --i) {
          TS_ASSERT_THROWS_NOTHING(bucket->erase(__potentials->at(i - 1)));
        }

        TS_ASSERT_EQUALS(bucket->bucketSize(), (gum::Size)0);

        TS_ASSERT(bucket->isBucketEmpty());
        delete bucket;
      }
    }

    void testComputation() {
      gum::MultiDimBucket<double> *bucket = 0;
      gum::Potential<double> product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>());

      if (bucket != 0) {
        TS_ASSERT_THROWS_NOTHING(__fillBucket(bucket));

        for (size_t i = 3; i < 6; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add(*(__variables->at(i))));
          product.add(*(__variables->at(i)));
        }

        TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());

        TS_GUM_ASSERT_THROWS_NOTHING(__makeProduct(product));
        gum::Instantiation inst(product);
        TS_ASSERT_EQUALS(bucket->domainSize(), product.domainSize());
        TS_ASSERT_EQUALS(bucket->nbrDim(), product.nbrDim());

        for (inst.setFirst(); !inst.end(); inst.inc()) {
          TS_ASSERT_DELTA(bucket->get(inst), product.get(inst), (double)0.01);
        }

        delete bucket;
      }
    }

    void testOnTheFly() {
      gum::MultiDimBucket<double> *bucket = 0;
      gum::Potential<double> product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>(0));

      if (bucket != 0) {
        TS_ASSERT_THROWS_NOTHING(__fillBucket(bucket));

        for (size_t i = 3; i < 6; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add(*(__variables->at(i))));
          product.add(*(__variables->at(i)));
        }

        TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());

        TS_GUM_ASSERT_THROWS_NOTHING(__makeProduct(product));
        gum::Instantiation inst(product);
        TS_ASSERT_EQUALS(bucket->domainSize(), product.domainSize());
        TS_ASSERT_EQUALS(bucket->nbrDim(), product.nbrDim());
        TS_ASSERT_EQUALS(bucket->realSize(), (gum::Size)0);

        for (inst.setFirst(); !inst.end(); inst.inc()) {
          TS_ASSERT_DELTA(bucket->get(inst), product.get(inst), (double)0.01);
        }

        delete bucket;
      }
    }

    void testInstantiationsWithBuffer() {
      gum::MultiDimBucket<double> *bucket = 0;
      gum::Potential<double> product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>());

      if (bucket != 0) {
        TS_ASSERT_THROWS_NOTHING(__fillBucket(bucket));

        for (size_t i = 3; i < 6; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add(*(__variables->at(i))));
          product.add(*(__variables->at(i)));
        }

        TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());

        TS_GUM_ASSERT_THROWS_NOTHING(__makeProduct(product));

        gum::Instantiation *inst = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(inst = new gum::Instantiation(*bucket));

        if (inst != 0) {
          TS_ASSERT(!inst->isMaster(bucket));
          TS_ASSERT(inst->isMaster(bucket->getMasterRef()));

          for (inst->setFirst(); !inst->end(); inst->inc()) {
            TS_ASSERT_DELTA(bucket->get(*inst), product.get(*inst), (double)0.01);
          }

          delete inst;
        }

        delete bucket;
      }
    }

    void testInstantiationsWithBufferAndAutoCompute() {
      gum::MultiDimBucket<double> *bucket = 0;
      gum::Potential<double> product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>());

      if (bucket != 0) {
        TS_ASSERT_THROWS_NOTHING(__fillBucket(bucket));

        for (size_t i = 3; i < 6; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add(*(__variables->at(i))));
          product.add(*(__variables->at(i)));
        }

        TS_GUM_ASSERT_THROWS_NOTHING(__makeProduct(product));

        gum::Instantiation *inst = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(inst = new gum::Instantiation(*bucket));

        if (inst != 0) {
          TS_ASSERT(!inst->isMaster(bucket));
          TS_ASSERT(inst->isMaster(bucket->getMasterRef()));

          for (inst->setFirst(); !inst->end(); inst->inc()) {
            TS_ASSERT_DELTA(bucket->get(*inst), product.get(*inst), (double)0.01);
          }

          delete inst;
        }

        delete bucket;
      }
    }

    void testInstantiationsOnTheFly() {
      gum::MultiDimBucket<double> *bucket = 0;
      gum::Potential<double> product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>(0));

      if (bucket != 0) {
        TS_ASSERT_THROWS_NOTHING(__fillBucket(bucket));

        for (size_t i = 3; i < 6; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add(*(__variables->at(i))));
          product.add(*(__variables->at(i)));
        }

        TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());

        TS_GUM_ASSERT_THROWS_NOTHING(__makeProduct(product));

        gum::Instantiation *inst = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(inst = new gum::Instantiation(*bucket));

        if (inst != 0) {
          TS_ASSERT(inst->isMaster(bucket));
          TS_ASSERT(inst->isMaster(bucket->getMasterRef()));

          for (inst->setFirst(); !inst->end(); inst->inc()) {
            TS_ASSERT_DELTA(bucket->get(*inst), product.get(*inst), (double)0.01);
          }

          delete inst;
        }

        delete bucket;
      }
    }

    void testBucketSizeChanges() {
      gum::MultiDimBucket<double> *bucket = 0;
      gum::Potential<double> product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>(0));

      if (bucket != 0) {
        TS_ASSERT_THROWS_NOTHING(__fillBucket(bucket));
        TS_ASSERT(bucket->bucketChanged());

        for (size_t i = 3; i < 6; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add(*(__variables->at(i))));
          TS_ASSERT(bucket->bucketChanged());
          product.add(*(__variables->at(i)));
        }

        TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());

        TS_ASSERT(!bucket->bucketChanged());
        TS_GUM_ASSERT_THROWS_NOTHING(__makeProduct(product));

        TS_ASSERT_EQUALS(bucket->realSize(), (gum::Size)0);

        gum::Instantiation *inst = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(inst = new gum::Instantiation(*bucket));
        TS_ASSERT(!bucket->bucketChanged());

        if (inst != 0) {
          TS_ASSERT(inst->isMaster(bucket));
          TS_ASSERT(inst->isMaster(bucket->getMasterRef()));

          for (inst->setFirst(); !inst->end(); inst->inc()) {
            TS_ASSERT_DELTA(bucket->get(*inst), product.get(*inst), (double)0.01);
            TS_ASSERT(!bucket->bucketChanged());
          }

          TS_GUM_ASSERT_THROWS_NOTHING(bucket->setBufferSize((gum::Size)65536));

          TS_ASSERT(bucket->bucketChanged());
          TS_ASSERT(bucket->realSize() > (gum::Size)0);
          TS_ASSERT(bucket->bucketChanged());
          TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());
          TS_ASSERT(!bucket->bucketChanged());

          TS_ASSERT(inst->isMaster(bucket));
          TS_ASSERT(inst->isMaster(bucket));

          for (inst->setFirst(); !inst->end(); inst->inc()) {
            TS_ASSERT_DELTA(bucket->get(*inst), product.get(*inst), (double)0.01);
          }

          delete inst;
        }

        delete bucket;
      }
    }

    void testAllVariables() {
      gum::MultiDimBucket<double> *bucket = 0;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>(0));

      if (bucket != 0) {
        TS_ASSERT_THROWS_NOTHING(__fillBucket(bucket));
        TS_ASSERT_EQUALS(bucket->allVariables().size(), (gum::Size)10);
        gum::Size inBucket = 0;
        gum::Size outBucket = 0;

        try {
          for (const auto var : bucket->allVariables())
            if (bucket->contains(*var))
              inBucket++;
            else
              outBucket++;
        } catch (gum::Exception &e) {
          std::cerr << std::endl << e.errorContent() << std::endl;
          TS_ASSERT(false);
        }

        TS_ASSERT_EQUALS(inBucket, bucket->nbrDim());

        TS_ASSERT_EQUALS(inBucket + outBucket, (gum::Size)10);
        delete bucket;
      }
    }

    void testWithSmallBN() {
      gum::BayesNet<float> *bn = new gum::BayesNet<float>();
      gum::LabelizedVariable vc("c", "cloudy", 2), vs("s", "sprinklet", 2);
      gum::LabelizedVariable vr("r", "rain", 2), vw("w", "wet grass", 2);
      gum::Id c = bn->add(vc);
      gum::Id s = bn->add(vs);
      gum::Id r = bn->add(vr);
      gum::Id w = bn->add(vw);
      bn->addArc(c, s);
      bn->addArc(c, r);
      bn->addArc(s, w);
      bn->addArc(r, w);
      {
        const float t[2] = {0.5, 0.5};
        const std::vector<float> ca(t, t + 2);
        bn->cpt(c).fillWith(ca);
      }

      {
        const float t[4] = {0.5, 0.5, 0.9, 0.1};
        const std::vector<float> sa(t, t + 4);
        bn->cpt(s).fillWith(sa);
      }

      {
        const float t[4] = {0.8, 0.2, 0.2, 0.8};
        const std::vector<float> ra(t, t + 4);
        bn->cpt(r).fillWith(ra);
      }

      {
        const float t[8] = {1., 0., 0.1, 0.9, 0.1, 0.9, 0.01, 0.99};
        const std::vector<float> wa(t, t + 8);
        bn->cpt(w).fillWith(wa);
      }

      gum::Potential<float> *e_s = new gum::Potential<float>();
      {
        e_s->add(bn->variable(s));
        const float t[2] = {0., 1.};
        const std::vector<float> sa(t, t + 2);
        e_s->fillWith(sa);
      }

      gum::Potential<float> *e_c = new gum::Potential<float>();
      {
        e_c->add(bn->variable(c));
        const float t[2] = {1., 0.};
        const std::vector<float> ca(t, t + 2);
        e_c->fillWith(ca);
      }

      gum::Potential<float> clique_csr;
      gum::MultiDimBucket<float> bucket_csr;
      clique_csr.add(bn->variable(c));
      bucket_csr.add(bn->variable(c));
      clique_csr.add(bn->variable(r));
      bucket_csr.add(bn->variable(s));
      clique_csr.add(bn->variable(s));
      bucket_csr.add(bn->variable(r));
      clique_csr.fill((float)1);
      bucket_csr.add(bn->cpt(c));
      clique_csr.multiplicateBy(bn->cpt(c));
      bucket_csr.add(bn->cpt(s));
      clique_csr.multiplicateBy(bn->cpt(r));
      bucket_csr.add(bn->cpt(r));
      clique_csr.multiplicateBy(bn->cpt(s));
      bucket_csr.add(e_s);
      clique_csr.multiplicateBy(*e_s);
      bucket_csr.add(e_c);
      clique_csr.multiplicateBy(*e_c);

      {
        gum::Instantiation i;
        i.add(bn->variable(c));
        i.add(bn->variable(r));
        i.add(bn->variable(s));

        for (i.setFirst(); not i.end(); i.inc()) {
          TS_ASSERT_DELTA(clique_csr.get(i), bucket_csr.get(i), 1e-7);
        }
      }

      gum::Potential<float> sep_sr;
      gum::MultiDimBucket<float> bucket_sr;
      sep_sr.add(bn->variable(s));
      bucket_sr.add(bn->variable(s));
      sep_sr.add(bn->variable(r));
      bucket_sr.add(bn->variable(r));
      sep_sr.marginalize(clique_csr);
      bucket_sr.add(bucket_csr);

      {
        gum::Instantiation i;
        i.add(bn->variable(r));
        i.add(bn->variable(s));

        for (i.setFirst(); not i.end(); i.inc()) {
          TS_ASSERT_DELTA(sep_sr.get(i), bucket_sr.get(i), 1e-7);
        }
      }

      gum::Potential<float> clique_wsr;
      gum::MultiDimBucket<float> bucket_wsr;
      clique_wsr.add(bn->variable(w));
      bucket_wsr.add(bn->variable(w));
      clique_wsr.add(bn->variable(s));
      bucket_wsr.add(bn->variable(s));
      clique_wsr.add(bn->variable(r));
      bucket_wsr.add(bn->variable(r));
      clique_wsr.fill((float)1);
      bucket_wsr.add(bn->cpt(w));
      clique_wsr.multiplicateBy(bn->cpt(w));

      {
        gum::Instantiation i;
        i.add(bn->variable(w));
        i.add(bn->variable(r));
        i.add(bn->variable(s));

        for (i.setFirst(); not i.end(); i.inc()) {
          TS_ASSERT_DELTA(clique_wsr.get(i), bucket_wsr.get(i), 1e-7);
        }
      }

      gum::Potential<float> tmp;
      gum::MultiDimBucket<float> bucket_marg_w;
      tmp.add(bn->variable(w));
      bucket_marg_w.add(bn->variable(w));
      tmp.add(bn->variable(s));
      bucket_marg_w.add(bucket_wsr);
      tmp.add(bn->variable(r));
      bucket_marg_w.add(bucket_sr);
      tmp.fill((float)1);
      tmp.multiplicateBy(clique_wsr);
      tmp.multiplicateBy(sep_sr);
      gum::Potential<float> marg_w;
      marg_w.add(bn->variable(w));
      marg_w.marginalize(tmp);

      {
        gum::Instantiation i;
        i.add(bn->variable(w));

        for (i.setFirst(); not i.end(); i.inc()) {
          TS_ASSERT_DELTA(marg_w.get(i), bucket_marg_w.get(i), 1e-7);
        }
      }

      gum::Potential<float> norm_b_m_w;
      norm_b_m_w.add(bn->variable(w));
      {
        gum::Instantiation i(norm_b_m_w);

        for (i.setFirst(); not i.end(); i.inc()) {
          norm_b_m_w.set(i, bucket_marg_w.get(i));
        }
      }

      gum::MultiDimBucket<float> false_sep_sr;
      false_sep_sr.add(bn->variable(s));
      false_sep_sr.add(bn->variable(r));
      false_sep_sr.add(bucket_wsr);

      gum::MultiDimBucket<float> false_marg_w;
      false_marg_w.add(bn->variable(w));
      false_marg_w.add(false_sep_sr);
      false_marg_w.add(bucket_wsr);
      gum::Potential<float> fnw;
      fnw.add(bn->variable(w));
      {
        gum::Instantiation i;
        i.add(bn->variable(w));

        for (i.setFirst(); not i.end(); i.inc()) {
          fnw.set(i, false_marg_w.get(i));
        }
      }

      fnw.normalize();
      marg_w.normalize();
      norm_b_m_w.normalize();
      {
        gum::Instantiation i;
        i.add(bn->variable(w));

        for (i.setFirst(); not i.end(); i.inc()) {
          TS_ASSERT_DELTA(marg_w.get(i), norm_b_m_w.get(i), 1e-7);
        }
      }

      delete bn;
      delete e_s;
      delete e_c;
    }
  };
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
