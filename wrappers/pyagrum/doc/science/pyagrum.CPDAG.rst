A CPDAG (completed partially directed acyclic graph, or essential graph) represents a Markov equivalence class of
DAGs. Two DAGs are Markov equivalent iff they have the same skeleton and the same v-structures
:cite:`verma1990equivalence`. The CPDAG of a class has its skeleton, an arc for each compelled arc (with the same
orientation in all the DAGs of the class) and an undirected edge for each reversible one
:cite:`chickering1995transformational`.

**Characterization** :cite:`andersson1997characterization` (Theorem 4.1). A graph is the CPDAG of a class of DAGs iff
(i) it is a chain graph, (ii) its chain components are chordal, (iii) it has no induced :math:`a \to b - c`, and (iv)
each of its arcs :math:`a \to b` is strongly protected, i.e. occurs in one of these induced configurations:
:math:`c \to a \to b` with :math:`c`, :math:`b` non adjacent; :math:`a \to b \leftarrow c` with :math:`a`, :math:`c`
non adjacent; :math:`a \to c \to b` together with :math:`a \to b`; :math:`a - c_1 \to b` and :math:`a - c_2 \to b`
together with :math:`a \to b`, :math:`c_1` and :math:`c_2` non adjacent. Reversing the arc would create or destroy a
v-structure in the first two cases, a directed cycle in the last two.

**Construction.** ``fromDAG`` (or `pyagrum.graph.cpdag`) runs the algorithm Find-Compelled of
:cite:`chickering1995transformational` in linear time: the arcs are visited in the order of Order-Edges (heads by
increasing topological rank, then tails by decreasing rank) and labelled compelled or reversible. ``fromPDAG`` accepts
a PDAG iff it has a consistent extension whose CPDAG is the PDAG itself :cite:`wienobst2021extendability`, and
``contains(dag)`` checks that the CPDAG of the DAG is this one. A CPDAG is immutable: adding an arc to a CPDAG gives in
general a graph that is not a CPDAG.

**Separation.** ``dSeparation`` is the d-separation of any DAG of the class (they are Markov equivalent), computed on
a consistent extension kept from the construction. It is also the c-separation of the chain graph (see
`pyagrum.ChainGraph`): a DAG and its essential graph are Markov equivalent :cite:`andersson1997characterization`, the
c-separation is complete :cite:`studeny1998chain`, and on a chain graph without induced :math:`a \to b - c` the LWF and
AMP interpretations coincide :cite:`andersson1996alternative` (Theorem 4.2).
