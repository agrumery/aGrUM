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
SearchStrategy::SearchStrategy():
  _tree(0)
{
  GUM_CONSTRUCTOR(SearchStrategy);
}

INLINE
SearchStrategy::~SearchStrategy() {
  GUM_DESTRUCTOR(SearchStrategy);
}

INLINE
void
SearchStrategy::setTree(DFSTree* tree) {
  _tree = tree;
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
                               const DFSTree::EdgeGrowth& growh)
{
  return _tree->frequency(*child) >= __freq;
}

INLINE
bool
FrequenceSearch::operator()(gspan::Pattern* i, gspan::Pattern* j) {
  // We want a descending order
  return _tree->frequency(*i) > _tree->frequency(*j);
}


// The StrictSearch class
INLINE
StrictSearch::StrictSearch():
  SearchStrategy()
{
  GUM_CONSTRUCTOR(StrictSearch);
}

INLINE
StrictSearch::~StrictSearch() {
  GUM_DESTRUCTOR(StrictSearch);
}

INLINE
double
StrictSearch::__getCost(const Pattern& p) {
  try {
    return __map[&p].first;
  } catch (NotFound&) {
    __map.insert(&p, std::make_pair((double) 0.0, (double) 0.0));
    return (double) 0.0;
  }
}

INLINE
void
StrictSearch::__setCost(const Pattern& p, double cost) {
  try {
    __map[&p].first = cost;
  } catch (NotFound&) {
    __map.insert(&p, std::make_pair(cost, (double) 0.0));
  }
}

INLINE
double
StrictSearch::__getGain(const Pattern& p) {
  try {
    return __map[&p].second;
  } catch (NotFound&) {
    __map.insert(&p, std::make_pair((double) 0.0, (double) 0.0));
    return (double) 0.0;
  }
}

INLINE
void
StrictSearch::__setGain(const Pattern& p, double gain) {
  try {
    __map[&p].second = gain;
  } catch (NotFound&) {
    __map.insert(&p, std::make_pair((double) 0.0, gain));
  }
}


INLINE
bool
StrictSearch::accept_growth(const Pattern* parent,
                            const Pattern* child,
                            const DFSTree::EdgeGrowth& growth)
{
  return _tree->frequency(*child) * (cost(*parent) + gain(*child)) > cost(*child);
}

INLINE
bool
StrictSearch::operator()(gspan::Pattern* i, gspan::Pattern* j) {
  // We want a descending order
  return _tree->frequency(*i) * gain(*i) / cost(*i) >
         _tree->frequency(*j) * gain(*j) / cost(*j);
}

} /* namespace gspan */
} /* namespace prm */
} /* namespace gum */
// ============================================================================
