Random numbers from aGrUM
=========================

These functions use the aGrUM's peuso-random number generator (`mt19937`)from pyAgrum. It produces 32-bit pseudo-random numbers using the well-known and popular algorithm named Mersenne twister algorithm. `mt19937` stands for `m`ersenne `t`wister with a long period of 2`19937` – 1 which means mt19937 produces a sequence of 32-bit integers that only repeats itself after 219937 – 1 number have been generated.

Random functions
----------------

.. autofunction:: pyAgrum.initRandom

.. autofunction:: pyAgrum.randomProba

.. autofunction:: pyAgrum.randomDistribution

.. autofunction:: pyAgrum.generateSample

.. autofunction:: pyAgrum.randomBN
  :noindex:
