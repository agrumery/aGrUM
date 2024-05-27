%feature("docstring") InformationTheory
"
This class gathers information theory concepts for subsets named X,Y and Z computed with only one (optimized) inference.


**it=gum.InformationTheory(ie,X,Y,Z)**

  Parameters

    - **ie** (*InferenceEngine*) -- the inference algorithme to use (for instance, `pyAgrum.LazyPropagation`)
    - **X** (*int or str* or *iterable[int or str]* ) -- a first nodeset
    - **Y** (*int or str* or *iterable[int or str]* ) -- a second nodeset
    - **Z** (*int or str* or *iterable[int or str]* ) -- a third (an optional) nodeset

Examples
--------
>>> import pyAgrum as gum
>>> bn=gum.fastBN('A->B<-C<-D->E<-F->G->A')
>>> ie=gum.LazyPropagation(bn)
>>> it=gum.InformationTheory(ie,'A',['B','G'],['C'])
>>> print(f'Entropy(A)={it.entropyX()}'')
>>> print(f'MutualInformation(A;B,G)={it.mutualInformationXY()}')
>>> print(f'MutualInformation(A;B,G| C)={it.mutualInformationXYgivenZ()}')
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
