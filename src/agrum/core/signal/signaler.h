/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief Classes of signaler. For more documentation, @see signaler0.h
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
/*!
* @ingroup signal_group
\page  signal_page How to use signaler/listener in aGrUM ?

aGrUM has its own implementation of signalers/listeners.

Let's say that <tt>class A</tt> has a method <tt>f(int i,char ch)</tt> and would like
to give to everyone the right to know whenever <tt>f()</tt> is run.

@code
#include <agrum/core/signal/signaler2.h> // Note the '2' because the signal we want
will send 2 args (i and ch)
class A {
 public:
  gum::signaler2<int,char> onF; // Note that onF is public !

 void f(int i,char ch) {
    //do something
  GUM_EMIT2(onF,i,ch); // a signal (with 2 args i and ch) is emitted via onF.
  }
}
@endcode

Note that for <tt>class A</tt>, adding signaler is very light : a new public
attribute and a GUM_EMITx when needed (where x is the number of args).

Now, how to listen to such a signal ?
@code
#include <agrum/core/signal/listener.h>

class B : gum::listener {
 public:
  void whenF(const void *src,int i, char ch) {std::cout<<"object "<<src<<"run f on
"<<i<<" and "<<ch<<std::endl;}
};

A a;

B b1;
B b2;

GUM_CONNECT( a,onF,b1,B::whenF );
GUM_CONNECT( a,onF,b2,B::whenF );

a.f(3,'a');
// b1.whenF and b2.whenF has been executed.

class C : gum::listener {
 public:
    void dummy(void *src,int i,char ch) {};
};

{
  C c;
  GUM_CONNECT( a,onF,c,C::dummy );
  a.f(4,'x');
  //c.dummy(), b1.whenF and b2.whenF has been executed.
} // c is destroyed...

a.f(5,'y');
//b1.whenF and b2.whenF has been executed.
@endcode
*/
#ifndef __SIGNALER_H
#define __SIGNALER_H
#include <functional>

#include <agrum/core/signal/listener.h>

#include <agrum/core/signal/signaler0.h>
#include <agrum/core/signal/signaler1.h>
#include <agrum/core/signal/signaler2.h>
#include <agrum/core/signal/signaler3.h>
#include <agrum/core/signal/signaler4.h>
#include <agrum/core/signal/signaler5.h>
#include <agrum/core/signal/signaler6.h>
#include <agrum/core/signal/signaler7.h>
#endif // __SIGNALER_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
