#ifndef __POW__H__
#define __POW__H__

namespace gum {
  
////////////////////////////////////////// 
  /// @name Integers Pow utility methods
//////////////////////////////////////////
  /// @{

  /**
   * Specialized pow function with integers (faster implementation).
   * @param base The constant integer base used to compute \f$ base^{exponent} \f$.
   * @param exponent The integer exponent used which will hold the result afterward.
   */
  inline void intPow ( const Size & base, Size & exponent ) {
    if ( exponent == 0 ) {
      exponent = 1;
      return;
    }

    int out = base;

    for ( Size i = 1; i < exponent; i++ )
      out *= base;

    exponent = out;
  }

  /**
   * Specialized base 2 pow function with integer.
   *
   * @param exponent The integer exponent used to compute \f$ 2^{exponent} \f$ which will hold the result of afterward.
   */
  inline void int2Pow ( Size & exponent ) {
    int base = 1;
    base <<= ( exponent /*- 1*/ );
    exponent = base;
  }

  /**
   * Given an integer, compute it's - superior - and closest power of two, i.e. the number of bits necessary to represent this integer as well as the maximum integer that can be represented by those bits.
   *
   * @param card The constant choosen integer we wish to represent by bits.
   * @param num_bits The integer used as a "return" value to get the minimum number of bits used to represend card.
   * @param new_card The integer used as a "return" value to get the maximum number those bits can represent, i.e. \f$ 2^{num\_bits} \f$.
   */
  inline void superiorPow ( const Size & card, Size & num_bits, Size & new_card ) {
    if ( card <= 0 ) {
      num_bits = 0;
      new_card = 1;
      return;
    }

    num_bits = 1;
    new_card = 2;

    while ( new_card < card ) {
      new_card *= 2;
      num_bits++;
    }
  }

  /// @}

} // end of gum namespace

#endif
