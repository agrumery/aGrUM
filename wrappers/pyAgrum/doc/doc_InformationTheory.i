%feature("docstring") InformationTheory
"
This class gathers information theory concepts for subsets named X,Y and Z computed with only one inference.

Parameters
----------
    - **X** (*int|str* or *iterable[int|str]* ) -- a first nodeset
    - **Y** (*int|str* or *iterable[int|str]* ) -- a second nodeset
    - **Z** (*int|str* or *iterable[int|str]* ) -- a third (an optional) nodeset
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
