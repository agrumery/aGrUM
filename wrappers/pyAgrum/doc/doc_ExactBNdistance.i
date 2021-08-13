%feature("docstring") gum::ExactBNdistance
"
Class representing exacte computation of divergence and distance between BNs

ExactBNdistance(P,Q) -> ExactBNdistance
    Parameters:
        * **P** (*pyAgrum.BayesNet*)
          a Bayesian network
        * **Q** (*pyAgrum.BayesNet*)
          another Bayesian network to compare with the first one

ExactBNdistance(ebnd) -> ExactBNdistance
    Parameters:
        * **ebnd** (*pyAgrum.ExactBNdistance*)
          the exact BNdistance to copy

Raises
------
pyAgrum.OperationNotAllowed
	If the 2BNs have not the same domain size of compatible node sets
"
