%define ADD_CNINFERENCE_DOC(classname...)
%feature("docstring") gum::credal::classname::dynamicExpMax
"
Get the upper dynamic expectation of a given variable prefix.

Parameters
----------
varName : str
	the variable name prefix which upper expectation we want.

Returns
-------
double
    a constant reference to the variable upper expectation over all time steps. 
"

%feature("docstring") gum::credal::classname::dynamicExpMin
"
Get the lower dynamic expectation of a given variable prefix.

Parameters
----------
varName : str
	the variable name prefix which lower expectation we want.

Returns
-------
double
    a constant reference to the variable lower expectation over all time steps. 
"

%feature("docstring") gum::credal::classname::eraseAllEvidence
"
Erase all inference related data to perform another one.

You need to insert evidence again if needed but modalities are kept. You can insert new ones by using the appropriate method which will delete the old ones.
"

%feature("docstring") gum::credal::classname::insertEvidenceFile
"
Insert evidence from file.

Parameters
----------
path : str
	the path to the evidence file. 
"

%feature("docstring") gum::credal::classname::insertModalsFile
"
Insert variables modalities from file to compute expectations.

Parameters
----------
path : str
	The path to the modalities file. 
"

%feature("docstring") gum::credal::classname::makeInference
"
Starts the inference.
"

%feature("docstring") gum::credal::classname::marginalMax
"
Get the upper marginals of a given node id.

Parameters
----------
id : int
	the node id which upper marginals we want.
varName : str
	the variable name which upper marginals we want. 

Returns
-------
list
    a constant reference to this node upper marginals.

Raises
------
gum.IndexError
	If the node does not belong to the Credal network
"

%feature("docstring") gum::credal::classname::marginalMin
"
Get the lower marginals of a given node id.

Parameters
----------
id : int
	the node id which lower marginals we want.
varName : str
	the variable name which lower marginals we want. 

Returns
-------
list
    a constant reference to this node lower marginals.

Raises
------
gum.IndexError
	If the node does not belong to the Credal network
"

%feature("docstring") gum::credal::classname::saveInference
"
Saves marginals.

Parameters
----------
path : str
	The path to the file to save marginals. 
"

%feature("docstring") gum::credal::classname::setRepetitiveInd
"
Parameters
----------
flag : bool
	True if repetitive independence is to be used, false otherwise. Only usefull with dynamic networks. 
"

%feature("docstring") gum::credal::classname::asIApproximationSchemeConfiguration
"
Returns
-------
asIApproximationSchemeConfiguration
"

%feature("docstring") gum::credal::classname::inferenceType
"
Returns
-------
int
	the inference type
"
%enddef

ADD_CNINFERENCE_DOC(CNMonteCarloSampling<double>)
ADD_CNINFERENCE_DOC(CNLoopyPropagation<double>)