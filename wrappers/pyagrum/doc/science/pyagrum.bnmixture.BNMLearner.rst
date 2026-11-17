BNMLearner learns a `pyagrum.bnmixture.BNMixture` from several databases on the same variables: one Bayesian network
per database, learned by a `pyagrum.BNLearner`, with the weight given by the user to this database. The reference
network is the one learned from the database of highest weight.

Each database has its own learning settings (algorithm, score, correction, prior). The reference database starts
from the default settings of `pyagrum.BNLearner`, the other ones from the settings of the reference; the settings
given for a database then replace these ones, the missing ones being kept. Identical learned networks are merged,
their weights being added (see `pyagrum.bnmixture.BNMixture`). The inference in the mixture averages the networks,
their weights being updated by the evidence as in Bayesian model averaging :cite:`hoeting1999bayesian` (see
`pyagrum.bnmixture.BNMixtureInference`).
