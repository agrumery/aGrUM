/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it udnder the terms of the GNU General Public License as published by *
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

#include <agrum/core/debug.h>
#include <agrum/prm/utils_prm.h>
#include <agrum/prm/classElement.h>

namespace gum {
namespace prm {

// Decompose a string in a vector of strings using "." as separators.
void
decomposePath(const std::string& path, std::vector<std::string>& v) {
  size_t prev = 0;
  size_t length = 0;
  size_t idx_1 = path.find(".");
  size_t idx_2 = path.find(ClassElement::LEFT_CAST());
  if (idx_2 == std::string::npos) {
    // ignore safe names
    size_t idx = idx_1;
    while (idx != std::string::npos) {
      length = idx - prev;
      v.push_back(path.substr(prev, length));
      prev = idx + 1;
      idx = path.find(".", prev);
    }
  } else {
    size_t tmp = 0;
    while (idx_1 != std::string::npos) {
      if (idx_1 < idx_2) {
        length = idx_1 - prev;
        v.push_back(path.substr(prev, length));
        prev = idx_1 + 1;
        idx_1 = path.find(".", prev);
      } else if (idx_2 < idx_1) {
        tmp = path.find(ClassElement::RIGHT_CAST(), idx_2);
        idx_1 = path.find(".", tmp);
        idx_2 = path.find(ClassElement::LEFT_CAST(), tmp);
      }
    }
  }
  v.push_back(path.substr(prev, std::string::npos));
}

Potential<prm_float>* copyPotential(const Bijection<const DiscreteVariable*, const DiscreteVariable*>& bij,
                                    const Potential<prm_float>& source)
{
  const MultiDimImplementation<prm_float>* impl = source.getContent();
  Potential<prm_float>* p = 0;
  if (dynamic_cast< const MultiDimReadOnly<prm_float>* >(impl)) {
    if (dynamic_cast< const MultiDimNoisyORCompound<prm_float>* >(impl)) {
      p = new Potential<prm_float>(new MultiDimNoisyORCompound<prm_float>(bij, static_cast<const MultiDimNoisyORCompound<prm_float>&>(*impl)));
    } else if (dynamic_cast< const MultiDimNoisyORNet<prm_float>* >(impl)) {
      p = new Potential<prm_float>(new MultiDimNoisyORNet<prm_float>(bij, static_cast<const MultiDimNoisyORNet<prm_float>&>(*impl)));
    } else if (dynamic_cast< const MultiDimAggregator<prm_float>* >(impl)) {
      try {
        p = new Potential<prm_float>(static_cast<MultiDimImplementation<prm_float>*>(impl->newFactory()));
        for (MultiDimInterface::iterator iter = impl->begin(); iter != impl->end(); ++iter)
          p->add(*(bij.second(*iter)));
      } catch (NotFound& e) {
        // Can happen when attr is an internal aggregator
        delete p;
        throw e;
      }
    } else {
      GUM_CHECKPOINT;
      GUM_ERROR(FatalError, "encountered an unexpected MultiDim implementation");
    }
  } else {
    if (dynamic_cast< const MultiDimArray<prm_float>* >(impl)) {
      p = new Potential<prm_float>(new MultiDimBijArray<prm_float>(bij, static_cast< const MultiDimArray<prm_float>& >(*impl)));
    } else if (dynamic_cast< const MultiDimSparse<prm_float>* >(impl)) {
      // Copy the sparse array in a MultiDimArray, this is sloppy but fast
      // Still MultiDimSparse are not yet used so you this is only in case someone add them without reading the code first
      MultiDimArray<prm_float>* array = new MultiDimArray<prm_float>();
      for (MultiDimInterface::iterator iter = impl->begin(); iter != impl->end(); ++iter)
        array->add(**iter);
      for (Instantiation i(*impl); not i.end(); i.inc())
        array->set(i, impl->get(i));
      p = new Potential<prm_float>(new MultiDimBijArray<prm_float>(bij, *array));
    } else if (dynamic_cast<const MultiDimBijArray<prm_float>*>(impl)) {
      p = new Potential<prm_float>(new MultiDimBijArray<prm_float>(bij, static_cast<const MultiDimBijArray<prm_float>&>(*impl)));
    } else {
      // Just need to make the copy using the bijection but we only use multidim array
      GUM_CHECKPOINT;
      GUM_ERROR(FatalError, "encountered an unexpected MultiDim implementation");
    }
  }
  return p;
}


} /* namespace prm */
}
