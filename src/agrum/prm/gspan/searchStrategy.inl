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
/**
 * @file
 * @brief Inline implementation of the SearchStrategy class.
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {
namespace gspan {

// The SearchStrategy class
INLINE
SearchStrategy::SearchStrategy() {
  GUM_CONSTRUCTOR(SearchStrategy);
}

INLINE
SearchStrategy::~SearchStrategy() {
  GUM_DESTRUCTOR(SearchStrategy);
}

// The FrequenceSearch class
INLINE
FrequenceSearch::FrequenceSearch(Size freq):
  SearchStrategy(), __freq(freq)
{
  GUM_CONSTRUCTOR(FrequenceSearch);
}

INLINE
FrequenceSearch::~FrequenceSearch() {
  GUM_DESTRUCTOR(FrequenceSearch);
}

INLINE
bool
FrequenceSearch::accept_growth(const Pattern* parent,
                               const Pattern* child,
                               const DFSTree::PatternData* data)
{
  return data->max_indep_set.size() >= __freq;
}

// The StrictSearch class
INLINE
StrictSearch::StrictSearch(DFSTree* tree):
  SearchStrategy(), __tree(tree)
{
  GUM_CONSTRUCTOR(StrictSearch);
}

INLINE
StrictSearch::~StrictSearch() {
  GUM_DESTRUCTOR(StrictSearch);
}

INLINE
bool
StrictSearch::accept_growth(const Pattern* parent,
                            const Pattern* child,
                            const DFSTree::PatternData* data)
{
  // double prnt_cost = std::log(growth->count()) +
  //  std::log(__tree->cost(*parent) + growth->edge->tree_width);
  // return prnt_cost >= std::log(__tree->cost(*child));
  return false;
}

} /* namespace gspan */
} /* namespace prm */
} /* namespace gum */
// ============================================================================
