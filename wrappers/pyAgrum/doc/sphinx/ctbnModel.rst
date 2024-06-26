The CTBN model
^^^^^^^^^^^^^^

A CTBN is :
    - A bayesian network with :class:`pyAgrum.DiscreteVariable` random variables.
    - A :class:`pyAgrum.DiGraph` to represents dependency between random variables. The model allows cycles.
    - A :class:`pyAgrum.ctbn.CIM` containing exponential distribution parameters for each variable describing its transition duration.

A CIM is :
    - A Conditional Intensity Matrix. The CIM of a variable :math:`X` contains at least two variables :math:`X_i` and :math:`X_j`, from/to states.
    - Diagonal coefficients :math:`q_{i,j}`.
    - :math:`D_{i,j}` ~ :math:`exp(q_{i,j})` and :math:`D_i` ~ :math:`exp(\sum_jq_{i,j})`, waiting time before going from state `i` to state `j` (resp. before leaving state `i`).

Properties :
    - :math:`P_X(x_1\rightarrow x_2) = \frac{q_{i,j}}{q_i}`
    - :math:`\mathbb{E}(D_{i,j}) = \frac{1}{q_{i,j}}`
    - :math:`\forall i \sum_{j}q_{i,j} = 0`

.. autoclass:: pyAgrum.ctbn.CIM

.. autoclass:: pyAgrum.ctbn.CTBN

Other functions
---------------

.. autofunction:: pyAgrum.ctbn.randomCTBN