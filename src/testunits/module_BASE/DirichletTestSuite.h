/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
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

#include <iostream>
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/core/math/Dirichlet.h>

namespace gum_tests {

  class DirichletTestSuite : public CxxTest::TestSuite {
    public:
    void testXX() {
      std::vector<float> param{1, 1};
      gum::Dirichlet dir(param);

      std::vector<float> res(2, 0);
      float sum = 0;
      for (unsigned int i = 0; i < 10000; ++i) {
        std::vector<float> sample = dir();
        for (unsigned int j = 0; j < sample.size(); ++j) {
          res[j] += sample[j];
          sum += sample[j];
        }
      }
      for (unsigned int j = 0; j < res.size(); ++j) {
        res[j] /= sum;
      }

      TS_ASSERT(fabs(res[0] - res[1]) < 0.03);

      std::default_random_engine gen;

      param[0] = 10000;
      param[1] = 1;
      sum = 0;
      res[0] = res[1] = 0;
      for (unsigned int i = 0; i < 10000; ++i) {
        std::vector<float> sample = dir(gen, param);
        for (unsigned int j = 0; j < sample.size(); ++j) {
          res[j] += sample[j];
          sum += sample[j];
        }
      }
      for (unsigned int j = 0; j < res.size(); ++j) {
        res[j] /= sum;
      }
      TS_ASSERT(1 - res[0] < 0.01);

      sum = 0;
      res[0] = res[1] = 0;
      for (unsigned int i = 0; i < 10000; ++i) {
        std::vector<float> sample = dir(param);
        for (unsigned int j = 0; j < sample.size(); ++j) {
          res[j] += sample[j];
          sum += sample[j];
        }
      }
      for (unsigned int j = 0; j < res.size(); ++j) {
        res[j] /= sum;
      }
      TS_ASSERT(1 - res[0] < 0.01);

      sum = 0;
      res[0] = res[1] = 0;
      for (unsigned int i = 0; i < 10000; ++i) {
        std::vector<float> sample = dir(gen, dir.param());
        for (unsigned int j = 0; j < sample.size(); ++j) {
          res[j] += sample[j];
          sum += sample[j];
        }
      }
      for (unsigned int j = 0; j < res.size(); ++j) {
        res[j] /= sum;
      }
      TS_ASSERT(fabs(res[0] - res[1]) < 0.03);
    }
  };

} /* namespace gum_tests */
