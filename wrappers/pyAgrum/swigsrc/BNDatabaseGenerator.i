/**
*
*  Copyright 2005-2022 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
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
%ignore gum::learning::BNDatabaseGenerator< double>::toDatabaseTable const;
%ignore gum::learning::BNDatabaseGenerator< double>::database const;

%extend gum::learning::BNDatabaseGenerator< double > {
  PyObject* varOrder() const { return PyAgrumHelper::PyListFromNodeVect(self->varOrder()); };
};
%ignore gum::learning::BNDatabaseGenerator< double >::varOrder const;

%extend gum::learning::BNDatabaseGenerator< double > {
double drawSamples(Size nbSamples,PyObject *arg) {
    if (PyDict_Check(arg)) {
      gum::Instantiation inst;
      PyAgrumHelper::fillInstantiationFromPyObject(self->bn(), inst, arg);
      return self->drawSamples(nbSamples,inst);
    } else {
      GUM_ERROR(gum::InvalidArgument,"arg is neither a dict or an pyAgrum.Instantiation.");
    }
  }

%pythoncode {
def to_pandas(self,with_labels=True):
  r"""
  export the samples as a pandas.DataFrame.

  Parameters
  ----------
  with_labels: bool
    is the DataFrame full of labels of variables or full of index of labels of variables
  """
  import pandas

  nrow=self.samplesNbRows()
  ncol=self.samplesNbCols()

  if with_labels:
    ldatas=[[self.samplesLabelAt(row,col) for col in range(ncol)] for row in range(nrow)]
  else:
    ldatas=[[self.samplesAt(row,col) for col in range(ncol)] for row in range(nrow)]

  return pandas.DataFrame(columns=self.varOrderNames(),data=ldatas)
}

};
