//*************************************************************************
//   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES  //
//   {prenom.nom}_at_lip6.fr                                              //
//                                                                        //
//   This program is free software; you can redistribute it and/or modify //
//   it under the terms of the GNU General Public License as published by //
//   the Free Software Foundation; either version 2 of the License, or    //
//   (at your option) any later version.                                  //
//                                                                        //
//   This program is distributed in the hope that it will be useful,      //
//   but WITHOUT ANY WARRANTY; without even the implied warranty of       //
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
//   GNU General Public License for more details.                         //
//                                                                        //
//   You should have received a copy of the GNU General Public License    //
//   along with this program; if not, write to the                        //
//   Free Software Foundation, Inc.,                                      //
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.            //
//**************************************************************************/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

//
// Default constructor.
//
// All parameters are randomly generated.
// @param maxNodes The maximum number of nodes allowed.
// @param maxDomainSize The maximum domain size allowed.
//
INLINE
gumDAGGenerator::gumDAGGenerator( gumSize /*(@todo ?) maxNodes*/, gumSize /*(@todo ?) maxDomainSize*/ ) {
  _param.insert( "NBR_NODES", "20" );
  _param.insert( "NBR_ROOT", "1" );
  _param.insert( "NBR_LEAF", "5" );
  _param.insert( "NBR_COMP", "1" );
  _param.insert( "MIN_NBR_PARENT", "1" );
  _param.insert( "MAX_NBR_PARENT", "3" );
  _param.insert( "MIN_NBR_CHILD", "1" );
  _param.insert( "MAX_NBR_CHILD", "3" );
}

//
// Fast configuration constructor.
//
// @param config A hashtable which key's are a parameters and value's are those of
// their respective parameter // value. The list of the parameters is given in the
// class documentation.
//
INLINE
gumDAGGenerator::gumDAGGenerator( gumHashTable<std::string, std::string> config ) {
  setConfiguration( config );
}

//
// Destuctor.
//
INLINE
gumDAGGenerator::~gumDAGGenerator() {
  // Empty!
}

//
// See the class documentation for a list of the parameters.
//
// @return Returns the configuration hashtable.
//
INLINE
const gumHashTable<std::string, std::string>&
gumDAGGenerator::getConfiguration() const {
  return _param;
}


//
// @return Returns the number of nodes.
//
INLINE
gumSize
gumDAGGenerator::getNbrNodes() const {
  if ( _param.exists( "NBR_NODES" ) ) {
    std::istringstream input( _param["NBR_NODES"] );
    gumSize retVal;

    if ( !( input >> retVal ) ) {
      throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_NODES" );
    }

    return retVal;
  } else {
    throw gumNotFound( "Paramater not initialized.", "NBR_NODES" );
  }
}

//
// Set the number of nodes.
//
INLINE
void
gumDAGGenerator::setNbrNodes( gumSize n ) {
  std::ostringstream output;

  if ( !( output << n ) ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_NODES" );
  }

  try {
    _param["NBR_NODES"] = output.str();
  } catch ( gumNotFound ) {
    _param.insert( "NBR_NODES", output.str() );
  }
}

//
// @return Returns the minimal number of parents of a node.
//
INLINE
gumSize
gumDAGGenerator::getMinParents() const {
  if ( _param.exists( "MIN_NBR_PARENT" ) ) {
    std::istringstream input( _param["MIN_NBR_PARENT"] );
    gumSize retVal;

    if ( !( input >> retVal ) ) {
      throw gumOperationNotAllowed( "Incorrect parameter value.", "MIN_NBR_PARENT" );
    }

    return retVal;
  } else {
    throw gumNotFound( "Paramater not initialized.", "MIN_NBR_PARENT" );
  }
}

//
// Set the minimal number of parents added to a node.
//
INLINE
void
gumDAGGenerator::setMinParents( gumSize n ) {
  std::ostringstream output;

  if ( !( output << n ) ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "MIN_NBR_PARENT" );
  }

  try {
    _param["MIN_NBR_PARENT"] = output.str();
  } catch ( gumNotFound ) {
    _param.insert( "MIN_NBR_PARENT", output.str() );
  }
}

//
// @return Returns the maximal number of parents of a node.
//
INLINE
gumSize
gumDAGGenerator::getMaxParents() const {
  if ( _param.exists( "MAX_NBR_PARENT" ) ) {
    std::istringstream input( _param["MAX_NBR_PARENT"] );
    gumSize retVal;

    if ( !( input >> retVal ) ) {
      throw gumOperationNotAllowed( "Incorrect parameter value.", "MAX_NBR_PARENT" );
    }

    return retVal;
  } else {
    throw gumNotFound( "Paramater not initialized.", "MAX_NBR_PARENT" );
  }
}

