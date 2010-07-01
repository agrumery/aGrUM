

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"


namespace gum {
namespace prm {
namespace skool {


void Parser::SynErr(int n) {
  if (errDist >= minErrDist) errors->SynErr(scanner->filename, la->line, n);
  errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
  if (errDist >= minErrDist) errors->Error(scanner->filename, t->line, msg);
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
		if (la->kind == 19) {
			Package();
		}
		while (la->kind == 20) {
			Import();
		}
		while (StartOf(1)) {
			Unit();
		}
		factory().popPackage(); 
}

void Parser::Package() {
		Expect(19);
		std::string name; 
		Ident(name);
		Expect(8);
		factory().pushPackage(name); 
}

void Parser::Import() {
		Expect(20);
		std::string name; 
		Ident(name);
		Expect(8);
		TRY(import(name)) 
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
		} else SynErr(31);
}

void Parser::Ident(std::string& s) {
		std::stringstream sBuff; 
		Expect(3);
		sBuff << narrow(t->val); 
		while (la->kind == 5) {
			Get();
			Expect(3);
			sBuff << "." << narrow(t->val); 
		}
		s = sBuff.str(); 
}

void Parser::Type() {
		Expect(9);
		Expect(3);
		std::string name, extends, l, s; name = narrow(t->val); 
		if (la->kind == 12) {
			Get();
			Ident(extends);
		}
		TRY(factory().startDiscreteType(name, extends)) 
		Label(l);
		if (la->kind == 7) {
			Get();
			Label(s);
		}
		TRY(factory().addLabel(l, s)) 
		while (la->kind == 6) {
			s = ""; 
			Get();
			Label(l);
			if (la->kind == 7) {
				Get();
				Label(s);
			}
			TRY(factory().addLabel(l, s)) 
		}
		Expect(8);
		TRY(factory().endDiscreteType()) 
}

void Parser::Interface() {
		Expect(11);
		Expect(3);
		std::string interface, extends; interface = narrow(t->val); 
		if (la->kind == 12) {
			Get();
			Ident(extends);
		}
		TRY(factory().startInterface(interface, extends)) 
		Expect(21);
		while (la->kind == 3) {
			std::string type, name; 
			Ident(type);
			Expect(3);
			name = narrow(t->val); 
			if (isClassOrInterface(type)) {
				Reference(type, name);
			} else if (la->kind == 8) {
				Get();
				TRY(factory().addAttribute(type, name)) 
			} else SynErr(32);
		}
		Expect(22);
		TRY(factory().endInterface()) 
}

void Parser::Class() {
		Expect(10);
		Expect(3);
		std::string c, super; c = narrow(t->val); 
		if (la->kind == 12) {
			Get();
			Ident(super);
		}
		gum::Set<std::string> set; 
		if (la->kind == 16) {
			std::string i; 
			Get();
			Ident(i);
			set.insert(i); 
			while (la->kind == 6) {
				Get();
				Ident(i);
				set.insert(i); 
			}
		}
		TRY(factory().startClass(c, super, (set.empty()?0:&set))) 
		Expect(21);
		while (la->kind == 3) {
			std::string type, name; 
			Ident(type);
			Expect(3);
			name = narrow(t->val); 
			if (la->kind == 8 || la->kind == 15 || la->kind == 23) {
				RefOrParam(type, name);
			} else if (la->kind == 14 || la->kind == 21) {
				Attribute(type, name);
			} else if (la->kind == 26) {
				Aggregate(type, name);
			} else if (la->kind == 27) {
				Functions(type, name);
			} else SynErr(33);
		}
		Expect(22);
		TRY(factory().endClass()) 
}

void Parser::System() {
		Expect(13);
		Expect(3);
		TRY(factory().startSystem(narrow(t->val))) 
		Expect(21);
		while (la->kind == 3) {
			std::string l1, r1; 
			Ident(l1);
			if (la->kind == 23) {
				ArrayDecl(l1);
			} else if (la->kind == 3) {
				Get();
				TRY(factory().addInstance(l1, narrow(t->val))) 
			} else if (la->kind == 29) {
				Get();
				Ident(r1);
				if (static_cast<gum::prm::System*>(factory().getCurrent())->isArray(l1))
				{ TRY(factory().incArray(l1, r1)) }
				else
				{ TRY(factory().setReferenceSlot(l1, r1)) } 
			} else if (la->kind == 26) {
				Get();
				Ident(r1);
				TRY( try {factory().setReferenceSlot(l1, r1);}
				    catch (gum::Exception& e) {
				      size_t pos = l1.find_last_of('.');
				      if (pos != std::string::npos) {
				        std::string i = l1.substr(0, pos);
				        std::string p = l1.substr(pos+1, std::string::npos);
				        factory().setParameter(i, p, r1);
				      }
				    }) 
			} else SynErr(34);
			Expect(8);
		}
		Expect(22);
		TRY(factory().endSystem()) 
}

void Parser::Label(std::string& s) {
		if (la->kind == 3) {
			Get();
			s = narrow(t->val); 
		} else if (la->kind == 1 || la->kind == 2) {
			float val; 
			Number(val);
			std::stringstream sBuff; sBuff << val; s = sBuff.str(); 
		} else SynErr(35);
}

void Parser::Reference(std::string type, std::string name) {
		bool array = false; 
		if (la->kind == 23) {
			Get();
			Expect(24);
			array = true; 
		}
		Expect(8);
		TRY(factory().addReferenceSlot(type, name, array)) 
}

void Parser::RefOrParam(std::string type, std::string name) {
		if (isClassOrInterface(type)) {
			Reference(type, name);
		} else if (la->kind == 8 || la->kind == 15) {
			Parameter(type, name);
		} else SynErr(36);
}

void Parser::Attribute(std::string type, std::string name) {
		TRY(factory().startAttribute(type, name)) 
		if (la->kind == 14) {
			std::string p; 
			Get();
			CastIdent(p);
			TRY(factory().addParent(p)) 
			while (la->kind == 6) {
				Get();
				CastIdent(p);
				TRY(factory().addParent(p)) 
			}
		}
		Expect(21);
		if (la->kind == 23) {
			Get();
			std::vector<float> cpt; float f; 
			Number(f);
			cpt.push_back(f); 
			while (la->kind == 6) {
				Get();
				Number(f);
				cpt.push_back(f); 
			}
			Expect(24);
			TRY(factory().setRawCPFByColumns(cpt)) 
		} else if (la->kind == 3 || la->kind == 25) {
			CPTRule();
			while (la->kind == 3 || la->kind == 25) {
				CPTRule();
			}
		} else SynErr(37);
		Expect(22);
		Expect(8);
		TRY(factory().endAttribute()) 
}

void Parser::Aggregate(std::string type, std::string name) {
		std::string func, s; std::vector<std::string> chains, labels; 
		Expect(26);
		Expect(3);
		func = narrow(t->val); 
		Expect(17);
		AggChains(chains);
		Expect(6);
		AggLabels(labels);
		Expect(18);
		Expect(8);
		TRY(factory().addAggregator(name, func, chains, labels)) 
}

void Parser::Functions(std::string type, std::string name) {
		if (type != "boolean") {TRY(throw gum::OperationNotAllowed("noisy-or attributes must be booleans"))} 
		Expect(27);
		Expect(28);
		Expect(17);
		std::vector<std::string> chains, labels; std::vector<float> numbers; float leak = 0.0; 
		AggChains(chains);
		Expect(6);
		NumberList(numbers);
		if (la->kind == 6) {
			Get();
			Number(leak);
			if (la->kind == 6) {
				Get();
				AggLabels(labels);
			}
		}
		Expect(18);
		Expect(8);
		TRY(factory().addNoisyOrCompound(name, chains, numbers, leak, labels)) 
}

void Parser::Parameter(std::string type, std::string name) {
		std::string l; 
		if (la->kind == 15) {
			Get();
			Label(l);
		}
		Expect(8);
		TRY(factory().addParameter(type, name, l)) 
}

void Parser::CastIdent(std::string& s) {
		std::string cast, open(gum::prm::ClassElement::LEFT_CAST()), close(gum::prm::ClassElement::RIGHT_CAST()); std::stringstream sBuff; 
		if (la->kind == 17) {
			Get();
			Ident(cast);
			Expect(18);
			sBuff << open << cast << close; 
		}
		Expect(3);
		sBuff << narrow(t->val); 
		while (la->kind == 5) {
			cast = ""; 
			Get();
			sBuff << '.'; 
			if (la->kind == 17) {
				Get();
				Ident(cast);
				Expect(18);
				sBuff << open << cast << close; 
			}
			Expect(3);
			sBuff << narrow(t->val); 
		}
		s = sBuff.str(); 
}

void Parser::Number(float& val) {
		if (la->kind == 1) {
			Get();
			swscanf(t->val, L"%f", &val); 
		} else if (la->kind == 2) {
			Get();
			swscanf(t->val, L"%f", &val); 
		} else SynErr(38);
}

void Parser::CPTRule() {
		std::vector<std::string> labels; std::vector<float> values; 
		std::string s; float f; 
		CPTRuleValue(s);
		labels.push_back(s); 
		while (la->kind == 6) {
			Get();
			CPTRuleValue(s);
			labels.push_back(s); 
		}
		Expect(7);
		Number(f);
		values.push_back(f); 
		while (la->kind == 6) {
			Get();
			Number(f);
			values.push_back(f); 
		}
		Expect(8);
		TRY(factory().setCPFByRule(labels, values)) 
}

void Parser::CPTRuleValue(std::string& s ) {
		if (la->kind == 3) {
			Get();
			s = narrow(t->val); 
		} else if (la->kind == 25) {
			Get();
			s = "*"; 
		} else SynErr(39);
}

void Parser::AggChains(std::vector<std::string>& chains ) {
		if (la->kind == 3 || la->kind == 17) {
			std::string s; 
			CastIdent(s);
			chains.push_back(s); 
		} else if (la->kind == 23) {
			Get();
			std::string s; 
			CastIdent(s);
			chains.push_back(s); 
			while (la->kind == 6) {
				Get();
				CastIdent(s);
				chains.push_back(s); 
			}
			Expect(24);
		} else SynErr(40);
}

void Parser::AggLabels(std::vector<std::string>& labels ) {
		if (la->kind == 3) {
			Get();
			labels.push_back(narrow(t->val)); 
		} else if (la->kind == 23) {
			Get();
			Expect(3);
			labels.push_back(narrow(t->val)); 
			while (la->kind == 6) {
				Get();
				Expect(3);
				labels.push_back(narrow(t->val)); 
			}
			Expect(24);
		} else SynErr(41);
}

void Parser::NumberList(std::vector<float>& numbers ) {
		if (la->kind == 1 || la->kind == 2) {
			float f; 
			Number(f);
			numbers.push_back(f); 
		} else if (la->kind == 23) {
			Get();
			float f; 
			Number(f);
			numbers.push_back(f); 
			while (la->kind == 6) {
				Get();
				Number(f);
				numbers.push_back(f); 
			}
			Expect(24);
		} else SynErr(42);
}

void Parser::ArrayDecl(std::string l1) {
		std::string r1; std::stringstream sBuff; sBuff << l1; int size = 0; 
		Expect(23);
		if (la->kind == 1) {
			Get();
			sBuff << '[' << narrow(t->val) << ']'; swscanf(t->val, L"%d", &size); 
		}
		Expect(24);
		if (la->kind == 3) {
			Get();
			TRY(factory().addArray(l1, narrow(t->val), size)) 
		} else if (la->kind == 5) {
			Get();
			Expect(3);
			std::string l2 = narrow(t->val); 
			if (la->kind == 29) {
				Get();
				Ident(r1);
				TRY(factory().setReferenceSlot(sBuff.str(), l2, r1)) 
			} else if (la->kind == 26) {
				Get();
				Ident(r1);
				TRY(try { factory().setReferenceSlot(sBuff.str(), l2, r1); }
				catch (gum::Exception& e) {factory().setParameter(sBuff.str(), l2, r1);}) 
			} else SynErr(43);
		} else SynErr(44);
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

void Errors::SynErr(std::string filename, int line, int n) {
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
			case 14: s = coco_string_create(L"dependson expected"); break;
			case 15: s = coco_string_create(L"default expected"); break;
			case 16: s = coco_string_create(L"implements expected"); break;
			case 17: s = coco_string_create(L"LEFT_CAST expected"); break;
			case 18: s = coco_string_create(L"RIGHT_CAST expected"); break;
			case 19: s = coco_string_create(L"\"package\" expected"); break;
			case 20: s = coco_string_create(L"\"import\" expected"); break;
			case 21: s = coco_string_create(L"\"{\" expected"); break;
			case 22: s = coco_string_create(L"\"}\" expected"); break;
			case 23: s = coco_string_create(L"\"[\" expected"); break;
			case 24: s = coco_string_create(L"\"]\" expected"); break;
			case 25: s = coco_string_create(L"\"*\" expected"); break;
			case 26: s = coco_string_create(L"\"=\" expected"); break;
			case 27: s = coco_string_create(L"\"~\" expected"); break;
			case 28: s = coco_string_create(L"\"noisyOr\" expected"); break;
			case 29: s = coco_string_create(L"\"+=\" expected"); break;
			case 30: s = coco_string_create(L"??? expected"); break;
			case 31: s = coco_string_create(L"invalid Unit"); break;
			case 32: s = coco_string_create(L"invalid Interface"); break;
			case 33: s = coco_string_create(L"invalid Class"); break;
			case 34: s = coco_string_create(L"invalid System"); break;
			case 35: s = coco_string_create(L"invalid Label"); break;
			case 36: s = coco_string_create(L"invalid RefOrParam"); break;
			case 37: s = coco_string_create(L"invalid Attribute"); break;
			case 38: s = coco_string_create(L"invalid Number"); break;
			case 39: s = coco_string_create(L"invalid CPTRuleValue"); break;
			case 40: s = coco_string_create(L"invalid AggChains"); break;
			case 41: s = coco_string_create(L"invalid AggLabels"); break;
			case 42: s = coco_string_create(L"invalid NumberList"); break;
			case 43: s = coco_string_create(L"invalid ArrayDecl"); break;
			case 44: s = coco_string_create(L"invalid ArrayDecl"); break;

    default:
    {
      wchar_t format[20];
      coco_swprintf(format, 20, L"error %d", n);
      s = coco_string_create(format);
    }
    break;
  }
  wprintf(L"%s|%d| syntax error: %ls\n", filename.c_str(), line, s);
  coco_string_delete(s);
  count++;
}

void Errors::Error(std::string filename, int line, const wchar_t *s) {
  wprintf(L"%s|%d| error: %ls\n", filename.c_str(), line, s);
  count++;
}

} // namespace
} // namespace
} // namespace


