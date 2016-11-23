/***************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
/**
* @file
* @brief This file contains definition for a PRMexplorer (read-only access to
*PRM)
*
* @author Pierre-Henri WUILLEMIN and Yangfan XIANG
*/
#include <string>
#include <vector>
#include <Python.h>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/o3prm/O3prmReader.h>
#include <agrum/PRM/elements/PRMClassElement.h>
#include <agrum/PRM/elements/PRMAggregate.h>

template <typename GUM_SCALAR>
class classElement;

template <typename GUM_SCALAR>
class Aggregate;

class PRMexplorer {
public:
  /**
  * Create an explorer
  */
  PRMexplorer(void){
  }

  ~PRMexplorer(void) {
  }
  /**
  * Fill an explorer from a filename and a classpath
  * @param filename
  * @param classpath
  * @param verbose to print warnings and errors
  */
  void load(std::string filename,
    std::string classpath = "",
    bool verbose = false ) {
      __prm=nullptr;

      std::stringstream stream;

      __reader.setClassPath( classpath );

      auto nbErr = __reader.readFile( filename );
      
      __reader.showElegantErrorsAndWarnings( stream );
      if ( nbErr > 0 ) {
        __reader.showErrorCounts( stream );
        GUM_ERROR( gum::FatalError, stream.str() );
      }
      if (verbose) {
        std::cout << stream.str() << std::endl;
      }

      __prm = __reader.prm();
    }

    /**
    * @param name The name of a possible Type<GUM_SCALAR> in this PRM.
    * @return Returns true if name names a Type<GUM_SCALAR> in this PRM.
    */
    PyObject* isType(std::string name){
      return __prm->isType(name) ? Py_True : Py_False;
    }

    /**
    * @param name The name of a possible Class<GUM_SCALAR> in this PRM.
    * @return Returns true if name names a Class<GUM_SCALAR> in this PRM.
    */
    PyObject* isClass(std::string name){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
          
      return __prm->isClass(name) ? Py_True : Py_False;
    }

    /**
    * @param name The name of a possible Interface<GUM_SCALAR> in this PRM.
    * @return Returns true if name names a Interface<GUM_SCALAR> in this PRM.
    */
    PyObject* isInterface(std::string name){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
      return __prm->isInterface(name) ? Py_True : Py_False;
    }


    /************CLASS**************/

    /**
    * @return a list of class names
    */
    PyObject* classes() {
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
      PyObject* q = PyList_New( 0 );

      for ( auto c : __prm->classes() ) {
        PyList_Append( q, PyString_FromString( c->name().c_str() ) );
      }

      return q;
    }

    /**
    * @return a list of Tuplet(typename, attribute_name, depenson)
    * @param class_name : the name of the class
    * @param allAttributes : even automatically generated attributes
    */
    PyObject* classAttributes( std::string class_name/*, bool allAttributes = false */) {
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
      PyObject* q = PyList_New(0);

      auto& c = __prm->getClass(class_name);
      gum::DAG gra = c.dag();

      for ( auto a : c.attributes() ){
        /*if ( allAttributes ) {
        PyObject* uplet = PyTuple_New(2);
        PyTuple_SetItem(uplet, 0, PyString_FromString( c->type().name().c_str()) );
        PyTuple_SetItem(uplet, 1, PyString_FromString( c->name().c_str() ));
        PyList_Append( q, uplet);
      } else*/
      if ( &( c.get( a->name() ) ) == a ) {
        // remove automatically created attributes (cast-descendant)
        PyObject* uplet = PyTuple_New(3);
        PyTuple_SetItem(uplet, 0, PyString_FromString( a->type().name().c_str()) );
        PyTuple_SetItem(uplet, 1, PyString_FromString( a->name().c_str() ));

        PyObject* depenson = PyList_New(0);
        for(auto parentId : gra.parents(a->id())){
          PyList_Append(depenson, PyString_FromString(c.get(parentId).name().c_str()));
        }

        PyTuple_SetItem(uplet, 2, depenson);

        PyList_Append( q, uplet);
      }
    }
    return q;
  }

  /**
  * @param class_name : the name of the class
  * @param att_name : the name of the attribute contained in this class
  */
  PyObject* isAttribute( std::string class_name, std::string att_name){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
    auto& ob = __prm->getClass(class_name).get(att_name);

    return gum::prm::PRMClassElement<double>::isAttribute(ob) ? Py_True : Py_False;
  }

  /**
  * @return a list of Tuplet(typename,reference_name, isArray)
  * @param class_name : the name of the class
  */
  PyObject* classReferences( std::string class_name) {
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
    PyObject* q = PyList_New(0);

    for ( auto r : __prm->getClass( class_name ).referenceSlots() ){
      PyObject* uplet = PyTuple_New(3);
      PyTuple_SetItem(uplet, 0, PyString_FromString(r->slotType().name().c_str()) );
      PyTuple_SetItem(uplet, 1, PyString_FromString( r->name().c_str() ));
      if(r->isArray()){
        PyTuple_SetItem(uplet, 2, Py_True );
      }
      else{
        PyTuple_SetItem(uplet, 2, Py_False );
      }
      PyList_Append(q, uplet);
    }
    return q;
  }



  /**
  * @return a list of parameters from a class
  * @param class_name : the name of the class
  */
  PyObject* classParameters( std::string class_name) {
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
    PyObject* q = PyList_New( 0 );

    for ( auto c : __prm->getClass( class_name ).parameters() ){
      PyList_Append( q, PyString_FromString( c->safeName().c_str() ) );
    }
    return q;
  }

  /**
  * @return a list of interface which the class implemented
  * @param class_name : the name of the class
  */
  PyObject* classImplements(std::string class_name) {
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
    PyObject* l = PyList_New(0);

    try{
      //raise NotFound if this class doesn't implement any interface
      for( auto c : __prm->getClass(class_name).implements()){
        PyList_Append(l, PyString_FromString(c->name().c_str()));
      }
    }
    catch(gum::NotFound&){ /*do nothing and return empty list*/}
    return l;
  }


  /**
  * @return a list of Tuplet(typename, name, agg_type, agg_label=None, applies_label_list)
  * @param class_name : the name of the class
  */
  std::string aggType[9] = {"min", "max", "count", "exists", "forall", "or", "and", "amplitude", "median"};

  PyObject* classAggregates( std::string class_name) {
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
    PyObject* l = PyList_New( 0 );

    auto& c = __prm->getClass(class_name);

    gum::DAG gra = c.dag();

    for(auto a : c.aggregates()){
      PyObject* uplet = PyTuple_New(5);
      PyTuple_SetItem(uplet, 0, PyString_FromString(a->type().name().c_str()));
      PyTuple_SetItem(uplet, 1, PyString_FromString(a->name().c_str()));
      PyTuple_SetItem(uplet, 2, PyString_FromString(aggType[(int)a->agg_type()].c_str()));
      if(a->hasLabel()){
        PyTuple_SetItem(uplet, 3, PyString_FromString(a->labelValue().c_str()));
      }
      else {
        PyTuple_SetItem(uplet, 3, Py_None);
      }
      PyObject* param = PyList_New( 0 );

      for(auto parentId : gra.parents(a->id())){
        PyList_Append(param, PyString_FromString(c.get(parentId).name().c_str()));
      }

      PyTuple_SetItem(uplet, 4, param);

      PyList_Append(l, uplet);
    }
    return l;
  }

  /**
  * @return a list of Tuplet(typename, slotname, isMultiple)
  * @param class_name : the name of the class
  */
  PyObject* classSlotChains( std::string class_name){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
    PyObject* l = PyList_New(0);

    for(auto c : __prm->getClass(class_name).slotChains()){
      PyObject* uplet = PyTuple_New(3);
      PyTuple_SetItem(uplet, 0, PyString_FromString( c->type().name().c_str()) );
      PyTuple_SetItem(uplet, 1, PyString_FromString( c->name().c_str() ));
      if(c->isMultiple()){
        PyTuple_SetItem(uplet, 2, Py_True );
      }
      else{
        PyTuple_SetItem(uplet, 2, Py_False );
      }


      /*
      auto chains = c->chain();
      PyObject* uplets = PyTuple_New(chains.size()-1);
      for(unsigned int i =0; i != chains.size(); i++){
        PyTuple_SetItem(uplets, i, PyString_FromString( chains[i]->name().c_str()) );
      }
      PyTuple_SetItem(uplet, 3, uplets);
      */
      PyList_Append(l, uplet);
    }
    return l;
  }

  /**
  * @param class_name : the name of the class
  */
  PyObject* classDag(std::string class_name){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }

