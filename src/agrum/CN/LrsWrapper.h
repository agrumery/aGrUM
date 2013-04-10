#ifndef __LRS_WRAPPER__H__
#define __LRS_WRAPPER__H__

#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <vector>

#include <agrum/core/exceptions.h>
#include <agrum/core/Rational.h>

// lrs stuff
//we force MP (not long or GMP)
#define MP
#undef long
#undef GMP
#include <agrum/CN/lrslib/lrslib.h>

/**
 * Polytopes functions using Lexicographic Reverse Search by David Avis.
 */

#define enumStringify( name ) # name

namespace gum {
  namespace credal {

    template < typename GUM_SCALAR >
    class LRS {
      private :
        typedef typename std::vector< std::vector< GUM_SCALAR > > matrix;
        matrix __input;
        matrix __output;

        int __card;

        std::vector< GUM_SCALAR > __vertex;

        enum __states {
          none,
          Hup,
          Vup,
          H2Vready,
          V2Hready
        };

        __states __state;

        // to print enum text and not value with GUM_ERROR
        const char * __setUpStateNames[ 5 ] = {
          enumStringify ( none ),
          enumStringify ( Hup ),
          enumStringify ( Vup ),
          enumStringify ( H2Vready ),
          enumStringify ( V2Hready ),
        };

        mutable int __oldCout;

        void __coutOff () const;
        void __coutOn () const;

        void __fill ( lrs_dic * P, lrs_dat * Q ) const;

      public :
    //////////////////////////////////////////
      /// @name Constructors / Destructors
    //////////////////////////////////////////
      /// @{

        /**
         * Default Constructor.
         */
        LRS ();

        /**
         * Default Destructor.
         */
        ~LRS ();

      /// @}
      
    //////////////////////////////////////////
      /// @name setUp / tearDown
    //////////////////////////////////////////
      /// @{

        void setUpH ( const int & card );

        //void setUpV ( const int & card );

        void tearDown ();

        /// @}

        /**
         * Build H-representation
         */
        void fillH ( const GUM_SCALAR & min, const GUM_SCALAR & max, const int & modal );

        /**
         * H-representation to V-representation.
         *
         * Computes the V-representation of a credal set, i.e. it's vertices, from it's H-representation, the hyper-plan inequalities i.e. it's interval specificaton. Uses lrs.
         *
         */
        matrix H2V ();


        /**
         * V-representation to H-representation.
         *
         */


        /**
         * V-Redundancy elimination.
         */


        /**
         * H-redundancy elimination.
         *
         */


        /**
         * Polytope volume from V-representation.
         *
         */

    };

  } // end of credal namespace
} // end of gum namespace

#include<agrum/CN/LrsWrapper.tcc>

#endif
