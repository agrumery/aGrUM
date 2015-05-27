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

# for timeout in popen (cross-platform)
from subprocess import PIPE, Popen, STDOUT
from threading  import Thread

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
    res+=C_WARNING+"%]".join(s[:-1])+"%]"+C_END+" "
    line=s[-1].strip()

  if line=="":
    return res.rstrip()

  remove_dirs=lambda s : s[:s.find("/")-1]

  # prettifying (compacting) path
  if line[0]!="/": # we keep message beginning with full path
    s=line.split("/agrum/")
    if len(s)==2:
      res+=remove_dirs(s[0])+WARNING+" agrum/"+s[1].rstrip()+ENDC
      line=""
    else:
      s=line.split("/generated-files/")
      if len(s)==2:
        line=""
        res+=remove_dirs(s[0])+WARNING+" generated-files/"+s[1].rstrip()+ENDC

  if line=="":
    return res.rstrip()

  #prettifying test execution
  s=line.split(". [")
  if len(s)==2:
    res+=s[0]+". "+OKGREEN+"["+s[1].rstrip()+ENDC
  else:
    s=line.split("# [")
    if len(s)==2:
      res+=s[0]+"# "+OKGREEN+"["+s[1].rstrip()+ENDC
    else: # test pyAgrum
      s=line.split(" ... ")
      if len(s)==2:
        res+=OKGREEN+s[0]+ENDC+" ... "+WARNING+"OK   "+ENDC
      else: # end of test execution
        s=line.split("##")
        if len(s)==3:
          res+=WARNING+"##"+ENDC+s[1]+WARNING+"##"+ENDC
        elif line[0:6]=="Failed":
          res+="Failed "+WARNING+line[7:]+ENDC
        elif line[0:6]=="Succes":
          res+="Success rate: "+WARNING+line[14:]+ENDC
        elif line[-11:]=="<--- failed":
          res=line[0:-11]+ERROR+"<--- failed"+ENDC
        else:
          s=line.split("Memory leaks found")
          if len(s)==2:
            res=s[0]+ERROR+"Memory leaks found"+ENDC+s[1]
          else:
            res=line

  return res.rstrip()


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
          print("\r"+prettifying(lines[i]))
      lastline=lines[-1]
      if lastline!="":
        print("\r"+prettifying(lastline),end="")
      sys.stdout.flush()
    elif chan=='stderr' and lines is not None:
      lines=lines.split("\n")
      for line in lines:
        if line!="":
          if line[0]=="/":
            print(line)
          else:
            print(ERROR+line+ENDC)
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
