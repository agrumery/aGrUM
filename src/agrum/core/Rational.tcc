#include "Rational.h"

namespace gum {

  template < typename GUM_SCALAR >
  void Rational< GUM_SCALAR >::testRationalAlgorithms ( const unsigned int & iters ) {
    std::cout << " TESTING RATIONAL ALGORITHMS " << std::endl;
    
    srand ( time ( nullptr ) );

		std::cout << std::setw(10) << std::left << "iter" << std::setw(20) << std::left << "number" << std::setw(35) << std::left << "frac" << std::setw(35) << std::left << "fracC" << std::setw(35) << std::left << "farey" << std::setw(35) << std::left << "continued" << std::endl;

    double zero = 1e-6;
    double PRECISION = 1000000;
    double DECI = 6;
    double den_max = 1000000000;

		long int num, den;
    GUM_SCALAR number;
    std::stringstream ss;

    for ( unsigned int i = 0; i < iters; i++ ) {
      std::cout << std::setw(10) << std::left << i;
      number = (GUM_SCALAR) rand() / (GUM_SCALAR) RAND_MAX;
      std::cout << std::setw(20) << std::left << number;

      frac ( num, den, number );
      ss << num << "/" << den << " : " << (double)num/den;
      std::cout << std::setw(35) << std::left << ss.str();
      ss.str( std::string() );

      fracC ( num, den, ( int ) ( PRECISION * number ), DECI, PRECISION );
      ss << num << "/" << den << " : " << (double)num/den;
      std::cout << std::setw(35) << std::left << ss.str();
      ss.str( std::string() );

      farey ( num, den, number, den_max, zero );
      ss << num << "/" << den << " : " << (double)num/den;
      std::cout << std::setw(35) << std::left << ss.str();
      ss.str( std::string() );
			
			continuedFrac ( num, den, number, den_max, zero );
			ss << num << "/" << den << " : " << (double)num/den;
			std::cout << std::setw(35) << std::left << ss.str() << std::endl;
			ss.str( std::string() );
    }

    std::cout << std::endl;

    // test 0 and 1 !

    std::cout << std::setw(10) << std::left << iters;
    number = 0;
    std::cout << std::setw(20) << std::left << number;
    frac ( num, den, number );
    ss << num << "/" << den << " : " << (double)num/den;
    std::cout << std::setw(35) << std::left << ss.str();
    ss.str( std::string() );

    fracC ( num, den, ( int ) ( PRECISION * number ), DECI, PRECISION );
    ss << num << "/" << den << " : " << (double)num/den;
    std::cout << std::setw(35) << std::left << ss.str();
    ss.str( std::string() );

    farey ( num, den, number, den_max, zero );
    ss << num << "/" << den << " : " << (double)num/den;
    std::cout << std::setw(35) << std::left << ss.str();
    ss.str( std::string() );
		
		continuedFrac ( num, den, number, den_max, zero );
		ss << num << "/" << den << " : " << (double)num/den;
		std::cout << std::setw(35) << std::left << ss.str() << std::endl;
		ss.str( std::string() );
		//////////////////////////////////////////////////////////////////

    std::cout << std::setw(10) << std::left << iters+1;
    number = 1;
    std::cout << std::setw(20) << std::left << number;
    frac ( num, den, number );
    ss << num << "/" << den << " : " << (double)num/den;
    std::cout << std::setw(35) << std::left << ss.str();
    ss.str( std::string() );

    fracC ( num, den, ( int ) ( PRECISION * number ), DECI, PRECISION );
    ss << num << "/" << den << " : " << (double)num/den;
    std::cout << std::setw(35) << std::left << ss.str();
    ss.str( std::string() );

    farey ( num, den, number, den_max, zero );
    ss << num << "/" << den << " : " << (double)num/den;
    std::cout << std::setw(35) << std::left << ss.str();
    ss.str( std::string() );
		
		continuedFrac ( num, den, number, den_max, zero );
		ss << num << "/" << den << " : " << (double)num/den;
		std::cout << std::setw(35) << std::left << ss.str() << std::endl;
		ss.str( std::string() );

    std::cout << std::endl;
		
		/////////////////////////////////////////////////////////////////////////

    // 1 <= number <= 100 ( don't call farey, it will go through all farey rationals until den_max is met, which is 1000000 )
    double fmin = 1.0, fmax = 100.0;
    for ( unsigned int i = iters+2; i < iters*2+2; i++ ) {
      std::cout << std::setw(10) << std::left << i;
      number = (GUM_SCALAR) rand() / RAND_MAX;
      number = fmin + number * ( fmax - fmin );
      std::cout << std::setw(20) << std::left << number;

      frac ( num, den, number );
      ss << num << "/" << den << " : " << (double)num/den;
      std::cout << std::setw(35) << std::left << ss.str();
      ss.str( std::string() );

      fracC ( num, den, ( int ) ( PRECISION * number ), DECI, PRECISION );
      ss << num << "/" << den << " : " << (double)num/den;
      std::cout << std::setw(35) << std::left << ss.str();
      ss.str( std::string() );

			// farey
      std::cout << std::setw(35) << std::left << "1/1, number > 1 !";    
			
			continuedFrac ( num, den, number, den_max, zero );
			ss << num << "/" << den << " : " << (double)num/den;
			std::cout << std::setw(35) << std::left << ss.str() << std::endl;
			ss.str( std::string() );
		}

  }

