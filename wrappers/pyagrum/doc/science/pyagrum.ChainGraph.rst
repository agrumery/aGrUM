A chain graph is a graph with arcs and undirected edges, at most one connection between two nodes and no partially
directed cycle (a cycle with at least one arc, every arc followed in its direction and every edge in either direction)
:cite:`lauritzen1989graphical,frydenberg1990chain,andersson2001alternative`. Equivalently, the connected components of
its undirected part (the chain components) are linked by arcs that form a DAG over the components. DAGs and
undirected graphs are chain graphs; a PDAG (`pyagrum.PDAG`) may have partially directed cycles.

The independence model of a chain graph depends on its interpretation: LWF, AMP or MVR :cite:`sonntag2015chain`. pyAgrum
follows the LWF interpretation :cite:`lauritzen1989graphical,frydenberg1990chain`. ``moralGraph`` links all the parents
of each chain component together and drops the directions. ``cSeparation`` applies the moralization criterion: :math:`X`
and :math:`Y` are separated by :math:`Z` if every path between them in the moral graph of the smallest set containing
:math:`X \cup Y \cup Z` and closed under parents and neighbours (``moralizedAncestralGraph``) meets :math:`Z`. This
criterion is equivalent to the c-separation of :cite:`bouckaert1995chain,studeny1998chain`, which reads it on the paths
of the chain graph itself, and both are complete: for every chain graph, some positive discrete distribution has exactly
the independences displayed by the graph :cite:`studeny1998chain`. As for the d-separation (`pyagrum.DAG`), a node of
:math:`X` or :math:`Y` that belongs to :math:`Z` is separated from any node. On a DAG, the c-separation is the
d-separation.
