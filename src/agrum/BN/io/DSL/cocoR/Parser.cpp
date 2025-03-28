/***************************************************************************
 *  aGrUM modified frames and atg files for cocoR
 *   Copyright (c) 2005-2024 by Pierre-Henri WUILLEMIN(@LIP6) and Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
***************************************************************************/
/*----------------------------------------------------------------------
Compiler Generator Coco/R,
Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
extended by M. Loeberbauer & A. Woess, Univ. of Linz
ported to C++ by Csaba Balazs, University of Szeged
with improvements by Pat Terry, Rhodes University

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

As an exception, it is allowed to write an extension of Coco/R that is
used as a plugin in non-free software.

If not otherwise stated, any source code generated by Coco/R (other than
Coco/R itself) does not fall under the GNU General Public License.
-----------------------------------------------------------------------*/


#include <iostream>
#include <wchar.h>

#include "Parser.h"
#include "Scanner.h"


namespace gum {
namespace DSL {


void Parser::SynErr( int n ) {
  if ( errDist >= minErrDist ) SynErr( scanner->filename(),la->line, la->col, n );

  errDist = 0;
}


const ErrorsContainer& Parser::errors( void ) const {
  return errors__;
}
ErrorsContainer& Parser::errors( void ) {
  return errors__;
}

void Parser::Get() {
  for ( ;; ) {
    t = la;
    la = scanner->Scan();

    if ( la->kind <= maxT ) { ++errDist; break; }

    

    if ( dummyToken != t ) {
      dummyToken->kind = t->kind;
      dummyToken->pos = t->pos;
      dummyToken->col = t->col;
      dummyToken->line = t->line;
      dummyToken->next = nullptr;
      coco_string_delete( dummyToken->val );
      dummyToken->val = coco_string_create( t->val );
      t = dummyToken;
    }

    la = t;
  }
}

void Parser::Expect( int n ) {
  if ( la->kind==n ) Get(); else { SynErr( n ); }
}

void Parser::ExpectWeak( int n, int follow ) {
  if ( la->kind == n ) Get();
  else {
    SynErr( n );

    while ( !StartOf( follow ) ) Get();
  }
}

bool Parser::WeakSeparator( int n, int syFol, int repFol ) {
  if ( la->kind == n ) {Get(); return true;}
  else if ( StartOf( repFol ) ) {return false;}
  else {
    SynErr( n );

    while ( !( StartOf( syFol ) || StartOf( repFol ) || StartOf( 0 ) ) ) {
      Get();
    }

    return StartOf( syFol );
  }
}

void Parser::DSL() {
		std::string name_of_network;
		factory().startNetworkDeclaration();
		
		Expect(6 /* "net" */);
		if (la->kind == _ident) {
			IDENT(name_of_network);
		} else if (la->kind == _string) {
			STRING(name_of_network);
		} else SynErr(34);
		factory().addNetworkProperty("name", name_of_network); 
		Expect(7 /* "{" */);
		factory().endNetworkDeclaration(); 
		if (la->kind == 20 /* "HEADER" */) {
			HEADER_PART();
		}
		if (la->kind == 21 /* "CREATION" */) {
			CREATION_PART();
		}
		if (la->kind == 10 /* "NUMSAMPLES" */) {
			NUM_SAMPLES();
		}
		if (la->kind == 15 /* "SCREEN" */) {
			SCREEN_PART();
		}
		if (la->kind == 22 /* "WINDOWPOSITION" */) {
			WINDOWPOSITION_PART();
		}
		if (la->kind == 11 /* "BKCOLOR" */) {
			BK_COLOR();
		}
		if (la->kind == 16 /* "USER_PROPERTIES" */) {
			USER_PROPERTIES_PART();
		}
		if (la->kind == 17 /* "DOCUMENTATION" */) {
			DOCUMENTATION_PART();
		}
		if (la->kind == 12 /* "SHOWAS" */) {
			SHOW_AS();
		}
		while (la->kind == 13 /* "node" */) {
			NODE();
		}
		if (la->kind == 19 /* "OBSERVATION_COST" */) {
			OBSERVATION_COST_PART();
		}
		Expect(8 /* "}" */);
		Expect(9 /* ";" */);
}

void Parser::IDENT(std::string& name) {
		Expect(_ident);
		name=narrow(t->val); 
}

void Parser::STRING(std::string& str) {
		Expect(_string);
		str=narrow(t->val); 
}

void Parser::HEADER_PART() {
		Expect(20 /* "HEADER" */);
		BLOC_PART();
}

void Parser::CREATION_PART() {
		Expect(21 /* "CREATION" */);
		BLOC_PART();
}

void Parser::NUM_SAMPLES() {
		Expect(10 /* "NUMSAMPLES" */);
		Expect(_integer);
		Expect(9 /* ";" */);
}

void Parser::SCREEN_PART() {
		Expect(15 /* "SCREEN" */);
		BLOC_PART();
}

void Parser::WINDOWPOSITION_PART() {
		Expect(22 /* "WINDOWPOSITION" */);
		BLOC_PART();
}

void Parser::BK_COLOR() {
		Expect(11 /* "BKCOLOR" */);
		Expect(_integer);
		Expect(9 /* ";" */);
}

void Parser::USER_PROPERTIES_PART() {
		Expect(16 /* "USER_PROPERTIES" */);
		BLOC_PART();
}

void Parser::DOCUMENTATION_PART() {
		Expect(17 /* "DOCUMENTATION" */);
		BLOC_PART();
}

void Parser::SHOW_AS() {
		Expect(12 /* "SHOWAS" */);
		Expect(_integer);
		Expect(9 /* ";" */);
}

void Parser::NODE() {
		std::string var;
		std::vector<std::string> parents;
		Size nbrMod = 0;
		
		Expect(13 /* "node" */);
		IDENT(var);
		Expect(7 /* "{" */);
		Expect(14 /* "TYPE" */);
		Expect(_ident);
		Expect(9 /* ";" */);
		HEADER();
		if (la->kind == 15 /* "SCREEN" */) {
			SCREEN_PART();
		}
		if (la->kind == 16 /* "USER_PROPERTIES" */) {
			USER_PROPERTIES_PART();
		}
		if (la->kind == 17 /* "DOCUMENTATION" */) {
			DOCUMENTATION_PART();
		}
		PARENTS(parents);
		VARIABLE_DEFINITION(nbrMod, var,parents);
		if (la->kind == 18 /* "EXTRA_DEFINITION" */) {
			EXTRA_DEFINITION_PART();
		}
		Expect(8 /* "}" */);
		Expect(9 /* ";" */);
}

void Parser::OBSERVATION_COST_PART() {
		Expect(19 /* "OBSERVATION_COST" */);
		BLOC_PART();
}

void Parser::HEADER() {
		std::string content; 
		Expect(20 /* "HEADER" */);
		Expect(7 /* "{" */);
		Expect(23 /* "ID" */);
		Expect(_ident);
		Expect(9 /* ";" */);
		Expect(24 /* "NAME" */);
		Expect(_string);
		Expect(9 /* ";" */);
		Expect(8 /* "}" */);
		Expect(9 /* ";" */);
}

void Parser::PARENTS(std::vector<std::string>& parents ) {
		Expect(25 /* "PARENTS" */);
		Expect(26 /* "(" */);
		if (la->kind == _ident) {
			PARENTS_LIST(parents);
		}
		Expect(27 /* ")" */);
		Expect(9 /* ";" */);
}

void Parser::VARIABLE_DEFINITION(Size& nbrMod, std::string& var, const std::vector<std::string>& parents ) {
		Expect(29 /* "DEFINITION" */);
		Expect(7 /* "{" */);
		Expect(30 /* "NAMESTATES" */);
		Expect(26 /* "(" */);
		TRY(factory().startVariableDeclaration());
		TRY(factory().variableName(var));
		
		MODALITY_LIST(nbrMod);
		Expect(27 /* ")" */);
		Expect(9 /* ";" */);
		TRY(factory().endVariableDeclaration());
		gum::Size i;
		TRY(factory().startParentsDeclaration(var));
		
		for(i = 0; i < parents.size(); i++){
			TRY(factory().variableId(parents[i]));
			TRY(factory().addParent(parents[i]));
		}
		
		TRY(factory().endParentsDeclaration());
		
		PROBA(var, parents);
		Size nbr=0;
		TRY(nbr=factory().varInBN(factory().variableId(var)).domainSize());
		if (nbrMod<nbr) SemErr("Too much modalities for variable "+var);
		if (nbrMod>nbr) SemErr("Too many modalities for variable "+var);
		
		Expect(8 /* "}" */);
		Expect(9 /* ";" */);
}

void Parser::EXTRA_DEFINITION_PART() {
		Expect(18 /* "EXTRA_DEFINITION" */);
		BLOC_PART();
}

void Parser::BLOC_PART() {
		Expect(7 /* "{" */);
		while (StartOf(1)) {
			if (StartOf(2)) {
				Get();
			} else {
				BLOC_PART();
			}
		}
		Expect(8 /* "}" */);
		Expect(9 /* ";" */);
}

void Parser::PARENTS_LIST(std::vector<std::string>& parents ) {
		std::string parent;
		
		IDENT(parent);
		parents.push_back(parent);	
		while (la->kind == 28 /* "," */) {
			Get();
			IDENT(parent);
			parents.push_back(parent); 
		}
}

void Parser::MODALITY_LIST(Size& nbrMod) {
		std::string label; 
		IDENT_OR_INTEGER(label);
		if ((label=="") && (nbrMod == 0)) SemErr("Not enough modalities for a discrete variable");
			TRY(factory().addModality(label));
			nbrMod++;
		
		if (la->kind == 28 /* "," */) {
			Get();
			MODALITY_LIST(nbrMod);
		}
}

void Parser::PROBA(const std::string& var, const std::vector<std::string>& parents ) {
		Expect(31 /* "PROBABILITIES" */);
		Expect(26 /* "(" */);
		RAW_PROBA(var, parents);
		Expect(27 /* ")" */);
		Expect(9 /* ";" */);
}

void Parser::IDENT_OR_INTEGER(std::string& name) {
		if (la->kind == _ident) {
			IDENT(name);
		} else if (la->kind == _integer) {
			Get();
			name=narrow(t->val);
		} else SynErr(35);
}

void Parser::RAW_PROBA(const std::string& var, const std::vector<std::string>& parents ) {
		std::vector<float> v;
		std::vector<float> prob;
		gum::Size i,j, k;
		gum::Size res, max, nbLabels;
		
		res = factory().varInBN(factory().variableId(var)).domainSize();
		
		for(i = 0; i < parents.size(); i++){
		res = res*(factory().varInBN(factory().variableId(parents[i])).domainSize());
		}
		
		//v.resize(res);
		//prob	.resize(res);
		
		
		FLOAT_LIST(v);
		nbLabels = factory().varInBN(factory().variableId(var)).domainSize();
		max = res / nbLabels;
		
		j = 0;
		k = 0;
		for(i = 0; i < res; i++){
		if(i%max == 0){
			prob.push_back(v[k]);
			k++; j=1;
		}
		else{
			prob.push_back(v[j*nbLabels + k-1]);
			j++;
		}
		}
		
		TRY(factory().startRawProbabilityDeclaration(var));
		     gum::Size s=(gum::Size)0;
		     TRY(s=factory().cptDomainSize(factory().variableId(var)));
		     if ((int)prob.size()<(int)s) {
				Warning("Not enough data for cpt of node "+var);
			}
		     if ((int)prob.size()>(int)s) {
				Warning("Too many data for cpt of node "+var);
			}
		     TRY(factory().rawConditionalTable(prob));
		     TRY(factory().endRawProbabilityDeclaration());
		
		
}

void Parser::FLOAT_LIST(std::vector<float>& v ) {
		float value=0.0; 
		FLOAT(value);
		v.push_back(value); 
		while (StartOf(3)) {
			if (la->kind == 28 /* "," */ || la->kind == 32 /* "|" */) {
				if (la->kind == 28 /* "," */) {
					Get();
				} else {
					Get();
				}
			}
			FLOAT(value);
			v.push_back(value); 
		}
}

void Parser::FLOAT(float& val) {
		if (la->kind == _number) {
			Get();
			val=coco_atof(t->val); 
		} else if (la->kind == _integer) {
			Get();
			val=float(coco_atoi(t->val)); 
		} else SynErr(36);
}



// If the user declared a method Init and a mehtod Destroy they should
// be called in the contructur and the destructor respctively.
//
// The following templates are used to recognize if the user declared
// the methods Init and Destroy.

template<typename T>
struct ParserInitExistsRecognizer {
  template<typename U, void ( U::* )() = &U::Init>
  struct ExistsIfInitIsDefinedMarker {};

