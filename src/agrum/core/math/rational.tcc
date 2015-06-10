#include "rational.h"

namespace gum {

  template <typename GUM_SCALAR>
  void Rational<GUM_SCALAR>::testRationalAlgorithms(const unsigned int &iters) {
    std::cout << " TESTING RATIONAL ALGORITHMS " << std::endl;
    std::cout.precision(10);

    std::cout << std::setw(10) << std::left << "iter" << std::setw(20) << std::left
              << "number" << std::setw(35) << std::left << "farey" << std::setw(35)
              << std::left << "continued" << std::endl;

    double zero = 1e-6;
    double PRECISION = 1000000;
    double DECI = 6;
    double den_max = 1000000000;

    long int num, den;
    GUM_SCALAR number;
    std::stringstream ss;

    for (unsigned int i = 0; i < iters; i++) {
      std::cout << std::setw(10) << std::left << i;
      number = (GUM_SCALAR)rand() / (GUM_SCALAR)RAND_MAX;
      std::cout << std::setw(20) << std::left << number;

      farey(num, den, number, den_max, zero);
      ss << num << "/" << den << " : ";
      ss.precision(10);
      ss << (double)num / den;
      std::cout << std::setw(35) << std::left << ss.str();
      ss.str(std::string());

      continuedFracFirst(num, den, number, zero);
      ss << num << "/" << den << " : ";
      ss.precision(10);
      ss << (double)num / den;
      std::cout << std::setw(35) << std::left << ss.str() << std::endl;
      ss.str(std::string());
    }

    std::cout << std::endl;

    // test 0 and 1 !

    std::cout << std::setw(10) << std::left << iters;
    number = 0;
    std::cout << std::setw(20) << std::left << number;

    farey(num, den, number, den_max, zero);
    ss << num << "/" << den << " : " << (double)num / den;
    std::cout << std::setw(35) << std::left << ss.str();
    ss.str(std::string());

    continuedFracFirst(num, den, number, zero);
    ss << num << "/" << den << " : " << (double)num / den;
    std::cout << std::setw(35) << std::left << ss.str() << std::endl;
    ss.str(std::string());
    //////////////////////////////////////////////////////////////////

    std::cout << std::setw(10) << std::left << iters + 1;
    number = 1;
    std::cout << std::setw(20) << std::left << number;

    farey(num, den, number, den_max, zero);
    ss << num << "/" << den << " : " << (double)num / den;
    std::cout << std::setw(35) << std::left << ss.str();
    ss.str(std::string());

    continuedFracFirst(num, den, number, zero);
    ss << num << "/" << den << " : " << (double)num / den;
    std::cout << std::setw(35) << std::left << ss.str() << std::endl;
    ss.str(std::string());

    std::cout << std::endl;

    /////////////////////////////////////////////////////////////////////////

    // 1 <= number <= 100 ( don't call farey, it will go through all farey rationals
    // until den_max is met, which is 1000000 )
    double fmin = 1.0, fmax = 100.0;

    for (unsigned int i = iters + 2; i < iters * 2 + 2; i++) {
      std::cout << std::setw(10) << std::left << i;
      number = (GUM_SCALAR)rand() / RAND_MAX;
      number = fmin + number * (fmax - fmin);
      std::cout << std::setw(20) << std::left << number;

      // farey
      std::cout << std::setw(35) << std::left << "1/1, number > 1 !";

      continuedFracFirst(num, den, number, zero);
      ss << num << "/" << den << " : " << (double)num / den;
      std::cout << std::setw(35) << std::left << ss.str() << std::endl;
      ss.str(std::string());
    }
  }

  template <typename GUM_SCALAR>
  void Rational<GUM_SCALAR>::farey(long int &numerator, long int &denominator,
                                   const GUM_SCALAR &number, const long int &den_max,
                                   const double &zero) {
    bool isNegative = (number < 0) ? true : false;
    GUM_SCALAR pnumber = (isNegative) ? -number : number;

    if (std::fabs(pnumber - 1.) < zero) {
      numerator = (isNegative) ? -1 : 1;
      denominator = 1;
      return;
    } else if (pnumber < zero) {
      numerator = 0;
      denominator = 1;
      return;
    }

    int a(0), b(1), c(1), d(1);
    double mediant(0.0F);

    while (b <= den_max && d <= den_max) {
      mediant = (double)(a + c) / (double)(b + d);

      if (std::fabs(pnumber - mediant) < zero) {
        if (b + d <= den_max) {
          numerator = (isNegative) ? -(a + c) : (a + c);
          denominator = b + d;
          return;
        } else if (d > b) {
          numerator = (isNegative) ? -c : c;
          denominator = d;
          return;
        } else {
          numerator = (isNegative) ? -a : a;
          denominator = b;
          return;
        }
      } else if (pnumber > mediant) {
        a = a + c;
        b = b + d;
      } else {
        c = a + c;
        d = b + d;
      }
    }

    if (b > den_max) {
      numerator = (isNegative) ? -c : c;
      denominator = d;
      return;
    } else {
      numerator = (isNegative) ? -a : a;
      denominator = b;
      return;
    }
  } /// end of farey func

