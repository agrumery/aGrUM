LoopyBeliefPropagation applies Pearl's message passing for polytrees, with its :math:`\pi` and :math:`\lambda`
messages along the arcs :cite:`pearl1988probabilistic`, to a Bayesian network with loops: the messages are updated
until they no longer change (or a limit of time or iterations is reached). The result is an approximation, often good
but without guarantee of convergence :cite:`murphy1999loopy`.