  struct InitIsMissingType {
    char dummy1;
  };

  struct InitExistsType {
    char dummy1; char dummy2;
  };

  // exists always
  template<typename U>
  static InitIsMissingType is_here( ... );

  // exist only if ExistsIfInitIsDefinedMarker is defined
  template<typename U>
  static InitExistsType is_here( ExistsIfInitIsDefinedMarker<U>* );

  enum { InitExists = ( sizeof( is_here<T>( nullptr ) ) == sizeof( InitExistsType ) ) };
};

template<typename T>
struct ParserDestroyExistsRecognizer {
  template<typename U, void ( U::* )() = &U::Destroy>
  struct ExistsIfDestroyIsDefinedMarker {};

  struct DestroyIsMissingType {
    char dummy1;
  };

  struct DestroyExistsType {
    char dummy1; char dummy2;
  };

  // exists always
  template<typename U>
  static DestroyIsMissingType is_here( ... );

  // exist only if ExistsIfDestroyIsDefinedMarker is defined
  template<typename U>
  static DestroyExistsType is_here( ExistsIfDestroyIsDefinedMarker<U>* );

  enum { DestroyExists = ( sizeof( is_here<T>( nullptr ) ) == sizeof( DestroyExistsType ) ) };
};

// The folloing templates are used to call the Init and Destroy methods if they exist.

// Generic case of the ParserInitCaller, gets used if the Init method is missing
template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
struct ParserInitCaller {
  static void CallInit( T* t ) {
    // nothing to do
  }
};

// True case of the ParserInitCaller, gets used if the Init method exists
template<typename T>
struct ParserInitCaller<T, true> {
  static void CallInit( T* t ) {
    t->Init();
  }
};

// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
struct ParserDestroyCaller {
  static void CallDestroy( T* t ) {
    // nothing to do
  }
};

// True case of the ParserDestroyCaller, gets used if the Destroy method exists
template<typename T>
struct ParserDestroyCaller<T, true> {
  static void CallDestroy( T* t ) {
    t->Destroy();
  }
};
void Parser::Parse() {
  t = nullptr;
  la = dummyToken = new Token();
  la->val = coco_string_create( L"Dummy Token" );
  Get();
  	DSL();
	Expect(0);
}

Parser::Parser( Scanner* scanner ) {
  	maxT = 33;

  ParserInitCaller<Parser>::CallInit( this );
  dummyToken = nullptr;
  t = la = nullptr;
  minErrDist = 2;
  errDist = minErrDist;
  this->scanner = scanner;
}

bool Parser::StartOf( int s ) {
  const bool T = true;
  const bool x = false;

  	static bool set[4][35] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,T,T, T,T,T,T, x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x},
		{x,T,T,T, T,T,T,x, x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x},
		{x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, T,x,x}
	};



