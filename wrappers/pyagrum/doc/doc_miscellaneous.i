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







%feature("docstring") gum::initRandom
"
Initialize random generator seed. If ``seed=0``, the generator is initialized from the current time in ms. ``seed!=0`` allows to fix the generator in a certain stage and then allows to repeat the same pseudo-random numbers sequence.

This is the way to act directly on the generator at any time. At start, pyAgrum uses the key ``core.randomSeed`` of the configuration (0, the current time, by default): changing this key later has no effect on the generator.

A given seed gives the same draws on every platform.

Parameters
----------
seed : int
  the seed used to initialize the random generator (0 if using time)
"

%feature("docstring") gum::randomProba
"
Returns
-------
float
    a random number in [0, 1) (i.e. a proba).
"

%feature("docstring") gum::randomGamma
"
Draws a value of the Gamma distribution of shape ``shape`` and scale 1, with the random generator of pyAgrum.

For a given seed (see `pyagrum.initRandom`), the value is the same on every platform.

Parameters
----------
shape : float
  the shape of the distribution (> 0)

Returns
-------
float
  a value of the Gamma distribution (its mean is ``shape``).

Raises
------
pyagrum.OutOfBounds
  If ``shape`` is not > 0.
"

%feature("docstring") gum::randomDistribution
"
Parameters
----------
n : int
  The number of modalities for the ditribution.

Returns
-------
a random discrete distribution.
"

%feature("docstring") gum::isOMP
"
Returns
-------
bool
  True if OMP has been set at compilation, False otherwise
"

%feature("docstring") gum::setNumberOfThreads
"
To avoid spare cycles (less then 100% CPU occupied), use more threads than logical processors (x2 is a good all-around value).

Returns
-------
number : int
  the number of threads to be used
"

%feature("docstring") gum::multithreading::getNumberOfLogicalProcessors
"
Return the number of logical processors available on the system.

Returns
-------
int
    the number of logical processors
"

%feature("docstring") gum::multithreading::getMaxNumberOfThreads
"
Return the maximum number of threads available (equal to the number of logical processors by default).

Returns
-------
int
    the maximum number of threads
"

%feature("docstring") gum::multithreading::getNumberOfThreads
"
Return the current number of threads used in parallel regions.

This is equal to getMaxNumberOfThreads() unless the user has called setNumberOfThreads().

Returns
-------
int
    the current thread count for parallel regions
"

%feature("docstring") gum::statsObj
"
Print debug statistics on aGrUM objects.

Only active in debug build mode (GUM_DEBUG_MODE). No output in release mode.
"

%feature("docstring") gum::randomGeneratorSeed
"
Returns
-------
int
  the seed of the generator: the value given to the last call to ``initRandom`` or, if it was 0, the value taken from the current time. Used as ``core.randomSeed`` in a file ``pyagrum.ini``, it repeats the same pseudo-random numbers sequence.
"

%feature("docstring") gum::randomValue
"
Returns
-------
int
  a value randomly drawn (0 or 1)
"

%define FAST_GRAPH_DOC_HEAD
"
Build a graph from a dot-like syntax : ``'A->B->C;B->E'``.

``'->'`` denotes a directed arc, ``'<-'`` a directed arc in the other direction
(e.g. ``'A->B<-C'`` means both A and C point to B), ``'-'`` an undirected edge,
``';'`` separates independent chains. If every node token in the description is
a non-negative integer, those integers are used directly as node ids ;
otherwise every token -- including numeric-looking ones -- is used as a node
name.

Note
----
A single ``'-'`` (not ``'--'``) is used for edges on purpose : `pyagrum.fastMRF`
already uses ``'--'`` to list the variables of a single factor (a clique), a
different construct from a chain of pairwise edges. ``'A--B'`` is therefore
rejected here rather than silently misread.

Raises
------
pyagrum.InvalidArc
  If the description requires a directed arc but the target graph type does
  not support arcs, or if the description is malformed.
pyagrum.InvalidEdge
  If the description requires an undirected edge but the target graph type
  does not support edges.

Parameters
----------
desc : str
  the string containing the dot-like specification
"
%enddef

%define FAST_MARKED_GRAPH_DOC_HEAD
"
Create a marked graph (`pyagrum.MarkedGraph`, or `pyagrum.PAG` for ``fastPAG``) from a dot-like
description.

An edge is given by its two marks around a ``'-'``: ``'<'`` or ``'>'`` for an arrowhead, ``'o'`` for a
circle, nothing for a tail. A circle must be separated from the name of its node by a space
(``'C o-> D'``; in ``'Co->D'``, ``'Co'`` is a node). Without a node name on its side of the edge,
``'o'`` is a node: ``'o->B'``, ``'A->o->B'`` and ``'A-o'`` use a node named ``'o'``. An edge may be repeated with the same marks
(``'A->B;A->B'``) but not with different ones (``'A->B;B->A'``).

Raises
------
pyagrum.InvalidEdge
  If the description is malformed or describes an edge with different marks.

Parameters
----------
desc : str
  the string containing the dot-like specification
"
%enddef

// one docstring per instantiation, so that each fast* function gives its own return type
%define FAST_GRAPH_DOC(FUNCTION, HEAD, GRAPHTYPE)
%feature("docstring") gum::FUNCTION< gum::GRAPHTYPE > HEAD
"
Returns
-------
pyagrum." #GRAPHTYPE "
";
%enddef

FAST_GRAPH_DOC(fastGraph, FAST_GRAPH_DOC_HEAD, DiGraph)
FAST_GRAPH_DOC(fastGraph, FAST_GRAPH_DOC_HEAD, UndiGraph)
FAST_GRAPH_DOC(fastGraph, FAST_GRAPH_DOC_HEAD, MixedGraph)
FAST_GRAPH_DOC(fastGraph, FAST_GRAPH_DOC_HEAD, DAG)
FAST_GRAPH_DOC(fastGraph, FAST_GRAPH_DOC_HEAD, PDAG)
FAST_GRAPH_DOC(fastGraph, FAST_GRAPH_DOC_HEAD, ChainGraph)
FAST_GRAPH_DOC(fastMarkedGraph, FAST_MARKED_GRAPH_DOC_HEAD, MarkedGraph)
FAST_GRAPH_DOC(fastMarkedGraph, FAST_MARKED_GRAPH_DOC_HEAD, PAG)
