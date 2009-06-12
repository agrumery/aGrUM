/***************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
*   {prenom.nom}_at_lip6.fr   *
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

#include <iostream>

#include <agrum/core/signal/listener.h>
#include <agrum/core/signal/signaler.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class A {
    std::string _label;

  public:
    gum::Signaler0 onAddNode;
    gum::Signaler6<int,std::string,float,char,char,char> onChangeLabel;


    A( std::string s ) : _label( s ) {}

    const std::string& label() const { return _label;}

    void addNode() { GUM_EMIT0( onAddNode );}

    void changeLabel( int id,std::string s ) { GUM_EMIT6( onChangeLabel,id,s,3.1415,'h','e','l' );_label=s;}
};

class B : public gum::Listener {
  public:
    void nodeAdded( const void* v ) {
      const A* source=static_cast<const A*>( v );
      std::cout<<"in "<<source->label()<<": node  added ..."<<std::endl;
    }

    void labelChanged( const void* v,int i,std::string s,float f ,char ,char ,char ) {
      const A* source=static_cast<const A*>( v );
      std::cout<<"in "<<source->label()<<": label '"<<s <<"' changed  for id "<<i<<"..."<<f<<std::endl;
    }
};

class C : public gum::Listener {
  public:
    void f( const void* v,int i,std::string s,float f ,char ,char ,char ) {
      std::cout<<v<<" "<<i<<" "<<s<<" "<<f<<std::endl;
    }
};

void f( void ) {

  B gui;
  {
    A x( "x" );
    A Y( "y" );
    //  GUM_CONNECT(sender,signal,receiver,target)
    GUM_CONNECT( x,onAddNode,gui,B::nodeAdded );
    GUM_CONNECT( x,onChangeLabel,gui,B::labelChanged );

    std::cout<<std::endl<<"******"<<std::endl;
    x.addNode();

    {
      C anonymous;
      GUM_CONNECT( x,onChangeLabel, anonymous,C::f );

      std::cout<<std::endl<<"******"<<std::endl;
      x.changeLabel( 1,"my new label" );
    }

    // anonymous has been destroyed. He should not listen anymore.

    std::cout<<std::endl<<"******"<<std::endl;
    x.addNode();

    std::cout<<std::endl<<"******"<<std::endl;

    // is not validate
    // x.GUM_EMIT2( onChangeLabel,1,"toto" );


    std::cout<<std::endl<<"******"<<std::endl;
  } // here signaler are destroyed before listener
}

int main( void ) {
  f();

  gum::__atexit();
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
