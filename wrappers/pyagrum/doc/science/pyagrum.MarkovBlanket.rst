The Markov blanket of a node :math:`X` of a Bayesian network is the set of its parents, its children and the other
parents of its children: given its blanket, :math:`X` is independent of all the other variables
:cite:`pearl1988probabilistic`. MarkovBlanket returns it as a DAG made of :math:`X`, its blanket, the arcs into
:math:`X`, out of :math:`X` and into its children, plus the other arcs of the model between the nodes of the blanket
(from a parent to a co-parent, for instance), which ``toDot`` draws in grey.

With ``level`` :math:`k > 1` (an extension of pyAgrum), the same construction is repeated from the nodes added at the
previous level, :math:`k` times or until no node is added: level 2 adds the parents, children and co-parents of the
nodes of the blanket.
