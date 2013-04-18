#ifndef __RATIONAL__H__
#define __RATIONAL__H__

#include <iostream>
#include <sstream>
#include <stdlib.h> // srand, rand
#include <time.h>
#include <iomanip>

namespace gum {

  template < typename GUM_SCALAR >
  class Rational {
    public :

      /**
       * Test the algorithms iters times with random doubles.
       *
       * From this, only farey should be used with doubles in [0,1].
       *
       * @param iters Number of tests of each type ( lower than 1, greater than 1 ).
       */
      static void testRationalAlgorithms ( const unsigned int & iters = 10 );

    //////////////////////////////////////////
      /// @name Real approximation by rational
    //////////////////////////////////////////
      /// @{

      /**
       * @warning Prefer farey, much more precise
       *
       * Find the rational close enough to a given ( decimal ) number. Works with integers only so a little work is needed before calling this function.
       *
       * @param numerator The numerator of the rational.
       * @param denominator The denominator of the rational.
       * @param alpha_num The number we want to approximate expressed as an integer, i.e. \f$ alpha\_num = number * denum \f$.
       * @param d_num The number of decimals used.
       * @param denum The precision with which we multiplied the integer to approximate.
       *
       * E.g. : number = 0.2222...
       *
       * 1. Choose denum >= 10, high enough like denum = 1000 
       *
       * 2. Which gives us alpha_num = (int) number * denum = 222 
       *
       * 3. From which we get d_num = 3 since we used 3 decimals
       *
       * 4. fracC( numerator, denominator, (int)( number * denum ), d_num, denum );
       */
			static void fracC ( long int & numerator, long int & denominator, const int & alpha_num, const int & d_num, const int & denum );


      /**
       * @warning Prefer farey, much more precise
       *
       * Find the rational close enough to a given ( decimal ) number.
       *
       * @param numerator The numerator of the rational.
       * @param denominator The denominator of the rational.
       * @param number The constant number we want to approximate using rationals.
       */
			static void frac ( long int & numerator, long int & denominator, const GUM_SCALAR & number );


      /**
       * Find the rational close enough to a given ( decimal ) number in [0,1] and whose denominator is not higher than a given integer number.
       *
       * @param numerator The numerator of the rational.
       * @param denominator The denominator of the rational.
       * @param number The constant number we want to approximate using rationals.
       * @param den_max The constant highest authorized denominator.
       * @param zero The positive value below which a number is considered zero. 1e-6 by default.
       */
      static void farey ( long int & numerator, long int & denominator, const GUM_SCALAR & number, const long int & den_max = 1000000, const double & zero = 1e-6 );

      /// @}
      
    private :
    //////////////////////////////////////////
      /// @name fracC intermediate functions
    //////////////////////////////////////////
      /// @{

      /** @brief Used by fracC */
      static bool __less ( const double & a, const double & b, const int & c, const int & d );

      /** @brief Used by fracC */
      static bool __leq ( const double & a, const double & b, const int & c, const int & d );

      /** @brief Used by fracC */
      static int __matches ( const double & a, const double & b, const int & alpha_num, const int & d_num, const int & denum );

      /** @brief Used by fracC */
			static void __find_exact_left ( const double & p_a, const double & q_a, const double & p_b, const double & q_b, long int & num, long int & den, const int & alpha_num, const int & d_num, const int & denum );

      /** @brief Used by fracC */
			static void __find_exact_right ( const double & p_a, const double & q_a, const double & p_b, const double & q_b, long int & num, long int & den, const int & alpha_num, const int & d_num, const int & denum );

      /// @}

  };

} // end of gum namespace

#include <agrum/core/Rational.tcc>

#endif