  template <typename GUM_SCALAR>
  void Rational<GUM_SCALAR>::continuedFracFirst(long int &numerator,
                                                long int &denominator,
                                                const GUM_SCALAR &number,
                                                const double &zero) {
    const GUM_SCALAR pnumber = (number > 0) ? number : -number;

    /// reciprocal over iterations
    GUM_SCALAR rnumber = pnumber;

    /// convergents
    std::vector<unsigned long int> p({0, 1});
    std::vector<unsigned long int> q({1, 0});

    /// quotients
    std::vector<unsigned long int> a;

    unsigned long int p_tmp, q_tmp;

    unsigned long int n;
    double delta, delta_tmp;

    /// we find all convergents until we found a best one
    /// since we look for a delta < zero, we can start looking for semi-convergents
    /// when we found a convergent with delta < zero, and look for the
    /// semi-convergents before
    while (true) {
      a.push_back(std::lrint(std::floor(rnumber)));
      p.push_back(a.back() * p.back() + p[p.size() - 2]);
      q.push_back(a.back() * q.back() + q[q.size() - 2]);

      delta = std::fabs(pnumber - (GUM_SCALAR)p.back() / q.back());

      if (delta < zero) {
        numerator = (number > 0) ? p.back() : -p.back();
        denominator = q.back();
        break;
      }

      if (std::fabs(rnumber - a.back()) < 1e-6)
        break;

      rnumber = 1. / (rnumber - a.back());
    } /// end of while

    if (a.size() < 2)
      return;

    /// we can start looking at the semi-convergents made of the last two convergents
    /// before the one within precision zero of number found previously
    unsigned int i = p.size() - 2;
    /// the last convergent has already been computed previously : end of for is
    /// p.size() - 2
    /// for ( ; i < p.size() - 1; ++i ) {
    // Test n = a[i-1]/2 ( when a[i-1] is even )
    n = a[i - 1] / 2;
    p_tmp = n * p[i] + p[i - 1];
    q_tmp = n * q[i] + q[i - 1];

    delta = std::fabs(pnumber - ((double)p[i]) / q[i]);
    delta_tmp = std::fabs(pnumber - ((double)p_tmp) / q_tmp);

    if (delta < zero) {
      numerator = (number > 0) ? p[i] : -p[i];
      denominator = q[i];
      return;
    }

    if (delta_tmp < zero) {
      numerator = (number > 0) ? p_tmp : -p_tmp;
      denominator = q_tmp;
      return;
    }

    // next semi-convergents until next convergent from smaller denominator to bigger
    // denominator
    for (n = (a[i - 1] + 2) / 2; n < a[i - 1]; ++n) {
      p_tmp = n * p[i] + p[i - 1];
      q_tmp = n * q[i] + q[i - 1];

      delta_tmp = std::fabs(pnumber - ((double)p_tmp) / q_tmp);

      if (delta_tmp < zero) {
        numerator = (number > 0) ? p_tmp : -p_tmp;
        denominator = q_tmp;
        return;
      }
    } /// end of for

    ///} // end of for
  }

  template <typename GUM_SCALAR>
  void Rational<GUM_SCALAR>::continuedFracBest(long int &numerator,
                                               long int &denominator,
                                               const GUM_SCALAR &number,
                                               const long int &den_max) {
    const GUM_SCALAR pnumber = (number > 0) ? number : -number;

    const unsigned long int denMax =
        (unsigned long int)den_max; /// signed and unsigned comparison resolution ...

    /// reciprocal over iterations
    GUM_SCALAR rnumber = pnumber;

    /// convergents
    std::vector<unsigned long int> p({0, 1});
    std::vector<unsigned long int> q({1, 0});

    /// quotients
    std::vector<unsigned long int> a;

    unsigned long int p_tmp, q_tmp;

    unsigned long int n;
    double delta, delta_tmp;

    /// we find all convergents until we met den_max
    while (true) {
      a.push_back(std::lrint(std::floor(rnumber)));

      p_tmp = a.back() * p.back() + p[p.size() - 2];
      q_tmp = a.back() * q.back() + q[q.size() - 2];

      if (q_tmp > denMax || p_tmp > denMax)
        break;

      p.push_back(p_tmp);
      q.push_back(q_tmp);

      if (std::fabs(rnumber - a.back()) < 1e-6)
        break;

      rnumber = 1. / (rnumber - a.back());
    } /// end of while

    if (a.size() < 2 || q.back() == denMax || p.back() == denMax) {
      numerator = (number > 0) ? p.back() : -p.back();
      denominator = q.back();
      return;
    }

    /// we can start looking at the semi-convergents made of the last two convergents
    /// before the one within precision zero of number found previously
    unsigned int i = p.size() - 1;

    /// the last convergent has already been computed previously : end of for is
    /// p.size() - 2
    /// for ( ; i < p.size() - 1; ++i ) {
    for (n = a[i - 1] - 1; n >= (a[i - 1] + 2) / 2; --n) {

      p_tmp = n * p[i] + p[i - 1];
      q_tmp = n * q[i] + q[i - 1];

      if (q_tmp > denMax || p_tmp > denMax)
        continue;

      numerator = (number > 0) ? p_tmp : -p_tmp;
      denominator = q_tmp;
      return;
    } // end of for

    // Test n = a[i-1]/2
    n = a[i - 1] / 2;
    p_tmp = n * p[i] + p[i - 1];
    q_tmp = n * q[i] + q[i - 1];

    delta_tmp = std::fabs(pnumber - ((double)p_tmp) / q_tmp);
    delta = std::fabs(pnumber - ((double)p[i]) / q[i]);

    if (delta_tmp < delta && q_tmp <= denMax && p_tmp <= denMax) {
      numerator = (number > 0) ? p_tmp : -p_tmp;
      denominator = q_tmp;
    } else {
      numerator = (number > 0) ? p[i] : -p[i];
      denominator = q[i];
    }

    ///}
  }

} // end of gum namespace
