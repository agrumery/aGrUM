qBNRejection implements the quantum rejection sampling of :cite:`low2014quantum` on the circuit of
`pyagrum.qBNSampling.qBNMC`. Classical rejection sampling draws samples of the network and keeps those that agree
with the evidence :math:`e`, which needs about :math:`1/P(e)` draws per kept sample. Its quantum version amplifies the
amplitude of the states that agree with the evidence before measuring, with the Grover iterate

.. math::

   G = -A\, S_0\, A^{-1}\, S_e,

where :math:`A` is the circuit of `pyagrum.qBNSampling.qBNMC` (without measurement), :math:`S_e` flips the phase of the
states that agree with the evidence and :math:`S_0` flips the phase of the all-zero state (the global sign does not
change the measurements). One sample is drawn by Algorithm 1 of :cite:`low2014quantum`: for :math:`k = 0, 1, 2, \dots`
the state :math:`A|0\rangle` is prepared, :math:`G^{2^k}` is applied, and all the qubits are measured, until the
measured values of the evidence variables are the observed ones; the measured state is then an exact sample of
:math:`P(X \mid e)`. This needs :math:`O(P(e)^{-1/2})` applications of :math:`G` per sample instead of
:math:`O(P(e)^{-1})` draws :cite:`low2014quantum`. pyAgrum stops with an error when :math:`k` exceeds 21 (evidence
impossible or almost).

``makeInference`` draws ``maxIter`` samples (1000 by default) and the posterior of each variable is the frequency of its
values among them; the evidence is a hard evidence (one value per observed variable). ``useFragmentBN`` restricts the
circuit to the ancestors of the targets and of the evidence, the other variables having no effect on these
posteriors, which reduces the number of qubits. **Implementation limit.** As for `pyagrum.qBNSampling.qBNMC`, the
circuits are simulated on a classical computer: the speedup is the one of the number of operations of the algorithm,
not a gain of time, and the cost grows exponentially with the number of qubits.
