Inference in CTNB
^^^^^^^^^^^^^^^^^

Exact Inference in CTBN
-----------------------

Amalgamation is used to compute exact inference. It consists in merging all the CIMs of a CTBN into one CIM.
Then we use the properties of markov process : :math:`P(X_t) = P(X_0)*exp(Q_Xt)` for enough time to notice convergence.

.. autoclass:: pyagrum.ctbn.SimpleInference

Sampling Inference in CTBN
--------------------------

Sampling method used is Forward Sampling.

Forward sampling works this way:
    - At time :math:`t` :
    - Draw a "time until next change" :math:`dt` value for each variable's exponential distribution of their waiting time.
    - Select the variable with smallest transition time : :math:`next time = t + dt`.
    - Draw the variable next state uniformly.
    - loop until :math:`t = timeHorizon`.

When doing forward sampling, we first iterate this process over a given amount of iterations (called burnIn). We consider the last values as the initial configuration for sampling. The reason is that the initial state of a CTBN is unknown without real data.

.. autoclass:: pyagrum.ctbn.ForwardSamplingInference
