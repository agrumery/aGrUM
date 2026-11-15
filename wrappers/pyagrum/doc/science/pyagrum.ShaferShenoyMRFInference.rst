ShaferShenoyMRFInference is the propagation of :cite:`shafer1990probability`, described in
`pyagrum.ShaferShenoyInference` and `pyagrum.LazyPropagation`, applied to the factors of a Markov random field: since
the messages only multiply and sum out factors, they need no conditional probability.

What differs from the Bayesian networks: the junction tree is a triangulation of the undirected graph of the Markov
random field itself (with the joint targets made cliques and the observed variables removed); there is no barren node
and no d-separation pruning, each factor goes to a clique containing its variables, and the factors of a clique are
multiplied into one table as in `pyagrum.ShaferShenoyInference`. The posteriors are normalized, so that the partition
function :math:`Z = \sum_x \prod_f \phi_f(x_f)` is not needed for them; ``evidenceProbability`` returns
:math:`Z_e / Z`, where :math:`Z_e` is the same sum restricted by the evidence, :math:`Z` being computed once by a
propagation without evidence.
