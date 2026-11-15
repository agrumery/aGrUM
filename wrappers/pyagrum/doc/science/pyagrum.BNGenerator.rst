BNGenerator draws the DAG of a random Bayesian network with the mixed Markov chain (PMMixed) of
:cite:`ide2004generating`, on the connected DAGs with ``n_nodes`` nodes and at most ``n_arcs`` arcs (pyAgrum bounds the
number of arcs, not the induced width). The chain combines the moves of :cite:`ide2002random`: *add or remove* an arc
(a removal must keep the graph connected, an addition must keep it acyclic) and, for polytrees, *add and remove* (an
arc is added between two nodes and the arc of the path between them that would close a loop is removed), with the
*jump* of :cite:`ide2004generating`: from a polytree, an arc is added; from a multiconnected graph, an arc is removed.
pyAgrum starts from a random tree on which ``n_nodes``/2 arc reversals are applied, then runs 5000 steps. From a
polytree, a step uses *add or remove* with probability :math:`p = 0.3`, *add and remove* with probability
:math:`q = 0.4`, and a jump otherwise; from a multiconnected graph, it uses *add or remove* with probability
:math:`p + q` (a step back to a polytree being then accepted with probability :math:`p/(p+q)`), and a jump otherwise.
A change that exceeds ``n_arcs`` is rejected: the result has ``n_arcs`` arcs or slightly fewer for a sparse graph,
often clearly fewer for a dense one.

Each variable gets a domain size drawn uniformly between 2 and ``n_modmax``. As in :cite:`ide2002random`, each
distribution of a CPT (one per configuration of the parents) is drawn uniformly on the probability simplex, i.e. from a
Dirichlet distribution of parameter 1, by the sorted spacings of uniform draws.
