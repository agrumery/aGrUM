

#include <iostream>
#include <wchar.h>

#include "Parser.h"
#include "Scanner.h"


namespace gum {
namespace BIF {


void Parser::SynErr(int n) {
	if (errDist >= minErrDist) errors->SynErr(scanner->filename(),la->line, la->col, n);
	errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
	if (errDist >= minErrDist) errors->Error(scanner->filename(),t->line, t->col, msg);
	errDist = 0;
}

void Parser::Warning(const wchar_t* msg) {
	errors->Warning(scanner->filename(),t->line, t->col, msg);
}

void Parser::Get() {
	for (;;) {
		t = la;
		la = scanner->Scan();
		if (la->kind <= maxT) { ++errDist; break; }

		if (dummyToken != t) {
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}

void Parser::Expect(int n) {
	if (la->kind==n) Get(); else { SynErr(n); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::BIF() {
		NETWORK();
		while (la->kind == 9 || la->kind == 16) {
			if (la->kind == 9) {
				while (!(la->kind == 0 || la->kind == 9)) {SynErr(26); Get();}
				VARIABLE();
			} else {
				while (!(la->kind == 0 || la->kind == 16)) {SynErr(27); Get();}
				PROBA();
			}
		}
}

void Parser::NETWORK() {
		std::string name_of_network;
		factory().startNetworkDeclaration();
		
		Expect(6);
		if (la->kind == 1) {
			IDENT(name_of_network);
		} else if (la->kind == 4) {
			STRING(name_of_network);
		} else SynErr(28);
		factory().addNetworkProperty("name", name_of_network); 
		Expect(7);
		while (la->kind == 23) {
			PROPERTY();
		}
		Expect(8);
		factory().endNetworkDeclaration(); 
}

void Parser::VARIABLE() {
		std::string name_of_var;
		std::string name_of_type;
		int nbrMod;
		factory().startVariableDeclaration();
		
		Expect(9);
		IDENT(name_of_var);
		TRY( factory().variableName(name_of_var)); 
		Expect(7);
		while (la->kind == 23) {
			PROPERTY();
		}
		LABELIZE_VAR(nbrMod);
		TRY(factory().endVariableDeclaration());
		int nbr=0;
		TRY(nbr=factory().variableNodeMap()->variable(factory().variableId(name_of_var)).domainSize());
		if (nbrMod<nbr) SemErr("Too much modalities for variable "+name_of_var);
		if (nbrMod>nbr) SemErr("Too many modalities for variable "+name_of_var);
		             
		while (la->kind == 23) {
			PROPERTY();
		}
		Expect(8);
}

void Parser::PROBA() {
		Expect(16);
		std::string var;
		std::vector<float> proba;
		std::vector<std::string> parents;
		bool error_on_variable=false;
		
		Expect(17);
		IDENT(var);
		try { factory().variableId(var); } catch (gum::Exception& e) { error_on_variable=true;SemErr(e.getType());}
		TRY(factory().startParentsDeclaration(var));
		
		if (la->kind == 18) {
			Get();
			LISTE_PARENTS(parents);
		}
		Expect(19);
		TRY(factory().endParentsDeclaration()); 
		Expect(7);
		while (la->kind == 23) {
			PROPERTY();
		}
		if (la->kind == 20) {
			RAW_PROBA(proba);
			if (! error_on_variable) {
			TRY(factory().startRawProbabilityDeclaration(var));
			gum::Size s=(gum::Size)0;
			TRY(s=factory().cptDomainSize(factory().variableId(var)));
			if (proba.size()<s) Warning("Not enough data for cpt of node "+var);
			if (proba.size()>s) Warning("Too many data for cpt of node "+var);
			TRY(factory().rawConditionalTable(proba));
			TRY(factory().endRawProbabilityDeclaration());
			}
			
			Expect(14);
		} else if (la->kind == 17 || la->kind == 21) {
			FACTORIZED_PROBA(var,parents);
		} else SynErr(29);
		Expect(8);
}

void Parser::IDENT(std::string& name) {
		Expect(1);
		name=narrow(t->val);  
}

void Parser::STRING(std::string& str) {
		Expect(4);
		str=narrow(t->val); 
}

void Parser::PROPERTY() {
		std::string name_of_prop,content; float val; 
		Expect(23);
		IDENT(name_of_prop);
		if (la->kind == 24) {
			Get();
			STRING(content);
			Expect(14);
		} else if (la->kind == 5) {
			Get();
		} else if (la->kind == 1) {
			IDENT(content);
			Expect(14);
		} else if (la->kind == 2 || la->kind == 3) {
			FLOAT(val);
			Expect(14);
		} else SynErr(30);
		Warning("Properties are not supported yet"); 
}

void Parser::LABELIZE_VAR(int& nbrMod) {
		Expect(10);
		Expect(11);
		Expect(12);
		NBR(nbrMod);
		Expect(13);
		if (nbrMod<=1) SemErr("Not enough modalities for a discrete variable"); 
		Expect(7);
		MODALITY_LIST();
		Expect(8);
		Expect(14);
}

void Parser::NBR(int& val) {
		Expect(2);
		swscanf(t->val, L"%d", &val); 
}

void Parser::MODALITY_LIST() {
		std::string label; 
		IDENT_OR_INTEGER(label);
		TRY(factory().addModality(label)); 
		if (la->kind == 15) {
			ExpectWeak(15, 1);
			MODALITY_LIST();
		}
}

void Parser::IDENT_OR_INTEGER(std::string& name) {
		if (la->kind == 1) {
			IDENT(name);
		} else if (la->kind == 2) {
			Get();
			name=narrow(t->val);  
		} else SynErr(31);
}

void Parser::LISTE_PARENTS(std::vector<std::string>& parents ) {
		std::string parent; 
		IDENT(parent);
		TRY(factory().variableId(parent));
		TRY( factory().addParent(parent));
		parents.push_back(parent);
		
		if (la->kind == 15) {
			ExpectWeak(15, 2);
			LISTE_PARENTS(parents);
		}
}

void Parser::RAW_PROBA(std::vector<float>& v ) {
		Expect(20);
		v.clear(); 
		LISTE_FLOAT(v);
}

void Parser::FACTORIZED_PROBA(std::string& var,const std::vector<std::string>& parents ) {
		TRY(factory().startFactorizedProbabilityDeclaration(var));
		
		ASSIGNATION(var,parents,true);
		while (la->kind == 17 || la->kind == 21) {
			ASSIGNATION(var,parents,false);
		}
		TRY(factory().endFactorizedProbabilityDeclaration()); 
}

void Parser::LISTE_FLOAT(std::vector<float>& v ) {
		float value; 
		FLOAT(value);
		v.push_back(value); 
		if (StartOf(3)) {
			if (la->kind == 15 || la->kind == 18) {
				if (la->kind == 15) {
					Get();
				} else {
					Get();
				}
			}
			LISTE_FLOAT(v);
		}
}

void Parser::ASSIGNATION(const std::string& var,const std::vector<std::string>& parents,bool is_first ) {
		std::vector<float> v; std::vector<std::string> labels;
		
		if (la->kind == 17) {
			Get();
			TRY(factory().startFactorizedEntry()); 
			LISTE_LABELS(parents,labels,0);
			Expect(19);
			LISTE_FLOAT(v);
			__checkSizeOfProbabilityAssignation(v,var);
			TRY(factory().setVariableValuesUnchecked(v));
			TRY(factory().endFactorizedEntry());
			
		} else if (la->kind == 21) {
			Get();
			if (! is_first) SemErr("'default' assignation has to be the first."); 
			LISTE_FLOAT(v);
			TRY(factory().startFactorizedEntry());
			__checkSizeOfProbabilityAssignation(v,var);
			TRY(factory().setVariableValuesUnchecked(v));
			TRY(factory().endFactorizedEntry());
			
		} else SynErr(32);
		Expect(14);
}

void Parser::LISTE_LABELS(const std::vector<std::string>& parents,std::vector<std::string>& labels, unsigned int num_label ) {
		std::string name_of_label;
		if (num_label>=parents.size()) SemErr("Too many labels in this assignation");
		
		if (la->kind == 1 || la->kind == 2) {
			IDENT_OR_INTEGER(name_of_label);
			labels.push_back(name_of_label);
			TRY(factory().setParentModality(parents[num_label],name_of_label));
			
		} else if (la->kind == 22) {
			Get();
			labels.push_back("*"); 
		} else SynErr(33);
		if (la->kind == 15) {
			ExpectWeak(15, 4);
			LISTE_LABELS(parents,labels,num_label+1);
		}
}

void Parser::FLOAT(float& val) {
		if (la->kind == 3) {
			Get();
			swscanf(t->val, L"%f", &val); 
		} else if (la->kind == 2) {
			Get();
			swscanf(t->val, L"%f", &val); 
		} else SynErr(34);
}



void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	BIF();

	Expect(0);
}

Parser::Parser(Scanner *scanner) {
	maxT = 25;

	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors();
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[5][27] = {
		{T,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x},
		{T,T,T,x, x,x,x,x, x,T,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x},
		{T,T,x,x, x,x,x,x, x,T,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x},
		{x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,T, x,x,T,x, x,x,x,x, x,x,x},
		{T,T,T,x, x,x,x,x, x,T,x,x, x,x,x,x, T,x,x,x, x,x,T,x, x,x,x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	delete errors;
	delete dummyToken;
}

Errors::Errors() {
	error_count = 0;
	warning_count=0;
}

void Errors::SynErr(const std::wstring& filename,int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"ident expected"); break;
			case 2: s = coco_string_create(L"integer expected"); break;
			case 3: s = coco_string_create(L"number expected"); break;
			case 4: s = coco_string_create(L"string expected"); break;
			case 5: s = coco_string_create(L"largestring expected"); break;
			case 6: s = coco_string_create(L"\"network\" expected"); break;
			case 7: s = coco_string_create(L"\"{\" expected"); break;
			case 8: s = coco_string_create(L"\"}\" expected"); break;
			case 9: s = coco_string_create(L"\"variable\" expected"); break;
			case 10: s = coco_string_create(L"\"type\" expected"); break;
			case 11: s = coco_string_create(L"\"discrete\" expected"); break;
			case 12: s = coco_string_create(L"\"[\" expected"); break;
			case 13: s = coco_string_create(L"\"]\" expected"); break;
			case 14: s = coco_string_create(L"\";\" expected"); break;
			case 15: s = coco_string_create(L"\",\" expected"); break;
			case 16: s = coco_string_create(L"\"probability\" expected"); break;
			case 17: s = coco_string_create(L"\"(\" expected"); break;
			case 18: s = coco_string_create(L"\"|\" expected"); break;
			case 19: s = coco_string_create(L"\")\" expected"); break;
			case 20: s = coco_string_create(L"\"table\" expected"); break;
			case 21: s = coco_string_create(L"\"default\" expected"); break;
			case 22: s = coco_string_create(L"\"*\" expected"); break;
			case 23: s = coco_string_create(L"\"property\" expected"); break;
			case 24: s = coco_string_create(L"\"=\" expected"); break;
			case 25: s = coco_string_create(L"??? expected"); break;
			case 26: s = coco_string_create(L"this symbol not expected in BIF"); break;
			case 27: s = coco_string_create(L"this symbol not expected in BIF"); break;
			case 28: s = coco_string_create(L"invalid NETWORK"); break;
			case 29: s = coco_string_create(L"invalid PROBA"); break;
			case 30: s = coco_string_create(L"invalid PROPERTY"); break;
			case 31: s = coco_string_create(L"invalid IDENT_OR_INTEGER"); break;
			case 32: s = coco_string_create(L"invalid ASSIGNATION"); break;
			case 33: s = coco_string_create(L"invalid LISTE_LABELS"); break;
			case 34: s = coco_string_create(L"invalid FLOAT"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	//wprintf(L"-- line %d col %d: %ls\n", line, col, s);
  add_error(true,filename,line,col,L"Syntax error : "+std::wstring(s));
	coco_string_delete(s);
	error_count++;
}

void Errors::Error(const std::wstring& filename,int line, int col, const wchar_t *s) {
	//wprintf(L"-- line %d col %d: %ls\n", line, col, s);
  add_error(true,filename,line,col,std::wstring(s));
	error_count++;
}

void Errors::Warning(const std::wstring& filename,int line, int col, const wchar_t *s) {
	//wprintf(L"-- line %d col %d: %ls\n", line, col, s);
  add_error(false,filename,line,col,std::wstring(s));
	warning_count++;
}

void Errors::Warning(const std::wstring& filename,const wchar_t *s) {
	//wprintf(L"%ls\n", s);
  add_error(false,filename,0,0,std::wstring(s));
	warning_count++;
}

void Errors::Exception(const std::wstring& filename,const wchar_t* s) {
	//wprintf(L"%ls", s);
  add_error(true,filename,0,0,std::wstring(s));
	exit(1);
}

void Errors::add_error(const bool is_error,const std::wstring& filename,int line ,int col,const std::wstring& s) const {
	storer.add(is_error,filename,s,line,col);
}
} // namespace
} // namespace


