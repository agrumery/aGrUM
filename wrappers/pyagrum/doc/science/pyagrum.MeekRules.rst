MeekRules applies the four orientation rules of :cite:`meek1995causal`. Each rule orients an undirected edge
:math:`a - b` into :math:`a \to b` when the other orientation would create a new v-structure or a directed cycle in
every consistent extension (see `pyagrum.PDAG`):

- **R1**: :math:`c \to a - b`, with :math:`c` and :math:`b` non adjacent;
- **R2**: :math:`a \to c \to b` and :math:`a - b`;
- **R3**: :math:`a - c \to b` and :math:`a - d \to b`, with :math:`c` and :math:`d` non adjacent;
- **R4**: :math:`a - c \to d \to b`, with :math:`a` adjacent to :math:`d` and :math:`c`, :math:`b` non adjacent.

The closure of an extendable PDAG under these rules is unique: it orients exactly the edges that have the same
orientation in all its consistent extensions, its maximally oriented PDAG (`pyagrum.MPDAG`). When its arcs are those of
its v-structures, R1-R3 suffice and the closure is the CPDAG of its extensions (`pyagrum.CPDAG`)
:cite:`meek1995causal` (Theorems 3 and 4). For such an input, ``propagate`` and ``propagateToCPDAG`` compute the
closure in a single pass over a topological order of a consistent extension (Algorithm 2 of
:cite:`wienobst2021extendability`).

**Input not extendable** (a constraint-based learner may produce one from conflicting tests). The rules are applied
node after node until nothing changes, so that the result depends on the order of the nodes; an edge that receives
both orientations becomes a double arc, recorded in ``choices``. ``propagateToCPDAG`` then keeps, for each double arc,
the orientation of an existing directed path between its ends; without such a path, the arc into the node with fewer
parents (then fewer neighbours); with a path in both directions, both arcs are erased. An arc doubled by an edge keeps
the arc. Only these 2-cycles are resolved: a longer directed cycle raises an error.

**Orientation into a DAG** (``propagateToDAG``). The closure is oriented with small conditional probability tables:
the total size :math:`\sum_X |X| \prod_{P \in pa(X)} |P|` of the CPTs is minimized, :math:`|X|` being the domain size
given for :math:`X` (all equal by default). In the exact case, an extendable input whose closure gives the same parents
to all the nodes of an undirected component (always true for a CPDAG), each component :math:`H` is a connected chordal
graph oriented independently. Every orientation of :math:`H` without v-structure starts with a maximal clique
:math:`K`, after which the remaining components :math:`C` are again independent; the recursion of the counting
algorithm Clique-Picking :cite:`wienobst2021polynomial,wienobst2023polynomial` is used here for a minimum:

.. math::

   \mathrm{cost}(H) = \min_{K} \Big[\mathrm{cost}(K) + \sum_{C} \Big(\prod_{P \in pa(C)} |P|\Big)\,\mathrm{cost}(C)\Big],

memoized on the components, the nodes of :math:`K` being ordered by increasing domain size. The components with more
than ``pyagrum.config["graph", "orientAsDAGExactMaxSize"]`` nodes (100 by default), or without common parents
(background knowledge), are oriented by the algorithm of :cite:`dor1992simple`, which removes at each step the
potential sink with the smallest CPT: a consistent extension, with no guarantee of minimality. A non extendable input
gets new v-structures (a node without child is then removed as if it were a sink). The orientations of the edges
left undirected by ``propagateToCPDAG`` are added to ``choices``.
