

#include <iostream>
#include <wchar.h>

#include "Parser.h"
#include "Scanner.h"


namespace gum {
namespace DSL {


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

void Parser::DSL() {
		NETWORK();
		while (la->kind == 10) {
			NODE();
		}
		Expect(6);
		Expect(7);
}

void Parser::NETWORK() {
		std::string name_of_network;
		factory().startNetworkDeclaration();
		
		Expect(8);
		if (la->kind == 1) {
			IDENT(name_of_network);
		} else if (la->kind == 4) {
			STRING(name_of_network);
		} else SynErr(24);
		factory().addNetworkProperty("name", name_of_network); 
		Expect(9);
		factory().endNetworkDeclaration(); 
}

void Parser::NODE() {
		std::string var;
		std::vector<std::string> parents;
		int nbrMod = 0;
		
		Expect(10);
		IDENT(var);
		HEADER();
		PARENTS(parents);
		VARIABLE_DEFINITION(nbrMod, var);
		gum::Size i;
		TRY(factory().startParentsDeclaration(var));
		
		for(i = 0; i < parents.size(); i++){
		   	TRY(factory().variableId(parents[i]));
		   	TRY(factory().addParent(parents[i]));
		}
		
			TRY(factory().endParentsDeclaration());
		
		PROBA(var, parents);
		int nbr=0;
		TRY(nbr=factory().varInBN(factory().variableId(var)).domainSize());
		if (nbrMod<nbr) SemErr("Too much modalities for variable "+var);
		if (nbrMod>nbr) SemErr("Too many modalities for variable "+var);
		
}

void Parser::IDENT(std::string& name) {
		Expect(1);
		name=narrow(t->val); 
}

void Parser::STRING(std::string& str) {
		Expect(4);
		str=narrow(t->val); 
}

void Parser::HEADER() {
		std::string content; 
		Expect(9);
		Expect(11);
		Expect(1);
		Expect(7);
		Expect(12);
		Expect(9);
		Expect(13);
		Expect(1);
		Expect(7);
		Expect(14);
		Expect(4);
		Expect(7);
		Expect(6);
		Expect(7);
}

void Parser::PARENTS(std::vector<std::string>& parents ) {
		Expect(15);
		Expect(16);
		if (la->kind == 1) {
			PARENTS_LIST(parents);
		}
		Expect(17);
		Expect(7);
}

void Parser::VARIABLE_DEFINITION(int& nbrMod, std::string& var ) {
		Expect(19);
		Expect(9);
		Expect(20);
		Expect(16);
		TRY(factory().startVariableDeclaration());
		TRY(factory().variableName(var));
		
		MODALITY_LIST(nbrMod);
		Expect(17);
		Expect(7);
		TRY(factory().endVariableDeclaration()); 
}

void Parser::PROBA(const std::string& var, const std::vector<std::string>& parents ) {
		Expect(21);
		Expect(16);
		RAW_PROBA(var, parents);
		Expect(17);
		Expect(7);
		Expect(6);
		Expect(7);
		Expect(6);
		Expect(7);
}

void Parser::PARENTS_LIST(std::vector<std::string>& parents ) {
		std::string parent; 
		IDENT(parent);
		parents.push_back(parent);	
		if (la->kind == 18) {
			ExpectWeak(18, 1);
			PARENTS_LIST(parents);
		}
}

void Parser::MODALITY_LIST(int& nbrMod) {
		std::string label; 
		IDENT_OR_INTEGER(label);
		if ((label=="") && (nbrMod == 0)) SemErr("Not enough modalities for a discrete variable");
			TRY(factory().addModality(label)); 
			nbrMod++;
		
		if (la->kind == 18) {
			Get();
			MODALITY_LIST(nbrMod);
		}
}

void Parser::IDENT_OR_INTEGER(std::string& name) {
		if (la->kind == 1) {
			IDENT(name);
		} else if (la->kind == 2) {
			Get();
			name=narrow(t->val);
		} else SynErr(25);
}

void Parser::RAW_PROBA(const std::string& var, const std::vector<std::string>& parents ) {
		std::vector<float> v;
		std::vector<float> prob;
		gum::Size i,j, k;
		gum::Size res, max, nbLabels;
		
		FLOAT_LIST(v);
		res = factory().varInBN(factory().variableId(var)).domainSize();
		
		for(i = 0; i < parents.size(); i++){
			res = res*(factory().varInBN(factory().variableId(parents[i])).domainSize());
		}
		
		
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
		        if ((int)prob.size()<(int)s) Warning("Not enough data for cpt of node "+var);
		        if ((int)prob.size()>(int)s) Warning("Too many data for cpt of node "+var);
		        TRY(factory().rawConditionalTable(prob));
		        TRY(factory().endRawProbabilityDeclaration());
		
			
}

void Parser::FLOAT_LIST(std::vector<float>& v ) {
		float value; 
		FLOAT(value);
		v.push_back(value); 
		if (StartOf(2)) {
			if (la->kind == 18 || la->kind == 22) {
				if (la->kind == 18) {
					Get();
				} else {
					Get();
				}
			}
			FLOAT_LIST(v);
		}
}

void Parser::FLOAT(float& val) {
		if (la->kind == 3) {
			Get();
			swscanf(t->val, L"%f", &val); 
		} else if (la->kind == 2) {
			Get();
			swscanf(t->val, L"%f", &val); 
		} else SynErr(26);
}



void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	DSL();

	Expect(0);
}

Parser::Parser(Scanner *scanner) {
	maxT = 23;

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

	static bool set[3][25] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,T,x, x}
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
			case 6: s = coco_string_create(L"\"}\" expected"); break;
			case 7: s = coco_string_create(L"\";\" expected"); break;
			case 8: s = coco_string_create(L"\"net\" expected"); break;
			case 9: s = coco_string_create(L"\"{\" expected"); break;
			case 10: s = coco_string_create(L"\"node\" expected"); break;
			case 11: s = coco_string_create(L"\"TYPE\" expected"); break;
			case 12: s = coco_string_create(L"\"HEADER\" expected"); break;
			case 13: s = coco_string_create(L"\"ID\" expected"); break;
			case 14: s = coco_string_create(L"\"NAME\" expected"); break;
			case 15: s = coco_string_create(L"\"PARENTS\" expected"); break;
			case 16: s = coco_string_create(L"\"(\" expected"); break;
			case 17: s = coco_string_create(L"\")\" expected"); break;
			case 18: s = coco_string_create(L"\",\" expected"); break;
			case 19: s = coco_string_create(L"\"DEFINITION\" expected"); break;
			case 20: s = coco_string_create(L"\"NAMESTATES\" expected"); break;
			case 21: s = coco_string_create(L"\"PROBABILITIES\" expected"); break;
			case 22: s = coco_string_create(L"\"|\" expected"); break;
			case 23: s = coco_string_create(L"??? expected"); break;
			case 24: s = coco_string_create(L"invalid NETWORK"); break;
			case 25: s = coco_string_create(L"invalid IDENT_OR_INTEGER"); break;
			case 26: s = coco_string_create(L"invalid FLOAT"); break;

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


