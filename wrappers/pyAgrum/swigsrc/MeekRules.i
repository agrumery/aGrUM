/**
*
*  Copyright 2005-2024 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
*   {prenom.nom}_at_lip6.fr
*
*  This library is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public License
*  along with this library.  If not, see <http://www.gnu.org/licenses/>.
*
*/
// defines some macro for MeekRules classe
%extend gum::MeekRules {
  PyObject* choices() const {
    PyObject* q=PyList_New(0);

    PyObject* pyval;
    for ( auto arc : self->choices()) {
      pyval=Py_BuildValue("(i,i)", arc.tail(), arc.head());
      PyList_Append(q,pyval);
      Py_DecRef(pyval);
    }
    return q;
  };
}
%ignore gum::MeekRules::choices() const;
