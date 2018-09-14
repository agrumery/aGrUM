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
 * @brief the beginning of the CTable for the fNML score
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_SCORE_FNML_CTABLE_H
#define GUM_LEARNING_SCORE_FNML_CTABLE_H

#include <cstddef>

namespace gum {

  namespace learning {

    // the CTable for log2(C_n^r), with n in {0,...,999} and r in {2,3,4,5}
    extern const double ScorefNMLCTable[4][1000];

    // the size in r of ScorefNMLCTable
    extern const std::size_t ScorefNMLCTableRSize;

    // the size in n of ScorefNMLCTable
    extern const std::size_t ScorefNMLCTableNSize;

  } /* namespace learning */


} /* namespace gum */


#endif /* GUM_LEARNING_SCORE_FNML_CTABLE_H */
