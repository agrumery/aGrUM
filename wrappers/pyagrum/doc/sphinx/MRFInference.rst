Inference in Markov random fields
---------------------------------
Inference is the process that consists in computing new probabilistc information from a Markov random field and some evidence. aGrUM/pyAgrum mainly focus and the computation of (joint) posterior for some variables of the Markov random fields given soft or hard evidence that are the form of likelihoods on some variables.
Inference is a hard task (NP-complete). For now, aGrUM/pyAgrum implements only one exact inference for Markov random field.


Shafer-Shenoy Inference in Markov random field
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyagrum.ShaferShenoyMRFInference
			:exclude-members: setTriangulation