    PyObject* uplet = PyTuple_New(2);
    auto &  c = __prm->getClass(class_name);
    PyObject* d = PyDict_New();
    gum::DAG gra=c.dag();
    for(auto id : gra.nodes()){
      PyDict_SetItem(d, PyLong_FromUnsignedLong((unsigned long)id), PyString_FromString(c.get(id).name().c_str()) );

    }
    PyTuple_SetItem(uplet, 0, d);
    PyTuple_SetItem(uplet, 1, PyString_FromString( gra.toDot().c_str() ));
    return uplet;
  }

  /**
  * @return a list of 3-uplets (nameofsystem,dictinary of ids  their name & type,list of arcs:[(tail,head),..] )
  */
    PyObject* getalltheSystems(){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
      PyObject* l = PyList_New( 0 );

      auto the_systems=__prm->systems();

      unsigned long var=0;
      for (auto a_system : the_systems){
        PyObject* uplet = PyTuple_New(3);
        PyObject* a = PyList_New(0);
        PyObject* n = PyDict_New();

        auto graph=a_system->skeleton();

        PyTuple_SetItem(uplet, 0,  PyString_FromString(a_system->name().c_str()));

        for(auto node : graph.nodes()){
          PyObject* tnode = PyTuple_New(2);
          PyTuple_SetItem(tnode, 0, PyString_FromString(a_system->get(node).name().c_str()) );
          PyTuple_SetItem(tnode, 1, PyString_FromString(a_system->get(node).type().name().c_str()) );
          PyDict_SetItem(n, PyLong_FromUnsignedLong((unsigned long)node), tnode );

        }


        for(auto arc : graph.arcs()){
          PyObject* tarc = PyTuple_New(2);
          PyTuple_SetItem(tarc, 0, PyLong_FromUnsignedLong((unsigned long)arc.tail()));
          PyTuple_SetItem(tarc, 1, PyLong_FromUnsignedLong((unsigned long)arc.head()));
          PyList_Append(a, tarc);
        }

        PyTuple_SetItem(uplet, 1, n);
        PyTuple_SetItem(uplet, 2, a);
        PyList_Append(l, uplet);

        var=var+1;
      }

      return l;
    }

  /**
  * @return the name of the super class
  * @param class_name : the name of the class
  */
  PyObject* getSuperClass(std::string class_name){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
    auto& c = __prm->getClass(class_name);
    try{
      //raise NotFound if this class haven't super
      return PyString_FromString(c.super().name().c_str());
    }
    catch(gum::NotFound&){
      return Py_None;
    }
  }

  /**
  * @return the list of subclass name
  * @param class_name : the name of the class
  */
  PyObject* getDirectSubClass(std::string class_name){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
    PyObject* l = PyList_New(0);
    auto& exten = __prm->getClass(class_name).extensions();
    for (auto c : exten){
      PyList_Append(l, PyString_FromString(c->name().c_str()));
    }
    return l;
  }

  /**
  * @return the potential of an attribute in a class
  * @param classname : the name of the class   *
  * @param attribute : the name of the attribute
  */
  const gum::Potential<double>& cpf( std::string class_name,std::string attribute ) {
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      return __prm->getClass( class_name ).get( attribute ).cpf();
    }


    /************TYPE**************/

    /**
    * @return the list of type names
    */
    PyObject* types() {
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
      PyObject* l = PyList_New( 0 );

      for(auto c : __prm->types())
      PyList_Append(l, PyString_FromString(c->name().c_str()));
      return l;
    }

    /**
    * @return the name of super type
    * @param type_name : the name of the type
    */
    PyObject* getSuperType(std::string type_name){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
      auto& c = __prm->type(type_name);
      if(c.isSubType()){
        return PyString_FromString(c.superType()->name().c_str());
      }
      else {
        return Py_None;
      }
    }

    /**
    * @return the list of subtype name
    * @param type_name : the name of the type
    */
    PyObject* getDirectSubTypes(std::string type_name){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
      PyObject* l = PyList_New(0);
      auto& selected = __prm->type(type_name);
      for (auto c : __prm->types()){
        if(c->isSubType()){
          if(c->superType().name() == selected.name()){
            PyList_Append(l, PyString_FromString(c->name().c_str()));
          }
        }
      }
      return l;
    }

    /**
    * @return the list of labels name
    * @param type_name : the name of the type
    */
    PyObject* getLabels(std::string type_name){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
      PyObject* l = PyList_New(0);
      auto& selected = __prm->type(type_name);
      for(auto lab : selected->labels()){
        PyList_Append(l, PyString_FromString(lab.c_str()));
      }
      return l;
    }

    /**
    * @return the dictionaty which the value is the super type of the key
    * @param type_name : the name of the type
    */
    PyObject* getLabelMap(std::string type_name){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
      PyObject* d = PyDict_New();
      auto& selected = __prm->type(type_name);
      if(!selected.isSubType()){
        return Py_None;
      }
      auto typeLabelVector = selected.variable().labels();
      auto superTypeLabelVector = selected.superType().variable().labels();
      auto& labelMapTypeToSuperType = selected.label_map();
      for(unsigned i = 0; i != labelMapTypeToSuperType.size(); i++){
        PyDict_SetItem(d, PyString_FromString(typeLabelVector[i].c_str()), PyString_FromString(superTypeLabelVector[labelMapTypeToSuperType[i]].c_str()));
      }
      return d;
    }

    /************INTERFACE**************/

    /**
    * @return the list of interface names
    */
    PyObject* interfaces() {
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
      PyObject* l = PyList_New( 0 );

      for(auto c : __prm->interfaces())
      PyList_Append(l, PyString_FromString(c->name().c_str()));
      return l;
    }

    /**
    * @return a list of attribute names from a interface
    * @param interface_name : the name of the interface
    * @param allAttributes : even automatically generated attributes
    */
    PyObject* interAttributes( std::string interface_name, bool allAttributes = false ) {
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
      PyObject* q = PyList_New( 0 );

      for ( auto c : __prm->getInterface( interface_name ).attributes() )
      if ( allAttributes ) {
        PyObject* uplet = PyTuple_New(2);
        PyTuple_SetItem(uplet, 0, PyString_FromString( c->type().name().c_str()) );
        PyTuple_SetItem(uplet, 1, PyString_FromString( c->name().c_str() ));
        PyList_Append( q, uplet);
      } else if ( &( __prm->getInterface( interface_name ).get( c->name() ) ) == c ){
        // remove automatically created attributes
        // (cast-descendant)
        PyObject* uplet = PyTuple_New(2);
        PyTuple_SetItem(uplet, 0, PyString_FromString( c->type().name().c_str()) );
        PyTuple_SetItem(uplet, 1, PyString_FromString( c->name().c_str() ));
        PyList_Append( q, uplet);
      }

      return q;
    }

    /**
    * @return a list of attribute names from a interface
    * @param interface_name : the name of the interface
    * @param allAttributes : even automatically generated attributes
    */
    PyObject* interReferences( std::string interface_name) {
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
      PyObject* q = PyList_New( 0 );

      for ( auto r : __prm->getInterface( interface_name ).referenceSlots() ){
        PyObject* uplet = PyTuple_New(3);
        PyTuple_SetItem(uplet, 0, PyString_FromString(r->slotType().name().c_str()) );
        PyTuple_SetItem(uplet, 1, PyString_FromString( r->name().c_str() ));
        if(r->isArray()){
          PyTuple_SetItem(uplet, 2, Py_True );
        }
        else{
          PyTuple_SetItem(uplet, 2, Py_False );
        }
        PyList_Append(q, uplet);
      }
      return q;
    }

    /**
    * @return the name of super interface
    * @param interface_name : the name of the interface
    */
    PyObject* getSuperInterface(std::string interface_name){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
      auto& c = __prm->getInterface(interface_name);
      try{
        //raise NotFound if this interface haven't super
        return PyString_FromString(c.super().name().c_str());
      }
      catch(gum::NotFound&) {
        return Py_None;
      }
    }

    /**
    * @return the list of subinterface name
    * @param interface_name : the name of the interface
    */
    PyObject* getDirectSubInterfaces(std::string interface_name){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }
      
      PyObject* l = PyList_New(0);
      auto& selected = __prm->getInterface(interface_name);
      for (auto c : __prm->interfaces()){
        try{
          //raise NotFound if this interface haven't super
          if(c->super().name() == selected.name()){
            PyList_Append(l, PyString_FromString(c->name().c_str()));
          }
        }
        catch(gum::NotFound&){ /*do nothing, skip this iteration*/}
      }
      return l;
    }

    /**
    * @return the list of class name how implement this interface
    * @param interface_name : the name of the interface
    */
    PyObject* getImplementations(std::string interface_name){
      if (__prm==nullptr) {
          GUM_ERROR( gum::FatalError, "No loaded prm." );
      }

      PyObject* l = PyList_New(0);
      auto& selected = __prm->getInterface(interface_name);
      for(auto c : selected.implementations()){
        PyList_Append(l, PyString_FromString(c->name().c_str()));
      }
      return l;
    }



  private:
    gum::prm::PRM<double>* __prm; // will contain a reference to reader.prm() if reader is OK
    gum::prm::o3prm::O3prmReader<double> __reader;
  };
