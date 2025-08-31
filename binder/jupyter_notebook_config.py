# See http://ipython.org/ipython-doc/1/interactive/public_server.html for more information.
# Configuration file for ipython-notebook.


c = get_config()

c.IPKernelApp.pylab = "inline"
c.NotebookApp.terminals_enabled = False
c.NotebookApp.websocket_compression_options = {}

# jupyter>3
# c.NotebookApp.notebook_dir = "wrappers/pyAgrum/doc/sphinx/notebooks"
c.ServerApp.root_dir = "wrappers/pyAgrum/doc/sphinx/notebooks"
