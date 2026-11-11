LazyPropagation is the lazy propagation of :cite:`madsen1999lazy`: a message passing in a junction tree, following the
Shafer-Shenoy architecture :cite:`shafer1990probability`, where the cliques and the messages are kept as sets of
factors, combined only when needed. By default, the barren nodes are removed and the factors relevant to each message
are selected with the Bayes-Ball algorithm :cite:`shachter1998bayes`. The most probable explanation (``mpe``) is
computed by the same propagation where the marginalization by a sum is replaced by a maximization
:cite:`dawid1992applications`.
