# requires: pandas
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
clg.eraseArc(clg.idFromName("A"), clg.idFromName("B"))
clg.existsArc("A", "B")
# False
