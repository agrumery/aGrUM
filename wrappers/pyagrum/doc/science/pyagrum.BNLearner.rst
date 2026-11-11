BNLearner learns the structure and the parameters of a Bayesian network from data:

- score-based structure search: greedy hill climbing and local search with a tabu list :cite:`glover1989tabu` (as
  in :cite:`heckerman1995learning`), greedy thick-thinning, and K2, with a given order of the variables
  :cite:`cooper1992bayesian`;
- scores: AIC :cite:`akaike1974new`, BIC :cite:`schwarz1978estimating`, BD and BDeu
  :cite:`heckerman1995learning,buntine1991theory`, K2 :cite:`cooper1992bayesian` and the log2-likelihood;
- constraint-based structure learning: PC :cite:`spirtes1991algorithm,spirtes2000causation`, stable by default
  :cite:`colombo2014order`, with the orientation rules of :cite:`meek1995causal`; FCI
  :cite:`spirtes2000causation,zhang2008completeness`, whose PAG is turned into a DAG through its representative MAG
  :cite:`zhang2008completeness`; MIIC :cite:`verny2017learning`, with the MDL :cite:`rissanen1978modeling` or the NML
  correction (multinomial stochastic complexity computed as in :cite:`kontkanen2003efficient,kontkanen2007linear`);
- independence tests: chi2 and G2 :cite:`agresti2013categorical`;
- priors: smoothing (Laplace), Dirichlet and BDeu :cite:`heckerman1995learning,buntine1991theory`;
- parameters: maximum likelihood (or a posteriori with a prior), and EM with missing values
  :cite:`dempster1977maximum,lauritzen1995em`.
