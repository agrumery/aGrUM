A Markov random field factorizes a positive distribution as a product of factors over the cliques of an undirected
graph; by the Hammersley-Clifford theorem, these are exactly the positive distributions that satisfy the Markov
properties of the graph :cite:`besag1974spatial`. ``fromBN`` uses the conditional probability tables of a Bayesian
network as factors, so that the graph is the moral graph of the Bayesian network :cite:`lauritzen1988local`;
``toDotAsFactorGraph`` draws the model as a factor graph :cite:`kschischang2001factor`.
