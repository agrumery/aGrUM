/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
/** @file
 * @brief the class for computing Chi2 scores
 *
 * The class is only composed of an inline static method score that actually
 * computes the Chi2 score. To do so, it parses the number of occurrences
 * stored into a CountingTargetSetBox (see file countingTreeBoxes.h for
 * more details about such a box).
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


namespace gum {

  
  namespace learning {

    
    /// computes the Chi2 of (X,Y) given conditioning set Z (stored in the box)
    ALWAYS_INLINE float ScoreChi2::score ( const CountingTreeTargetSetBox& box,
                                           unsigned int x,
                                           unsigned int y,
                                           unsigned int xy ) {
      // get the number Nz of occurences of the parents
      const float Nz = box.nbParentRecords ();

      // get the numbers of occurences corresponding to #XZ, #YZ and #XYZ
      const std::vector<unsigned int>& Nx  = box.child ( x  )->nbRecords ();
      const std::vector<unsigned int>& Ny  = box.child ( y  )->nbRecords ();
      const std::vector<unsigned int>& Nxy = box.child ( xy )->nbRecords ();

      // get the domain size of x (consecutive occurences of Y in Nxy are
      // separated by |X| elements
      const unsigned int x_size = Nx.size ();
      const unsigned int y_size = Ny.size ();

      // now, perform sum_X sum_Y sum_Z ( #XYZ - #XZ * #YZ / #Z )^2 /
      // (#XZ * #YZ / #Z )
      float sum = 0;
      for ( unsigned int j = 0, k = 0; j < y_size; ++j ) {
        for ( unsigned int i = 0; i < x_size; ++i, k++ ) {
          const float tmp1 = ( Nx[i] * Ny[j] ) / Nz;
          const float tmp2 = Nxy[k] - tmp1;
          sum += ( tmp2 * tmp2 ) / tmp1;
        }
      }
      
      return sum;
    }


  } /* namespace learning */
  
  
} /* namespace gum */

