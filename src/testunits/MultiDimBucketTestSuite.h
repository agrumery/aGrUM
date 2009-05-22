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

#include <cxxtest/TestSuite.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/multidim/multiDimBucket.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/instantiation.h>

class MultiDimBucketTestSuite: public CxxTest::TestSuite {
  private:
    std::vector<gum::LabelizedVariable*>* __variables;
    std::vector< gum::Potential<double>* >* __potentials;

    void __fillBucket(gum::MultiDimBucket<double>* bucket) {
      for (size_t i = 0; i < 5; ++i) {
        bucket->add(__potentials->at(i));
      }
    }

    // Product must have variables
    void __makeProduct(gum::Potential<double>& product) {
      gum::Potential<double> temp;
      for (size_t i = 0; i < 5; ++i) {
        temp.multiplicateBy(*(__potentials->at(i)));
      }
      product.marginalize(temp);
    }

  public:
    void setUp() {
      gum::SimpleCPTGenerator cptGenerator;
      __variables = new std::vector<gum::LabelizedVariable*>();
      for (gum::Size i = 0; i < 10; ++i) {
        std::stringstream sBuff;
        sBuff << "var_" << i;
        __variables->push_back(new gum::LabelizedVariable(sBuff.str(), "A binary variable", 2));
      }
      __potentials = new std::vector< gum::Potential<double>* >();
      for (gum::Size i = 0; i < 5; ++i) {
        __potentials->push_back(new gum::Potential<double>( new gum::MultiDimArray<double>() ));
      }
      // Creating a table of 2 elements
      __potentials->at(0)->add(*(__variables->at(0)));
      cptGenerator.generateCPT(__potentials->at(0)->pos(*(__variables->at(0))), *(__potentials->at(0)));
      // Creating a table of 2 elements
      __potentials->at(1)->add(*(__variables->at(1)));
      cptGenerator.generateCPT(__potentials->at(1)->pos(*(__variables->at(1))), *(__potentials->at(1)));
      // Creating a table of 2^4=16 elements
      for (size_t i = 2; i < 6; ++i) {
        __potentials->at(2)->add(*(__variables->at(i)));
      }
      cptGenerator.generateCPT(__potentials->at(2)->pos(*(__variables->at(2))), *(__potentials->at(2)));
      // Creatinh a table of 2^4=16 elements
      for (size_t i = 4; i < 8; ++i) {
        __potentials->at(3)->add(*(__variables->at(i)));
      }
      cptGenerator.generateCPT(__potentials->at(3)->pos(*(__variables->at(4))), *(__potentials->at(3)));
      // Creatinh a table of 2^4=16 elements
      for (size_t i = 6; i < 10; ++i) {
        __potentials->at(4)->add(*(__variables->at(i)));
      }
      cptGenerator.generateCPT(__potentials->at(4)->pos(*(__variables->at(6))), *(__potentials->at(4)));
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
      gum::MultiDimBucket<double>* bucket = 0;

      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>());
      TS_ASSERT_THROWS_NOTHING(delete bucket);
    }

    void testAddEraseTables() {
      gum::MultiDimBucket<double>* bucket = 0;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>());
      if (bucket != 0) {
        TS_ASSERT(bucket->isBucketEmpty());
        for (size_t i = 0; i < 5; ++i) {
          TS_ASSERT_THROWS_NOTHING(bucket->add(__potentials->at(i)));
        }
        TS_ASSERT(! bucket->isBucketEmpty());
        TS_ASSERT_EQUALS(bucket->bucketSize(), (gum::Size) 5);
        TS_ASSERT_THROWS_NOTHING(bucket->erase(__potentials->at(4)));
        TS_ASSERT_EQUALS(bucket->bucketSize(), (gum::Size) 4);
        for (size_t i = 5; i > 0; --i) {
          TS_ASSERT_THROWS_NOTHING(bucket->erase(__potentials->at(i-1)));
        }
        TS_ASSERT_EQUALS(bucket->bucketSize(), (gum::Size) 0);
        TS_ASSERT(bucket->isBucketEmpty());
        delete bucket;
      }
    }

    void testComputation() {
      gum::MultiDimBucket<double>* bucket = 0;
      gum::Potential<double> product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>());
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
      for (inst.setFirst(); ! inst.end(); inst.inc()) {
        TS_ASSERT_DELTA(bucket->get(inst), product.get(inst), (double) 0.01);
      }
    }

    void testOnTheFly() {
      gum::MultiDimBucket<double>* bucket = 0;
      gum::Potential<double> product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>(0));
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
      TS_ASSERT_EQUALS(bucket->realSize(), (gum::Size) 0);
      for (inst.setFirst(); ! inst.end(); inst.inc()) {
        TS_ASSERT_DELTA(bucket->get(inst), product.get(inst), (double) 0.01);
      }
    }

    void testInstantiationsWithBuffer() {
      gum::MultiDimBucket<double>* bucket = 0;
      gum::Potential<double> product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>());
      TS_ASSERT_THROWS_NOTHING(__fillBucket(bucket));
      for (size_t i = 3; i < 6; ++i) {
        TS_ASSERT_THROWS_NOTHING(bucket->add(*(__variables->at(i))));
        product.add(*(__variables->at(i)));
      }
      TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());
      TS_GUM_ASSERT_THROWS_NOTHING(__makeProduct(product));

      gum::Instantiation* inst = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(inst = new gum::Instantiation(*bucket));
      if (inst != 0) {
        TS_ASSERT(! inst->isMaster(bucket));
        TS_ASSERT(inst->isMaster(bucket->getMasterRef()));
        for (inst->setFirst(); ! inst->end(); inst->inc()) {
          TS_ASSERT_DELTA(bucket->get(*inst), product.get(*inst), (double) 0.01);
        }
      }
    }

    void testInstantiationsWithBufferAndAutoCompute() {
      gum::MultiDimBucket<double>* bucket = 0;
      gum::Potential<double> product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>());
      TS_ASSERT_THROWS_NOTHING(__fillBucket(bucket));
      for (size_t i = 3; i < 6; ++i) {
        TS_ASSERT_THROWS_NOTHING(bucket->add(*(__variables->at(i))));
        product.add(*(__variables->at(i)));
      }
      TS_GUM_ASSERT_THROWS_NOTHING(__makeProduct(product));

      gum::Instantiation* inst = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(inst = new gum::Instantiation(*bucket));
      if (inst != 0) {
        TS_ASSERT(! inst->isMaster(bucket));
        TS_ASSERT(inst->isMaster(bucket->getMasterRef()));
        for (inst->setFirst(); ! inst->end(); inst->inc()) {
          TS_ASSERT_DELTA(bucket->get(*inst), product.get(*inst), (double) 0.01);
        }
      }
    }

    void testInstantiationsOnTheFly() {
      gum::MultiDimBucket<double>* bucket = 0;
      gum::Potential<double> product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>(0));
      TS_ASSERT_THROWS_NOTHING(__fillBucket(bucket));
      for (size_t i = 3; i < 6; ++i) {
        TS_ASSERT_THROWS_NOTHING(bucket->add(*(__variables->at(i))));
        product.add(*(__variables->at(i)));
      }
      TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());
      TS_GUM_ASSERT_THROWS_NOTHING(__makeProduct(product));

      gum::Instantiation* inst = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(inst = new gum::Instantiation(*bucket));
      if (inst != 0) {
        TS_ASSERT(inst->isMaster(bucket));
        TS_ASSERT(inst->isMaster(bucket->getMasterRef()));
        for (inst->setFirst(); ! inst->end(); inst->inc()) {
          TS_ASSERT_DELTA(bucket->get(*inst), product.get(*inst), (double) 0.01);
        }
      }
    }

    void testBucketSizeChanges() {
      gum::MultiDimBucket<double>* bucket = 0;
      gum::Potential<double> product;
      TS_ASSERT_THROWS_NOTHING(bucket = new gum::MultiDimBucket<double>(0));
      TS_ASSERT_THROWS_NOTHING(__fillBucket(bucket));
      TS_ASSERT(bucket->bucketChanged());
      for (size_t i = 3; i < 6; ++i) {
        TS_ASSERT_THROWS_NOTHING(bucket->add(*(__variables->at(i))));
        TS_ASSERT(bucket->bucketChanged());
        product.add(*(__variables->at(i)));
      }
      TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());
      TS_ASSERT(! bucket->bucketChanged());
      TS_GUM_ASSERT_THROWS_NOTHING(__makeProduct(product));

      TS_ASSERT_EQUALS(bucket->realSize(), (gum::Size) 0);

      gum::Instantiation* inst = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(inst = new gum::Instantiation(*bucket));
      TS_ASSERT(! bucket->bucketChanged());
      if (inst != 0) {
        TS_ASSERT(inst->isMaster(bucket));
        TS_ASSERT(inst->isMaster(bucket->getMasterRef()));
        for (inst->setFirst(); ! inst->end(); inst->inc()) {
          TS_ASSERT_DELTA(bucket->get(*inst), product.get(*inst), (double) 0.01);
          TS_ASSERT(! bucket->bucketChanged());
        }
      }

      TS_GUM_ASSERT_THROWS_NOTHING(bucket->setBufferSize((gum::Size) 65536));
      TS_ASSERT(bucket->bucketChanged());
      TS_ASSERT(bucket->realSize() > (gum::Size) 0);
      TS_ASSERT(bucket->bucketChanged());
      TS_GUM_ASSERT_THROWS_NOTHING(bucket->compute());
      TS_ASSERT(! bucket->bucketChanged());

      if (inst != 0) {
        TS_ASSERT(!inst->isMaster(bucket));
        TS_ASSERT(inst->isMaster(bucket->getMasterRef()));
        for (inst->setFirst(); ! inst->end(); inst->inc()) {
          TS_ASSERT_DELTA(bucket->get(*inst), product.get(*inst), (double) 0.01);
        }
      }

    }
};

