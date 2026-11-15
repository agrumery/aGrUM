ShaferShenoyInference is the propagation of :cite:`shafer1990probability` in a junction tree: each clique sends to
each neighbour the product of its factors and of the messages received from its other neighbours, summed out onto
their separator, without any division. The junction tree (pruning of the barren and irrelevant nodes, triangulation,
binary join tree) and the formula of the messages are those of `pyagrum.LazyPropagation`.

What differs from `pyagrum.LazyPropagation` is that nothing is lazy: the CPTs (and the soft evidence) assigned to a
clique are multiplied once into a single table on the whole clique, and each message is a single table on its
separator. The memory used grows with the size of the cliques instead of the size of the factors, but a message
already computed is reused as is. The results are the same (``evidenceProbability`` included); ``mpe`` is not
available.
