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


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/tools/multidim/implementations/multiDimBucket.h>
#include <agrum/tools/multidim/instantiation.h>
#include <agrum/tools/multidim/utils/operators/operators4MultiDim.h>
#include <agrum/tools/multidim/utils/operators/projections4MultiDim.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class MultiDimBucketTestSuite: public CxxTest::TestSuite {
    private:
    std::vector< gum::LabelizedVariable* >*    _variables_;
    std::vector< gum::Potential< double >* >*  _potentials_;

    void  _fillBucket_(gum::MultiDimBucket< double >* bucket) {
      for (size_t i = 0; i < 5; ++i) {
        bucket->add( _potentials_->at(i));
      }
    }

    // Product must have variables
    void  _makeProduct_(gum::Potential< double >& product) {
      if ( _potentials_->size() == 0) return;

      std::vector< gum::Potential< double >* > temp(* _potentials_);

      // we use the first element as init for the result
      gum::Potential< double > result = *temp.back();
      temp.pop_back();
      while (temp.size() > 0) {
        result *= (*temp.back());
        temp.pop_back();
      }

      auto del_vars = gum::Set< const gum::DiscreteVariable* >();
      for (auto var: result.variablesSequence()) {
        if (!product.contains(*var)) { del_vars.insert(var); }
      }
      product = result.margSumOut(del_vars);
    }

    public:
    void setUp() {
      gum::SimpleCPTGenerator< double > cptGenerator;
       _variables_ = new std::vector< gum::LabelizedVariable* >();

      for (gum::Size i = 0; i < 10; ++i) {
        std::stringstream sBuff;
        sBuff << "var_" << i;
         _variables_->push_back(
           new gum::LabelizedVariable(sBuff.str(), "A binary variable", 2));
      }

       _potentials_ = new std::vector< gum::Potential< double >* >();

      for (gum::Size i = 0; i < 5; ++i) {
         _potentials_->push_back(
           new gum::Potential< double >(new gum::MultiDimArray< double >()));
      }

      // Creating a table of 2 elements
       _potentials_->at(0)->add(*( _variables_->at(0)));

      cptGenerator.generateCPT( _potentials_->at(0)->pos(*( _variables_->at(0))),
                               *( _potentials_->at(0)));

      // Creating a table of 2 elements
       _potentials_->at(1)->add(*( _variables_->at(1)));

      cptGenerator.generateCPT( _potentials_->at(1)->pos(*( _variables_->at(1))),
                               *( _potentials_->at(1)));

      // Creating a table of 2^4=16 elements
      for (size_t i = 2; i < 6; ++i) {
         _potentials_->at(2)->add(*( _variables_->at(i)));
      }

      cptGenerator.generateCPT( _potentials_->at(2)->pos(*( _variables_->at(2))),
                               *( _potentials_->at(2)));

      // Creatinh a table of 2^4=16 elements

      for (size_t i = 4; i < 8; ++i) {
         _potentials_->at(3)->add(*( _variables_->at(i)));
      }

      cptGenerator.generateCPT( _potentials_->at(3)->pos(*( _variables_->at(4))),
                               *( _potentials_->at(3)));

      // Creatinh a table of 2^4=16 elements

      for (size_t i = 6; i < 10; ++i) {
         _potentials_->at(4)->add(*( _variables_->at(i)));
      }

      cptGenerator.generateCPT( _potentials_->at(4)->pos(*( _variables_->at(6))),
                               *( _potentials_->at(4)));
    }

    void tearDown() {
      while ( _potentials_->size() > 0) {
        delete  _potentials_->back();
         _potentials_->pop_back();
      }

      while ( _variables_->size() > 0) {
        delete  _variables_->back();
         _variables_->pop_back();
      }

      delete  _variables_;

      delete  _potentials_;
    }

    void testCreation() {
      gum::MultiDimBucket< double >* bucket = 0;

      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket< double >());
      TS_ASSERT_THROWS_NOTHING(delete bucket);
    }

    void testAddEraseTables() {
      gum::MultiDimBucket< double >* bucket = 0;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket< double >());

      if (bucket != 0) {
        TS_ASSERT(bucket->isBucketEmpty());

        for (size_t i = 0; i < 5; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add( _potentials_->at(i)));
        }

        TS_ASSERT(!bucket->isBucketEmpty());

        TS_ASSERT_EQUALS(bucket->bucketSize(), (gum::Size)5);
        TS_ASSERT_THROWS_NOTHING(bucket->erase( _potentials_->at(4)));
        TS_ASSERT_EQUALS(bucket->bucketSize(), (gum::Size)4);

        for (size_t i = 5; i > 0; --i) {
          TS_ASSERT_THROWS_NOTHING(bucket->erase( _potentials_->at(i - 1)));
        }

        TS_ASSERT_EQUALS(bucket->bucketSize(), (gum::Size)0);

        TS_ASSERT(bucket->isBucketEmpty());
        delete bucket;
      }
    }

    void testComputation() {
      gum::MultiDimBucket< double >* bucket = 0;
      gum::Potential< double >       product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket< double >());
      if (bucket != 0) {
        TS_ASSERT_THROWS_NOTHING( _fillBucket_(bucket));

        for (size_t i = 3; i < 6; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add(*( _variables_->at(i))));
          product.add(*( _variables_->at(i)));
        }

        TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());
        TS_GUM_ASSERT_THROWS_NOTHING( _makeProduct_(product));
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
      gum::MultiDimBucket< double >* bucket = 0;
      gum::Potential< double >       product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket< double >(0));

      if (bucket != 0) {
        TS_ASSERT_THROWS_NOTHING( _fillBucket_(bucket));

        for (size_t i = 3; i < 6; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add(*( _variables_->at(i))));
          product.add(*( _variables_->at(i)));
        }

        TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());

        TS_GUM_ASSERT_THROWS_NOTHING( _makeProduct_(product));
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
      gum::MultiDimBucket< double >* bucket = 0;
      gum::Potential< double >       product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket< double >());

      if (bucket != 0) {
        TS_ASSERT_THROWS_NOTHING( _fillBucket_(bucket));

        for (size_t i = 3; i < 6; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add(*( _variables_->at(i))));
          product.add(*( _variables_->at(i)));
        }

        TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());

        TS_GUM_ASSERT_THROWS_NOTHING( _makeProduct_(product));

        gum::Instantiation* inst = 0;
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
      gum::MultiDimBucket< double >* bucket = 0;
      gum::Potential< double >       product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket< double >());

      if (bucket != 0) {
        TS_ASSERT_THROWS_NOTHING( _fillBucket_(bucket));

        for (size_t i = 3; i < 6; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add(*( _variables_->at(i))));
          product.add(*( _variables_->at(i)));
        }

        TS_GUM_ASSERT_THROWS_NOTHING( _makeProduct_(product));

        gum::Instantiation* inst = 0;
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
      gum::MultiDimBucket< double >* bucket = 0;
      gum::Potential< double >       product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket< double >(0));

      if (bucket != 0) {
        TS_ASSERT_THROWS_NOTHING( _fillBucket_(bucket));

        for (size_t i = 3; i < 6; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add(*( _variables_->at(i))));
          product.add(*( _variables_->at(i)));
        }

        TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());

        TS_GUM_ASSERT_THROWS_NOTHING( _makeProduct_(product));

        gum::Instantiation* inst = 0;
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
      gum::MultiDimBucket< double >* bucket = 0;
      gum::Potential< double >       product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket< double >(0));

      if (bucket != 0) {
        TS_ASSERT_THROWS_NOTHING( _fillBucket_(bucket));
        TS_ASSERT(bucket->bucketChanged());

        for (size_t i = 3; i < 6; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add(*( _variables_->at(i))));
          TS_ASSERT(bucket->bucketChanged());
          product.add(*( _variables_->at(i)));
        }

        TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());

        TS_ASSERT(!bucket->bucketChanged());
        TS_GUM_ASSERT_THROWS_NOTHING( _makeProduct_(product));

        TS_ASSERT_EQUALS(bucket->realSize(), (gum::Size)0);

        gum::Instantiation* inst = 0;
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
      gum::MultiDimBucket< double >* bucket = 0;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket< double >(0));

      if (bucket != 0) {
        TS_ASSERT_THROWS_NOTHING( _fillBucket_(bucket));
        TS_ASSERT_EQUALS(bucket->allVariables().size(), (gum::Size)10);
        gum::Size inBucket  = 0;
        gum::Size outBucket = 0;

        try {
          for (const auto var: bucket->allVariables())
            if (bucket->contains(*var))
              inBucket++;
            else
              outBucket++;
        } catch (gum::Exception& e) {
          std::cerr << std::endl << e.errorContent() << std::endl;
          TS_ASSERT(false);
        }

        TS_ASSERT_EQUALS(inBucket, bucket->nbrDim());

        TS_ASSERT_EQUALS(inBucket + outBucket, (gum::Size)10);
        delete bucket;
      }
    }

    void testWithSmallBN() {
      gum::BayesNet< double >* bn = new gum::BayesNet< double >();
      gum::LabelizedVariable   vc("c", "cloudy", 2), vs("s", "sprinklet", 2);
      gum::LabelizedVariable   vr("r", "rain", 2), vw("w", "wet grass", 2);
      gum::NodeId              c = bn->add(vc);
      gum::NodeId              s = bn->add(vs);
      gum::NodeId              r = bn->add(vr);
      gum::NodeId              w = bn->add(vw);
      bn->addArc(c, s);
      bn->addArc(c, r);
      bn->addArc(s, w);
      bn->addArc(r, w);
      bn->cpt(c).fillWith({0.5, 0.5});
      bn->cpt(s).fillWith({0.5, 0.5, 0.9, 0.1});
      bn->cpt(r).fillWith({0.8, 0.2, 0.2, 0.8});
      bn->cpt(w).fillWith({1., 0., 0.1, 0.9, 0.1, 0.9, 0.01, 0.99});


      gum::Potential< double >* e_s = new gum::Potential< double >();
      e_s->add(bn->variable(s));
      e_s->fillWith({0., 1.});

      gum::Potential< double >* e_c = new gum::Potential< double >();
      e_c->add(bn->variable(c));
      e_c->fillWith({1., 0.});

      gum::Potential< double >      clique_csr;
      gum::MultiDimBucket< double > bucket_csr;
      clique_csr.add(bn->variable(c));
      bucket_csr.add(bn->variable(c));
      clique_csr.add(bn->variable(r));
      bucket_csr.add(bn->variable(s));
      clique_csr.add(bn->variable(s));
      bucket_csr.add(bn->variable(r));
      clique_csr.fill((double)1);
      bucket_csr.add(bn->cpt(c));
      clique_csr = gum::Potential< double >(clique_csr * bn->cpt(c));
      bucket_csr.add(bn->cpt(s));
      clique_csr = gum::Potential< double >(clique_csr * bn->cpt(r));
      bucket_csr.add(bn->cpt(r));
      clique_csr = gum::Potential< double >(clique_csr * bn->cpt(s));
      bucket_csr.add(e_s);
      clique_csr = gum::Potential< double >(clique_csr * *e_s);
      bucket_csr.add(e_c);
      clique_csr = gum::Potential< double >(clique_csr * *e_c);

      {
        gum::Instantiation i;
        i.add(bn->variable(c));
        i.add(bn->variable(r));
        i.add(bn->variable(s));

        for (i.setFirst(); !i.end(); i.inc()) {
          TS_ASSERT_DELTA(clique_csr.get(i), bucket_csr.get(i), 1e-7);
        }
      }

      gum::Potential< double >      sep_sr;
      gum::MultiDimBucket< double > bucket_sr;
      sep_sr.add(bn->variable(s));
      bucket_sr.add(bn->variable(s));
      sep_sr.add(bn->variable(r));
      bucket_sr.add(bn->variable(r));

      auto del_vars = gum::Set< const gum::DiscreteVariable* >();
      for (auto var: clique_csr.variablesSequence()) {
        if (!sep_sr.contains(*var)) { del_vars.insert(var); }
      }
      sep_sr = clique_csr.margSumOut(del_vars);
      bucket_sr.add(bucket_csr);

      {
        gum::Instantiation i;
        i.add(bn->variable(r));
        i.add(bn->variable(s));

        for (i.setFirst(); !i.end(); i.inc()) {
          TS_ASSERT_DELTA(sep_sr.get(i), bucket_sr.get(i), 1e-7);
        }
      }

      gum::Potential< double >      clique_wsr;
      gum::MultiDimBucket< double > bucket_wsr;
      clique_wsr.add(bn->variable(w));
      bucket_wsr.add(bn->variable(w));
      clique_wsr.add(bn->variable(s));
      bucket_wsr.add(bn->variable(s));
      clique_wsr.add(bn->variable(r));
      bucket_wsr.add(bn->variable(r));
      clique_wsr.fill((double)1);
      bucket_wsr.add(bn->cpt(w));
      clique_wsr = gum::Potential< double >(clique_wsr * bn->cpt(w));

      {
        gum::Instantiation i;
        i.add(bn->variable(w));
        i.add(bn->variable(r));
        i.add(bn->variable(s));

        for (i.setFirst(); !i.end(); i.inc()) {
          TS_ASSERT_DELTA(clique_wsr.get(i), bucket_wsr.get(i), 1e-7);
        }
      }

      gum::Potential< double >      tmp;
      gum::MultiDimBucket< double > bucket_marg_w;
      tmp.add(bn->variable(w));
      bucket_marg_w.add(bn->variable(w));
      tmp.add(bn->variable(s));
      bucket_marg_w.add(bucket_wsr);
      tmp.add(bn->variable(r));
      bucket_marg_w.add(bucket_sr);
      tmp.fill((double)1);
      tmp = gum::Potential< double >(tmp * clique_wsr);
      tmp = gum::Potential< double >(tmp * sep_sr);
      gum::Potential< double > marg_w;
      marg_w.add(bn->variable(w));

      del_vars = gum::Set< const gum::DiscreteVariable* >();
      for (auto var: tmp.variablesSequence()) {
        if (!marg_w.contains(*var)) { del_vars.insert(var); }
      }
      marg_w = gum::Potential< double >(tmp.margSumOut(del_vars));

      {
        gum::Instantiation i;
        i.add(bn->variable(w));

        for (i.setFirst(); !i.end(); i.inc()) {
          TS_ASSERT_DELTA(marg_w.get(i), bucket_marg_w.get(i), 1e-7);
        }
      }

      gum::Potential< double > norm_b_m_w;
      norm_b_m_w.add(bn->variable(w));
      {
        gum::Instantiation i(norm_b_m_w);

        for (i.setFirst(); !i.end(); i.inc()) {
          norm_b_m_w.set(i, bucket_marg_w.get(i));
        }
      }

      gum::MultiDimBucket< double > false_sep_sr;
      false_sep_sr.add(bn->variable(s));
      false_sep_sr.add(bucket_wsr);
      false_sep_sr.add(bn->variable(r));

      gum::MultiDimBucket< double > false_marg_w;
      false_marg_w.add(bn->variable(w));
      false_marg_w.add(false_sep_sr);
      false_marg_w.add(bucket_wsr);
      gum::Potential< double > fnw;
      fnw.add(bn->variable(w));
      {
        gum::Instantiation i;
        i.add(bn->variable(w));

        for (i.setFirst(); !i.end(); i.inc()) {
          fnw.set(i, false_marg_w.get(i));
        }
      }

      fnw.normalize();
      marg_w.normalize();
      norm_b_m_w.normalize();
      {
        gum::Instantiation i;
        i.add(bn->variable(w));

        for (i.setFirst(); !i.end(); i.inc()) {
          TS_ASSERT_DELTA(marg_w.get(i), norm_b_m_w.get(i), 1e-7);
        }
      }

      delete bn;
      delete e_s;
      delete e_c;
    }
  };
}   // namespace gum_tests
