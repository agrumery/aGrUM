%feature("docstring") gum::BayesNet
"
BayesNet represents a Bayesian Network.

Available constructors:
    `BayesNet(name='') -> BayesNet`

    `BayesNet(source) -> BayesNet`

Parameters
----------
name: str
  the name of the Bayes Net (optional)
source : :class:BayesNet
  the Bayesian network to copy
"

%feature("docstring") gum::BayesNet::fastPrototype
"
"

%feature("docstring") gum::BayesNet_double::dag
"
Returns
-------
pyAgrum.DAG
  Returns the topology of the BayesNet as a pyAgrum.DAG.
"

