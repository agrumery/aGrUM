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

/*
 * The documentation of gum::dot::DotOptions for pyAgrum (the class is declared in wrappers/swig/dot.i).
 */

%feature("docstring") gum::dot::DotOptions
"
What you give to the ``toDot(options)`` of a graph, a Bayesian network, a Markov random field, an influence diagram,
a causal model, a credal network, a junction tree... beyond the look given by ``pyagrum.config`` (sections
``notebook``, ``factorgraph``, ``dynamicBN``, ``influenceDiagram``, ``credalnet`` and ``causal``).

What is not given is taken from the configuration. The nodes are designated by their identifiers, the arcs by the
identifiers of their tail and of their head (an undirected edge by its two ends, in any order) and the factor of a
factor graph by the identifiers of its variables.

Examples
--------
  >>> import pyagrum as gum
  >>> bn = gum.fastBN('A->B->C')
  >>> options = gum.DotOptions()
  >>> options.setSize('12!')
  >>> options.setNodeBgColor(bn.idFromName('A'), '#FF7F64')
  >>> options.setArcWidth(bn.idFromName('A'), bn.idFromName('B'), 3)
  >>> dot = bn.toDot(options)
"

%feature("docstring") gum::dot::DotOptions::setSize
"
Parameters
----------
size : str
  the size (for graphviz, in inches) of the graph, such as ``'5'`` or ``'12!'`` for an exact size.
"

%feature("docstring") gum::dot::DotOptions::setRankDir
"
Parameters
----------
rankDir : str
  the direction of the layout: ``'TB'`` (top to bottom) or ``'LR'`` (left to right).
"

%feature("docstring") gum::dot::DotOptions::setLayout
"
Parameters
----------
layout : str
  the graphviz layout engine (``'dot'``, ``'neato'``, ``'fdp'``...).
"

%feature("docstring") gum::dot::DotOptions::setLabel
"
Parameters
----------
label : str
  the label of the graph.
"

%feature("docstring") gum::dot::DotOptions::size
"
Returns
-------
str
  the size given for the graph, an empty string if none was given.
"

%feature("docstring") gum::dot::DotOptions::rankDir
"
Returns
-------
str
  the direction of the layout given for the graph, an empty string if none was given.
"

%feature("docstring") gum::dot::DotOptions::layout
"
Returns
-------
str
  the layout given for the graph, an empty string if none was given.
"

%feature("docstring") gum::dot::DotOptions::label
"
Returns
-------
str
  the label given for the graph, an empty string if none was given.
"

%feature("docstring") gum::dot::DotOptions::setNodeBgColor
"
Parameters
----------
node : int
  the id of the node.
color : str
  the background color of the node (``'#RRGGBB'`` or a name of color). If no font color is given for the node, the text
  is black or white according to the brightness of this background.
"

%feature("docstring") gum::dot::DotOptions::setNodeFgColor
"
Parameters
----------
node : int
  the id of the node.
color : str
  the font color of the node (``'adaptive'`` for black or white according to the background).
"

%feature("docstring") gum::dot::DotOptions::setNodeLabel
"
Parameters
----------
node : int
  the id of the node.
label : str
  the text displayed in the node (by default, its name).
"

%feature("docstring") gum::dot::DotOptions::setNodeTooltip
"
Parameters
----------
node : int
  the id of the node.
tooltip : str
  the text displayed when the mouse is over the node.
"

%feature("docstring") gum::dot::DotOptions::setNodeAttribute
"
Parameters
----------
node : int
  the id of the node.
key : str
  the name of a graphviz attribute of the node (letters, digits and underscores).
value : str
  its value.
verbatim : bool
  if True, the value is written as it is (with its own quotes if it needs some), for instance for an HTML label.
"

%feature("docstring") gum::dot::DotOptions::setArcColor
"
Parameters
----------
tail : int
  the id of the tail of the arc.
head : int
  the id of the head of the arc.
color : str
  the color of the arc.
"

%feature("docstring") gum::dot::DotOptions::setArcWidth
"
Parameters
----------
tail : int
  the id of the tail of the arc.
head : int
  the id of the head of the arc.
width : float
  the width of the line of the arc (at least 0).

Raises
------
pyagrum.InvalidArgument
  if the width is negative or not a finite number.
"

%feature("docstring") gum::dot::DotOptions::setArcLabel
"
Parameters
----------
tail : int
  the id of the tail of the arc.
head : int
  the id of the head of the arc.
label : str
  the text displayed next to the arc.
"

%feature("docstring") gum::dot::DotOptions::setArcTooltip
"
Parameters
----------
tail : int
  the id of the tail of the arc.
head : int
  the id of the head of the arc.
tooltip : str
  the text displayed when the mouse is over the arc.
"

%feature("docstring") gum::dot::DotOptions::setArcAttribute
"
Parameters
----------
tail : int
  the id of the tail of the arc.
head : int
  the id of the head of the arc.
key : str
  the name of a graphviz attribute of the arc (letters, digits and underscores).
value : str
  its value.
verbatim : bool
  if True, the value is written as it is.
"

%feature("docstring") gum::dot::DotOptions::setFactorBgColor
"
Parameters
----------
variables : Set[int]
  the ids of the variables of the factor (in a factor graph of a Markov random field).
color : str
  the background color of the factor.
"

%feature("docstring") gum::dot::DotOptions::setFactorFgColor
"
Parameters
----------
variables : Set[int]
  the ids of the variables of the factor (in a factor graph of a Markov random field).
color : str
  the font color of the factor.
"

%feature("docstring") gum::dot::DotOptions::setFactorLabel
"
Parameters
----------
variables : Set[int]
  the ids of the variables of the factor (in a factor graph of a Markov random field).
label : str
  the text displayed in the factor.
"

%feature("docstring") gum::dot::DotOptions::setFactorTooltip
"
Parameters
----------
variables : Set[int]
  the ids of the variables of the factor (in a factor graph of a Markov random field).
tooltip : str
  the text displayed when the mouse is over the factor.
"

%feature("docstring") gum::dot::DotOptions::setFactorAttribute
"
Parameters
----------
variables : Set[int]
  the ids of the variables of the factor (in a factor graph of a Markov random field).
key : str
  the name of a graphviz attribute of the factor.
value : str
  its value.
verbatim : bool
  if True, the value is written as it is.
"

%feature("docstring") gum::dot::DotOptions::empty
"
Returns
-------
bool
  True if nothing was given.
"
