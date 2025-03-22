The CLG model
^^^^^^^^^^^^^^

A CLG is :
    - A :class:`pyagrum.DiGraph` to represents dependency between random variables. The model does not allows cycles.
    - A dictionary id2var to map each NodeID to a :class:`pyagrum.clg.GaussianVariable` random variable.
    - A dictionary name2id to map each variable's name to its NodeID.
    - A dictionary arc2coef to map each arc to its coefficient.

A CLG is equivalent to a SEM (Structural Equation Model) with Gaussian variables.

.. autoclass:: pyagrum.clg.CLG
    :members:

.. autoclass:: pyagrum.clg.SEM
    :members:

Other functions for CLG
-----------------------

.. autofunction:: pyagrum.clg.randomCLG
