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







%pythoncode %{
def availableIDExts():
  """ Give the list of all formats known by pyAgrum to save a influence diagram.

  Returns
  ------
  str
    a string which lists all suffixes for supported ID file formats.
  """
  return "xmlbif|bifxml|xml|jgum|bgum|pkl"


def loadID(filename):
  """
  read a gum.InfluenceDiagram from a ID file

  Parameters
  ----------
  filename: str
    the name of the input file

  Returns
  -------
  pyagrum.InfluenceDiagram
    the InfluenceDiagram

  Notes
  -----
  pkl suffix is used to load a pickled InfluenceDiagram. In this case, options are ignored.

  jgum/bgum suffixes use the native aGrUM JSON/binary format.
  """
  extension = filename.split('.')[-1].upper()

  if extension in {"BIFXML", "XMLBIF", "XML"}:
    diag = pyagrum.InfluenceDiagram()
    # for now, just one format
    res = diag.loadBIFXML(filename)

    if not res:
      raise IOError(f"Error(s) in {filename}")
  elif extension == "JGUM":
    diag = pyagrum.InfluenceDiagram()
    diag.loadGUM(filename)
  elif extension == "BGUM":
    diag = pyagrum.InfluenceDiagram()
    diag.loadGUM(filename, binary=True)
  elif extension == "PKL":
    diag = _gum_pickle_load(filename)
  else:
    raise InvalidArgument("extension " + filename.split('.')[-1] + " unknown. Please use among " + availableIDExts())

  _gum_set_name_property(diag, filename)
  return diag


def saveID(infdiag, filename):
  """
  save an ID into a file using the format corresponding to one of the availableWriteIDExts() suffixes.

  Parameters
  ----------
  infdiag : pyagrum.InfluenceDiagram
    the Influence Diagram to save
  filename : str
    the name of the output file

  Notes
  -----
  pkl suffix is used to save an InfluenceDiagram using pickle.

  jgum/bgum suffixes use the native aGrUM JSON/binary format.
  """
  extension = filename.split('.')[-1].upper()

  if extension in {"BIFXML", "XMLBIF", "XML"}:
    infdiag.saveBIFXML(filename)
  elif extension == "JGUM":
    infdiag.saveGUM(filename)
  elif extension == "BGUM":
    infdiag.saveGUM(filename, binary=True)
  elif extension == "PKL":
    _gum_pickle_save(infdiag, filename)
  else:
    raise InvalidArgument("extension " + filename.split('.')[-1] + " unknown. Please use among " + availableIDExts())


def fastID(structure, domain="[2]"):
  """
  Create an Influence Diagram with a modified dot-like syntax which specifies:
      - the structure and the type of the variables following :ref:`fast syntax<Quick specification of (randomly parameterized) graphical models>`,
      - a prefix for the type of node (chance/decision/utiliy nodes):

        - ``a`` : a chance node named 'a' (by default)
        - ``$a`` : a utility node named 'a'
        - ``*a`` : a decision node named 'a'

  Examples
  --------
  >>> import pyagrum as gum
  >>> bn=gum.fastID('A->B[1,3]<-*C{yes|No}->$D<-E[1,2.5,3.9]',6)

  Parameters
  ----------
  structure : str
          the string containing the specification
  domain: int or str
          the default domain size (int) or domain specification (str) for variables (default is "[2]"

  Returns
  -------
  pyagrum.InfluenceDiagram
          the resulting Influence Diagram
  """
  return InfluenceDiagram.fastPrototype(structure, domain)
%}
