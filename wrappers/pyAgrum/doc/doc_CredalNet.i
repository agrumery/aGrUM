%feature("docstring") gum::credal::CredalNet
"
TBD
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

Warnings
--------
	InvalidDirectedCircle raised if any (directed) cycle is created by this arc
	
Warnings
--------
	InvalidNode raised if head or tail does not belong to the graph nodes

Warnings
--------
	DuplicateElement raised if one of the arc already exists
"

%feature("docstring") gum::credal::CredalNet::addNode
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

Warnings
--------
DuplicateLabel raised if a node with the label already exists.
"

%feature("docstring") gum::credal::CredalNet::approximatedBinarization
"
Approximate binarization.

Each bit has a lower and upper probability which is the lowest - resp. highest - over all vertices of the credal set. Enlarge the orignal credal sets and may induce huge imprecision.

Warnings
--------
    Enlarge the orignal credal sets and therefor induce huge imprecision by propagation. Not recommanded, use MCSampling or something else instead. 
"

%feature("docstring") gum::credal::CredalNet::bnToCredal
"
Perturbates the BayesNet provided as input for this CredalNet by generating intervals instead of point probabilities and then computes each vertex of each credal set.

Parameters
----------
beta : double
	The beta used to perturbate the network. 0≤β∗≤1∗.
oneNet : bool
	used as a flag. Set to True if one BayesNet if provided with counts, to False if two BayesNet are provided; one with probabilities (the lower net) and one with denominators over the first modalities (the upper net).
keepZeroes : bool
	used as a flag as whether or not - respectively True or False - we keep zeroes as zeroes. Default is False, i.e. zeroes are not kept.
"

%feature("docstring") gum::credal::CredalNet::computeCPTMinMax
"
Used with binary networks to speed-up L2U inference.

Store the lower and upper probabilities of each node X over the 'true' modality.
"

%feature("docstring") gum::credal::CredalNet::credalNet_currentCpt
"
Returns
-------
    a constant reference to the ( up-to-date ) CredalNet CPTs.

Warnings
--------
Type de retour ?
"

%feature("docstring") gum::credal::CredalNet::credalNet_srcCpt
"
Returns
-------
    a constant reference to the ( up-to-date ) CredalNet CPTs.

Warnings
--------
Type de retour ?
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
    the type of the choosen node in the ( up-to-date ) CredalNet __current_bn if any, __src_bn otherwise. 
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
Set the interval constraints of a credal set of a given node ( from an instantiation index )

Parameters
----------
id : int 
	The id of the node
entry : int
	The index of the instantiation excluding the given node ( only the parents are used to compute the index of the credal set )
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
Set the interval constraints of the credal sets of a given node ( all instantiations ) 

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
Returns
-------
Returns a constant reference to the upper probabilities of each node X over the 'true' modality

Warnings
--------
Type de retour ?
"

%feature("docstring") gum::credal::CredalNet::get_CPT_min
"
Returns
-------
Returns a constant reference to the lower probabilities of each node X over the 'true' modality

Warnings
--------
Type de retour ?
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
Parameters
----------

Returns
-------

Warnings
--------
A faire
"

%feature("docstring") gum::credal::CredalNet::instantiation
"
Parameters
----------

Returns
-------

Warnings
--------
A faire
"

%feature("docstring") gum::credal::CredalNet::intervalToCredal
"
Parameters
----------

Returns
-------

Warnings
--------
A faire
"

%feature("docstring") gum::credal::CredalNet::intervalToCredalWithFiles
"
Parameters
----------

Returns
-------

Warnings
--------
A faire
"

%feature("docstring") gum::credal::CredalNet::isSeparatelySpecified
"
Parameters
----------

Returns
-------

Warnings
--------
A faire
"

%feature("docstring") gum::credal::CredalNet::lagrangeNormalization
"
Parameters
----------

Returns
-------

Warnings
--------
A faire
"

%feature("docstring") gum::credal::CredalNet::nodeType
"
Parameters
----------

Returns
-------

Warnings
--------
A faire
"

%feature("docstring") gum::credal::CredalNet::saveBNsMinMax
"
Parameters
----------

Returns
-------

Warnings
--------
A faire
"

%feature("docstring") gum::credal::CredalNet::setCPT
"
Parameters
----------

Returns
-------

Warnings
--------
A faire
"

%feature("docstring") gum::credal::CredalNet::setCPTs
"
Parameters
----------

Returns
-------

Warnings
--------
A faire
"

%feature("docstring") gum::credal::CredalNet::src_bn
"
Parameters
----------

Returns
-------

Warnings
--------
A faire
"