//
// Set the maximal number of parents added to a node.
//
INLINE
void
gumDAGGenerator::setMaxParents( gumSize n ) {
  std::ostringstream output;

  if ( !( output << n ) ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "MAX_NBR_PARENT" );
  }

  try {
    _param["MAX_NBR_PARENT"] = output.str();
  } catch ( gumNotFound ) {
    _param.insert( "MAX_NBR_PARENT", output.str() );
  }
}

//
// @return Returns the minimal number of children of a node.
//
INLINE
gumSize
gumDAGGenerator::getMinChildren() const {
  if ( _param.exists( "MIN_NBR_CHILD" ) ) {
    std::istringstream input( _param["MIN_NBR_CHILD"] );
    gumSize retVal;

    if ( !( input >> retVal ) ) {
      throw gumOperationNotAllowed( "Incorrect parameter value.", "MIN_NBR_CHILD" );
    }

    return retVal;
  } else {
    throw gumNotFound( "Paramater not initialized.", "MIN_NBR_CHILD" );
  }
}

//
// Set the maximal number of children of a node.
//
INLINE
void
gumDAGGenerator::setMinChildren( gumSize n ) {
  std::ostringstream output;

  if ( !( output << n ) ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "MIN_NBR_CHILD" );
  }

  try {
    _param["MIN_NBR_CHILD"] = output.str();
  } catch ( gumNotFound ) {
    _param.insert( "MIN_NBR_CHILD", output.str() );
  }
}

//
// @return Returns the maximal number of children of a node.
//
INLINE
gumSize
gumDAGGenerator::getMaxChildren() const {
  if ( _param.exists( "MAX_NBR_CHILD" ) ) {
    std::istringstream input( _param["MAX_NBR_CHILD"] );
    gumSize retVal;

    if ( !( input >> retVal ) ) {
      throw gumOperationNotAllowed( "Incorrect parameter value.", "MAX_NBR_CHILD" );
    }

    return retVal;
  } else {
    throw gumNotFound( "Paramater not initialized.", "MAX_NBR_CHILD" );
  }
}

//
// Set the minimal number of children of a node.
//
INLINE
void
gumDAGGenerator::setMaxChildren( gumSize n ) {
  std::ostringstream output;

  if ( !( output << n ) ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "MAX_NBR_CHILD" );
  }

  try {
    _param["MAX_NBR_CHILD"] = output.str();
  } catch ( gumNotFound ) {
    _param.insert( "MAX_NBR_CHILD", output.str() );
  }
}

//
// @return Returns the number of root nodes.
//
INLINE
gumSize
gumDAGGenerator::getNbrRootNodes() const {
  if ( _param.exists( "NBR_ROOT" ) ) {
    std::istringstream input( _param["NBR_ROOT"] );
    gumSize retVal;

    if ( !( input >> retVal ) ) {
      throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_ROOT" );
    }

    return retVal;
  } else {
    throw gumNotFound( "Paramater not initialized.", "NBR_ROOT" );
  }
}

//
// Set the number of root nodes.
//
INLINE
void
gumDAGGenerator::setNbrRootNodes( gumSize n ) {
  std::ostringstream output;

  if ( !( output << n ) ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_ROOT" );
  }

  try {
    _param["NBR_ROOT"] = output.str();
  } catch ( gumNotFound ) {
    _param.insert( "NBR_ROOT", output.str() );
  }
}

//
// @return Returns the number of leaf nodes.
//
INLINE
gumSize
gumDAGGenerator::getNbrLeafNodes() const {
  if ( _param.exists( "NBR_LEAF" ) ) {
    std::istringstream input( _param["NBR_LEAF"] );
    gumSize retVal;

    if ( !( input >> retVal ) ) {
      throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_LEAF" );
    }

    return retVal;
  } else {
    throw gumNotFound( "Paramater not initialized.", "NBR_LEAF" );
  }
}

//
// Set the number of leaf nodes.
//
INLINE
void
gumDAGGenerator::setNbrLeafNodes( gumSize n ) {
  std::ostringstream output;

  if ( !( output << n ) ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_LEAF" );
  }

  try {
    _param["NBR_LEAF"] = output.str();
  } catch ( gumNotFound ) {
    _param.insert( "NBR_LEAF", output.str() );
  }
}

//
// @return Returns the number of connex component.
//
INLINE
gumSize
gumDAGGenerator::getNbrConnexComponent() const {
  if ( _param.exists( "NBR_COMP" ) ) {
    std::istringstream input( _param["NBR_COMP"] );
    gumSize retVal;

    if ( !( input >> retVal ) ) {
      throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_COMP" );
    }

    return retVal;
  } else {
    throw gumNotFound( "Paramater not initialized.", "NBR_COMP" );
  }
}

