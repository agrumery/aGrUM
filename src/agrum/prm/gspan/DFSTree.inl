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
 * @brief Inline implementation of the DFSTree class.
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {
namespace gspan {

INLINE
DFSTree::DFSTree(const InterfaceGraph& graph):
  __graph(&graph)
{
  GUM_CONSTRUCTOR( DFSTree );
}

INLINE
DFSTree::~DFSTree() {
  GUM_DESTRUCTOR( DFSTree );
  typedef HashTable<Pattern*, PatternData*>::iterator Iter;
  for (Iter iter = __data.begin(); iter != __data.end(); ++iter) {
    delete iter.key();
    delete *iter;
  }
}

INLINE
std::list<NodeId>&
DFSTree::roots() { return __roots; }

INLINE
const std::list<NodeId>&
DFSTree::roots() const { return __roots; }

INLINE
Pattern&
DFSTree::parent(const Pattern& p) {
  try {
    return *(__node_map.second(DiGraph::parents(__node_map.first(const_cast<Pattern*>(&p))).begin()->tail()));
  } catch (NotFound&) {
    if (__node_map.existsSecond(const_cast<Pattern*>(&p))) {
      GUM_ERROR(NotFound, "the given pattern is a root node");
    } else {
      GUM_ERROR(NotFound, "pattern not found in this DFSTree");
    }
  }
}

INLINE
const Pattern&
DFSTree::parent(const Pattern& p) const {
  try {
    return *(__node_map.second(DiGraph::parents(__node_map.first(const_cast<Pattern*>(&p))).begin()->tail()));
  } catch (NotFound&) {
    if (__node_map.existsSecond(const_cast<Pattern*>(&p))) {
      GUM_ERROR(NotFound, "the given pattern is a root node");
    } else {
      GUM_ERROR(NotFound, "pattern not found in this DFSTree");
    }
  }
}

INLINE
std::list<NodeId>&
DFSTree::children(const Pattern& p) {
  try {
    return __data[const_cast<Pattern*>(&p)]->children;
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "pattern not found in this DFSTree");
  }
}

INLINE
const std::list<NodeId>&
DFSTree::children(const Pattern& p) const {
  try {
    return __data[const_cast<Pattern*>(&p)]->children;
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "pattern not found in this DFSTree");
  }
}

INLINE
Pattern&
DFSTree::pattern(NodeId id) {
  try {
    return *(__node_map.second(id));
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no pattern matching the given id");
  }
}

INLINE
const Pattern&
DFSTree::pattern(NodeId id) const {
  try {
    return *(__node_map.second(id));
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no pattern matching the given id");
  }
}

INLINE
UndiGraph&
DFSTree::iso_graph(const Pattern& p) {
  try {
    return __data[const_cast<Pattern*>(&p)]->iso_graph;
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "pattern not found in this DFSTree");
  }
}

INLINE
Sequence<Instance*>&
DFSTree::iso_map(const Pattern& p, NodeId node) {
  try {
    return *(__data[const_cast<Pattern*>(&p)]->iso_map[node]);
  } catch (NotFound&) {
    if (__data.exists(const_cast<Pattern*>(&p))) {
      GUM_ERROR(NotFound, "node not found in Pattern's isomorphism graph");
    } else {
      GUM_ERROR(NotFound, "pattern not found in this DFSTree");
    }
  }
}

INLINE
Set<NodeId>&
DFSTree::max_indep_set(const Pattern& p) {
  try {
    return __data[const_cast<Pattern*>(&p)]->max_indep_set;
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "pattern not found in this DFSTree");
  }
}

INLINE
void
DFSTree::__checkGrowth(Pattern& p, Pattern* child, EdgeGrowth& edge_growth) {
  NodeId v = edge_growth.v;
  // First we check if the edge is legal
  if (v == 0) {
    v = child->insertNode(*(edge_growth.l_v));
  }
  child->insertArc(edge_growth.u, v, *(edge_growth.edge));
  // Neighborhood restriction is checked by the Pattern class
  EdgeCode& edge = child->edgeCode(edge_growth.u, v);
  // Then we check if the edge we added is valid
  if (edge < *(child->code().codes.front())) {
    GUM_ERROR(OperationNotAllowed, "added edge code is lesser than the first one in the pattern's DFSCode");
  }
  if (edge.isBackward()) {
    typedef std::vector<EdgeCode*>::iterator EdgeIter;
    for (EdgeIter iter = child->code().codes.begin(); (iter + 1) != child->code().codes.end(); ++iter) {
      if ( (((**iter).i == v) or ((**iter).j == v)) and edge < (**iter) ) {
        GUM_ERROR(OperationNotAllowed, "added backward edge is lesser than an existing edge on v");
      }
    }
  }
  // Finally we check if child is minimal.
  if (not child->isMinimal()) {
    GUM_ERROR(OperationNotAllowed, "the DFSCode for this growth is not minimal");
  }
}

INLINE
void
DFSTree::__addChild(Pattern& p, Pattern* child, EdgeGrowth& edge_growth) {
  // Adding child to the tree
  NodeId node = DiGraph::insertNode();
  __node_map.insert(node, child);
  // Adding child in p's children list
  std::list<NodeId>& children = __data[&p]->children;
  if (children.empty()) {
    children.push_back(node);
  } else {
    size_t size = children.size();
    for (std::list<NodeId>::iterator iter = children.begin(); iter != children.end(); ++iter) {
      if (child->code() < pattern(*iter).code()) {
        children.insert(iter, node);
        break;
      }
    }
    if (size == children.size()) {
      children.push_back(node);
    }
  }
}

