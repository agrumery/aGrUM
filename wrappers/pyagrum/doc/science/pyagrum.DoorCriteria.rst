DoorCriteria implements the two graphical criteria of :cite:`pearl1995causal` for the effect of a variable :math:`X`
on a variable :math:`Y` in a causal DAG.

- **Back-door**: a set :math:`Z` satisfies the back-door criterion if no node of :math:`Z` is a descendant of
  :math:`X` and :math:`Z` blocks every path between :math:`X` and :math:`Y` that starts with an arc into :math:`X`
  (d-separation of :math:`X` and :math:`Y` by :math:`Z` once the arcs out of :math:`X` are removed). Then
  :math:`P(y \mid do(x)) = \sum_z P(y \mid x, z)\,P(z)`.
- **Front-door**: a non-empty set :math:`Z` satisfies the front-door criterion if (i) it intercepts every directed
  path from :math:`X` to :math:`Y`, (ii) there is no unblocked back-door path from :math:`X` to :math:`Z`, and (iii)
  every back-door path from :math:`Z` to :math:`Y` is blocked by :math:`X` (checked for each node of :math:`Z`).
  Then :math:`P(y \mid do(x)) = \sum_z P(z \mid x) \sum_{x'} P(y \mid x', z)\,P(x')`.

**Enumeration of the back-door sets.** If :math:`X` has no parent, the only set returned is the empty one; if
:math:`Y` is a parent of :math:`X`, there is none. Otherwise the empty set is tested first, then the candidates are
the ancestors of :math:`X` or :math:`Y` that are neither :math:`X`, :math:`Y`, a descendant of :math:`X`, nor an
excluded node (the latent variables, when called from `pyagrum.CausalModel`). The sets are tried by increasing size
and, within a size, in the increasing order of the node ids. With ``only_minimal`` (default), a set is kept only if
it contains no set already found, so that only minimal sets are returned; ``max_cardinality`` bounds their size.

**Enumeration of the front-door sets.** If :math:`X` is a parent of :math:`Y`, there is none (a direct arc cannot be
intercepted). The candidates are the nodes on the directed paths from :math:`X` to :math:`Y`, minus the nodes reached
from :math:`X` by a back-door path open without conditioning (condition ii), the nodes with a back-door path to
:math:`Y` not blocked by :math:`X` (condition iii) and the excluded nodes; the sets are tried by increasing size and
kept when they intercept every directed path (condition i) and, with ``only_minimal``, when no node can be removed.
When there is no directed path from :math:`X` to :math:`Y` (no causal effect), each remaining candidate of the
connected component is returned alone.

**Which set is used.** ``firstBackdoor`` and ``firstFrontdoor`` return the first set met by the enumeration: a
smallest set, ties broken by the node ids. `pyagrum.CausalModel.backDoor` and ``frontDoor`` (used by
`pyagrum.causalImpact`) return the same set. The enumerations return all their sets sorted in the lexicographic order
of the node ids (:math:`\{0, 5\}` comes before :math:`\{2\}`).
