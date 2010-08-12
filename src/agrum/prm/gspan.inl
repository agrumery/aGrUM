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
 * @brief Inline implementation of gspan.
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {

INLINE
GSpan::GSpan(PRM& prm, System& sys, Size min_freq, Size depth_stop):
  __graph(new gspan::InterfaceGraph(sys)), __tree(*__graph), __min_freq(min_freq), __depth_stop(depth_stop)
{
  GUM_CONSTRUCTOR( GSpan );
}

INLINE
GSpan::~GSpan() {
  GUM_DESTRUCTOR( GSpan );
  typedef HashTable<gspan::Pattern*, GSpan::MatchedInstances* >::iterator HashIterator;
  for (HashIterator match = __matched_instances.begin(); match != __matched_instances.end(); ++match) {
    delete *match;
  }
  typedef HashTable<gspan::Pattern*, BayesNet<prm_float>*>::iterator BNIter;
  for (BNIter iter = __bn_map.begin(); iter != __bn_map.end(); ++iter) {
    delete *iter;
  }
}

INLINE
Size
GSpan::getMinFrequency() const {
  return __min_freq;
}

INLINE
void
GSpan::setMinFrequency(Size freq) {
  __min_freq = freq;
}

INLINE
Size
GSpan::getMaxDFSDepth() const {
  return __depth_stop;
}

INLINE
void
GSpan::setMaxDFSDepth(Size depth) {
  __depth_stop = depth;
}

INLINE
gspan::DFSTree&
GSpan::tree() { return __tree; }

INLINE
const gspan::DFSTree&
GSpan::tree() const { return __tree; }

INLINE
unsigned long
GSpan::__cost_func(unsigned int interface, unsigned int frequency) {
  return interface * frequency;
}

INLINE
std::vector<gspan::Pattern*>&
GSpan::patterns() {
  return __patterns;
}

INLINE
const std::vector<gspan::Pattern*>&
GSpan::patterns() const {
  return __patterns;
}

INLINE
HashTable<gspan::Pattern*, GSpan::MatchedInstances*>&
GSpan::matches() {
  return __matched_instances;
}

INLINE
const HashTable<gspan::Pattern*, GSpan::MatchedInstances*>&
GSpan::matches() const {
  return __matched_instances;
}

INLINE
gspan::InterfaceGraph&
GSpan::interfaceGraph() {
  return *__graph;
}

INLINE
const gspan::InterfaceGraph&
GSpan::interfaceGraph() const {
  return *__graph;
}


INLINE
BayesNet<prm_float>&
GSpan::baseBN(gspan::Pattern& p) {
  return *(__bn_map[&p]);
}

// ============================================================================
// LalbeSort
// ============================================================================
INLINE
GSpan::LabelSort::LabelSort(GSpan* my_gspan):
  gspan(my_gspan)
{
  GUM_CONSTRUCTOR( GSpan::LabelSort );
}

INLINE
GSpan::LabelSort::LabelSort(const LabelSort& source):
  gspan(source.gspan)
{
  GUM_CONS_CPY( GSpan::LabelSort );
}

INLINE
GSpan::LabelSort::~LabelSort() {
  GUM_DESTRUCTOR( GSpan::LabelSort );
}

INLINE
bool
GSpan::LabelSort::operator()(gspan::LabelData* i, gspan::LabelData* j) {
  // We want a descending order
  return gspan->__cost[i] > gspan->__cost[j];
}

// ============================================================================
// PatternSort
// ============================================================================

INLINE
GSpan::PatternSort::PatternSort(GSpan* my_gspan):
  gspan(my_gspan)
{
  GUM_CONSTRUCTOR( GSpan::PatternSort );
}

INLINE
GSpan::PatternSort::PatternSort(const PatternSort& source):
  gspan(source.gspan)
{
  GUM_CONS_CPY( GSpan::PatternSort );
}

INLINE
GSpan::PatternSort::~PatternSort() {
  GUM_DESTRUCTOR( GSpan::PatternSort );
}

INLINE
bool
GSpan::PatternSort::operator()(gspan::Pattern* i, gspan::Pattern* j) {
  // We want a descending order
  return gspan->tree().frequency(*i) * (gspan->tree().gain(*i) / gspan->tree().cost(*i)) >
         gspan->tree().frequency(*j) * (gspan->tree().gain(*j) / gspan->tree().cost(*j));
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
