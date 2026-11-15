A clique graph is an undirected graph whose nodes are sets of variables (the cliques); the separator of an edge is the
intersection of its two cliques. It is a join tree (or junction tree) when it has no cycle and has the running
intersection property: for each variable, the cliques that contain it form a connected subgraph. The junction trees
of the local computations of :cite:`lauritzen1988local` and of the exact inferences of pyAgrum are such graphs (see
`pyagrum.JunctionTreeGenerator` and `pyagrum.LazyPropagation`).

Convention of pyAgrum: ``hasRunningIntersection`` checks the property on the whole graph, so that a variable contained
in cliques of two different connected components breaks it; a junction forest is valid only if its trees share no
variable.
