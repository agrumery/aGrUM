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
SearchStrategy::SearchStrategy(const SearchStrategy& from):
  _tree(from._tree)
{
  GUM_CONS_CPY(SearchStrategy);
}

INLINE
SearchStrategy::~SearchStrategy() {
  GUM_DESTRUCTOR(SearchStrategy);
}

INLINE
SearchStrategy&
SearchStrategy::operator=(const SearchStrategy& from) {
  _tree = from._tree;
  return *this;
}

INLINE
void
SearchStrategy::setTree(DFSTree* tree) {
  _tree = tree;
}

// ============================================================================
// FrequenceSearch
// ============================================================================

// The FrequenceSearch class
INLINE
FrequenceSearch::FrequenceSearch(Size freq):
  SearchStrategy(), __freq(freq)
{
  GUM_CONSTRUCTOR(FrequenceSearch);
}

INLINE
FrequenceSearch::FrequenceSearch(const FrequenceSearch& from):
  SearchStrategy(from), __freq(from.__freq)
{
  GUM_CONS_CPY(FrequenceSearch);
}

INLINE
FrequenceSearch::~FrequenceSearch() {
  GUM_DESTRUCTOR(FrequenceSearch);
}

INLINE
FrequenceSearch&
FrequenceSearch::operator=(const FrequenceSearch& from) {
  __freq = from.__freq;
  return *this;
}

INLINE
bool
FrequenceSearch::accept_root(const Pattern* r) {
  return _tree->frequency(*r) >= __freq;
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

INLINE
bool
FrequenceSearch::operator() ( LabelData* i, LabelData* j ) {
  return _tree->graph().size(i) > _tree->graph().size(j);
}

// ============================================================================
// StrictSearch
// ============================================================================

// The StrictSearch class
INLINE
StrictSearch::StrictSearch(Size freq):
  SearchStrategy(), __freq(freq)
{
  GUM_CONSTRUCTOR(StrictSearch);
}

INLINE
StrictSearch::StrictSearch(const StrictSearch& from):
  SearchStrategy(from), __freq(from.__freq)
{
  GUM_CONS_CPY(StrictSearch);
}

INLINE
StrictSearch::~StrictSearch() {
  GUM_DESTRUCTOR(StrictSearch);
}

INLINE
StrictSearch&
StrictSearch::operator=(const StrictSearch& from) {
  __freq = from.__freq;
  return *this;
}

INLINE
double
StrictSearch::cost(const Pattern& p) {
  try {
    return __map[&p];
  } catch (NotFound&) {
    __map.insert(&p, _computeCost(p));
    return __map[&p];
  }
}

INLINE
bool
StrictSearch::accept_root(const Pattern* r) {
  if (_tree->frequency(*r) >= __freq) {
    Size tree_width = 0;
    for (Pattern::NodeIterator n = r->beginNodes(); n != r->endNodes(); ++n)
      tree_width += r->label(*n).tree_width;
    return _tree->frequency(*r) * tree_width >= cost(*r);
  }
  return false;
}

INLINE
bool
StrictSearch::accept_growth(const Pattern* parent,
                            const Pattern* child,
                            const DFSTree::EdgeGrowth& growth)
{
  return _tree->frequency(*child) * (cost(*parent) + child->lastAdded().tree_width) >= cost(*child);
}

INLINE
bool
StrictSearch::operator()(gspan::Pattern* i, gspan::Pattern* j) {
  return _tree->frequency(*i) * cost(*i) < _tree->frequency(*j) * cost(*j);
}

INLINE
bool
StrictSearch::operator() ( LabelData* i, LabelData* j ) {
  return i->tree_width * _tree->graph().size(i) <
         j->tree_width * _tree->graph().size(j);
}

// ============================================================================
// TreeWidthSearch
// ============================================================================

INLINE
TreeWidthSearch::TreeWidthSearch():
  SearchStrategy()
{
  GUM_CONSTRUCTOR( TreeWidthSearch );
}

INLINE
TreeWidthSearch::TreeWidthSearch(const TreeWidthSearch& from):
  SearchStrategy(from)
{
  GUM_CONS_CPY(TreeWidthSearch);
}

INLINE
TreeWidthSearch::~TreeWidthSearch() {
  GUM_DESTRUCTOR( TreeWidthSearch );
}

INLINE
TreeWidthSearch&
TreeWidthSearch::operator=(const TreeWidthSearch& from) {
  return *this;
}

INLINE
double
TreeWidthSearch::cost(const Pattern& p) {
  try {
    return __map[&p];
  } catch (NotFound&) {
    __map.insert(&p, _computeCost(p));
    return __map[&p];
  }
}

INLINE
bool
TreeWidthSearch::accept_root(const Pattern* r) {
  Size tree_width = 0;
  for (Pattern::NodeIterator n = r->beginNodes(); n != r->endNodes(); ++n)
    tree_width += r->label(*n).tree_width;
  return tree_width >= cost(*r);
}

INLINE
bool
TreeWidthSearch::accept_growth(const Pattern* parent,
                               const Pattern* child,
                               const DFSTree::EdgeGrowth& growth)
{
  return cost(*parent) >= cost(*child);
}

INLINE
bool
TreeWidthSearch::operator()(gspan::Pattern* i, gspan::Pattern* j) {
  return cost(*i) < cost(*j);
}

INLINE
bool
TreeWidthSearch::operator() ( LabelData* i, LabelData* j ) {
  return i->tree_width < j->tree_width;
}

} /* namespace gspan */
} /* namespace prm */
} /* namespace gum */
// ============================================================================
