LazyPropagation is the lazy propagation of :cite:`madsen1999lazy`: a message passing in a junction tree, following the
Shafer-Shenoy architecture :cite:`shafer1990probability`, where the cliques and the messages are kept as sets of
factors, multiplied only when a variable has to be summed out. This page describes the junction tree and the messages
shared by all the exact inferences of pyAgrum.

**The junction tree.** It is rebuilt only when the targets or the set of observed variables change. pyAgrum first
removes the variables that cannot influence the targets: the barren nodes (neither targets nor observed, with no target
or observed descendant) and the nodes found irrelevant to the targets given the evidence by the Bayes-Ball algorithm
:cite:`shachter1998bayes`. The remaining graph is moralized, each joint target is made a clique, the observed variables
are removed (their CPTs are reduced to the observed values), and the graph is triangulated. The default triangulation
eliminates first the simplicial nodes, then the almost simplicial nodes whose clique does not exceed the largest clique
created so far :cite:`bodlaender2001preprocessing`, and otherwise the node that creates the clique of smallest weight
(the product of the domain sizes of its variables). The junction tree is then turned into a binary join tree
:cite:`shenoy1997binary`, and each CPT goes to a clique containing its family.

**The messages.** With :math:`\Phi_C` the set of factors of clique :math:`C` and :math:`S_{ij} = C_i \cap C_j`, the
message from :math:`C_i` to :math:`C_j` is

.. math::

   \Psi_{i\to j} = \sum_{C_i \setminus S_{ij}} \;\prod_{\phi \in \Phi_{C_i} \cup \bigcup_{k \neq j} \Psi_{k\to i}} \phi ,

computed lazily: the variables of :math:`C_i \setminus S_{ij}` are summed out one by one, first the one producing the
smallest table, each time from the product of the factors where it appears only, so that a message is in general a
set of small factors. A variable to be summed out that appears in a single factor and is not observed is summed out of
that factor alone (barren variable). Only the messages towards the cliques of the targets are computed (collect
phase); a posterior is the product of the factors of the clique and of its incoming messages, marginalized and
normalized. A soft evidence is a likelihood vector :math:`\lambda(x)` multiplied as one more factor; a hard evidence
removes the observed variable, as said above.

**Exact queries.** ``evidenceProbability`` multiplies the masses of the connected components and the constants left by
the evidence, with the irrelevance pruning disabled. ``mpe`` replaces the sum by a maximization
:cite:`dawid1992applications`, all the variables being targets, then reads the most probable configuration clique by
clique (ties are broken by the first value found).
