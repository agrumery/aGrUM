# See http://ipython.org/ipython-doc/1/interactive/public_server.html for more information.
# Configuration file for ipython-notebook.
import os


c = get_config()

c.IPKernelApp.pylab = 'inline' 
c.NotebookApp.terminals_enabled = False
c.NotebookApp.websocket_compression_options = {}
c.NotebookApp.notebook_dir="wrappers/pyAgrum/pyLibs/notebooks"

