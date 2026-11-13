# requires: pandas
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
ie = gclg.CLGVariableElimination(clg)
post = ie.posterior("C")
round(float(post.mu()), 4), round(float(post.sigma()), 4)
# (5.0, 2.2913)
ie.updateEvidence({"B": 4.0})
post = ie.posterior("C")
round(float(post.mu()), 4), round(float(post.sigma()), 4)
# (7.0, 1.0)
