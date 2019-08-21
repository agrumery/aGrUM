%feature("docstring") gum::credal::CredalNet
"
Constructor used to create a CredalNet (step by step or with two BayesNet)

CredalNet() -> CredalNet
    default constructor

CredalNet(src_min_num,src_max_den) -> CredalNet
    Parameters:
        * **src_min_num** (*str*) -- the path to a BayesNet which contains lower probabilities.
        * **src_max_den** (*str*) -- the (optional) path to a BayesNet which contains upper probabilities.

CredalNet(src_min_num,src_max_den) -> CredalNet
    Parameters:
        * **src_min_num (*pyAgrum.BayesNet*) -- the BayesNet which contains lower probabilities.
        * **src_max_den (*pyAgrum.BayesNet*) -- the (optional) BayesNet which contains upper probabilities. 
"

%feature("docstring") gum::credal::CredalNet::addArc
"
Adds an arc between two nodes

Parameters
----------
tail :
	the id of the tail node
head : int 
	the id of the head node

Raises
------
gum.InvalidDirectedCircle
	If any (directed) cycle is created by this arc
gum.InvalidNode
	If head or tail does not belong to the graph nodes
gum.DuplicateElement
	If one of the arc already exists
"

%feature("docstring") gum::credal::CredalNet::addNodeWithId
"
Adds a discrete node into the network.

Parameters
----------
name : str
	The name of the discrete variable to be added
card : int
	The cardinality of the variable

Returns
-------
int
    The NodeId of the variable in the network

Raises
------
gum.DuplicateLabel
	If a node with the label already exists.
"

%feature("docstring") gum::credal::CredalNet::approximatedBinarization
"
Approximate binarization.

Each bit has a lower and upper probability which is the lowest - resp. highest - over all vertices of the credal set. Enlarge the orignal credal sets and may induce huge imprecision.

Warnings
--------
Enlarge the orignal credal sets and therefor induce huge imprecision by propagation. Not recommended, use MCSampling or something else instead
"

%feature("docstring") gum::credal::CredalNet::bnToCredal
"
Perturbates the BayesNet provided as input for this CredalNet by generating intervals instead of point probabilities and then computes each vertex of each credal set.

Parameters
----------
beta : double
	The beta used to perturbate the network
oneNet : bool
	used as a flag. Set to True if one BayesNet if provided with counts, to False if two BayesNet are provided; one with probabilities (the lower net) and one with denominators over the first modalities (the upper net)
keepZeroes : bool
	used as a flag as whether or not - respectively True or False - we keep zeroes as zeroes. Default is False, i.e. zeroes are not kept
"

%feature("docstring") gum::credal::CredalNet::computeCPTMinMax
"
Used with binary networks to speed-up L2U inference.

Store the lower and upper probabilities of each node X over the 'True' modality.
"

%feature("docstring") gum::credal::CredalNet::credalNet_currentCpt
"
Warnings
--------
Experimental function - Return type to be wrapped

Returns
-------
tbw
    a constant reference to the (up-to-date) CredalNet CPTs.
"

%feature("docstring") gum::credal::CredalNet::credalNet_srcCpt
"
Warnings
--------
Experimental function - Return type to be wrapped

Returns
-------
tbw
    a constant reference to the (up-to-date) CredalNet CPTs.
"

%feature("docstring") gum::credal::CredalNet::currentNodeType
"
Parameters
----------
id : int 
	The constant reference to the choosen NodeId

Returns
-------
pyAgrum.CredalNet
    the type of the choosen node in the (up-to-date) CredalNet __current_bn if any, __src_bn otherwise. 
"

