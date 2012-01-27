# -*- coding: utf-8 -*-

#(c) Copyright by Pierre-Henri Wuillemin, UPMC, 2011  (pierre-henri.wuillemin@lip6.fr)

#Permission to use, copy, modify, and distribute this
#software and its documentation for any purpose and
#without fee or royalty is hereby granted, provided
#that the above copyright notice appear in all copies
#and that both that copyright notice and this permission
#notice appear in supporting documentation or portions
#thereof, including modifications, that you make.

#THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
#WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
#WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
#SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
#OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
#RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
#IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
#ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
#OR PERFORMANCE OF THIS SOFTWARE!
import sys,os

import pyAgrum as gum

from pyAgrum_header import pyAgrum_header
from progress_bar import ProgressBar

def module_help(exit_value=1):
	"""
	defines help viewed if args are not OK on command line, and exit with exit_value
	"""
	print os.path.basename(sys.argv[0]),"src.{"+gum.availableBNExts()+"}"
	sys.exit(exit_value)
	
def doLoadBN(s):
	# you could simply do that 
	# bn=gum.loadBN("test")
	# but listeners are fun !!
	progressbar=ProgressBar("Loading "+s,0,100,mode='dynamic', char='-')
	
	def local_update(pourcent):
		progressbar.update_amount(pourcent)
		progressbar.display()
		if pourcent==100: print
	
	x=0
	def local2(pourcent):
		x=x+1
		
	return gum.loadBN(s,[local_update,local2])
	

if __name__=="__main__":
	pyAgrum_header(2012)

	if len(sys.argv)<2:
			module_help()

	bn=doLoadBN(sys.argv[1])
	print(bn)