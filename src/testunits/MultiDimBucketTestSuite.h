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
#include <agrum/multidim/multiDimBucket.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/instantiation.h>

class MultiDimBucketTestSuite: public CxxTest::TestSuite {
  private:
    std::vector<gum::LabelizedVariable*>* __variables;
    std::vector< gum::Potential<double>* >* __potentials;

  public:
    void setUp() {
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
      // Creating a table of 2 elements
      __potentials->at(1)->add(*(__variables->at(1)));
      // Creating a table of 2^4=16 elements
      for (size_t i = 2; i < 6; ++i) {
        __potentials->at(2)->add(*(__variables->at(i)));
      }
      // Creatinh a table of 2^4=16 elements
      for (size_t i = 4; i < 8; ++i) {
        __potentials->at(3)->add(*(__variables->at(i)));
      }
      // Creatinh a table of 2^4=16 elements
      for (size_t i = 6; i < 10; ++i) {
        __potentials->at(4)->add(*(__variables->at(i)));
      }
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
      }
    }
};

