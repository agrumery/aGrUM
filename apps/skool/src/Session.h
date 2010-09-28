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
/**
 * @file
 *
 * @author Ni NI
 */
#ifndef _Session_H
#define _Session_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <ctime>
#include <cstdio>
#include "TokenizableString.h"
#include "Request.h"
#include "Import.h"
#include "Action.h"
#include "Engine.h"
#include <agrum/prm/skool/SkoolReader.h>
#include <agrum/prm/PRMFactory.h>

#define Err_unknownAction "Le mot clé entré n'est pas correct."
using namespace std;

class Session {

  private:
    string package;
    Engine engine;
    int package_line;
    vector < Import > imports;
    vector < Request > requests;
    bool inside_request;
    bool quit;

  public:
    int line_error;
    Session ();
    ~Session ();
    int getImportsSize ();
    int getRequestsSize ();
    vector < string > convertStringList (string s);
    void insertPackageInformation (int line, const string str);
    void insertImport (Import & imp);
    void insertEngine(Engine& eng);
    void setExitStatus(bool status);
    bool getExitStatus(); 
    string getPackageName ();
    Engine &getEngine();
    vector < Import > &getImports ();
    vector < Request > &getRequests ();
    void enterRequest (Request q);
    Request *getCurrentRequest ();
    bool isInsideRequest ();
    void quitRequest ();
    void parser (char *fichier);
    string getPackageSystemName(const string systemNameInRequest);
    string getPackageSystemFilename(const string systemImport);
    void act (const string requestFile, ostream& o, bool option);
};


#endif
