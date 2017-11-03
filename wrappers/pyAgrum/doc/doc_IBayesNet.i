%feature("docstring") gum::IBayesNet::dim
"
Returns the dimension (the number of free parameters) in this bayes net.

dim(G)=∑i∈nodes((ri−1)⋅qi) where ri is the number of instantiations of node i and qi is the number of instantiations of its parents. 
in IBayesNet

in IBayesNet

Warnings
--------
A Faire
"

%feature("docstring") gum::IBayesNet::jointProbability
"
in IBayesNet

Warnings
--------
A Faire
"

%feature("docstring") gum::IBayesNet::log2JointProbability
"
in IBayesNet

Warnings
--------
A Faire
"

%feature("docstring") gum::IBayesNet::maxNonOneParam
"
in IBayesNet

Warnings
--------
A Faire
"

%feature("docstring") gum::IBayesNet::maxParam
"
in IBayesNet

Warnings
--------
A Faire
"

%feature("docstring") gum::IBayesNet::maxVarDomainSize
"
in IBayesNet

Warnings
--------
A Faire
"

%feature("docstring") gum::IBayesNet::minNonZeroParam
"
in IBayesNet

Warnings
--------
A Faire
"

%feature("docstring") gum::IBayesNet::minParam
"
in IBayesNet

Warnings
--------
A Faire
"

%feature("docstring") gum::IBayesNet::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format 
"