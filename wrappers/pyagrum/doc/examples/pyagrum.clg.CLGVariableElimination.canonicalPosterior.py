# requires: pandas
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
ie = gclg.CLGVariableElimination(clg)
cf = ie.canonicalPosterior(["A", "B"])  # joint posterior density of A and B
cf.scope()
# [0, 1]
scope, mu, sigma = cf.toGaussian()  # mean vector and covariance matrix
sigma.round(4)
# array([[1.  , 0.5 ],
#        [0.5 , 4.25]])