//
// Set the number of connex component.
//
INLINE
void
gumDAGGenerator::setNbrConnexComponent( gumSize n ) {
  std::ostringstream output;

  if ( !( output << n ) ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_COMP" );
  }

  try {
    _param["NBR_COMP"] = output.str();
  } catch ( gumNotFound ) {
    _param.insert( "NBR_COMP", output.str() );
  }
}


INLINE
void
gumDAGGenerator::checkParameters() const {
  if ( getMinParents() > getMaxParents() ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "MIN_NBR_PARENT > MAX_NBR_PARENT" );
  }

  if ( getMinChildren() > getMaxChildren() ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "MIN_NBR_CHILD > MAX_NBR_CHILD" );
  }

  if ( getMaxParents() > getNbrNodes() ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "MAX_NBR_PARENT > NBR_NODES" );
  }

  if ( getMaxChildren() > getNbrNodes() ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "MAX_NBR_CHILD > NBR_NODES" );
  }

  if ( getNbrRootNodes() < 1 ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_ROOT < 1" );
  }

  if ( getNbrLeafNodes() < 1 ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_LEAF < 1" );
  }

  if ( getNbrConnexComponent() < 1 ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_COMP < 1" );
  }

  if ( getNbrConnexComponent() > getNbrNodes() ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_COMP > NBR_NODES" );
  }

  if ( getNbrConnexComponent() > getNbrRootNodes() ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_COMP > NBR_ROOT" );
  }

  if ( getNbrConnexComponent() > getNbrLeafNodes() ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_COMP > NBR_ROOT" );
  }

  if ( getNbrNodes() / getNbrConnexComponent() == 0 ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_NODES / NBR_COMP == 0" );
  }

  if ( getNbrRootNodes() / getNbrConnexComponent() == 0 ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_ROOT / NBR_COMP == 0" );
  }

  if ( getNbrLeafNodes() / getNbrConnexComponent() == 0 ) {
    throw gumOperationNotAllowed( "Incorrect parameter value.", "NBR_LEAF / NBR_COMP == 0" );
  }

  if ( getNbrNodes() < getNbrRootNodes() + getNbrLeafNodes() ) {
    throw gumOperationNotAllowed( "Invalid parameters.", "NBR_NODES < NBR_ROOT + NBR_LEAF" );
  }

}

//
// Generates a bayesian network.
//
// @return Returns a randomly generated bayesian network.
//
INLINE
gumDAGTpl<gumNode, gumArc>
gumDAGGenerator::generateDAG() const {
  checkParameters();
  gumDAGTpl<gumNode, gumArc> dag;

  if ( getNbrConnexComponent() > 1 ) {
    gumList< gumList<gumNode>* > rootLists;
    gumList< gumList<gumNode>* > middleLists;
    gumList< gumList<gumNode>* > leafLists;

    for ( gumSize i = 0; i < getNbrConnexComponent(); i++ ) {
      rootLists.pushBack( new gumList<gumNode>() );
      middleLists.pushBack( new gumList<gumNode>() );
      leafLists.pushBack( new gumList<gumNode>() );
    }

    __generateNodes( dag, rootLists, middleLists, leafLists );

    __generateArcs( dag, rootLists, middleLists, leafLists );

    for ( gumSize i = 0; i <getNbrConnexComponent(); i++ ) {
      rootLists[i]->clear();
      middleLists[i]->clear();
      leafLists[i]->clear();

      delete rootLists[i];
      delete middleLists[i];
      delete leafLists[i];
    }

  } else {
    gumList<gumNode> rootList;
    gumList<gumNode> middleList;
    gumList<gumNode> leafList;

    __generateNodes( dag, rootList, middleList, leafList );
    __generateArcs( dag, rootList, middleList, leafList );
  }

  return dag;
}

// Removes ancestors of aNode from candidates, and memorize them.
// If restore is set at true, then it will restore the last memorized
// nodes.
INLINE
void
gumDAGGenerator::__removeAncestors( const gumNode &aNode,
                                    const gumDAGTpl<gumNode, gumArc> &dag,
                                    gumList<gumNode>& candidates,
                                    bool restore ) const {
  static gumList<gumNode> ancestors;

  if ( !restore ) {
    gumSize i = 0;
    ancestors.clear();
    ancestors.pushBack( aNode );
    // Retrieving the ancestors of this node

    while ( i < ancestors.size() ) {
      for ( gumList< gumRefPtr< gumArc > >::iterator childIter = dag.getChildrenArcs( ancestors[i].getID() ).begin();
            childIter != dag.getChildrenArcs( ancestors[i].getID() ).end(); ++childIter ) {
        ancestors.pushBack( dag.getNode(( *childIter )->head() ) );
      }

      i++;
    }
  } else {
    for ( gumList<gumNode>::iterator iter = ancestors.begin(); iter != ancestors.end(); ++iter ) {
      candidates.pushBack( *iter );
    }
  }
}

#endif // DOXYGEN_SHOULD_SKIP_THIS

