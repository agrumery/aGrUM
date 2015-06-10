#!/usr/bin/python
# -*- coding: utf-8 -*-
import datetime
import pyAgrum as gum
import pyAgrum.lib.bn2csv as bn2csv

NBRNOEUDS=15
NBRARCS=20
MODMAX=7

NBRPARENTSMIN=6

NBCASELEARN=20000
NBCASETEST=10000

print("(*) generating BN")
gum.initRandom()

generator=gum.BNGenerator()
bn=generator.generate(NBRNOEUDS,NBRARCS,MODMAX)

CLASSE=bn.topologicalOrder()[-1]

print("(*) forcing node {0} to have at least {1} parents".format(CLASSE,NBRPARENTSMIN))
while (len(bn.parents(CLASSE))<NBRPARENTSMIN):
  nvparent=int(gum.randomProba()*bn.size())
  try:
    bn.addArc(nvparent,CLASSE)
  except gum.Exception:
    print("  Failed in {0} with {1}".format(str(bn.parents(0)),nvparent))

print("(*) re-generating CPTs")
bn.generateCPTs()

fileid="{0}-{1}-{2}-{3}".format(datetime.datetime.now().strftime("%Y%m%d-%H%M%S"),NBRNOEUDS,NBRARCS,MODMAX)

print("(*) saving bif file {0}".format(fileid))
gum.saveBN(bn,"BN{0}.bif".format(fileid))

print("(*) saving learning csv file {0}".format(fileid))
print
LLlearn=bn2csv.generateCSV(bn,"BN{0}-{1}.csv".format(fileid,NBCASELEARN),NBCASELEARN,True)

print("(*) saving test csv file {0}".format(fileid))
print
LLtest=bn2csv.generateCSV(bn,"BN{0}-{1}.csv".format(fileid,NBCASETEST),NBCASETEST,True)

print("classe : {0}".format(CLASSE))
print("Modalités : {0}".format(bn.variable(CLASSE)))
print("parents :{0}".format(str(bn.parents(CLASSE))))
print("log-likelihood (learning): {0}".format(LLlearn))
print("log-likelihood (testing): {0}".format(LLtest))

with open("BN{0}.txt".format(fileid), "w") as outfile:
  outfile.write("classe : {0}\n".format(CLASSE))
  outfile.write("Modalités : {0}\n".format(bn.variable(CLASSE)))
  outfile.write("parents :{0}\n".format(str(bn.parents(CLASSE))))
  outfile.write("log-likelihood (learning): {0}\n".format(LLlearn))
  outfile.write("log-likelihood (testing): {0}\n".format(LLtest))
