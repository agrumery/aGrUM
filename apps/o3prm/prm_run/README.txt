http://agrum.lip6.fr

What Is This?
-------------

This is the successor of th O3PRM Editor.

It aims to ease integration of the O3PRM language in general purpose IDE and
text editors. For this, prm_run offers two mode of execution: syntax mode and
normal mode.

You can also use it as a classical interpreter for the O3PRM language.

How To Use
----------

Invoque prm_run as follow:
  prm_run [options] file1 file2 [...]

Files must be O3PRMR files (O3PRM request files). The application will try to resolve imports
from its invocation folder. For example, if you have an O3PRM Editor project whith the following
files:

Project/
  request.o3prmr
  fr/
    lip6/
      myTypes.o3prm
      myClasses.o3prm
      mySytems.o3prm

If you invoque prm_run from the Project folder as follow:
  prm_run request.o3prmr

Your request should show in the terminal.

If you want to set yourself the paths prm_run should look up for imports you
can use the option -p (or --claspath) as many time as needed:
  prm_run request.o3prmr -p C:\my\first\package -p C:\my\second\package

How To Install
--------------

If you build from source, you can proceed with a make install to install in
default location or use the CMAKE_INSTALL_PREFIX option with cmake (see How To
Build for more details).

You simply copy paste the binary anywhere you want it to be. Under Windows, you
should put is in C:\Program Files or in under C:\Users\YourLogin\ if you don't
have administrator privilege.

Don't forget to add prm_run in your PATH, to do so under linux:
  export PATH=$PATH:/path/to/prm_run/install/dir/
And under Windows:
  set PATH=%PATH%;C:\path\to\prm_run\install\dir\

How To Build
------------

To build prm_run you will need to build and install aGrUM first, usually :
  act install release aGrUM -d /usr/local
works fine. If not, please check out http://agrum.lip6.fr for more help on installing aGrUM.

Once you have sucessfully installed aGrUM you can configure your build using cmake :
  cd path/to/prm_run/root/dir
  mkdir build
  cd build
  cmake .. -DCMAKE_INSTALL_PREFIX=/where/you/want/to/install/it -DCMAKE_PREFIX_PATH=/where/you/installed/agrum/
  make install



