Inference in k-TBNs
---------------------
Inference in a k-TBN computes P(target[t] | observations, interventions) for
every declared target at every time slice, under any mix of soft/hard
observations and Pearl-style do-interventions -- without ever unrolling the
model. aGrUM/pyAgrum implements one exact algorithm, a generalization of
Murphy's interface algorithm to order k.

.. autoclass:: pyagrum.ktbn.KTBNInference
