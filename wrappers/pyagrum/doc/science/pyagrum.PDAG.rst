A PDAG (partially directed acyclic graph) has arcs and undirected edges, at most one connection between two nodes and
no directed cycle (a cycle made of arcs only, followed in their direction): the definition used for the orientation of
partially directed graphs in causal discovery :cite:`meek1995causal,chickering2002optimal,wienobst2021extendability`.
A cycle that mixes arcs and edges is allowed: a PDAG without such a partially directed cycle is a chain graph
(`pyagrum.ChainGraph`), and the closure of a PDAG under the Meek rules (`pyagrum.MeekRules`) is a PDAG, not always a
chain graph.

A PDAG stands for the set of its consistent extensions: the DAGs with its skeleton and its v-structures (induced
:math:`x \to y \leftarrow z`, :math:`x` and :math:`z` non adjacent) which contain all its arcs. It is extendable if this
set is not empty. An extension is built by removing, one after the other, a potential sink (a node without child whose
neighbours through an edge form a clique and are adjacent to all its parents) and orienting its edges towards it
:cite:`dor1992simple`; `pyagrum.graph.consistentExtension` does it in :math:`O(n + dm)` expected time, :math:`d` being
the degeneracy of the skeleton :cite:`wienobst2021extendability`. A PDAG without arc is extendable iff it is chordal,
and a PDAG without edge (a DAG) is always extendable. The CPDAGs (`pyagrum.CPDAG`) and the MPDAGs (`pyagrum.MPDAG`) are
the PDAGs that represent these sets exactly.

``addArc`` and ``addEdge`` reject what would break the definition; ``addArc`` searches a directed path, so it is not in
constant time.
