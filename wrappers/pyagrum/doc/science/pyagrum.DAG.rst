A DAG is a directed graph without directed cycle. Its independence model is the d-separation
:cite:`pearl1988probabilistic`: :math:`X` and :math:`Y` are d-separated by :math:`Z` if every path between them
contains either a non-collider that belongs to :math:`Z`, or a collider (:math:`\to v \leftarrow`) that is not in
:math:`Z` and has no descendant in :math:`Z`. ``dSeparation`` decides it in linear time with the Bayes-Ball algorithm
:cite:`shachter1998bayes`. Convention of pyAgrum: :math:`X` and :math:`Y` must be disjoint, and a node of :math:`X` or
:math:`Y` that belongs to :math:`Z` is observed, hence separated from any node (the result is the one for
:math:`X \setminus Z` and :math:`Y \setminus Z`).

``moralGraph`` links the parents of each node and drops the directions; ``moralizedAncestralGraph(S)`` does the same on
the subgraph induced by :math:`S` and its ancestors (the ancestors of a node do not include the node itself).
``minimalCondSet(target, S)`` is the part of :math:`S` that matters for the target,
:math:`P(\mathit{target} \mid S) = P(\mathit{target} \mid S')`: it is read from the graph alone, by a Bayes-Ball-like
traversal from the target that stops at the nodes of :math:`S`, so it holds for every distribution of the DAG.
