Inference
---------
Inference is the process that consists in computing new probabilistc information from a Markov network and some evidence. aGrUM/pyAgrum mainly focus and the computation of (joint) posterior for some variables of the Markov networks given soft or hard evidence that are the form of likelihoods on some variables.
Inference is a hard task (NP-complete). For now, aGrUM/pyAgrum implements only one exact inference for Markov Network.


Shafer Shenoy Inference
~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.ShaferShenoyMNInference
			:exclude-members: setTriangulation
