Credal Network
==============

Credal networks are probabilistic graphical models based on imprecise probability. Credal networks can be regarded as an extension of Bayesian networks, where credal sets replace probability mass functions in the specification of the local models for the network variables given their parents. As a Bayesian network defines a joint probability mass function over its variables, a credal network defines a joint credal set (`from Wikipedia <https://en.wikipedia.org/wiki/Credal_network>`_).

**Tutorial**

* `Tutorial on Credal Networks <https://lip6.fr/Pierre-Henri.Wuillemin/aGrUM/docs/last/notebooks/credalNetworks.ipynb.html>`_

**Reference**

Model
-----

.. autoclass:: pyAgrum.CredalNet

Inference
---------

.. autoclass:: pyAgrum.CNMonteCarloSampling

.. autoclass:: pyAgrum.CNLoopyPropagation
			:exclude-members: asIApproximationSchemeConfiguration
