A marked graph has at most one edge between two nodes, with a mark at each end: a tail, an arrowhead or a circle. The
tails and arrowheads give the directed (:math:`\to`), bidirected (:math:`\leftrightarrow`) and undirected (:math:`-`)
edges of the ancestral graphs :cite:`richardson2002ancestral` (`pyagrum.MAG`); the circles are the marks that vary in
a Markov equivalence class of such graphs, in the partial ancestral graphs :cite:`zhang2008completeness`
(`pyagrum.PAG`).

Conventions of pyAgrum: ``markAt(x, y)`` is the mark at the :math:`y` end of the edge :math:`x - y`; an edge added
without marks is :math:`x \circ\!\!-\!\!\circ\, y`. The directed view is read from the marks: the parents of :math:`x`
are the nodes :math:`y` with :math:`y \to x`, its spouses the nodes with :math:`y \leftrightarrow x`, and a node
:math:`z` is a definite collider on :math:`x - z - y` when both edges have an arrowhead at :math:`z`. A marked graph
checks no other property.
