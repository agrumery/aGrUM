Random numbers from aGrUM
=========================

These functions use the pseudo-random number generator of aGrUM (``mt19937``). It produces 32-bit pseudo-random numbers with the Mersenne twister algorithm: ``mt19937`` stands for Mersenne twister with a period of 2\ :sup:`19937` − 1, which means that the sequence of 32-bit integers only repeats itself after 2\ :sup:`19937` − 1 numbers have been generated.

Random functions
----------------

.. pyagrum-functions::

   pyagrum.initRandom
   pyagrum.randomGeneratorSeed
   pyagrum.randomProba
   pyagrum.randomNumpyGenerator
   pyagrum.randomGamma
   pyagrum.randomShuffle
   pyagrum.randomDistribution
   pyagrum.generateSample
   pyagrum.randomBN

Reproducible block of code
--------------------------

.. pyagrum-class:: pyagrum.RandomGeneratorGuard
