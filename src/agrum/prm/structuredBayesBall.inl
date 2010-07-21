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
 * @brief Inline implementation of StructuredBayesBall.
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {

INLINE
StructuredBayesBall::StructuredBayesBall(const PRMInference& inference):
  __inf(&inference)
{
  GUM_CONSTRUCTOR( StructuredBayesBall );
}

INLINE
StructuredBayesBall::StructuredBayesBall(const StructuredBayesBall& source):
  __inf(0)
{
  GUM_CONS_CPY( StructuredBayesBall );
  GUM_ERROR(FatalError, "Not allowed.");
}

INLINE
StructuredBayesBall&
StructuredBayesBall::operator=(const StructuredBayesBall& source) {
  GUM_ERROR(FatalError, "Not allowed.");
}

INLINE
const std::string&
StructuredBayesBall::key(const Instance* i) const {
  return __keyMap[i].first;
}

INLINE
const std::string&
StructuredBayesBall::key(const Instance& i) const {
  return __keyMap[&i].first;
}

INLINE
const Set<NodeId>&
StructuredBayesBall::requisiteNodes(const Instance* i) const {
  return *(__keyMap[i].second);
}

INLINE
const Set<NodeId>&
StructuredBayesBall::requisiteNodes(const Instance& i) const {
  return *(__keyMap[&i].second);
}

INLINE
Size
StructuredBayesBall::occurrence(const std::string& key) const {
  return __reqMap[key].second;
}

INLINE
float
StructuredBayesBall::liftRatio() const {
  return ((float) __reqMap.size()) / ((float) __keyMap.size());
}

INLINE
bool
StructuredBayesBall::exists(const Instance* i) const {
  return __keyMap.exists(i);
}

INLINE
bool
StructuredBayesBall::exists(const Instance& i) const {
  return __keyMap.exists(&i);
}

INLINE
void
StructuredBayesBall::compute(const Instance* i, NodeId n) {
  __compute(i, n);
}

INLINE
void
StructuredBayesBall::compute(const Instance& i, NodeId n) {
  __compute(&i, n);
}

INLINE
const SlotChain&
StructuredBayesBall::__getSC(const Instance* i, NodeId n) {
  return static_cast<const SlotChain&>(i->type().get(n));
}

INLINE
std::pair<bool, bool>&
StructuredBayesBall::__getMark(InstanceMap& marks, const Instance* i, NodeId n) {
  return (*(marks[i]))[n];
}


} /* namespace prm */
} /* namespace gum */
// ============================================================================
