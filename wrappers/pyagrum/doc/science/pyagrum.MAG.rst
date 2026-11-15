A MAG (maximal ancestral graph) represents the independence model of a DAG over its observed variables, the latent
variables being marginalized and the selected ones conditioned on :cite:`richardson2002ancestral`. It is a marked
graph (`pyagrum.MarkedGraph`) without circle, with edges :math:`\to`, :math:`\leftrightarrow` and :math:`-`, which is

- ancestral: no node is anterior to one of its parents or spouses (anterior: linked to it by a path of edges
  :math:`-` and :math:`\to` pointing towards it), and a node with an undirected edge has neither parent nor spouse;
- maximal: two non adjacent nodes are m-separated by some set.

**m-separation** (``mSeparation``): :math:`X` and :math:`Y` are m-separated by :math:`Z` if no path between them has
all its non-colliders out of :math:`Z` and all its colliders anterior to :math:`Z` (a node of :math:`Z` being anterior
to :math:`Z`) :cite:`richardson2002ancestral`. It is computed by a linear search, and, as for the d-separation
(`pyagrum.DAG`), a node of :math:`X` or :math:`Y` that belongs to :math:`Z` is separated from any node. On a DAG it is
the d-separation.

**Construction from a DAG** (``fromDAG``, with latent nodes :math:`L` and selected nodes :math:`S`), following
:cite:`richardson2002ancestral`: two observed nodes :math:`a`, :math:`b` are adjacent iff they are not d-separated in
the DAG by :math:`\mathrm{an}(\{a,b\} \cup S) \setminus (L \cup \{a,b\})`, :math:`\mathrm{an}` including the nodes
themselves; the edge is :math:`a - b` if each one is an ancestor of the other or of :math:`S`, :math:`a \to b` if only
:math:`a` is, and :math:`a \leftrightarrow b` if none is.

**Representative of a PAG** (``fromPAG``): the circles of the edges :math:`\circ\!\!\to` become tails, the circles
of the edges :math:`-\!\!\circ` become arrowheads (both become :math:`\to`), and the edges
:math:`\circ\!\!-\!\!\circ` are oriented into a DAG without unshielded collider; the result is a MAG of the class of
the PAG :cite:`zhang2007characterization,zhang2008completeness`. The causal reasoning with MAGs and PAGs is studied in
:cite:`zhang2008causal`.