  template < typename GUM_SCALAR >
  bool Rational< GUM_SCALAR >::__less ( const double & a, const double & b, const long int & c, const long int & denum ) {
    return ( a * denum < b * c );
  }

  template < typename GUM_SCALAR >
  bool Rational< GUM_SCALAR >::__leq ( const double & a, const double & b, const long int & c, const long int & denum ) {
    return ( a * denum <= b * c );
  }

  template < typename GUM_SCALAR >
  int Rational< GUM_SCALAR >::__matches ( const double & a, const double & b, const long int & alpha_num, const long int & d_num, const long int & denum ) {
    if ( __leq ( a, b, alpha_num - d_num, denum ) ) return 0;

    if ( __less ( a, b, alpha_num + d_num, denum ) ) return 1;

    return 0;
  }

  template < typename GUM_SCALAR >
  void Rational< GUM_SCALAR >::__find_exact_left ( const double & p_a, const double & q_a, const double & p_b, const double & q_b, long int & num, long int & den, const long int & alpha_num, const long int & d_num, const long int & denum ) {
    double k_num = denum * p_b - ( alpha_num + d_num ) * q_b;
    double k_denum = ( alpha_num + d_num ) * q_a - denum * p_a;
    double k = ( ( double ) k_num / k_denum ) + 1;
    num = p_b + k * p_a;
    den = q_b + k * q_a;
  }

  template < typename GUM_SCALAR >
  void Rational< GUM_SCALAR >::__find_exact_right ( const double & p_a, const double & q_a, const double & p_b, const double & q_b, long int & num, long int & den, const long int & alpha_num, const long int & d_num, const long int & denum ) {
    double k_num = -denum * p_b - ( alpha_num - d_num ) * q_b;
    double k_denum = - ( alpha_num - d_num ) * q_a + denum * p_a;
    double k = ( ( double ) k_num / k_denum ) + 1;
    num = p_b + k * p_a;
    den = q_b + k * q_a;
  }

  template < typename GUM_SCALAR >
  void Rational< GUM_SCALAR >::fracC ( long int & numerator, long int & denominator, const long int & alpha_num, const long int & d_num, const long int & denum ) {
    double p_a = 0;
    double q_a = 1;
    double p_b = 1;
    double q_b = 1;

    double x, x_num, x_denum, new_p_a, new_q_a, new_p_b, new_q_b;
    int aa, bb;

    while ( true ) {
      x_num = denum * p_b - alpha_num * q_b;
      x_denum = -denum * p_a + alpha_num * q_a;
      x = ( int ) ( ( x_num + x_denum - 1 ) / x_denum );

      aa = __matches ( p_b + x * p_a, q_b + x * q_a, alpha_num, d_num, denum );
      bb = __matches ( p_b + ( x - 1 ) * p_a, q_b + ( x - 1 ) * q_a, alpha_num, d_num, denum );

      if ( aa || bb ) {
        __find_exact_left ( p_a, q_a, p_b, q_b, numerator, denominator, alpha_num, d_num, denum );
        break;
      }

      new_p_a = p_b + ( x - 1 ) * p_a;
      new_q_a = q_b + ( x - 1 ) * q_a;
      new_p_b = p_b + x * p_a;
      new_q_b = q_b + x * q_a;

      p_a = new_p_a;
      p_b = new_p_b;
      q_a = new_q_a;
      q_b = new_q_b;

      x_num = alpha_num * q_b - denum * p_b;
      x_denum = -alpha_num * q_a + denum * p_a;
      x = ( int ) ( ( x_num + x_denum - 1 ) / x_denum );

      aa = __matches ( p_b + x * p_a, q_b + x * q_a, alpha_num, d_num, denum );
      bb = __matches ( p_b + ( x - 1 ) * p_a, q_b + ( x - 1 ) * q_a, alpha_num, d_num, denum );

      if ( aa || bb ) {
        __find_exact_right ( p_a, q_a, p_b, q_b, numerator, denominator, alpha_num, d_num, denum );
        break;
      }

      new_p_a = p_b + ( x - 1 ) * p_a;
      new_q_a = q_b + ( x - 1 ) * q_a;
      new_p_b = p_b + x * p_a;
      new_q_b = q_b + x * q_a;

      p_a = new_p_a;
      p_b = new_p_b;
      q_a = new_q_a;
      q_b = new_q_b;
    }
  } // end of continued frac


