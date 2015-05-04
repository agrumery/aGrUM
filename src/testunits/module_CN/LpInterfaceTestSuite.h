/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
 *   {prenom.nom}_at_lip6.fr                                               *
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

#include <iostream>
#include <string>

#include <cstdlib> // rand

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/CN/LpInterface.h>

namespace gum_tests {

  class LpInterfacetTestSuite : public CxxTest::TestSuite {
    public:
    void testLpExpr() {
      gum::credal::lp::LpInterface<double> pl;
      auto v = pl.addCols(3);
      const gum::credal::lp::LpCol &a = v[0];

      gum::credal::lp::LpExpr expr = 2 + a <= 5;
      gum::credal::lp::LpRow row(expr, v);
    }
    void testLpCreation() {
      gum::credal::lp::LpInterface<double> pl;

      auto A = pl.addCols(3); // 3 variables for the lp
      pl.addRow(0.1 <= A[0] <= 0.7);
      pl.addRow(0.1 <= A[1] <= 0.5);
      pl.addRow(0.1 <= A[2] <= 0.6);
    }
  }; // end of class LpInterfacetTestSuite

} // end of namespace gum_tests
