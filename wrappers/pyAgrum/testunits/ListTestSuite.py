# -*- encoding: UTF-8 -*-

#import unittest
#from pyAgrum import LabelizedVar, Potential, Lis
#from pyAgrumTestSuite import pyAgrumTestCase


#class ListTestCase(pyAgrumTestCase):

    #def setUp(self):
        #a, b = [LabelizedVariable(name, name, 2) for name in 'a b'.split()]

        #self.pot1 = Potential()

        #self.pot2 = Potential()
        #self.pot2.add(a)
        #self.pot2[:] = [0.4, 0.6]

        #self.pot3 = Potential()
        #self.pot3.add(b)
        #self.pot3[:] = [0, 1]



#class TestConstructors(ListTestCase):

    #def testConstructor(self):
        #list_pot = ListPotentials()


    #def testCopyConstructor(self):
        #list_pot = ListPotentials()
        #list_pot2 = ListPotentials(list_pot)
        #self.assertEqual(list_pot, list_pot2)



#class TestAssessors(ListTestCase):

    #def testInsertion(self):
        #list_pot = ListPotentials()
        #self.assertTrue(list_pot.empty())

        #list_pot.append(self.pot1)
        #self.assertFalse(list_pot.empty())
        #list_pot.push_front(self.pot2)
        #list_pot.push_back(self.pot3)
        #self.assertEqual(list_pot.size(), 3)

        #for id, pot in enumerate([self.pot2, self.pot1, self.pot3]):
            #self.assertTrue(list_pot.exists(pot))
            ##XXX self.assertEqual(list_pot[id], pot)

        #self.assertFalse(list_pot.exists(Potential()))


    #def testDeletion(self):
        #list_pot = ListPotentials()
        #list_pot.append(self.pot1)
        #list_pot.push_front(self.pot2)
        #list_pot.push_back(self.pot3)

        #self.assertEqual(list_pot.size(), 3)
        #list_pot.remove(self.pot1)
        #self.assertEqual(list_pot.size(), 2)
        #list_pot.pop_front()
        #self.assertEqual(list_pot.size(), 1)
        #self.assertFalse(list_pot.empty())
        #list_pot.pop_back()
        #self.assertTrue(list_pot.empty())

        #list_pot.append(Potential())
        #list_pot.append(Potential())
        #self.assertEqual(list_pot.size(), 2)
        #list_pot.clear()
        #self.assertTrue(list_pot.empty())

        #list_pot.append(self.pot1)
        #list_pot.append(self.pot2)
        #list_pot.append(self.pot1)
        #self.assertEqual(list_pot.size(), 3)
        #list_pot.eradicate(self.pot1)
        #self.assertEqual(list_pot.size(), 1)


    #def testPersistence(self):
        #list_pot = ListPotentials()
        #potA = Potential()
        #potB = Potential()
        #list_pot.append(potA)
        #list_pot.append(potB)

        #del potA, potB
        #for pot in list_pot:
            #self.assertTrue(isinstance(pot, Potential))
            #pot.__fill_distrib__()



#ts = unittest.TestSuite()
#ts.addTest(TestConstructors('testConstructor'))
#ts.addTest(TestConstructors('testCopyConstructor'))
#ts.addTest(TestAssessors('testInsertion'))
#ts.addTest(TestAssessors('testDeletion'))
#ts.addTest(TestAssessors('testPersistence'))
#
