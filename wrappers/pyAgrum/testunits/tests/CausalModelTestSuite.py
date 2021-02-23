import sys
import os
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests
import pyAgrum.causal as csl


class TestCausalModel(pyAgrumTestCase):
  def test_tobacco1(self):
    obs1 = gum.fastBN("Smoking->Cancer")
    obs1.cpt("Smoking")[:] = [0.6, 0.4]
    obs1.cpt("Cancer")[{"Smoking": 0}] = [0.9, 0.1]
    obs1.cpt("Cancer")[{"Smoking": 1}] = [0.7, 0.3]

    modele1 = csl.CausalModel(obs1)
    lat, pot, expl = csl.causalImpact(
      modele1, "Cancer", "Smoking")  # when doing is 1
    self.assertEqual(pot, obs1.cpt("Cancer"))

    modele2 = csl.CausalModel(obs1, [("Genotype", ["Smoking", "Cancer"])])
    lat, pot, expl = csl.causalImpact(
      modele2, "Cancer", {"Smoking"}, values={"Smoking": 1})
    margCancer = (obs1.cpt("Smoking") * obs1.cpt("Cancer")
                  ).margSumOut(["Smoking"])
    self.assertEqual(pot, margCancer)

    modele3 = csl.CausalModel(
      obs1, [("Genotype", ["Smoking", "Cancer"])], True)
    lat, pot, expl = csl.causalImpact(
      modele3, "Cancer", {"Smoking"}, values={"Smoking": 1})
    self.assertIsNone(pot)

  def test_tobacco2(self):
    obs2 = gum.fastBN("Smoking->Tar->Cancer;Smoking->Cancer")
    obs2.cpt("Smoking")[:] = [0.6, 0.4]
    obs2.cpt("Tar")[{"Smoking": 0}] = [0.9, 0.1]
    obs2.cpt("Tar")[{"Smoking": 1}] = [0.7, 0.3]
    obs2.cpt("Cancer")[{"Tar": 0, "Smoking": 0}] = [0.9, 0.1]
    obs2.cpt("Cancer")[{"Tar": 1, "Smoking": 0}] = [0.8, 0.2]
    obs2.cpt("Cancer")[{"Tar": 0, "Smoking": 1}] = [0.7, 0.3]
    obs2.cpt("Cancer")[{"Tar": 1, "Smoking": 1}] = [0.6, 0.4]

    modele4 = csl.CausalModel(obs2, [("Genotype", ["Smoking", "Cancer"])])

    # from formula : \sum_{ Tar  }{ P(Tar\mid Smoking) \cdot
    #                               \sum_{ Smoking'  }{ P(Cancer\mid Smoking',Tar) \cdot P(Smoking') }
    #                             }
    margCancer = (obs2.cpt("Cancer") * obs2.cpt("Smoking")
                  ).margSumOut(['Smoking'])
    margCancer = (margCancer * obs2.cpt("Tar")).margSumOut(['Tar'])

    lat, pot, _ = csl.causalImpact(
      modele4, "Cancer", "Smoking")  # when smoking=1
    self.assertEqual(pot, margCancer)

  def test_simpson(self):
    m1 = gum.fastBN("Gender->Drug->Patient;Gender->Patient")

    # Gender = 0 == male
    # Gender = 1 == female

    # Drug == 0 - not taking the Drug
    # Drug == 1 - taking the Drug

    # Patient = 0 -- not healed
    # Patient = 1 - healed

    m1.cpt("Gender")[:] = [0.5, 0.5]
    m1.cpt("Drug")[:] = [[0.25, 0.75],  # Gender=0
                         [0.75, 0.25]]  # Gender=1

    m1.cpt("Patient")[{'Drug': 0, 'Gender': 0}] = [
      0.2, 0.8]  # No Drug, Male -> healed in 0.8 of cases
    # No Drug, Female -> healed in 0.4 of cases
    m1.cpt("Patient")[{'Drug': 0, 'Gender': 1}] = [0.6, 0.4]
    m1.cpt("Patient")[{'Drug': 1, 'Gender': 0}] = [
      0.3, 0.7]  # Drug, Male -> healed 0.7 of cases
    m1.cpt("Patient")[{'Drug': 1, 'Gender': 1}] = [
      0.8, 0.2]  # Drug, Female -> healed in 0.2 of cases

    d1 = csl.CausalModel(m1)

    # from formula : \sum_{ Gender  }{ P(Patient\mid Drug,Gender) \cdot P(Gender) }
    margPatient = (m1.cpt("Patient") * m1.cpt("Gender")).margSumOut(["Gender"])

    lat, pot, expl = csl.causalImpact(
      d1, on="Patient", doing={"Drug"})  # when drug=1
    self.assertEqual(pot, margPatient)

  def test_CRAN1(self):
    bn = gum.fastBN("w->x->z->y;w->z")

    bn.cpt("w")[:] = [0.5, 0.5]

    bn.cpt("x")[:] = [[0.4, 0.6], [0.3, 0.7]]

    bn.cpt("z")[{'w': 0, 'x': 0}] = [0.2, 0.8]
    bn.cpt("z")[{'w': 0, 'x': 1}] = [0.1, 0.9]
    bn.cpt("z")[{'w': 1, 'x': 0}] = [0.4, 0.6]
    bn.cpt("z")[{'w': 1, 'x': 1}] = [0.5, 0.5]

    bn.cpt("y")[:] = [[0.1, 0.9], [0.2, 0.8]]

    d = csl.CausalModel(bn, [("lat1", ["x", "y"])])

    # from the formula \sum_{ z ,w  }{ P(z\mid w,x) \cdot P(w) \cdot
    #                                 \sum_{ x'  }{ P(y\mid w,x',z) \cdot P(x'\mid w) }
    #                               }
    marg = (bn.cpt("x") * bn.cpt("y")).margSumOut(["x"])
    marg = (marg * bn.cpt("w") * bn.cpt("z")).margSumOut(["z", "w"])

    _, pot, _ = csl.causalImpact(d, on={"y"}, doing={"x"})  # when x=1
    self.assertEqual(pot, marg)
    _, pot, _ = csl.causalImpact(d, on="y", doing="x")  # when x=1
    self.assertEqual(pot, marg)

  def test_CRAN2(self):
    bn = gum.fastBN("Z1->X->Z2->Y")

    d = csl.CausalModel(bn, [("L1", ["Z1", "X"]),
                             ("L2", ["Z1", "Z2"]),
                             ("L3", ["Z1", "Y"]),
                             ("L4", ["Y", "X"])],
                        True)

    _, pot, _ = csl.causalImpact(d, on="Y", doing={"X"})

    self.assertIsNone(pot)

  def testFromNotebooks(self):
    bn = gum.fastBN("w->x->z->y;w->z")

    bn.cpt("w")[:] = [0.7, 0.3]

    bn.cpt("x")[:] = [[0.4, 0.6], [0.3, 0.7]]

    bn.cpt("z")[{'w': 0, 'x': 0}] = [0.2, 0.8]
    bn.cpt("z")[{'w': 0, 'x': 1}] = [0.1, 0.9]
    bn.cpt("z")[{'w': 1, 'x': 0}] = [0.9, 0.1]
    bn.cpt("z")[{'w': 1, 'x': 1}] = [0.5, 0.5]

    bn.cpt("y")[:] = [[0.1, 0.9], [0.8, 0.2]]

    d = csl.CausalModel(bn, [("lat1", ["x", "y"])])

    _, pot, _ = csl.causalImpact(d, on="y", doing="x")
    self.assertIsNotNone(pot)

  def testFromWHY19(self):
    bn = gum.fastBN("X->Y;U")
    d = csl.CausalModel(bn, [('Z', ["X", "Y", "U"])], True)
    _, pot, _ = csl.causalImpact(d, on="Y", doing="X")
    self.assertIsNone(pot)  # HedgeException

    # Causality, Pearl, 2009, p66
    bn = gum.fastBN("Z1->Z2->Z3->Y<-X->Z2;Z2->Y;Z1->X->Z3<-Z1")
    c = csl.CausalModel(bn, [("Z0", ("X", "Z1", "Z3"))], False)
    _, pot, explanation = csl.causalImpact(c, "Y", "X")
    self.assertIsNotNone(pot)
    self.assertEqual(explanation, "Do-calculus computations")

  def testCounterfactual(self):
    # experience=10-4*education+Ux
    # salaire=65+2.5*experience+5*education+Us
    #
    # Ux : [-2,10], sans a priori
    # Us : [0,25], sans a priori
    # education : 0,1,2 (low, medium, high), a priori [0.4,0.40.2]
    # experience : [0,20]
    # salaire : [65,150]

    edex = gum.fastBN(
      "Ux[-2,10]->experience[0,20]<-education{low|medium|high}->salary[65,150]<-Us[0,25];experience->salary")
    edex.cpt("Us").fillWith(1).normalize()
    edex.cpt("Ux").fillWith(1).normalize()
    edex.cpt("education")[:] = [0.4, 0.4, 0.2]
    edex.cpt("experience").fillWithFunction("10-4*education+Ux")
    edex.cpt("salary").fillWithFunction("round(65+2.5*experience+5*education+Us)")

    pot = csl.counterfactual(cm=csl.CausalModel(edex),
                             profile={'experience': 8, 'education': 'low', 'salary': '86'},
                             whatif={"education"},
                             on={"salary"},
                             values={"education": "medium"})

    self.assertEqual(pot[81 - 65], 1.0)


ts = unittest.TestSuite()
addTests(ts, TestCausalModel)
