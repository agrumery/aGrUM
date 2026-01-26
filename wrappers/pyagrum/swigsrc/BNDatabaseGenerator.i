/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/







%ignore gum::learning::BNDatabaseGenerator< double>::toDatabaseTable const;
%ignore gum::learning::BNDatabaseGenerator< double>::database const;
%ignore gum::learning::BNDatabaseGenerator< double>::DiscretizedLabelMode;

%extend gum::learning::BNDatabaseGenerator< double > {
  PyObject* varOrder() const { return PyAgrumHelper::PyTupleFromNodeVect(self->varOrder()); };
};
%ignore gum::learning::BNDatabaseGenerator< double >::varOrder const;

%extend gum::learning::BNDatabaseGenerator< double > {
double drawSamples(Size nbSamples,PyObject *arg) {
    if (PyDict_Check(arg)) {
      gum::Instantiation inst;
      PyAgrumHelper::fillInstantiationFromPyObjectAndBN(&self->bn(), inst, arg);
      return self->drawSamples(nbSamples,inst);
    } else {
      GUM_ERROR(gum::InvalidArgument,"arg is neither a dict or an pyagrum.Instantiation.");
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
