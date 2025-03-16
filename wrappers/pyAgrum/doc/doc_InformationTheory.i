/****************************************************************************
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/




%feature("docstring") InformationTheory
"
This class gathers information theory concepts for subsets named X,Y and Z computed with only one (optimized) inference.


**it=gum.InformationTheory(ie,X,Y,Z)**

Parameters
----------
    ie : InferenceEngine
      the inference algorithme to use (for instance, `pyAgrum.LazyPropagation`)
    X : int or str or iterable[int or str]
      a first nodeset
    Y  : int or str or iterable[int or str]
      a second nodeset
    Z :  : int or str or iterable[int or str] (optional)
      a third (an optional) nodeset

Example
-------

      .. code:: python

          import pyAgrum as gum
          bn=gum.fastBN('A->B<-C<-D->E<-F->G->A')
          ie=gum.LazyPropagation(bn)
          it=gum.InformationTheory(ie,'A',['B','G'],['C'])
          print(f'Entropy(A)={it.entropyX()}'')
          print(f'MutualInformation(A;B,G)={it.mutualInformationXY()}')
          print(f'MutualInformation(A;B,G| C)={it.mutualInformationXYgivenZ()}')
          print(f'VariationOfInformation(A;B,G)={it.variationOfInformationXY()}')
"

%feature("docstring") InformationTheory::entropyX
"
  Returns
  -------
  float
    The entropy of nodeset X.
"

%feature("docstring") InformationTheory::entropyXY
"
Returns
-------
  float
    The entropy of nodeset, union of X and Y.
"
%feature("docstring") InformationTheory::XYgivenZ
"
Returns
-------
  float
    The conditional entropy of nodeset, union of X and Y conditionned by nodeset Z
"

%feature("docstring") InformationTheory::entropyXgivenY
"
Returns
-------
  float
    The conditional entropy of nodeset X conditionned by nodeset Y
"

%feature("docstring") InformationTheory::entropyY
"
Returns
-------
  float
    The entropy of nodeset X.
"

%feature("docstring") InformationTheory::entropyYgivenX
"
Returns
-------
  float
    The conditional entropy of nodeset Y conditionned by nodeset X
"

%feature("docstring") InformationTheory::mutualInformationXY(self)
"
Returns
-------
  float
    The mutual information between nodeset X and nodeset Y
"

%feature("docstring") InformationTheory::mutualInformationXYgivenZ
"
Returns
-------
  float
    The conditional mutual information between nodeset X and nodeset Y conditionned by nodeset Z
"

%feature("docstring") InformationTheory::variationOfInformationXY
"
Returns
-------
  float
    The variation of information between nodeset X and nodeset Y
"
