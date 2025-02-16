/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

%feature("docstring") gum::initRandom
"
Initialize random generator seed. If `seed=0`, the generator is initialized from the current time in ms. `seed!=0` allows to fix the generator in a certain stage and then allows to repeat the same pseudo-random numbers sequence.

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
    a random number between 0 and 1 included (i.e. a proba).
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

%feature("docstring") gum::getNumberOfLogicalProcessors
"
Returns
-------
int
  the number of logical processors
"

%feature("docstring") gum::getMaxNumberOfThreads
"
Returns
-------
int
  the max number of threads
"

%feature("docstring") gum::randomGeneratorSeed
"
Returns
-------
int
  a randomly generated seed
"

%feature("docstring") gum::randomValue
"
Returns
-------
int
  a value randomly drawn (0 or 1)
"
