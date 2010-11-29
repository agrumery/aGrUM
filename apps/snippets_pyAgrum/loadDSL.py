# -*- coding: utf-8 -*-
import pyAgrum as gum

bn=gum.BayesNet()
bn.loadDSL("alarm.dsl")

print bn