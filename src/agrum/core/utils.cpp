/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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

#include <agrum/config.h>
#include <agrum/core/utils.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  bool Memcmp(const void *const _in, const void *const _out, unsigned long size) {
    unsigned char *in = (unsigned char *)_in;
    unsigned char *out = (unsigned char *)_out;

    for (unsigned long i = 0; i < size; ++i)
      if (*(in++) != *(out++))
        return false;

    return true;
  }

  void __atexit(void) {
#ifndef NDEBUG
    gum::__debug__::__atexit();
#endif
  }

#endif // DOXYGEN_SHOULD_SKIP_THIS

} /* namespace gum */
