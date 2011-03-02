import os 
import sys

def pyAgrum_header():
    print os.path.basename(sys.argv[0])+" (c) 2011 -- Pierre-Henri Wuillemin -- UPMC"
    print "powered by pyAgrum/aGrUM"
    print
   
if __name__=="__main__":
    pyAgrum_header()