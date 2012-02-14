/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
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
#include <cmath>
#include <agrum/config.h>
#include <agrum/learning/learningTools.h>

#ifdef GUM_NO_INLINE
#include <agrum/learning/learningTools.inl>
#endif /* GUM_NO_INLINE */


namespace gum {


#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // constants used by Gary Perlman for his code for computing chi2 critical values
#define GUM_Z_MAX       6.0       /* maximum meaningful z value */
#define GUM_CHI_EPSILON 0.000001  /* accuracy of critchi approximation */
#define GUM_CHI_MAX     99999.0   /* maximum chi square value */
#define GUM_LOG_SQRT_PI 0.5723649429247000870717135 /* log (sqrt (pi)) */
#define GUM_I_SQRT_PI   0.5641895835477562869480795 /* 1 / sqrt (pi) */
#define GUM_BIGX        20.0      /* max value to represent exp (x) */
#define gum__ex(x) (((x) < -GUM_BIGX) ? 0.0 : exp (x))



  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                   GUM_LEARN_COUNTING IMPLEMENTATION                  === */
  /* ============================================================================ */
  /* ============================================================================ */

  // ==============================================================================
  /// basic constructor
  // ==============================================================================
  LearnCounting::LearnCounting( const Database& data ) :
      database( data ), level_beg( data.nbrNodes() ),
      level_end( data.nbrNodes() ),
      level_row( data.nbrNodes() ), tree_level( 0 ), garbage( 0 ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( LearnCounting );

    if ( database.hasMissingValue() ) {
      GUM_ERROR( OperationNotAllowed,"Learning Tools do not handle missing values (for now)" );
    }
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  LearnCounting::~LearnCounting() {
    // for debugging purposes
    GUM_DESTRUCTOR( LearnCounting );
    // put all the tree boxes into the garbage
    clear();
    // remove all the boxes of the garbage

    for ( CountingBox *ptr = garbage, *nextptr = 0; ptr; ptr = nextptr ) {
      nextptr = ptr->next;
      delete ptr;
    }
  }

  // ==============================================================================
  /// adds a new level corresponding to node "row"
  // ==============================================================================
  void LearnCounting::createLevel( unsigned int row ) {
    if ( tree_level == 0 ) {
      level_beg[0] = newCountingBox( database.nbrModalities( row ) );
      level_end[0] = level_beg[0];
    } else {
      // here the already exist some parents, so, below each of them
      // we should add the appropriate boxes
      unsigned int par_modal = database.nbrModalities( level_row[tree_level-1] );
      unsigned int cur_modal = database.nbrModalities( row );

      for ( CountingBox *parent = level_beg[tree_level - 1], *oldbox = 0;
            parent != 0; parent = parent->next ) {
        for ( unsigned int j = 0; j != par_modal; ++j ) {
          parent->children[j] = newCountingBox( cur_modal );

          if ( oldbox ) oldbox->next = parent->children[j];

          oldbox = parent->children[j];
        }
      }

      level_beg[tree_level] = level_beg[tree_level - 1]->children[0];

      level_end[tree_level] = level_end[tree_level - 1]->children[par_modal - 1];
    }

    level_row[tree_level] = row;

    ++tree_level;
  }

  // ==============================================================================
  /// adds a new already constructed level to the tree
  // ==============================================================================
  void LearnCounting::insertLevel( CountingBox* new_level_beg,
                                   CountingBox* new_level_end,
                                   unsigned int    new_level_row ) {
    level_beg[tree_level] = new_level_beg;
    level_end[tree_level] = new_level_end;
    level_row[tree_level] = new_level_row;

    if ( tree_level ) {
      unsigned int par_modal = database.nbrModalities( level_row[tree_level-1] );

      for ( CountingBox *parent = level_beg[tree_level - 1],
            *cur = level_beg[tree_level]; parent != 0; parent = parent->next ) {
        for ( unsigned int j = 0; j != par_modal; ++j, cur = cur->next ) {
          parent->children[j] = cur;
        }
      }
    }

    ++tree_level;
  }

  // ==============================================================================
  /// returns the row corresponding to a given level
  // ==============================================================================
  unsigned int LearnCounting::getRow( unsigned int level )
  const  {
    if ( level >= tree_level ) {
      GUM_ERROR( NotFound, "the level does not exist" );
    }

    return level_row[level];
  }





  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                  GUM_LEARN_CHI2_TEST IMPLEMENTATION                  === */
  /* ============================================================================ */
  /* ============================================================================ */

  // ==============================================================================
  /// basic constructor
  // ==============================================================================
  LearnChi2Test::LearnChi2Test( const Database& data, float conf_proba ) :
      database( data ), tree( data ), has_chi2_value( false ),
      has_chi2_threshold( false ),
      has_degrees_of_freedom( false ), confidence_proba( conf_proba ),
      min_parsing_level( 0 ), require_database_parsing( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( LearnChi2Test );
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  LearnChi2Test::~LearnChi2Test() {
    // for debugging purposes
    GUM_DESTRUCTOR( LearnChi2Test );
  }

  // ==============================================================================
  /// computes the Chi2 value of the current tree
  // ==============================================================================
  float LearnChi2Test::getChi2Value() {
    // check if the chi2 value has already been computed
    if ( has_chi2_value ) return chi2_value;

    // check if the tree requires a new database parsing
    if ( require_database_parsing ) {
      tree.parseDatabase( min_parsing_level, rows.size() - 1 );
      require_database_parsing = false;
      min_parsing_level = rows.size();
    }

    // here we need computing the chi2 value as the one stored into the structure
    // is incorrect
    // idea: parse the LearnCounting structure 3 last levels. Those contain
    // Nz, Nxz and Nxyz respectively and we want to compute:
    // sum_x sum_y sum_z (Nxyz - Nxz * Nyz / Nz)^2 / (Nxz Nyz / Nz)
    // if there is only two levels, then we need computing:
    // N * [[ sum_x sum_y Nxy^2 / (Nx * Ny)] - 1]
    if ( tree.getNbrLevels() == 2 ) {
      // compute the Ny's
      unsigned int modalX = database.nbrModalities( rows[0] );
      unsigned int modalY = database.nbrModalities( rows[1] );
      std::vector<int> Ny( modalY,0 );

      for ( LearnCounting::CountingBox* Nxy_box = tree.getLastLevel(); Nxy_box;
            Nxy_box = Nxy_box->getNextBox() ) {
        const std::vector<int>& Nxy = Nxy_box->getCounters();

        for ( unsigned int i = 0; i < modalY; ++i )
          Ny[i] += Nxy[i];
      }

      // parse all the boxes at the top of the tree, i.e. the Nx level
      chi2_value = 0;

      const LearnCounting::CountingBox& Nx_box = *( tree.getLevel( 0 ) );

      const std::vector<LearnCounting::CountingBox*>& Nx_box_children =
        Nx_box.getChildren();

      for ( unsigned int i = 0; i < modalX; ++i )
        if ( Nx_box_children[i] ) {
          unsigned int Nx = Nx_box.getCounter( i );
          const std::vector<int>& Nxy = Nx_box_children[i]->getCounters();

          for ( unsigned int j = 0; j< modalY; ++j ) {
            if ( Nxy[j] )
              chi2_value += (( float ) Nxy[j] * Nxy[j] ) / (( float ) Nx * Ny[j] );
          }
        }

      --chi2_value;

      chi2_value *= database.nbrLines();
    } else if ( tree.getNbrLevels() > 2 ) {
      chi2_value = 0;
      unsigned int modalZ = database.nbrModalities( rows[tree.getNbrLevels()-3] );
      unsigned int modalX = database.nbrModalities( rows[tree.getNbrLevels()-2] );
      unsigned int modalY = database.nbrModalities( rows[tree.getNbrLevels()-1] );
      // parse the Nz level

      for ( LearnCounting::CountingBox* Nz_box =
              tree.getLevel( tree.getNbrLevels() - 3 );
            Nz_box; Nz_box = Nz_box->getNextBox() ) {
        const std::vector<LearnCounting::CountingBox*>& Nz_box_children =
          Nz_box->getChildren();

        for ( unsigned int i = 0; i<modalZ; ++i ) {
          // here we are on a given value of Z
          if ( Nz_box_children[i] ) {
            unsigned int Nz = Nz_box->getCounter( i );

            if ( Nz ) {
              // get each box at the Nxz level
              const LearnCounting::CountingBox& Nxz_box = *Nz_box_children[i];
              // compute for this Z, the value of Nyz
              std::vector<int> Nyz( modalY,0 );
              const std::vector<LearnCounting::CountingBox*>& Nxz_box_children =
                Nxz_box.getChildren();

              for ( unsigned int j = 0; j < modalX; ++j )  {
                if ( Nxz_box_children[j] ) {
                  // get the counters at the Nxyz level
                  // note that Nxyz_box = *Nxz_box_children[j];
                  const std::vector<int>& Nxyz = Nxz_box_children[j]->getCounters();

                  for ( unsigned int k = 0; k < modalY; ++k )
                    Nyz[k] += Nxyz[k];
                }
              }

              // parse all the boxes at the Nxz level to update the value of the chi2
              for ( unsigned int j = 0; j < modalX; ++j )  {
                if ( Nxz_box_children[j] ) {
                  unsigned int Nxz = Nxz_box.getCounter( j );
                  const std::vector<int>& Nxyz = Nxz_box_children[j]->getCounters();

                  for ( unsigned int k = 0; k < modalY; ++k ) {
                    float N1 = (( float )Nxz * Nyz[k] ) / Nz;
                    float N2 = (( float ) Nxyz[k] - N1 );

                    if ( N1 ) chi2_value += ( N2 * N2 ) / N1;
                  }
                }
              }
            }
          }
        }
      }
    } else {
      GUM_ERROR( NotFound, "not enough variables to compute chi2" );
    }

    has_chi2_value = true;

    return chi2_value;
  }

  // sum_x sum_y sum_z (Nxyz - Nxz * Nyz / Nz)^2 / (Nxz Nyz / Nz)
  // ==============================================================================
  /// returns the number of degrees of freedom
  // ==============================================================================
  unsigned long LearnChi2Test::getChi2DegreesOfFreedom() {
    // check if we have already computed the degrees of freedom
    if ( has_degrees_of_freedom ) return degrees_of_freedom;

    // if the tree has not enough levels, throw an exception
    if ( tree.getNbrLevels() < 2 ) {
      GUM_ERROR( NotFound, "not enough variables to compute chi2's DF" );
    }

    // compute from the tree the number of degrees of freedom
    degrees_of_freedom = 1;

    for ( unsigned int i = 0; i + 2 < rows.size(); ++i ) {
      degrees_of_freedom *= database.nbrModalities( rows[i] );
    }

    degrees_of_freedom *= ( database.nbrModalities( rows[rows.size() - 2] ) - 1 );

    degrees_of_freedom *= ( database.nbrModalities( rows[rows.size() - 1] ) - 1 );
    has_degrees_of_freedom = true;
    return degrees_of_freedom;
  }

  // ==============================================================================
  /// computes the thresholdaccording to the number of degrees of freedom
  // ==============================================================================
  float LearnChi2Test::getChi2Threshold() {
    // check if we have already computed the threshold of the chi2
    if ( has_chi2_threshold ) return chi2_threshold;

    // try to see if the threshold is not already in cache
    unsigned long DF = getChi2DegreesOfFreedom();

    if ( chi2_thresholds.size() <= DF ) {
      chi2_thresholds.resize( DF+1 );
      has_chi2_thresholds.resize( DF+1,false );
    }

    if ( has_chi2_thresholds[DF] )
      return chi2_thresholds[DF];

    // here we have to compute the theshold of the chi2
    // we use Gary Perlman's algorithm
    chi2_threshold = getCriticalValue( confidence_proba, DF );

    has_chi2_threshold = true;

    chi2_thresholds[DF] = chi2_threshold;

    has_chi2_thresholds[DF] = true;

    return chi2_threshold;
  }

  // ==============================================================================
  /// modifies the confidence proba
  // ==============================================================================
  void LearnChi2Test::setConfidenceProba( float new_proba ) {
    if ( confidence_proba == new_proba ) return;

    confidence_proba = new_proba;

    has_chi2_threshold = false;

    for ( unsigned int i = 0; i < has_chi2_thresholds.size(); ++i )
      has_chi2_thresholds[i] = false;
  }

  // ==============================================================================
  /// computes the probability of normal z value (used by the cache)
  // ==============================================================================
  double LearnChi2Test::getProbaZValue( double z ) {
    double y, x, w;

    if ( z == 0.0 )
      x = 0.0;
    else {
      y = 0.5 * fabs( z );

      if ( y >= ( GUM_Z_MAX * 0.5 ) )
        x = 1.0;
      else if ( y < 1.0 ) {
        w = y*y;
        x = (((((((( 0.000124818987 * w
                     -0.001075204047 ) * w +0.005198775019 ) * w
                  -0.019198292004 ) * w +0.059054035642 ) * w
                -0.151968751364 ) * w +0.319152932694 ) * w
              -0.531923007300 ) * w +0.797884560593 ) * y * 2.0;
      } else {
        y -= 2.0;
        x = ((((((((((((( -0.000045255659 * y
                          +0.000152529290 ) * y -0.000019538132 ) * y
                       -0.000676904986 ) * y +0.001390604284 ) * y
                     -0.000794620820 ) * y -0.002034254874 ) * y
                   +0.006549791214 ) * y -0.010557625006 ) * y
                 +0.011630447319 ) * y -0.009279453341 ) * y
               +0.005353579108 ) * y -0.002141268741 ) * y
             +0.000535310849 ) * y +0.999936657524;
      }
    }

    return ( z > 0.0 ? (( x + 1.0 ) * 0.5 ) : (( 1.0 - x ) * 0.5 ) );
  }

  // ==============================================================================
  /// computes the probability of chi2 value (used by the cache)
  // ==============================================================================
  double LearnChi2Test::getProbaChi2( double x, unsigned long df ) {
    double a, y = 0, s;
    double e, c, z;
    int even; /* true if df is an even number */

    if (( x <= 0.0 ) || ( df < 1 ) )
      return ( 1.0 );

    a = 0.5 * x;

    even = ( 2*( df/2 ) ) == df;

    if ( df > 1 )
      y = gum__ex( -a );

    s = ( even ? y : ( 2.0 * getProbaZValue( -sqrt( x ) ) ) );

    if ( df > 2 ) {
      x = 0.5 * ( df - 1.0 );
      z = ( even ? 1.0 : 0.5 );

      if ( a > GUM_BIGX ) {
        e = ( even ? 0.0 : GUM_LOG_SQRT_PI );
        c = log( a );

        while ( z <= x ) {
          e = log( z ) + e;
          s += gum__ex( c*z-a-e );
          z += 1.0;
        }

        return ( s );
      } else {
        e = ( even ? 1.0 : ( GUM_I_SQRT_PI / sqrt( a ) ) );
        c = 0.0;

        while ( z <= x ) {
          e = e * ( a / z );
          c = c + e;
          z += 1.0;
        }

        return ( c * y + s );
      }
    } else
      return ( s );
  }

  // ==============================================================================
  /// computes the critical value of a given chi2 test (used by the cache)
  // ==============================================================================
  double LearnChi2Test::getCriticalValue( double proba, unsigned long df ) {
    double minchisq = 0.0;
    double maxchisq = GUM_CHI_MAX;
    double chisqval;

    if ( proba <= 0.0 )
      return ( maxchisq );
    else if ( proba >= 1.0 )
      return ( 0.0 );

    chisqval = df / sqrt( proba );   /* fair first value */

    while ( maxchisq - minchisq > GUM_CHI_EPSILON ) {
      if ( getProbaChi2( chisqval, df ) < proba )
        maxchisq = chisqval;
      else
        minchisq = chisqval;

      chisqval = ( maxchisq + minchisq ) * 0.5;
    }

    return ( chisqval );
  }

  // ==============================================================================
  /// deletes a variable from the structure containing all the variables
  /**
   * @throw NotFound
   */
  // ==============================================================================
  void LearnChi2Test::deleteVariable( unsigned int row ) {
    // look for the variable
    unsigned int i;

    for ( i = rows.size() - 1; i < rows.size(); --i )
      if ( rows[i] == row ) break;

    // check if we found the variable
    if ( i >= rows.size() ) {
      GUM_ERROR( NotFound, "could not find the variable to remove" );
    }

    // update the tree structure
    min_parsing_level = i;

    tree.deleteLevel( i );

    require_database_parsing = false;

    for ( unsigned int j = i+1; j < rows.size(); ++j ) {
      rows[j-1] = rows[j];
      tree.createLevel( rows[j] );
      require_database_parsing = true;
    }

    rows.pop_back();

    has_chi2_value = false;
    has_chi2_threshold = false;
    has_degrees_of_freedom = false;
  }

  // ==============================================================================
  /// inserts a new variable at the end of the tree
  // ==============================================================================
  void LearnChi2Test::insertVariable( unsigned int row ) {
    require_database_parsing = true;
    rows.push_back( row );
    tree.createLevel( row );
    has_chi2_value = false;
    has_chi2_threshold = false;
    has_degrees_of_freedom = false;
  }

  // ==============================================================================
  /// returns the level in the tree of a given variable
  // ==============================================================================
  unsigned int LearnChi2Test::getLevel( unsigned int row )
  const  {
    for ( unsigned int i = 0; i < rows.size(); ++i )
      if ( rows[i] == row ) return i;

    GUM_ERROR( NotFound, "the row does not belong to the tree" );
  }

  // ==============================================================================
  /// returns the variable (its row in the database) of a given tree level
  // ==============================================================================
  unsigned int LearnChi2Test::variable( unsigned int level )
  const  {
    if ( level < rows.size() )
      return rows[level];

    GUM_ERROR( NotFound, "the tree does not contain this level" );
  }

  // ==============================================================================
  /// removes all the variables from the current structure
  // ==============================================================================
  void LearnChi2Test::clear() {
    rows.clear();
    tree.clear();
    has_chi2_value = false;
    has_chi2_threshold = false;
    has_degrees_of_freedom = false;
    min_parsing_level = 0;
    require_database_parsing = false;
  }

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


} /* namespace gum */

