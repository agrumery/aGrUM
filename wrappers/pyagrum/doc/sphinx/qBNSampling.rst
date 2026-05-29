Quantum Bayesian Network Sampling
==================================

``pyagrum.qBNSampling`` provides a quantum-circuit encoding of Bayesian networks
and a quantum rejection-sampling inference engine, built on top of
`Qiskit <https://www.ibm.com/quantum/qiskit>`_ and the Aer simulator.

The module contains two classes:

- :class:`~pyagrum.qBNSampling.qBNMC` — encodes a :class:`~pyagrum.BayesNet` as a
  quantum circuit so that measuring the circuit samples from the network's joint
  distribution.
- :class:`~pyagrum.qBNSampling.qBNRejection` — runs quantum rejection sampling on
  top of a :class:`~pyagrum.qBNSampling.qBNMC` circuit to compute posterior
  distributions conditioned on evidence.

.. code-block:: python

   import pyagrum as gum
   import pyagrum.qBNSampling as qBNS

   bn = gum.loadBN("asia.bif")

   # Build the quantum circuit encoding
   qbn = qBNS.qBNMC(bn)
   marginals = qbn.runBN(shots=10000)   # dict[str, Tensor]

   # Quantum rejection-sampling inference
   ie = qBNS.qBNRejection(qbn)
   ie.setEvidence({"dyspnoea": 1})
   ie.makeInference()
   print(ie.posterior("bronchitis"))


qBNMC — Circuit encoding
--------------------------

Based on :cite:t:`borujeni2020quantum`.

Each variable in the Bayesian network is mapped to ⌈log₂(domainSize)⌉ qubits
(equation 21 of the paper). The CPT of each node is encoded via multi-qubit
RY rotations (Section 3.2, Fig. 5):

- root nodes receive unconditional rotations;
- non-root nodes receive controlled rotations, one block per parent
  configuration, with X-gate framing to select the correct control state.

Measuring all qubits of the resulting circuit samples from the joint
distribution of the network.

.. autoclass:: pyagrum.qBNSampling.qBNMC
   :members:
   :undoc-members:
   :show-inheritance:


qBNRejection — Quantum inference
----------------------------------

Based on :cite:t:`low2014quantum`.

Inference is performed via quantum rejection sampling with Grover-based
amplitude amplification (Algorithm 1 of the paper). The key operators are:

- **A** — the sample-preparation circuit (the :class:`~pyagrum.qBNSampling.qBNMC`
  circuit without measurement).
- **G = S_e A⁻¹ S₀ A** — the Grover iterate, where S_e is a phase flip on
  the evidence qubits and S₀ is a phase flip on the all-zero state.

Each call to :meth:`~pyagrum.qBNSampling.qBNRejection.getSample` runs
Algorithm 1: it applies G^{⌈2^k⌉} for increasing k until a measurement
consistent with the evidence is obtained.

.. autoclass:: pyagrum.qBNSampling.qBNRejection
   :members:
   :undoc-members:
   :show-inheritance:
