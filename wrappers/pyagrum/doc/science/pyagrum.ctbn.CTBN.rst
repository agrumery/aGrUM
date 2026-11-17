A continuous-time Bayesian network (CTBN) :cite:`nodelman2002continuous` describes a process with finitely many states
evolving over continuous time. Each variable :math:`X` is a continuous-time Markov process whose transitions depend
only on the current values :math:`u` of its parents in a directed graph, which may contain cycles (an arc
:math:`Y \rightarrow X` only says that the speed and the targets of the transitions of :math:`X` depend on the current
value of :math:`Y`). The dynamics of :math:`X` is given by its conditional intensity matrices :math:`Q_{X \mid u}`,
one per configuration of its parents (see `pyagrum.ctbn.CIM`). Two variables never change at the same instant.

The CTBN is a factored representation of a single homogeneous Markov process over the joint state space, whose
intensity matrix is the amalgamation of all the CIMs :cite:`nodelman2002continuous`:

.. math::

   Q_{\mathbf{X}} = \mathop{\ast}_{X} Q_{X \mid \mathrm{Pa}(X)},

of size :math:`\prod_X |\mathrm{Val}(X)|`: its entry for a change of the single variable :math:`X` from :math:`x` to
:math:`x'` is :math:`q_{xx' \mid u}`, with :math:`u` read in the current state, its entries for simultaneous changes of
several variables are 0, and its diagonal is the sum of the diagonals :math:`-\sum_X q_{x \mid u}`. Then
:math:`P(\mathbf{X}(t)) = P(\mathbf{X}(0))\, \exp(t\, Q_{\mathbf{X}})`.

**Implementation limit.** Unlike :cite:`nodelman2002continuous`, the model holds no initial distribution
:math:`P(\mathbf{X}(0))`: the inferences start from the uniform distribution (`pyagrum.ctbn.SimpleInference`,
`pyagrum.ctbn.ForwardSamplingInference`). Learning is done by `pyagrum.ctbn.Learner`.
