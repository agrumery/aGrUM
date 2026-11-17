qBNMC encodes a Bayesian network as a quantum circuit, following :cite:`borujeni2021quantum`, so that measuring all
the qubits draws a sample of the joint distribution. A variable with :math:`d` values is represented by
:math:`\lceil \log_2 d \rceil` qubits, a value by its binary code (the codes beyond :math:`d - 1` get a null
probability). The probabilities are loaded by rotations: the gate :math:`R_Y(\theta)` maps :math:`|0\rangle` to
:math:`\cos(\theta/2)|0\rangle + \sin(\theta/2)|1\rangle`, so a qubit gets the probabilities :math:`P_0, P_1` with

.. math::

   \theta = 2 \arctan \sqrt{P_1 / P_0} \qquad (\theta = \pi \text{ when } P_0 = 0).

The variables are encoded in a topological order. The first qubit of a variable is rotated with the probability of its
bit, each following qubit by rotations controlled by the preceding ones, with the conditional probability of its bit
given theirs. A variable with parents receives one such block per configuration of its parents, controlled by the
qubits of the parents (X gates around the block select the configuration). The circuit therefore has a number of
controlled rotations proportional to the size of the conditional probability tables.

``runBN`` builds and measures the circuit ``shots`` times (10000 by default) on the Aer simulator of Qiskit, and the
marginals are the frequencies of the values among the shots: a Monte Carlo estimate, whose precision depends on the
number of shots (see `pyagrum.MonteCarloSampling` for the classical forward sampling it reproduces). The shots are
seeded by the random generator of pyAgrum. **Implementation limit.** The circuit is simulated on a classical computer,
whose cost grows exponentially with the total number of qubits: only small networks can be handled.