INLINE
double
DFSTree::score(const Pattern& p)  const {
  return frequency(p) * (gain(p) / cost(p));
}

INLINE
std::ostream&
operator<<(std::ostream& out, const DFSTree::EdgeGrowth &edge) {
  out << edge.u << ", " << *(edge.edge) << ", " << *(edge.l_v) << ", " << edge.v;
  return out;
}

INLINE
Sequence< HashTable<ClassElement*, Size>* >&
DFSTree::sub_patterns(const Pattern& p) {
  return __data[const_cast<Pattern*>(&p)]->sub_patterns;
}

INLINE
HashTable<NodeId, Idx>&
DFSTree::sub_patterns_map(const Pattern& p) {
  return __data[const_cast<Pattern*>(&p)]->sub_patterns_map;
}

// ============================================================================
// EdgeGrowth
// ============================================================================

INLINE
DFSTree::EdgeGrowth::EdgeGrowth(NodeId a_u, LabelData* an_edge,
                                LabelData* a_l_v, NodeId a_v):
  u(a_u), edge(an_edge), l_v(a_l_v), v(a_v),
  degree_list(new std::vector<NodeId>())
{
  GUM_CONSTRUCTOR( DFSTree::EdgeGrowth );
}

INLINE
DFSTree::EdgeGrowth::EdgeGrowth(const EdgeGrowth& from):
  u(from.u), edge(from.edge), v(from.v), matches(from.matches),
  iso_graph(from.iso_graph), degree_list(0), max_indep_set(from.max_indep_set)
{
  GUM_CONS_CPY( DFSTree::EdgeGrowth );
  if (from.degree_list != 0) {
    degree_list = new std::vector<NodeId>(*(from.degree_list));
  }
}

INLINE
DFSTree::EdgeGrowth::~EdgeGrowth() {
  GUM_DESTRUCTOR( DFSTree::EdgeGrowth );
  if (degree_list != 0) {
    delete degree_list;
  }
}

INLINE
void
DFSTree::EdgeGrowth::insert(Instance* u, Instance* v) {
  NodeId id = iso_graph.insertNode();
  degree_list->push_back(id);
  typedef Property< std::pair<Instance*, Instance*> >::onNodes::iterator MatchIterator;
  for (MatchIterator iter = matches.begin(); iter != matches.end(); ++iter) {
    if ( (iter->first == u) or (iter->second == u) or
         (iter->first == v) or (iter->second == v) ) {
      iso_graph.insertEdge(iter.key(), id);
    }
  }
  // The order between u and v is important ! DO NOT INVERSE IT !
  matches.insert(id, std::make_pair(u, v));
}

INLINE
std::string
DFSTree::EdgeGrowth::toString() {
  std::stringstream str;
  str << u << "-" << edge << "-" << l_v << "-" << v;
  return str.str();
}

// ============================================================================
// NeighborDegreeSort
// ============================================================================

INLINE
DFSTree::NeighborDegreeSort::NeighborDegreeSort(UndiGraph& graph):
  g(graph)
{
  GUM_CONSTRUCTOR( DFSTree::NeighborDegreeSort );
}

INLINE
DFSTree::NeighborDegreeSort::NeighborDegreeSort(const NeighborDegreeSort& source):
  g(source.g)
{
  GUM_CONS_CPY( DFSTree::NeighborDegreeSort );
}

INLINE
DFSTree::NeighborDegreeSort::~NeighborDegreeSort() {
  GUM_DESTRUCTOR( DFSTree::NeighborDegreeSort );
}

INLINE
bool
DFSTree::NeighborDegreeSort::operator() (NodeId i, NodeId j) {
  return g.neighbours(i).size() < g.neighbours(j).size();
}

// ============================================================================
// PatternData
// ============================================================================

INLINE
DFSTree::PatternData::PatternData(Pattern* p):
  pattern(p), cost(0), gain(0)
{
  GUM_CONSTRUCTOR( DFSTree::PatternData );
}

INLINE
DFSTree::PatternData::PatternData(const PatternData& from):
  pattern(from.pattern), children(from.children), iso_graph(from.iso_graph),
  max_indep_set(from.max_indep_set), cost(from.cost), gain(from.gain)
{
  GUM_CONS_CPY( DFSTree::PatternData );
  typedef Property<Sequence<Instance*>*>::onNodes::const_iterator Iter;
  for (Iter iter = from.iso_map.begin(); iter != from.iso_map.end(); ++iter) {
    iso_map.insert(iter.key(), new Sequence<Instance*>(**iter));
  }
}

INLINE
DFSTree::PatternData::~PatternData() {
  GUM_DESTRUCTOR( DFSTree::PatternData );
  typedef Property<Sequence<Instance*>*>::onNodes::const_iterator Iter;
  for (Iter iter = iso_map.begin(); iter != iso_map.end(); ++iter) {
    delete *iter;
  }
  typedef Sequence< HashTable<ClassElement*, Size>* >::iterator SubPatIter;
  for (SubPatIter iter = sub_patterns.begin(); iter != sub_patterns.end(); ++iter) {
    delete *iter;
  }
}

} /* namespace gspan */
} /* namespace prm */
} /* namespace gum */
// ============================================================================
