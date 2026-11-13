# requires: pandas
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
ie = gclg.CLGVariableElimination(clg)
ie.updateEvidence({"A": 3.0, "C": 7.0})
ie.eraseEvidence("A")
ie.hasEvidence("A")
# False
ie.nbrEvidence()
# 1
