The essential graph of a Bayesian network is the CPDAG of the Markov equivalence class of its DAG
:cite:`andersson1997characterization`: its arcs are the compelled ones, which have the same orientation in all the DAGs
of the class, and its undirected edges the reversible ones. Built from a model, it is computed by the algorithm
Find-Compelled of :cite:`chickering1995transformational`; built from a PDAG, the PDAG is checked to be a CPDAG. Its
characterization and its separation are those of `pyagrum.CPDAG`; it only adds the names of the variables, kept in the
graph, so that it does not depend on the model it was built from.
