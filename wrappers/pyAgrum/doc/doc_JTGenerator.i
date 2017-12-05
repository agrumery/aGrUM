%feature("docstring") JTGenerator
"
JTGenerator is use to generate junction tree or binary junction tree from bayesian networks.

Available constructors:
	``JTGenerator() -> JTGenerator``
"

%feature("docstring") JTGenerator::junctionTree
"
Parameters
----------
g : pyAgrum.UndiGraph
	an Undirected graph
bn : pyAgrum.BayesNet
	a Bayesian network

Returns
-------
pyAgrum.CliqueGraph
	the current junction tree
"

%feature("docstring") JTGenerator::binaryJoinTree
"
Parameters
----------
g : pyAgrum.UndiGraph
	an Undirected graph
bn : pyAgrum.BayesNet
	a Bayesian network

Returns
-------
pyAgrum.CliqueGraph
	the binary join tree
"