  return set[s][la->kind];
}

Parser::~Parser() {
  ParserDestroyCaller<Parser>::CallDestroy( this );
  delete dummyToken;
}
void Parser::SemErr( const wchar_t* msg ) {
  if ( errDist >= minErrDist ) errors__.Error( scanner->filename(),t->line, t->col, msg );

  errDist = 0;
}

void Parser::Warning( const wchar_t* msg ) {
  errors__.Warning( scanner->filename(),t->line, t->col, msg );
}

void Parser::SynErr( const std::wstring& filename,int line, int col, int n ) {
  wchar_t* s;

  switch ( n ) {
      			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"ident expected"); break;
			case 2: s = coco_string_create(L"integer expected"); break;
			case 3: s = coco_string_create(L"number expected"); break;
			case 4: s = coco_string_create(L"string expected"); break;
			case 5: s = coco_string_create(L"largestring expected"); break;
			case 6: s = coco_string_create(L"\"net\" expected"); break;
			case 7: s = coco_string_create(L"\"{\" expected"); break;
			case 8: s = coco_string_create(L"\"}\" expected"); break;
			case 9: s = coco_string_create(L"\";\" expected"); break;
			case 10: s = coco_string_create(L"\"NUMSAMPLES\" expected"); break;
			case 11: s = coco_string_create(L"\"BKCOLOR\" expected"); break;
			case 12: s = coco_string_create(L"\"SHOWAS\" expected"); break;
			case 13: s = coco_string_create(L"\"node\" expected"); break;
			case 14: s = coco_string_create(L"\"TYPE\" expected"); break;
			case 15: s = coco_string_create(L"\"SCREEN\" expected"); break;
			case 16: s = coco_string_create(L"\"USER_PROPERTIES\" expected"); break;
			case 17: s = coco_string_create(L"\"DOCUMENTATION\" expected"); break;
			case 18: s = coco_string_create(L"\"EXTRA_DEFINITION\" expected"); break;
			case 19: s = coco_string_create(L"\"OBSERVATION_COST\" expected"); break;
			case 20: s = coco_string_create(L"\"HEADER\" expected"); break;
			case 21: s = coco_string_create(L"\"CREATION\" expected"); break;
			case 22: s = coco_string_create(L"\"WINDOWPOSITION\" expected"); break;
			case 23: s = coco_string_create(L"\"ID\" expected"); break;
			case 24: s = coco_string_create(L"\"NAME\" expected"); break;
			case 25: s = coco_string_create(L"\"PARENTS\" expected"); break;
			case 26: s = coco_string_create(L"\"(\" expected"); break;
			case 27: s = coco_string_create(L"\")\" expected"); break;
			case 28: s = coco_string_create(L"\",\" expected"); break;
			case 29: s = coco_string_create(L"\"DEFINITION\" expected"); break;
			case 30: s = coco_string_create(L"\"NAMESTATES\" expected"); break;
			case 31: s = coco_string_create(L"\"PROBABILITIES\" expected"); break;
			case 32: s = coco_string_create(L"\"|\" expected"); break;
			case 33: s = coco_string_create(L"??? expected"); break;
			case 34: s = coco_string_create(L"invalid DSL"); break;
			case 35: s = coco_string_create(L"invalid IDENT_OR_INTEGER"); break;
			case 36: s = coco_string_create(L"invalid FLOAT"); break;


    default: {
      wchar_t format[20];
      coco_swprintf( format, 20, L"error %d", n );
      s = coco_string_create( format );
    }
    break;
  }

  //wprintf(L"-- line %d col %d: %ls\n", line, col, s);
  std::wstring ss=L"Syntax error : "+std::wstring( s );
  errors__.Error( filename,line,col,ss.c_str() );
  coco_string_delete( s );
}

} // namespace
} // namespace



