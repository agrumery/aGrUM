JunctionTreeGenerator builds the junction trees of the exact inferences (described in `pyagrum.LazyPropagation`),
for a graph given by the user: the moral graph of a Bayesian network or of a DAG (the parents of each node are linked
and the arcs become edges), the graph of a Markov random field, or an undirected graph, is triangulated by
eliminating its nodes one by one, each elimination linking the remaining neighbours of the node (the fill-ins); the
cliques created by the eliminations, minus those included in another one, are linked into a junction tree whose
separators are the intersections of adjacent cliques :cite:`lauritzen1988local`.

**The elimination order** (``eliminationOrder``) is the one of the default triangulation of pyAgrum (see
`pyagrum.LazyPropagation`): a greedy choice guided by the weight of a node, the product of the domain sizes of the
node and of its remaining neighbours, i.e. the size of the table of the clique that its elimination creates. The
domain sizes are those of the variables for a Bayesian network or a Markov random field; for a graph or a DAG, every
node counts for 2, so that the weight of a node only depends on its number of neighbours. With a partial order (a
list of sets of nodes, which must cover all the nodes), all the nodes of a set are eliminated before those of the
next one, the same rules choosing inside each set.

**The binary join tree** (``binaryJoinTree``) is the junction tree transformed so that no clique has more than three
neighbours :cite:`shenoy1997binary`: around a clique with too many neighbours, new cliques are inserted, each one
joining two separators, the pair whose union has the smallest table being joined first.
