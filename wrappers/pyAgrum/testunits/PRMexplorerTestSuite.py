# -*- encoding: UTF-8 -*-
import pyAgrum as gum

import unittest
from pyAgrumTestSuite import pyAgrumTestCase


class PRMexplorerTestCase(pyAgrumTestCase):

    def testO3PRMLoad(self):
        prm=gum.PRMexplorer(self.agrumSrcDir('src/testunits/ressources/o3prm/Asia.o3prm')) # verbose=False : don't want to see the warnings
        
    def testO3PRMLoadErrors(self):
        try:
            prm=gum.PRMexplorer(self.agrumSrcDir('src/testunits/ressources/o3prm/DoesNotExist.o3prm')) 
            self.assertTrue(False)
        except:
            self.assertTrue(True)
            
        try:
            prm=gum.PRMexplorer(self.agrumSrcDir('src/testunits/ressources/o3prm/AsiaWithError.o3prm'),'',False)
            self.assertTrue(False)
        except:
            self.assertTrue(True)
        
    def testO3PRMClasses(self):
        prm=gum.PRMexplorer(self.agrumSrcDir('src/testunits/ressources/o3prm/Asia.o3prm')) 
        self.assertEqual(str(prm.classes()),"['Asia']")
        
    def testO3PRMAttributes(self):
        prm=gum.PRMexplorer(self.agrumSrcDir('src/testunits/ressources/o3prm/Asia.o3prm')) 
        
        witness=set(['tubOrCancer', 'positiveXRay', 'dyspnea', 'visitToAsia', 'tuberculosis', 'smoking', 'bronchitis', 'lungCancer'])
        attrs=prm.attributes('Asia')
        self.assertEqual(len(attrs),len(witness))
        for attr in attrs:
            self.assertTrue(attr in witness)
            
        
        witness=set(['(boolean)bronchitis', '(boolean)dyspnea', '(boolean)visitToAsia', '(boolean)tuberculosis', '(boolean)lungCancer', '(boolean)tubOrCancer', '(boolean)positiveXRay', '(boolean)smoking'])
        attrs=prm.attributes('Asia',allAttributes=True)        
        self.assertEqual(len(attrs),len(witness))
        for attr in attrs:
            self.assertTrue(attr in witness)
        


ts = unittest.TestSuite()
ts.addTest(PRMexplorerTestCase('testO3PRMLoad'))
ts.addTest(PRMexplorerTestCase('testO3PRMLoadErrors'))
ts.addTest(PRMexplorerTestCase('testO3PRMClasses'))
ts.addTest(PRMexplorerTestCase('testO3PRMAttributes'))