KTBNLearner splits the learning of a k-TBN into independent Bayesian network learning problems, as
:cite:`friedman1998learning` does for the initial and transition networks of a 2-TBN: the transition network on
sliding windows of k slices, the initial slices on the first k-1 time steps, and the atemporal variables, each solved
with the algorithms, scores and priors of :class:`pyagrum.BNLearner`.
