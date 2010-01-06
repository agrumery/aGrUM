

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"


namespace gum {
namespace prm {
namespace skool {


void Parser::SynErr(int n) {
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
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

void Parser::Skool() {
		while (la->kind == 11 || la->kind == 12) {
			Header();
		}
		while (la->kind == 9) {
			Unit();
		}
}

void Parser::Header() {
		if (la->kind == 11) {
			Package();
		} else if (la->kind == 12) {
			Import();
		} else SynErr(14);
}

void Parser::Unit() {
		Type();
}

void Parser::Package() {
		Expect(11);
		std::string name; 
		Ident(name);
		Expect(8);
		factory().setPackage(name); 
}

void Parser::Import() {
		Expect(12);
		std::string name; 
		Ident(name);
		Expect(8);
		std::cerr << "Import disabled."; 
}

void Parser::Ident(std::string& s) {
		if (isPath()) {
			std::string val; 
			Path(val);
			s = val; 
		} else if (la->kind == 3) {
			Get();
			s = narrow(t->val); 
		} else SynErr(15);
}

void Parser::Type() {
		if (isBaseType()) {
			BaseType();
		} else if (la->kind == 9) {
			SuperType();
		} else SynErr(16);
}

void Parser::BaseType() {
		std::string l; 
		Expect(9);
		Expect(3);
		factory().startDiscreteType(narrow(t->val)); 
		Label(l);
		Expect(6);
		factory().addLabel(l); 
		Label(l);
		factory().addLabel(l); 
		while (la->kind == 6) {
			Get();
			Label(l);
			factory().addLabel(l); 
		}
		Expect(8);
		factory().endDiscreteType(); 
}

void Parser::SuperType() {
		Expect(9);
		Expect(3);
		std::string s, l, name, super; name = narrow(t->val); 
		Expect(10);
		Ident(super);
		factory().startDiscreteType(name, super); 
		Label(l);
		Expect(7);
		Label(s);
		Expect(6);
		factory().addLabel(l, s); 
		Label(l);
		Expect(7);
		Label(s);
		factory().addLabel(l, s); 
		while (la->kind == 6) {
			Get();
			Label(l);
			Expect(7);
			Label(s);
			factory().addLabel(l, s); 
		}
		Expect(8);
		factory().endDiscreteType(); 
}

void Parser::Label(std::string& s) {
		if (la->kind == 3) {
			std::string val; 
			Ident(val);
			s = val; 
		} else if (la->kind == 1) {
			Get();
			s = narrow(t->val); 
		} else if (la->kind == 2) {
			Get();
			s = narrow(t->val); 
		} else SynErr(17);
}

void Parser::Path(std::string& s) {
		Expect(3);
		s += narrow(t->val); 
		Expect(5);
		s += '.'; 
		Expect(3);
		s += narrow(t->val); 
		while (la->kind == 5) {
			Get();
			s += '.'; 
			Expect(3);
			s+= narrow(t->val); 
		}
}



void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	Skool();

	Expect(0);
}

Parser::Parser(Scanner *scanner) {
	maxT = 13;

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

	static bool set[1][15] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	delete errors;
	delete dummyToken;
}

Errors::Errors() {
	count = 0;
}

void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"integer expected"); break;
			case 2: s = coco_string_create(L"float expected"); break;
			case 3: s = coco_string_create(L"word expected"); break;
			case 4: s = coco_string_create(L"eol expected"); break;
			case 5: s = coco_string_create(L"dot expected"); break;
			case 6: s = coco_string_create(L"comma expected"); break;
			case 7: s = coco_string_create(L"colon expected"); break;
			case 8: s = coco_string_create(L"semicolon expected"); break;
			case 9: s = coco_string_create(L"type expected"); break;
			case 10: s = coco_string_create(L"extends expected"); break;
			case 11: s = coco_string_create(L"\"package\" expected"); break;
			case 12: s = coco_string_create(L"\"import\" expected"); break;
			case 13: s = coco_string_create(L"??? expected"); break;
			case 14: s = coco_string_create(L"invalid Header"); break;
			case 15: s = coco_string_create(L"invalid Ident"); break;
			case 16: s = coco_string_create(L"invalid Type"); break;
			case 17: s = coco_string_create(L"invalid Label"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	coco_string_delete(s);
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
}

void Errors::Warning(const wchar_t *s) {
	wprintf(L"%ls\n", s);
}

void Errors::Exception(const wchar_t* s) {
	wprintf(L"%ls", s); 
	exit(1);
}

} // namespace
} // namespace
} // namespace


