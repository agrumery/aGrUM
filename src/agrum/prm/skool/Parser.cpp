

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
		while (la->kind == 14 || la->kind == 15) {
			Header();
		}
		while (StartOf(1)) {
			Unit();
		}
}

void Parser::Header() {
		if (la->kind == 14) {
			Package();
		} else if (la->kind == 15) {
			Import();
		} else SynErr(31);
}

void Parser::Unit() {
		if (la->kind == 9) {
			Type();
		} else if (la->kind == 11) {
			Interface();
		} else if (la->kind == 10) {
			Class();
		} else if (la->kind == 13) {
			System();
		} else SynErr(32);
}

void Parser::Package() {
		Expect(14);
		std::string name; 
		Ident(name);
		Expect(8);
		factory().setPackage(name); 
}

void Parser::Import() {
		Expect(15);
		std::string name; 
		Ident(name);
		Expect(8);
		std::cerr << "Import disabled."; 
}

void Parser::Ident(std::string& s) {
		Expect(3);
		s = narrow(t->val); 
		while (la->kind == 5) {
			Get();
			Expect(3);
			s += '.'; s += narrow(t->val); 
		}
}

void Parser::Type() {
		if (isBaseType()) {
			BaseType();
		} else if (la->kind == 9) {
			SuperType();
		} else SynErr(33);
}

void Parser::Interface() {
		Expect(11);
		Expect(3);
		std::string name, extends; name = narrow(t->val); 
		if (la->kind == 12) {
			Get();
			Ident(extends);
		}
		factory().startInterface(name, extends); 
		InterfaceBody();
		factory().endInterface(); 
}

void Parser::Class() {
		Expect(10);
		Expect(3);
		std::string name, super; name = narrow(t->val); 
		if (la->kind == 12) {
			Get();
			Ident(super);
		}
		gum::Set<std::string> set; 
		if (la->kind == 18) {
			Get();
			IdentSet(set);
		}
		factory().startClass(name, super, (set.empty()?0:&set)); 
		ClassBody();
		factory().endClass(); 
}

void Parser::System() {
		Expect(13);
		Expect(3);
		factory().startSystem(narrow(t->val)); 
		Expect(16);
		while (la->kind == 3) {
			SystemDecl();
		}
		Expect(17);
		factory().endSystem(); 
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
		Expect(12);
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
			Get();
			s = narrow(t->val); 
		} else if (la->kind == 1 || la->kind == 2) {
			float val; 
			Number(val);
			std::stringstream sBuff; sBuff << val; s = sBuff.str(); 
		} else SynErr(34);
}

void Parser::InterfaceBody() {
		Expect(16);
		while (la->kind == 3) {
			InterfaceStatement();
		}
		Expect(17);
}

void Parser::InterfaceStatement() {
		std::string type, name; 
		Ident(type);
		Expect(3);
		name = narrow(t->val); 
		RefOrAttr(type, name);
}

void Parser::RefOrAttr(std::string& type, std::string& name) {
		if (isClassOrInterface(type)) {
			Reference(type, name);
		} else if (la->kind == 8) {
			Get();
			factory().startAttribute(type, name); factory().endAttribute(); 
		} else SynErr(35);
}

void Parser::Reference(std::string type, std::string name) {
		if (la->kind == 8) {
			Get();
			factory().addReferenceSlot(type, name, false); 
		} else if (la->kind == 19) {
			Get();
			Expect(20);
			Expect(8);
			factory().addReferenceSlot(type, name, true); 
		} else SynErr(36);
}

void Parser::IdentSet(gum::Set<std::string>& set ) {
		std::string ident; 
		Ident(ident);
		set.insert(ident); 
		if (la->kind == 6) {
			Get();
			IdentSet(set);
		}
}

void Parser::ClassBody() {
		Expect(16);
		while (la->kind == 3) {
			ClassStatement();
		}
		Expect(17);
}

void Parser::ClassStatement() {
		std::string name, type; 
		Ident(type);
		Expect(3);
		name = narrow(t->val); 
		if (la->kind == 8 || la->kind == 19 || la->kind == 21) {
			RefOrParam(type, name);
		} else if (la->kind == 16 || la->kind == 22) {
			Attribute(type, name);
		} else if (la->kind == 23) {
			Aggregate(type, name);
		} else SynErr(37);
}

void Parser::RefOrParam(std::string type, std::string name) {
		if (isClassOrInterface(type)) {
			Reference(type, name);
		} else if (la->kind == 8 || la->kind == 21) {
			Parameter(type, name);
		} else SynErr(38);
}

void Parser::Attribute(std::string type, std::string name) {
		factory().startAttribute(type, name); 
		if (la->kind == 22) {
			Dependecies();
		}
		Expect(16);
		if (la->kind == 19) {
			std::vector<float> cpt; 
			Get();
			FloatList(cpt);
			Expect(20);
			factory().setRawCPFByColumns(cpt); 
		} else if (la->kind == 3 || la->kind == 26) {
			FactorizedCPT();
		} else SynErr(39);
		Expect(17);
		Expect(8);
		factory().endAttribute(); 
}

void Parser::Aggregate(std::string type, std::string name) {
		std::string func; std::vector<std::string> chains, params; 
		Expect(23);
		Function(func, chains, params);
		Expect(8);
		factory().addAggregator(name, func, chains, params); 
}

void Parser::Parameter(std::string type, std::string name) {
		if (la->kind == 8) {
			Get();
			factory().addParameter(type, name); 
		} else if (la->kind == 21) {
			std::string l; 
			Get();
			Label(l);
			Expect(8);
			factory().addParameter(type, name, l); 
		} else SynErr(40);
}

void Parser::Dependecies() {
		std::vector<std::string> prnts; 
		Expect(22);
		IdentList(prnts);
		for (size_t i = 0; i < prnts.size(); ++i) factory().addParent(prnts[i]); 
}

void Parser::FloatList(std::vector<float>& list ) {
		float f; 
		Number(f);
		list.push_back(f); 
		if (la->kind == 6) {
			Get();
			FloatList(list);
		}
}

void Parser::FactorizedCPT() {
		CPTRule();
		while (la->kind == 3 || la->kind == 26) {
			CPTRule();
		}
}

void Parser::IdentList(std::vector<std::string>& set ) {
		std::string ident; 
		SlotChain(ident);
		set.push_back(ident); 
		if (la->kind == 6) {
			Get();
			IdentList(set);
		}
}

void Parser::CPTRule() {
		std::vector<std::string> labels; std::vector<float> values; 
		LabelList(labels);
		Expect(7);
		FloatList(values);
		Expect(8);
		factory().setCPFByRule(labels, values); 
}

void Parser::LabelList(std::vector<std::string>& list ) {
		if (la->kind == 3) {
			Get();
			list.push_back(narrow(t->val)); 
		} else if (la->kind == 26) {
			Get();
			list.push_back(narrow(t->val)); 
		} else SynErr(41);
		if (la->kind == 6) {
			Get();
			LabelList(list);
		}
}

void Parser::Function(std::string& name,
std::vector<std::string>& chains,
std::vector<std::string>& params ) {
		Expect(3);
		name = narrow(t->val); 
		Expect(24);
		if (la->kind == 19) {
			Get();
			IdentList(chains);
			Expect(20);
			Expect(6);
			LabelList(params);
		} else if (la->kind == 3 || la->kind == 27) {
			std::vector<std::string> vec; 
			IdentList(vec);
			chains.push_back(vec.front());
			for (size_t idx = 1; idx < vec.size(); ++idx) params.push_back(vec[idx]); 
		} else SynErr(42);
		Expect(25);
}

void Parser::SlotChain(std::string& s) {
		if (la->kind == 27) {
			Get();
			s += '<'; std::string val; 
			Ident(val);
			s += val; 
			Expect(28);
			s += narrow(t->val); 
		}
		Expect(3);
		s += narrow(t->val); 
		while (la->kind == 5) {
			Get();
			s += '.'; 
			if (la->kind == 27) {
				Get();
				s += '<'; std::string val; 
				Ident(val);
				s += val; 
				Expect(28);
				s += narrow(t->val); 
			}
			Expect(3);
			s += narrow(t->val); 
		}
}

void Parser::Number(float& val) {
		if (la->kind == 1) {
			Get();
			swscanf(t->val, L"%f", &val); 
		} else if (la->kind == 2) {
			Get();
			swscanf(t->val, L"%f", &val); 
		} else SynErr(43);
}

void Parser::SystemDecl() {
		std::string left, last; std::string dot = "."; 
		SysLeftValue(left, last);
		std::string tmp = left; tmp += (last.size())?dot + last:""; 
		if (la->kind == 3) {
			SysInstDecl(tmp);
		} else if (la->kind == 19) {
			SysArray(tmp);
		} else if (la->kind == 23 || la->kind == 29) {
			SysAffect(left, last);
		} else SynErr(44);
}

void Parser::SysLeftValue(std::string& left, std::string& last) {
		Expect(3);
		left = narrow(t->val); 
		while (la->kind == 5) {
			Get();
			Expect(3);
			if (last != "") {left += '.'; left += last;} last = narrow(t->val); 
		}
}

void Parser::SysInstDecl(std::string type) {
		Expect(3);
		factory().addInstance(type, narrow(t->val)); 
		Expect(8);
}

void Parser::SysArray(std::string array) {
		int size = 0; 
		Expect(19);
		Expect(1);
		swscanf(t->val, L"%d", &size); 
		Expect(20);
		if (la->kind == 3) {
			Get();
			factory().addArray(array, narrow(t->val), size); 
			Expect(8);
		} else if (la->kind == 5) {
			Get();
			Expect(3);
			std::string ref = narrow(t->val); std::stringstream sBuff; sBuff << '[' << size << ']'; std::string inst = array + sBuff.str(); 
			SysAffect(inst, ref);
		} else SynErr(45);
}

void Parser::SysAffect(std::string inst, std::string ref) {
		bool ref_inc = false; 
		if (la->kind == 29) {
			Get();
			ref_inc = true; 
		}
		Expect(23);
		Expect(3);
		gum::prm::System& sys = factory().getCurrentSystem();
		if (sys.isInstance(inst) or sys.isArray(inst)) {
		  gum::prm::Class& c = (sys.isInstance(inst))?sys.get(inst).type():sys.getArray(inst).front()->type();
		  if (c.get(ref).elt_type() == ClassElement::prm_refslot) {
		    factory().setReferenceSlot(inst, ref, narrow(t->val));
		  } else if (c.get(ref).elt_type() == ClassElement::prm_attribute) {
		    factory().setParameter(inst, ref, narrow(t->val));
		  } else {
		    GUM_ERROR(NotFound, "unfound class element");
		  }
		} else {
		  GUM_ERROR(NotFound, "unknown instance");
		}
		
		Expect(8);
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
	maxT = 30;

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

	static bool set[2][32] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,T,T,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x}
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
			case 10: s = coco_string_create(L"class expected"); break;
			case 11: s = coco_string_create(L"interface expected"); break;
			case 12: s = coco_string_create(L"extends expected"); break;
			case 13: s = coco_string_create(L"system expected"); break;
			case 14: s = coco_string_create(L"\"package\" expected"); break;
			case 15: s = coco_string_create(L"\"import\" expected"); break;
			case 16: s = coco_string_create(L"\"{\" expected"); break;
			case 17: s = coco_string_create(L"\"}\" expected"); break;
			case 18: s = coco_string_create(L"\"implements\" expected"); break;
			case 19: s = coco_string_create(L"\"[\" expected"); break;
			case 20: s = coco_string_create(L"\"]\" expected"); break;
			case 21: s = coco_string_create(L"\"default\" expected"); break;
			case 22: s = coco_string_create(L"\"dependson\" expected"); break;
			case 23: s = coco_string_create(L"\"=\" expected"); break;
			case 24: s = coco_string_create(L"\"(\" expected"); break;
			case 25: s = coco_string_create(L"\")\" expected"); break;
			case 26: s = coco_string_create(L"\"*\" expected"); break;
			case 27: s = coco_string_create(L"\"<\" expected"); break;
			case 28: s = coco_string_create(L"\">\" expected"); break;
			case 29: s = coco_string_create(L"\"+\" expected"); break;
			case 30: s = coco_string_create(L"??? expected"); break;
			case 31: s = coco_string_create(L"invalid Header"); break;
			case 32: s = coco_string_create(L"invalid Unit"); break;
			case 33: s = coco_string_create(L"invalid Type"); break;
			case 34: s = coco_string_create(L"invalid Label"); break;
			case 35: s = coco_string_create(L"invalid RefOrAttr"); break;
			case 36: s = coco_string_create(L"invalid Reference"); break;
			case 37: s = coco_string_create(L"invalid ClassStatement"); break;
			case 38: s = coco_string_create(L"invalid RefOrParam"); break;
			case 39: s = coco_string_create(L"invalid Attribute"); break;
			case 40: s = coco_string_create(L"invalid Parameter"); break;
			case 41: s = coco_string_create(L"invalid LabelList"); break;
			case 42: s = coco_string_create(L"invalid Function"); break;
			case 43: s = coco_string_create(L"invalid Number"); break;
			case 44: s = coco_string_create(L"invalid SystemDecl"); break;
			case 45: s = coco_string_create(L"invalid SysArray"); break;

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


