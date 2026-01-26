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


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/multidim/implementations/multiDimBucket.h>
#include <agrum/base/multidim/instantiation.h>
#include <agrum/base/multidim/utils/operators/operators4MultiDim.h>
#include <agrum/base/multidim/utils/operators/projections4MultiDim.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  MultiDimBucket
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct MultiDimBucketTestSuite {
    private:
    std::vector< gum::LabelizedVariable* >* _variables_;
    std::vector< gum::Tensor< double >* >*  _tensors_;

    void _fillBucket_(gum::MultiDimBucket< double >* bucket) {
      for (size_t i = 0; i < 5; ++i) {
        bucket->add(_tensors_->at(i));
      }
    }

    // Product must have variables
    void _makeProduct_(gum::Tensor< double >& product) {
      if (_tensors_->size() == 0) return;

      std::vector< gum::Tensor< double >* > temp(*_tensors_);

      // we use the first element as init for the result
      gum::Tensor< double > result = *temp.back();
      temp.pop_back();
      while (temp.size() > 0) {
        result *= (*temp.back());
        temp.pop_back();
      }

      auto del_vars = gum::VariableSet();
      for (auto var: result.variablesSequence()) {
        if (!product.contains(*var)) { del_vars.insert(var); }
      }
      product = result.sumOut(del_vars);
    }

    public:
    static void testCreation() {
      gum::MultiDimBucket< double >* bucket = 0;

      CHECK_NOTHROW(bucket = new gum::MultiDimBucket< double >());
      CHECK_NOTHROW(delete bucket);
    }

    MultiDimBucketTestSuite() {
      gum::SimpleCPTGenerator< double > cptGenerator;

      _variables_ = new std::vector< gum::LabelizedVariable* >();

      for (gum::Size i = 0; i < 10; ++i) {
        std::stringstream sBuff;

        sBuff << "var_" << i;

        _variables_->push_back(new gum::LabelizedVariable(sBuff.str(), "A binary variable", 2));
      }

      _tensors_ = new std::vector< gum::Tensor< double >* >();

      for (gum::Size i = 0; i < 5; ++i) {
        _tensors_->push_back(new gum::Tensor< double >(new gum::MultiDimArray< double >()));
      }

      // Creating a table of 2 elements

      _tensors_->at(0)->add(*(_variables_->at(0)));

      cptGenerator.generateCPT(_tensors_->at(0)->pos(*(_variables_->at(0))), *(_tensors_->at(0)));

      // Creating a table of 2 elements

      _tensors_->at(1)->add(*(_variables_->at(1)));

      cptGenerator.generateCPT(_tensors_->at(1)->pos(*(_variables_->at(1))), *(_tensors_->at(1)));

      // Creating a table of 2^4=16 elements

      for (size_t i = 2; i < 6; ++i) {
        _tensors_->at(2)->add(*(_variables_->at(i)));
      }

      cptGenerator.generateCPT(_tensors_->at(2)->pos(*(_variables_->at(2))), *(_tensors_->at(2)));

      // Creatinh a table of 2^4=16 elements

      for (size_t i = 4; i < 8; ++i) {
        _tensors_->at(3)->add(*(_variables_->at(i)));
      }

      cptGenerator.generateCPT(_tensors_->at(3)->pos(*(_variables_->at(4))), *(_tensors_->at(3)));

      // Creatinh a table of 2^4=16 elements

      for (size_t i = 6; i < 10; ++i) {
        _tensors_->at(4)->add(*(_variables_->at(i)));
      }

      cptGenerator.generateCPT(_tensors_->at(4)->pos(*(_variables_->at(6))), *(_tensors_->at(4)));
    }

    ~MultiDimBucketTestSuite() {
      while (!_tensors_->empty()) {
        delete _tensors_->back();

        _tensors_->pop_back();
      }

      while (!_variables_->empty()) {
        delete _variables_->back();

        _variables_->pop_back();
      }

      delete _variables_;

      delete _tensors_;
    }

    void testAddEraseTables() {
      gum::MultiDimBucket< double >* bucket = 0;
      CHECK_NOTHROW(bucket = new gum::MultiDimBucket< double >());

      if (bucket != 0) {
        CHECK(bucket->isBucketEmpty());

        for (size_t i = 0; i < 5; ++i) {
          CHECK_NOTHROW(bucket->add(_tensors_->at(i)));
        }

        CHECK(!bucket->isBucketEmpty());

        CHECK((bucket->bucketSize()) == (static_cast< gum::Size >(5)));
        CHECK_NOTHROW(bucket->erase(_tensors_->at(4)));
        CHECK((bucket->bucketSize()) == (static_cast< gum::Size >(4)));

        for (size_t i = 5; i > 0; --i) {
          CHECK_NOTHROW(bucket->erase(_tensors_->at(i - 1)));
        }

        CHECK((bucket->bucketSize()) == (static_cast< gum::Size >(0)));

        CHECK(bucket->isBucketEmpty());
        delete bucket;
      }
    }

    void testComputation() {
      gum::MultiDimBucket< double >* bucket = 0;
      gum::Tensor< double >          product;
      CHECK_NOTHROW(bucket = new gum::MultiDimBucket< double >());
      if (bucket != 0) {
        CHECK_NOTHROW(_fillBucket_(bucket));

        for (size_t i = 3; i < 6; ++i) {
          CHECK_NOTHROW(bucket->add(*(_variables_->at(i))));
          product.add(*(_variables_->at(i)));
        }

        GUM_CHECK_ASSERT_THROWS_NOTHING(bucket->compute());
        GUM_CHECK_ASSERT_THROWS_NOTHING(_makeProduct_(product));
        gum::Instantiation inst(product);

        CHECK((bucket->domainSize()) == (product.domainSize()));
        CHECK((bucket->nbrDim()) == (product.nbrDim()));

        for (inst.setFirst(); !inst.end(); inst.inc()) {
          CHECK((bucket->get(inst))
                == doctest::Approx(product.get(inst)).epsilon(static_cast< double >(0.01)));
        }
        delete bucket;
      }
    }

    void testOnTheFly() {
      gum::MultiDimBucket< double >* bucket = 0;
      gum::Tensor< double >          product;
      CHECK_NOTHROW(bucket = new gum::MultiDimBucket< double >(0));

      if (bucket != 0) {
        CHECK_NOTHROW(_fillBucket_(bucket));

        for (size_t i = 3; i < 6; ++i) {
          CHECK_NOTHROW(bucket->add(*(_variables_->at(i))));
          product.add(*(_variables_->at(i)));
        }

        GUM_CHECK_ASSERT_THROWS_NOTHING(bucket->compute());

        GUM_CHECK_ASSERT_THROWS_NOTHING(_makeProduct_(product));
        gum::Instantiation inst(product);
        CHECK((bucket->domainSize()) == (product.domainSize()));
        CHECK((bucket->nbrDim()) == (product.nbrDim()));
        CHECK((bucket->realSize()) == (static_cast< gum::Size >(0)));

        for (inst.setFirst(); !inst.end(); inst.inc()) {
          CHECK((bucket->get(inst))
                == doctest::Approx(product.get(inst)).epsilon(static_cast< double >(0.01)));
        }

        delete bucket;
      }
    }

    void testInstantiationsWithBuffer() {
      gum::MultiDimBucket< double >* bucket = 0;
      gum::Tensor< double >          product;
      CHECK_NOTHROW(bucket = new gum::MultiDimBucket< double >());

      if (bucket != 0) {
        CHECK_NOTHROW(_fillBucket_(bucket));

        for (size_t i = 3; i < 6; ++i) {
          CHECK_NOTHROW(bucket->add(*(_variables_->at(i))));
          product.add(*(_variables_->at(i)));
        }

        GUM_CHECK_ASSERT_THROWS_NOTHING(bucket->compute());

        GUM_CHECK_ASSERT_THROWS_NOTHING(_makeProduct_(product));

        gum::Instantiation* inst = 0;
        GUM_CHECK_ASSERT_THROWS_NOTHING(inst = new gum::Instantiation(*bucket));

        if (inst != 0) {
          CHECK(!inst->isMaster(bucket));
          CHECK(inst->isMaster(bucket->getMasterRef()));

          for (inst->setFirst(); !inst->end(); inst->inc()) {
            CHECK((bucket->get(*inst))
                  == doctest::Approx(product.get(*inst)).epsilon(static_cast< double >(0.01)));
          }

          delete inst;
        }

        delete bucket;
      }
    }

    void testInstantiationsWithBufferAndAutoCompute() {
      gum::MultiDimBucket< double >* bucket = 0;
      gum::Tensor< double >          product;
      CHECK_NOTHROW(bucket = new gum::MultiDimBucket< double >());

      if (bucket != 0) {
        CHECK_NOTHROW(_fillBucket_(bucket));

        for (size_t i = 3; i < 6; ++i) {
          CHECK_NOTHROW(bucket->add(*(_variables_->at(i))));
          product.add(*(_variables_->at(i)));
        }

        GUM_CHECK_ASSERT_THROWS_NOTHING(_makeProduct_(product));

        gum::Instantiation* inst = 0;
        GUM_CHECK_ASSERT_THROWS_NOTHING(inst = new gum::Instantiation(*bucket));

        if (inst != 0) {
          CHECK(!inst->isMaster(bucket));
          CHECK(inst->isMaster(bucket->getMasterRef()));

          for (inst->setFirst(); !inst->end(); inst->inc()) {
            CHECK((bucket->get(*inst))
                  == doctest::Approx(product.get(*inst)).epsilon(static_cast< double >(0.01)));
          }

          delete inst;
        }

        delete bucket;
      }
    }

    void testInstantiationsOnTheFly() {
      gum::MultiDimBucket< double >* bucket = 0;
      gum::Tensor< double >          product;
      CHECK_NOTHROW(bucket = new gum::MultiDimBucket< double >(0));

      if (bucket != 0) {
        CHECK_NOTHROW(_fillBucket_(bucket));

        for (size_t i = 3; i < 6; ++i) {
          CHECK_NOTHROW(bucket->add(*(_variables_->at(i))));
          product.add(*(_variables_->at(i)));
        }

        GUM_CHECK_ASSERT_THROWS_NOTHING(bucket->compute());

        GUM_CHECK_ASSERT_THROWS_NOTHING(_makeProduct_(product));

        gum::Instantiation* inst = 0;
        GUM_CHECK_ASSERT_THROWS_NOTHING(inst = new gum::Instantiation(*bucket));

        if (inst != 0) {
          CHECK(inst->isMaster(bucket));
          CHECK(inst->isMaster(bucket->getMasterRef()));

          for (inst->setFirst(); !inst->end(); inst->inc()) {
            CHECK((bucket->get(*inst))
                  == doctest::Approx(product.get(*inst)).epsilon(static_cast< double >(0.01)));
          }

          delete inst;
        }

        delete bucket;
      }
    }

    void testBucketSizeChanges() {
      gum::MultiDimBucket< double >* bucket = 0;
      gum::Tensor< double >          product;
      CHECK_NOTHROW(bucket = new gum::MultiDimBucket< double >(0));

      if (bucket != 0) {
        CHECK_NOTHROW(_fillBucket_(bucket));
        CHECK(bucket->bucketChanged());

        for (size_t i = 3; i < 6; ++i) {
          CHECK_NOTHROW(bucket->add(*(_variables_->at(i))));
          CHECK(bucket->bucketChanged());
          product.add(*(_variables_->at(i)));
        }

        GUM_CHECK_ASSERT_THROWS_NOTHING(bucket->compute());

        CHECK(!bucket->bucketChanged());
        GUM_CHECK_ASSERT_THROWS_NOTHING(_makeProduct_(product));

        CHECK((bucket->realSize()) == (static_cast< gum::Size >(0)));

        gum::Instantiation* inst = nullptr;
        GUM_CHECK_ASSERT_THROWS_NOTHING(inst = new gum::Instantiation(*bucket));
        CHECK(!bucket->bucketChanged());

        if (inst != nullptr) {
          CHECK(inst->isMaster(bucket));
          CHECK(inst->isMaster(bucket->getMasterRef()));

          for (inst->setFirst(); !inst->end(); inst->inc()) {
            CHECK((bucket->get(*inst))
                  == doctest::Approx(product.get(*inst)).epsilon(static_cast< double >(0.01)));
            CHECK(!bucket->bucketChanged());
          }

          GUM_CHECK_ASSERT_THROWS_NOTHING(bucket->setBufferSize(static_cast< gum::Size >(65536)));

          CHECK(bucket->bucketChanged());
          CHECK(bucket->realSize() > static_cast< gum::Size >(0));
          CHECK(bucket->bucketChanged());
          GUM_CHECK_ASSERT_THROWS_NOTHING(bucket->compute());
          CHECK(!bucket->bucketChanged());

          CHECK(inst->isMaster(bucket));
          CHECK(inst->isMaster(bucket));

          for (inst->setFirst(); !inst->end(); inst->inc()) {
            CHECK((bucket->get(*inst))
                  == doctest::Approx(product.get(*inst)).epsilon(static_cast< double >(0.01)));
          }

          delete inst;
        }

        delete bucket;
      }
    }

    void testAllVariables() {
      gum::MultiDimBucket< double >* bucket = 0;
      CHECK_NOTHROW(bucket = new gum::MultiDimBucket< double >(0));

      if (bucket != 0) {
        CHECK_NOTHROW(_fillBucket_(bucket));
        CHECK((bucket->allVariables().size()) == (static_cast< gum::Size >(10)));
        gum::Size inBucket  = 0;
        gum::Size outBucket = 0;

        try {
          for (const auto var: bucket->allVariables())
            if (bucket->contains(*var)) inBucket++;
            else outBucket++;
        } catch (gum::Exception& e) {
          std::cerr << std::endl << e.errorContent() << std::endl;
          CHECK(false);
        }

        CHECK((inBucket) == (bucket->nbrDim()));

        CHECK((inBucket + outBucket) == (static_cast< gum::Size >(10)));
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


      auto e_s = new gum::Tensor< double >();
      e_s->add(bn->variable(s));
      e_s->fillWith({0., 1.});

      auto e_c = new gum::Tensor< double >();
      e_c->add(bn->variable(c));
      e_c->fillWith({1., 0.});

      gum::Tensor< double >         clique_csr;
      gum::MultiDimBucket< double > bucket_csr;
      clique_csr.add(bn->variable(c));
      bucket_csr.add(bn->variable(c));
      clique_csr.add(bn->variable(r));
      bucket_csr.add(bn->variable(s));
      clique_csr.add(bn->variable(s));
      bucket_csr.add(bn->variable(r));
      clique_csr.fill(static_cast< double >(1));
      bucket_csr.add(bn->cpt(c));
      clique_csr = gum::Tensor< double >(clique_csr * bn->cpt(c));
      bucket_csr.add(bn->cpt(s));
      clique_csr = gum::Tensor< double >(clique_csr * bn->cpt(r));
      bucket_csr.add(bn->cpt(r));
      clique_csr = gum::Tensor< double >(clique_csr * bn->cpt(s));
      bucket_csr.add(e_s);
      clique_csr = gum::Tensor< double >(clique_csr * *e_s);
      bucket_csr.add(e_c);
      clique_csr = gum::Tensor< double >(clique_csr * *e_c);

      {
        gum::Instantiation i;
        i.add(bn->variable(c));
        i.add(bn->variable(r));
        i.add(bn->variable(s));

        for (i.setFirst(); !i.end(); i.inc()) {
          CHECK((clique_csr.get(i)) == doctest::Approx(bucket_csr.get(i)).epsilon(1e-7));
        }
      }

      gum::Tensor< double >         sep_sr;
      gum::MultiDimBucket< double > bucket_sr;
      sep_sr.add(bn->variable(s));
      bucket_sr.add(bn->variable(s));
      sep_sr.add(bn->variable(r));
      bucket_sr.add(bn->variable(r));

      auto del_vars = gum::VariableSet();
      for (auto var: clique_csr.variablesSequence()) {
        if (!sep_sr.contains(*var)) { del_vars.insert(var); }
      }
      sep_sr = clique_csr.sumOut(del_vars);
      bucket_sr.add(bucket_csr);

      {
        gum::Instantiation i;
        i.add(bn->variable(r));
        i.add(bn->variable(s));

        for (i.setFirst(); !i.end(); i.inc()) {
          CHECK((sep_sr.get(i)) == doctest::Approx(bucket_sr.get(i)).epsilon(1e-7));
        }
      }

      gum::Tensor< double >         clique_wsr;
      gum::MultiDimBucket< double > bucket_wsr;
      clique_wsr.add(bn->variable(w));
      bucket_wsr.add(bn->variable(w));
      clique_wsr.add(bn->variable(s));
      bucket_wsr.add(bn->variable(s));
      clique_wsr.add(bn->variable(r));
      bucket_wsr.add(bn->variable(r));
      clique_wsr.fill(static_cast< double >(1));
      bucket_wsr.add(bn->cpt(w));
      clique_wsr = gum::Tensor< double >(clique_wsr * bn->cpt(w));

      {
        gum::Instantiation i;
        i.add(bn->variable(w));
        i.add(bn->variable(r));
        i.add(bn->variable(s));

        for (i.setFirst(); !i.end(); i.inc()) {
          CHECK((clique_wsr.get(i)) == doctest::Approx(bucket_wsr.get(i)).epsilon(1e-7));
        }
      }

      gum::Tensor< double >         tmp;
      gum::MultiDimBucket< double > bucket_marg_w;
      tmp.add(bn->variable(w));
      bucket_marg_w.add(bn->variable(w));
      tmp.add(bn->variable(s));
      bucket_marg_w.add(bucket_wsr);
      tmp.add(bn->variable(r));
      bucket_marg_w.add(bucket_sr);
      tmp.fill(static_cast< double >(1));
      tmp = gum::Tensor< double >(tmp * clique_wsr);
      tmp = gum::Tensor< double >(tmp * sep_sr);
      gum::Tensor< double > marg_w;
      marg_w.add(bn->variable(w));

      del_vars = gum::VariableSet();
      for (auto var: tmp.variablesSequence()) {
        if (!marg_w.contains(*var)) { del_vars.insert(var); }
      }
      marg_w = gum::Tensor< double >(tmp.sumOut(del_vars));

      {
        gum::Instantiation i;
        i.add(bn->variable(w));

        for (i.setFirst(); !i.end(); i.inc()) {
          CHECK((marg_w.get(i)) == doctest::Approx(bucket_marg_w.get(i)).epsilon(1e-7));
        }
      }

      gum::Tensor< double > norm_b_m_w;
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
      gum::Tensor< double > fnw;
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
          CHECK((marg_w.get(i)) == doctest::Approx(norm_b_m_w.get(i)).epsilon(1e-7));
        }
      }

      delete bn;
      delete e_s;
      delete e_c;
    }
  };

  GUM_TEST_ACTIF(Creation)
  GUM_TEST_ACTIF(AddEraseTables)
  GUM_TEST_ACTIF(Computation)
  GUM_TEST_ACTIF(OnTheFly)
  GUM_TEST_ACTIF(InstantiationsWithBuffer)
  GUM_TEST_ACTIF(InstantiationsWithBufferAndAutoCompute)
  GUM_TEST_ACTIF(InstantiationsOnTheFly)
  GUM_TEST_ACTIF(BucketSizeChanges)
  GUM_TEST_ACTIF(AllVariables)
  GUM_TEST_ACTIF(WithSmallBN)
}   // namespace gum_tests
