Learning a CTBN
^^^^^^^^^^^^^^^

One of the main features of this library is the possibility to learn a CTBN.

More precisely what can be learned is :
    - The dependency graph of a CTBN
    - The CIMs of a CTBN
    - (The variables and their labels from a sample)

Tools to extract data from samples are necessary. This is the role of class :class:`pyAgrum.ctbn.Trajectory` and function :func:`pyAgrum.ctbn.CTBNFromData`.

Before introducing the algorithms, here are the following definitions :
    - :math:`M_{xx'|u}` is the number of time a variable `X` go from a state `x` to a state `x'`, conditioned by an instance of its parents `u`. It is filled using samples.
    - :math:`M_{x|u}` is the number of time `X` goes to state `x`.
    - :math:`T_{x|u}` is the time spent in state `x`, conditioned by an instance of its parents `u`.
    - :math:`M_{xx'|y,u}` and :math:`T_{x|y,u}` are the same but with another conditioning variable `Y` in state `y`.
Those can be stored in :class:`pyAgrum.Tensor`.

Being conditioned by an instance means that the extracted data comes from time intervals where conditioning variables take specific values.

Learning parameters : learning the CIMs
---------------------------------------

Goal : finding the :math:`q_{i,j|u}` (i.e :math:`q_{x|u}` and :math:`q_{x \rightarrow x'|u}`) coefficients.

Idea : :math:`q_{x|u}` = :math:`\frac{M_{x|u}}{T_{x|u}}`;  :math:`P_X(x\rightarrow x') = \frac{M_{x \rightarrow x'|u}}{M_{x|u}} = \frac{q_{x \rightarrow x'|u}}{q_{x|u}}`
Then :math:`q_{x \rightarrow x'|u} = \frac{M_{x \rightarrow x'|u}}{T_{x|u}}`

Learning the graph
------------------

To learn the graph of a CTBN (ie the dependence between variables) we use the CTPC algorithm
from A. Bregoli, M. Scutari, F. Stella, Constraint-Based Learning for Continuous-Time Bayesian Networks, arXiv:2007.03248, 2020.
The independence test used is based on Fisher and chi2 tests to compare exponential distributions.

.. autoclass:: pyAgrum.ctbn.Learner

.. autofunction:: pyAgrum.ctbn.readTrajectoryCSV

.. autofunction:: pyAgrum.ctbn.CTBNFromData

.. autofunction:: pyAgrum.ctbn.computeCIMFromStats

.. autoclass:: pyAgrum.ctbn.Trajectory

.. autoclass:: pyAgrum.ctbn.Stats

.. automodule:: pyAgrum.ctbn.StatsIndepTest
    :members:
    :undoc-members:
    :show-inheritance:
