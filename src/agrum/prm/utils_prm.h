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
#ifndef GUM_UTILS_PRM_H
#define GUM_UTILS_PRM_H

#include <string>
#include <vector>

/*! \page prm_page How to use Probabilistic Relational Models.


  \section Memory management

  The following rule is used for freeing memory in a gum::PRM:
  the delete call is left to the highest container in the following hierarchy:
  gum::PRM -> gum::ClassElementContainer -> gum::ClassElement -> gum::Type.

  You will notice that most constructors and destructors are private. So to create
  a gum::PRM use a gum::PRMFactory. To delete the gum::PRM, simply call it's destructor
  which will recursively clean all objects allocated by the gum::PRMFactory in this
  gum::PRM.

  \section About gum::Attribute, gum::Aggregate and instantiations

  In a PRM special treatment of variables and conditional probability tables is necessary.
  This is due for two main reasons, the first one is the necessity to minimize memory
  use by duplicating the fewest elements possible; the second reason comes from
  how the gum::MultDimInterface hierarchy handles variables (the gum::DiscreteVariabe
  hierarchy).

  The basic postulate is that each gum::Attribute holds a gum::Type which is deleted
  with it's owner (i.e. the corresponding gum::Attribute). Why not using a shared pointer
  for each gum::Type? Simply because in gum::MultiDimInterface gum::DiscreteVariable are
  differentiated using their pointers (i.e. memory address), having one gum::Type for each
  gum::Attribute helps inference with evidences. With only one gum::Type for a given type
  in all the gum::PMR will make very difficult to know from which parent an evidence belongs if
  both of them share the same gum::DiscreteVariable.

  However the problem still exists when handling multiple gum::ReferenceSlot and gum::Aggregate.
  This problem still need to be solved elegantly ;)

  If you look at the implementation of gum::Instance and gum::Class you will see that
  gum::Instance delegates most of it's elements to it's type (which is a gum::Class).
  This prevents memory waste by not duplicating unnecessary gum::DiscreteVariable and gum::Potential.

  However it is necessary to overload gum::Aggregate in an instance, because the number of
  parents of a gum::Aggregate depends on it's context: the number of gum::Instance
  in a gum::ReferenceSlot can vary from one gum::Instance too another.

  To overload a gum::Aggregate we simply instantiate it as a gum::Attribute with the
  corresponding type and gum::MultiDimAggregator. However the gum::Attribute is not the
  owner of it's gum::Type: it's shared with the gum::Aggregate hold by it's gum::Class. This is
  the only exception of the rule mentioned above. Why is this necessary? Since the
  gum::MultiDimInterface hierarchy works on pointers, if we overload a gum::Aggregate
  as a classic gum::Attribute in a gum::Instance the gum::DiscreteVariable hold in the
  gum::Attribute's gum::Potential at gum::Class level is no longer the same than the
  gum::DiscreteVariabe in the instantiated gum::Aggregate. Which would force the overload
  of any children of a gum::Aggregate in a gum::Instance, thus duplicating unnecessary
  information. (unless you want to spent your time managing maps between variables)

*/
namespace gum {

  /// Type for real numbers
  typedef float prm_float;

  /// Decompose a string in a vector of strings using "." as separators.
  void decomposePath(const std::string& path, std::vector<std::string>& v);

} /* namespace gum */

#endif // GUM_UTILS_PRM_H
