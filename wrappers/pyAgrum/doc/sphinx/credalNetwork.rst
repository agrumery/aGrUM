Credal Network
==============

Credal networks are probabilistic graphical models based on imprecise probability. Credal networks can be regarded as an extension of Bayesian networks, where credal sets replace probability mass functions in the specification of the local models for the network variables given their parents. As a Bayesian network defines a joint probability mass function over its variables, a credal network defines a joint credal set (`from Wikipedia <https://en.wikipedia.org/wiki/Credal_network>`_).

**Tutorial**

* `Tutorial on Credal Networks <notebooks/24-Models_credalNetworks.ipynb>`_

**Reference**

CN Model
--------

.. autoclass:: pyAgrum.cn.CredalNet

CN Inference
------------

.. autoclass:: pyAgrum.cn.CNMonteCarloSampling

.. autoclass:: pyAgrum.cn.CNLoopyPropagation
			:exclude-members: asIApproximationSchemeConfiguration
