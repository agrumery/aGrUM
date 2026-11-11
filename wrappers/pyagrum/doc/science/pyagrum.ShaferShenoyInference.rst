ShaferShenoyInference is the propagation of :cite:`shafer1990probability` in a junction tree: each clique sends to
each neighbour the product of its factors and of the messages received from its other neighbours, marginalized on
their separator, without division. As in :cite:`madsen1999lazy`, the factors are kept as sets, the barren nodes are
removed and the factors relevant to each message are selected with the Bayes-Ball algorithm
:cite:`shachter1998bayes`.