  template < typename GUM_SCALAR >
  void Rational< GUM_SCALAR >::frac ( long int & numerator, long int & denominator, const GUM_SCALAR & number ) {
    double l = log10 ( abs ( number ) );

    int d1 = l + 1.000001;
    int d2 = ( d1 <= 0 ) ? 4 : 4 - d1;

    double dd = pow ( 10, d2 );
		long int di = dd;

    if ( di < dd )
      di++;

    double nd = number * di;
		long int ni = nd;

    if ( ni < nd )
      ni++;

    numerator = ni;
    denominator = di;

		long int a ( numerator ), b ( denominator ), t;

    while ( b != 0 ) {
      t = b;
      b = a % b;
      a = t;
    }

    numerator /= a;
    denominator /= a;
  } // end of frac

  template < typename GUM_SCALAR >
  void Rational< GUM_SCALAR >::farey ( long int & numerator, long int & denominator, const GUM_SCALAR & number, const long int & den_max, const double & zero ) {
    bool isNegative = ( number < 0 ) ? true : false;
    GUM_SCALAR pnumber = ( isNegative ) ? - number : number;

    if ( fabs ( pnumber - 1. ) < zero ) {
      numerator = ( isNegative ) ? - 1 : 1;
      denominator = 1;
      return;
    } else if ( pnumber < zero ) {
      numerator = 0;
      denominator = 1;
      return;
    }

    int a ( 0 ), b ( 1 ), c ( 1 ), d ( 1 );
    double mediant ( 0.0F );

    while ( b <= den_max && d <= den_max ) {
      mediant = ( double ) ( a + c ) / ( double ) ( b + d );

      if ( fabs ( pnumber - mediant ) < zero )  {
        if ( b + d <= den_max )  {
          numerator = ( isNegative ) ? - ( a + c ) : ( a + c );
          denominator = b + d;
          return;
        } 
        else if ( d > b )  {
          numerator = ( isNegative ) ? - c : c;
          denominator = d;
          return;
        } 
        else {
          numerator = ( isNegative ) ? - a : a;
          denominator = b;
          return;
        }
      }
      else if ( pnumber > mediant ) {
        a = a + c;
        b = b + d;
      } 
      else {
        c = a + c;
        d = b + d;
      }
    }

    if ( b > den_max ) {
      numerator = ( isNegative ) ? - c : c;
      denominator = d;
      return;
    } 
    else {
      numerator = ( isNegative ) ? - a : a;
      denominator = b;
      return;
    }
  } // end of farey func
  

  template < typename GUM_SCALAR >
  void Rational< GUM_SCALAR >::continuedFrac( long int & numerator, long int & denominator, const GUM_SCALAR & number, const long int & den_max, const double & zero ) {
		const GUM_SCALAR pnumber = ( number > 0 ) ? number : - number;
		
		// reciprocal over iterations
		GUM_SCALAR rnumber = pnumber;
		
		// a is the interger part of the ( current ) reciprocal
		// numerator_old / denominator_old are the previous numerator / denominator of the convergents
		// tmp is a temporary
		unsigned long int numerator_old, denominator_old, a, tmp;
		
		// first convergent
		numerator_old = 0; numerator = 1;
		// second convergent
		denominator_old = 1; denominator = 0;
		
		// denominators are monotonous, ascending order
		
		// continued fraction - we don't keep all of them, just the last one close enough to number
		do {
			a = lrint( floor( rnumber ) );
			
			tmp = numerator;
			numerator = a * numerator + numerator_old;
			numerator_old = tmp;
			
			tmp = denominator;
			denominator = a * denominator + denominator_old;
			denominator_old = tmp;
			
			if( fabs( rnumber - a ) < zero || denominator >= den_max || fabs( numerator * 1. / denominator - pnumber ) < zero ) {
				/// we return the last rational with denominator <= den_max
				/// knowing that fabs( pnumber - approx ) > zero
				if ( denominator > den_max ) {
					numerator = numerator_old;
					denominator = denominator_old;
				}
				
				if ( number < 0 ) {
					numerator = - numerator;
				}
				
				return;
			}
			
			rnumber = 1. / ( rnumber - a );
			
		} while ( 1 );
		
	}


} // end of gum namespace
