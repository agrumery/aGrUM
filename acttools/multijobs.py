#!/usr/bin/python
# -*- coding: utf-8 -*-

#***************************************************************************
#*   Copyright (C) 2015 by Pierre-Henri WUILLEMIN                          *
#*   {prenom.nom}_at_lip6.fr                                               *
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License as published by  *
#*   the Free Software Foundation; either version 2 of the License, or     *
#*   (at your option) any later version.                                   *
#*                                                                         *
#*   This program is distributed in the hope that it will be useful,       *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU General Public License for more details.                          *
#*                                                                         *
#*   You should have received a copy of the GNU General Public License     *
#*   along with this program; if not, write to the                         *
#*   Free Software Foundation, Inc.,                                       *
#*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
#***************************************************************************
from __future__ import print_function
import sys
import os
import io
# for timeout in popen (cross-platform)
from subprocess import PIPE, Popen, STDOUT,call
from threading  import Thread

from configuration import cfg
from utils import notif,error,critic

try:
    from Queue import Queue, Empty
except ImportError:
    from queue import Queue, Empty  # python 3.x

ON_POSIX = 'posix' in sys.builtin_module_names

def enqueue_output(out, queue):
    for line in iter(out.readline, b''):
        queue.put(line)
    out.close()

def prettifying(line):
  #prettifying the line
  res=""

  if line=="":
    return ""

  # prettifying compilation
  s=line.split("%]")

  if len(s)>=2:
    res+=cfg.C_VALUE+"%]".join(s[:-1])+"%]"+cfg.C_END+" "
    line=s[-1].strip()

  if line=="":
    return res.rstrip()

  remove_dirs=lambda s : s[:s.rfind(" ")]

  # prettifying (compacting) path
  if line[0]=="/": # we keep message beginning with full path
    return line

  s=line.split("agrum.dir/")
  if len(s)==2:
      return remove_dirs(s[0])+" "+cfg.C_MSG+s[1].rstrip()+cfg.C_END

  s=line.split("/generated-files/")
  if len(s)==2:
      return remove_dirs(s[0])+cfg.C_MSG+" generated-files/"+s[1].rstrip()+cfg.C_END

  s=line.split(" /")
  if len(s)==2:
      return remove_dirs(s[0])+cfg.C_MSG+" /"+s[1].rstrip()+cfg.C_END

  #prettifying test execution
  s=line.split(". [")
  if len(s)==2:
      return s[0]+". "+cfg.C_VALUE+"["+s[1].rstrip()+cfg.C_END

  s=line.split("# [")
  if len(s)==2:
      return s[0]+"# "+cfg.C_VALUE+"["+s[1].rstrip()+cfg.C_END

  s=line.split(" ... ")
  if len(s)==2:
      return cfg.C_WARNING+s[0]+cfg.C_END+" ... "+cfg.C_MSG+"OK   "+cfg.C_END

  # end of test execution
  s=line.split("##")
  if len(s)==3:
      return cfg.C_WARNING+"##"+cfg.C_END+s[1]+cfg.C_MSG+"##"+cfg.C_END
  if line[0:6]=="Failed":
      return "Failed "+cfg.C_MSG+line[7:]+cfg.C_END
  if line[0:6]=="Succes":
      return "Success rate: "+cfg.C_MSG+line[14:]+cfg.C_END
  if line[-11:]=="<--- failed":
      return line[0:-11]+cfg.C_ERROR+"<--- failed"+cfg.C_END

  s=line.split("Memory leaks found")
  if len(s)==2:
      return s[0]+cfg.C_ERROR+"Memory leaks found"+cfg.C_END+s[1]
  return line

def threaded_execution(cde,verbose):
  os.environ["PYTHONUNBUFFERED"]="1"
  p = Popen(cde,shell=True,bufsize=1,stdout=PIPE, stderr=PIPE )

  inp = Queue()
  sout = io.open(p.stdout.fileno(), 'rb', closefd=False)
  serr = io.open(p.stderr.fileno(), 'rb', closefd=False)

  def Pump(stream, category):
      queue = Queue()
      def rdr():
          while True:
              buf = stream.read1(8192)
              if len(buf)>0:
                  queue.put( buf )
              else:
                  queue.put( None )
                  return
      def clct():
          active = True
          while active:
              r = queue.get()
              try:
                  while True:
                      r1 = queue.get(timeout=0.005)
                      if r1 is None:
                          active = False
                          break
                      else:
                          r += r1
              except Empty:
                  pass
              inp.put( (category, r) )
      for tgt in [rdr, clct]:
          th = Thread(target=tgt)
          th.setDaemon(True)
          th.start()

  Pump(sout, 'stdout')
  Pump(serr, 'stderr')

  waiter = "|/-\\"
  w_pos=0
  lastline = ""

  def readerLoop(lastline):
    chan,lines = inp.get( True,timeout = 0.1)
    if chan=='stdout' and lines is not None:
      lines=(lastline+lines).split("\n")
      for i in range(len(lines)-1):
        if lines[i]!="":
          print("\r"+prettifying(lines[i]).rstrip())
      lastline=lines[-1]
      if lastline!="":
        print("\r"+prettifying(lastline).rstrip(),end="")
      sys.stdout.flush()
    elif chan=='stderr' and lines is not None:
      lines=lines.split("\n")
      for line in lines:
        if line!="":
          if line[0]=="/":
            print(line)
          else:
            error(line)
    return lastline

  while p.poll() is None:
    # App still working
    try:
      lastline=readerLoop(lastline)
    except Empty:
      print(waiter[w_pos]+"\b",end="")
      sys.stdout.flush()
      w_pos=(w_pos+1)%len(waiter)

  # flushing the buffers
  while True:
    try:
      lastline=readerLoop(lastline)
    except Empty:
      break


  return p.returncode

def execCde(commande,current):
    if current["no_fun"]:
      rc=call(commande,shell=True)
    else:
      rc=threaded_execution(commande,current["verbose"])

    return rc
