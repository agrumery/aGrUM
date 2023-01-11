/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/**
 * @file
 * @brief Template trick for efficient development
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#ifndef GUM_TEMPLATE_STRATEGY_H
#define GUM_TEMPLATE_STRATEGY_H

namespace gum {

  template < int v >
  struct Int2Type {
    enum {
      value = v
    };
  };

  enum TESTNAME {
    GTEST           = 1,
    CHI2TEST        = 2,
    LEASTSQUARETEST = 3
  };
  template < TESTNAME, class A, class B, class C >
  struct TestSelect {
    using type = A;
  };
  template < class A, class B, class C >
  struct TestSelect< CHI2TEST, A, B, C > {
    using type = B;
  };
  template < class A, class B, class C >
  struct TestSelect< LEASTSQUARETEST, A, B, C > {
    using type = C;
  };

  template < bool, class A, class B >
  struct ValueSelect {
    using type = A;
  };
  template < class A, class B >
  struct ValueSelect< false, A, B > {
    using type = B;
  };

  enum LEARNERNAME {
    IMDDILEARNER = 1,
    ITILEARNER   = 2
  };
  template < LEARNERNAME, class A, class B >
  struct LearnerSelect {
    using type = A;
  };
  template < class A, class B >
  struct LearnerSelect< ITILEARNER, A, B > {
    using type = B;
  };

}   // end of namespace gum

#endif   // GUM_TEMPLATE_STRATEGY_H
