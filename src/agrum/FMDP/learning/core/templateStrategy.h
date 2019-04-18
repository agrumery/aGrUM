
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_TEMPLATE_STRATEGY_H
#define GUM_TEMPLATE_STRATEGY_H

namespace gum {

  template < int v >
  struct Int2Type {
    enum { value = v };
  };

  enum TESTNAME { GTEST = 1, CHI2TEST = 2, LEASTSQUARETEST = 3 };
  template < TESTNAME, class A, class B, class C >
  struct TestSelect {
    typedef A type;
  };
  template < class A, class B, class C >
  struct TestSelect< CHI2TEST, A, B, C > {
    typedef B type;
  };
  template < class A, class B, class C >
  struct TestSelect< LEASTSQUARETEST, A, B, C > {
    typedef C type;
  };

  template < bool, class A, class B >
  struct ValueSelect {
    typedef A type;
  };
  template < class A, class B >
  struct ValueSelect< false, A, B > {
    typedef B type;
  };

  enum LEARNERNAME { IMDDILEARNER = 1, ITILEARNER = 2 };
  template < LEARNERNAME, class A, class B >
  struct LearnerSelect {
    typedef A type;
  };
  template < class A, class B >
  struct LearnerSelect< ITILEARNER, A, B > {
    typedef B type;
  };

}   // end of namespace gum

#endif   // GUM_TEMPLATE_STRATEGY_H