%feature("docstring") gum::credal::CredalNet::current_bn
"
Returns
-------
pyAgrum.BayesNet
    Returs a constant reference to the actual BayesNet (used as a DAG, it's CPTs does not matter). 
"

%feature("docstring") gum::credal::CredalNet::domainSize
"
Parameters
----------
id : int
	The id of the node

Returns
-------
int
    The cardinality of the node 
"

%feature("docstring") gum::credal::CredalNet::epsilonMax
"
Returns
-------
double
    a constant reference to the highest perturbation of the BayesNet provided as input for this CredalNet. 
"

%feature("docstring") gum::credal::CredalNet::epsilonMean
"
Returns
-------
double
    a constant reference to the average perturbation of the BayesNet provided as input for this CredalNet. 
"

%feature("docstring") gum::credal::CredalNet::epsilonMin
"
Returns
-------
double
    a constant reference to the lowest perturbation of the BayesNet provided as input for this CredalNet. 
"

%feature("docstring") gum::credal::CredalNet::fillConstraint
"
Set the interval constraints of a credal set of a given node (from an instantiation index)

Parameters
----------
id : int 
	The id of the node
entry : int
	The index of the instantiation excluding the given node (only the parents are used to compute the index of the credal set)
ins : pyAgrum.Instantiation
	The Instantiation
lower : list
	The lower value for each probability in correct order
upper : list
	The upper value for each probability in correct order

Warnings
--------
You need to call intervalToCredal when done filling all constraints.

Warning
-------
DOES change the BayesNet (s) associated to this credal net ! 
"

%feature("docstring") gum::credal::CredalNet::fillConstraints
"
Set the interval constraints of the credal sets of a given node (all instantiations) 

Parameters
----------
id : int 
	The id of the node
lower : list
	The lower value for each probability in correct order
upper : list
	The upper value for each probability in correct order

Warnings
--------
You need to call intervalToCredal when done filling all constraints.

Warning
-------
DOES change the BayesNet (s) associated to this credal net !
"

%feature("docstring") gum::credal::CredalNet::get_CPT_max
"
Warnings
--------
Experimental function - Return type to be wrapped

Returns
-------
tbw
	a constant reference to the upper probabilities of each node X over the 'True' modality
"

%feature("docstring") gum::credal::CredalNet::get_CPT_min
"
Warnings
--------
Experimental function - Return type to be wrapped

Returns
-------
tbw
	a constant reference to the lower probabilities of each node X over the 'True' modality
"

%feature("docstring") gum::credal::CredalNet::hasComputedCPTMinMax
"
Returns
-------
bool
    True this CredalNet has called computeCPTMinMax() to speed-up inference with binary networks and L2U.
"

%feature("docstring") gum::credal::CredalNet::idmLearning
"
Learns parameters from a BayesNet storing counts of events.

Use this method when using a single BayesNet storing counts of events. IDM model if s > 0, standard point probability if s = 0 (default value if none precised).

Parameters
----------
s : int
	the IDM parameter.
keepZeroes : bool
	used as a flag as whether or not - respectively True or False - we keep zeroes as zeroes. Default is False, i.e. zeroes are not kept. 
"

%feature("docstring") gum::credal::CredalNet::instantiation
"
Get an Instantiation from a node id, usefull to fill the constraints of the network.

bnet accessors / shortcuts.

Parameters
----------
id : int
	the id of the node we want an instantiation from

Returns
-------
pyAgrum.Instantiation
    the instantiation 
"

%feature("docstring") gum::credal::CredalNet::intervalToCredal
"
Computes the vertices of each credal set according to their interval definition (uses lrs).

Use this method when using two BayesNet, one with lower probabilities and one with upper probabilities.
"

%feature("docstring") gum::credal::CredalNet::intervalToCredalWithFiles
"
Warnings
--------
Deprecated : use intervalToCredal (lrsWrapper with no input / output files needed).


Computes the vertices of each credal set according to their interval definition (uses lrs).

Use this method when using a single BayesNet storing counts of events. 
"

%feature("docstring") gum::credal::CredalNet::isSeparatelySpecified
"
Returns
-------
bool
	True if this CredalNet is separately and interval specified, False otherwise. 
"

%feature("docstring") gum::credal::CredalNet::lagrangeNormalization
"
Normalize counts of a BayesNet storing counts of each events such that no probability is 0.

Use this method when using a single BayesNet storing counts of events. Lagrange normalization. This call is irreversible and modify counts stored by __src_bn.

Doest not performs computations of the parameters but keeps normalized counts of events only. Call idmLearning to compute the probabilities (with any parameter value). 
"

%feature("docstring") gum::credal::CredalNet::addVariable
"
Parameters
----------
name : str
	the name of the new variable
card: int
    the domainSize of the new variable

Returns
-------
int
	the id of the new node
"

%feature("docstring") gum::credal::CredalNet::nodeType
"
Parameters
----------
id : int
	the constant reference to the choosen NodeId

Returns
-------
pyAgrum.CredalNet
	the type of the choosen node in the (up-to-date) CredalNet in __src_bn. 
"

%feature("docstring") gum::credal::CredalNet::saveBNsMinMax
"
If this CredalNet was built over a perturbed BayesNet, one can save the intervals as two BayesNet.

to call after bnToCredal(GUM_SCALAR beta) save a BN with lower probabilities and a BN with upper ones

Parameters
----------
min_path : str
	the path to save the BayesNet which contains the lower probabilities of each node X.
max_path : str
	the path to save the BayesNet which contains the upper probabilities of each node X.
"

%feature("docstring") gum::credal::CredalNet::setCPT
"
Warnings
--------
(experimental function) - Parameters to be wrapped


Set the vertices of one credal set of a given node (any instantiation index)

Parameters
----------
id : int
	the Id of the node
entry : int
	the index of the instantiation (from 0 to K - 1) excluding the given node (only the parents are used to compute the index of the credal set)
ins : pyAgrum.Instantiation
	the Instantiation (only the parents matter to find the credal set index) 
cpt	: tbw
	the vertices of every credal set (for each instantiation of the parents)

Warnings
--------
DOES not change the BayesNet(s) associated to this credal net !
"

%feature("docstring") gum::credal::CredalNet::setCPTs
"
Warnings
--------
(experimental function) - Parameters to be wrapped


Set the vertices of the credal sets (all of the conditionals) of a given node

Parameters
----------
id : int
	the NodeId of the node
cpt	: tbw 
	the vertices of every credal set (for each instantiation of the parents)

Warning
-------
DOES not change the BayesNet (s) associated to this credal net !
"

%feature("docstring") gum::credal::CredalNet::src_bn
"
Returns
-------
pyAgrum.BayesNet
    Returns a constant reference to the original BayesNet (used as a DAG, it's CPTs does not matter). 
"
