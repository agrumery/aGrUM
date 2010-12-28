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
 * @brief Inline implementation of StructuredInference.
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {

INLINE
void
StructuredInference::setPaterMining(bool b) {
  __mining = b;
}

INLINE
std::string
StructuredInference::__str(const Instance* i, const Attribute* a) const {
  return i->name() + __dot + a->safeName();
}

INLINE
std::string
StructuredInference::__str(const Instance* i, const Attribute& a) const {
  return i->name() + __dot + a.safeName();
}

INLINE
std::string
StructuredInference::__str(const Instance* i, const SlotChain& a) const {
  return i->name() + __dot + a.lastElt().safeName();
}

INLINE
StructuredInference::RGData::~RGData() {
  GUM_DESTRUCTOR(StructuredInference::RGData);
}

INLINE
StructuredInference::PData::~PData() {
  GUM_DESTRUCTOR(StructuredInference::PData);
}

INLINE
std::string
StructuredInference::name() const {
  return "StructuredInference";
}

INLINE
GSpan&
StructuredInference::gspan() {
  return *__gspan;
}

INLINE
const GSpan&
StructuredInference::gspan() const {
  return *__gspan;
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
