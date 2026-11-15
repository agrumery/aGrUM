An MPDAG (maximally oriented PDAG) is a PDAG closed under the four Meek rules (stated in `pyagrum.MeekRules`). An
extendable PDAG :math:`G` represents the set :math:`[G]` of its consistent extensions (see `pyagrum.PDAG`); its closure
under the rules orients exactly the edges that have the same orientation in all the DAGs of :math:`[G]`
:cite:`meek1995causal` (Theorem 4). The typical MPDAG is a CPDAG (`pyagrum.CPDAG`) refined by arcs imposed by
background knowledge :cite:`perkovic2017interpreting`: ``fromCPDAG`` imposes the arcs, raises an error if no DAG of
the class contains them all (a directed cycle, for instance), and closes the result under the rules.

An MPDAG is not always a chain graph: imposing :math:`a \to b` in the undirected triangle :math:`a - b - c - a` orients
nothing else, which leaves the partially directed cycle :math:`a \to b - c - a`. ``meekClosure`` computes the closure
of an extendable PDAG with Algorithm 2 of :cite:`wienobst2021extendability`: from a consistent extension :math:`D`, it
starts from the CPDAG of :math:`D`, gives its undirected edges the orientation of the PDAG, then visits the nodes once
in a topological order of :math:`D`, orienting the edges into the current node by R1, R4, then R2 (R3 never applies
there). ``fromPDAG`` accepts a PDAG iff it is extendable and closed under the rules; an MPDAG is immutable.

The DAGs of :math:`[G]` are Markov equivalent (same skeleton and v-structures :cite:`verma1990equivalence`): ``cpdag``
returns the CPDAG of their class, and ``dSeparation`` is the d-separation of any of them, computed on a consistent
extension kept from the construction.
