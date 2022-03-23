/**
*
*  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
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

%extend gum::learning::BNDatabaseGenerator< double > {
  PyObject* varOrder() const { return PyAgrumHelper::PyListFromNodeVect(self->varOrder()); };
};
%ignore gum::learning::BNDatabaseGenerator< double >::varOrder const;

%extend gum::learning::BNDatabaseGenerator< double > {
%pythoncode {
def to_pandas(self,with_labels=False):
  r"""
  export the samples as a pandas.DataFrame.

  Parameters
  ----------
  with_labels: bool
    is the DataFrame full of labels of variables or full of index of labels of variables
  """
  import pandas

  t=pandas.DataFrame(columns=self.varOrderNames())
  nrow=self.samplesNbRows()
  ncol=self.samplesNbCols()

  if with_labels:
    for row in range(nrow):
      t.loc[len(t),self.varOrderNames()]= [self.samplesLabelAt(row,col) for col in range(ncol)]
  else:
    for row in range(nrow):
      t.loc[len(t),self.varOrderNames()]= [self.samplesAt(row,col) for col in range(ncol)]

  return t
}

};