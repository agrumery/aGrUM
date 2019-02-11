%feature("docstring") JunctionTreeGenerator
"
JunctionTreeGenerator is use to generate junction tree or binary junction tree from bayesian networks.

Available constructors:
	``JunctionTreeGenerator() -> JunctionTreeGenerator``
"

%feature("docstring") JunctionTreeGenerator::junctionTree
"
Computes the junction tree for its parameters. If the first parameter is a graph, the heurisitcs assume that all the node have the same domain size (2). If given, the heuristic takes into account the partial order for its elimination order.

Parameters
----------
g : pyAgrum.UndiGraph
	a undirected graph

dag : pyAgrum.DAG
	a dag

bn : pyAgrum.BayesNet
	a BayesianNetwork

partial_order: List[List[int]]
	a partial order among the nodeIDs

Returns
-------
pyAgrum.CliqueGraph
	the current junction tree. 
"

%feature("docstring") JunctionTreeGenerator::binaryJoinTree
"
Computes the binary joint tree for its parameters. If the first parameter is a graph, the heurisitcs assume that all the node have the same domain size (2). If given, the heuristic takes into account the partial order for its elimination order.

Parameters
----------
g : pyAgrum.UndiGraph
	a undirected graph

dag : pyAgrum.DAG
	a dag

bn : pyAgrum.BayesNet
	a BayesianNetwork

partial_order: List[List[int]]
	a partial order among the nodeIDs

Returns
-------
pyAgrum.CliqueGraph
	the current binary joint tree 
"

%feature("docstring") JunctionTreeGenerator::eliminationOrder
"
Computes the elimination for its parameters. If the first parameter is a graph, the heurisitcs assume that all the node have the same domain size (2). If given, the heuristic takes into account the partial order for its elimination order.

Parameters
----------
g : pyAgrum.UndiGraph
	a undirected graph

dag : pyAgrum.DAG
	a dag

bn : pyAgrum.BayesNet
	a BayesianNetwork

partial_order: List[List[int]]
	a partial order among the nodeIDs

Returns
-------
pyAgrum.CliqueGraph
	the current elimination order.
"