

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"



void Parser::SynErr(int n) {
  if (errDist >= minErrDist) errors->SynErr(scanner->filename, la->line, n);
  errDist = 0;
}

void Parser::warning(const std::string& s) {
  errors->Warning(scanner->filename, t->line, gum::widen(s).c_str());
}

void Parser::error(const std::string& s) {
  if (errDist >= minErrDist) errors->Error(scanner->filename, t->line, gum::widen(s).c_str());
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
        gum::coco_string_delete(dummyToken->val);
        dummyToken->val = gum::coco_string_create(t->val);
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

void Parser::prm_run() {
		this->inf = 0; 
		if (la->kind == 5) {
			Get();
			Expect(3);
			this->package = gum::narrow(t->val); 
			Expect(12);
		}
		Expect(6);
		Expect(3);
		this->import(gum::narrow(t->val)); 
		Expect(12);
		RequestBloc();
}

void Parser::RequestBloc() {
		Expect(7);
		Expect(3);
		Expect(13);
		while (StartOf(1)) {
			if (la->kind == 3) {
				Observe();
			} else if (la->kind == 9) {
				Unobserve();
			} else if (la->kind == 8) {
				Query();
			} else if (la->kind == 10) {
				SetEngine();
			} else {
				SetGrdEngine();
			}
		}
		Expect(14);
		if (inf) { delete inf; } 
}

void Parser::Observe() {
		std::string left_value, right_value; 
		Expect(3);
		left_value = gum::narrow(t->val); 
		Expect(15);
		Expect(3);
		right_value = gum::narrow(t->val); 
		Expect(12);
		this->addObservation(left_value, right_value); 
}

void Parser::Unobserve() {
		Expect(9);
		Expect(3);
		this->removeObervation(gum::narrow(t->val)); 
		Expect(12);
}

void Parser::Query() {
		Expect(8);
		Expect(3);
		this->query(gum::narrow(t->val)); 
		Expect(12);
}

void Parser::SetEngine() {
		Expect(10);
		Expect(3);
		this->engine = gum::narrow(t->val); 
		Expect(12);
}

void Parser::SetGrdEngine() {
		Expect(11);
		Expect(3);
		this->bn_engine = gum::narrow(t->val); 
		Expect(12);
}



void Parser::Parse() {
  t = NULL;
  la = dummyToken = new Token();
  la->val = gum::coco_string_create(L"Dummy Token");
  Get();

  	prm_run();


  Expect(0);
}

Parser::Parser(Scanner *scanner, std::ostream& out):
  log(out), __syntax_flag(false)
{
  	maxT = 16;


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

  	static bool set[2][18] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,T, x,x,x,x, T,T,T,T, x,x,x,x, x,x}
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

    case 0: s = gum::coco_string_create(L"EOF expected"); break;
    case 1: s = gum::coco_string_create(L"integer expected"); break;
    case 2: s = gum::coco_string_create(L"float expected"); break;
    case 3: s = gum::coco_string_create(L"word expected"); break;
    case 4: s = gum::coco_string_create(L"eol expected"); break;
    case 5: s = gum::coco_string_create(L"package expected"); break;
    case 6: s = gum::coco_string_create(L"import expected"); break;
    case 7: s = gum::coco_string_create(L"request expected"); break;
    case 8: s = gum::coco_string_create(L"query expected"); break;
    case 9: s = gum::coco_string_create(L"unobserve expected"); break;
    case 10: s = gum::coco_string_create(L"engine expected"); break;
    case 11: s = gum::coco_string_create(L"grd_engine expected"); break;
    case 12: s = gum::coco_string_create(L"\";\" expected"); break;
    case 13: s = gum::coco_string_create(L"\"{\" expected"); break;
    case 14: s = gum::coco_string_create(L"\"}\" expected"); break;
    case 15: s = gum::coco_string_create(L"\"=\" expected"); break;
    case 16: s = gum::coco_string_create(L"??? expected"); break;


    default:
    {
      wchar_t format[20];
      coco_swprintf(format, 20, L"error %d", n);
      s = gum::coco_string_create(format);
    }
    break;
  }
  wprintf(L"%s|%d| syntax error: %ls\n", filename.c_str(), line, s);
  gum::coco_string_delete(s);
  count++;
}

void Errors::Warning(std::string filename, int line, const wchar_t *s) {
  wprintf(L"%s|%d| warning: %ls\n", filename.c_str(), line, s);
}

void Errors::Error(std::string filename, int line, const wchar_t *s) {
  wprintf(L"%s|%d| error: %ls\n", filename.c_str(), line, s);
  count++;
}

void Parser::import(std::string import_name) {
  if (verbose) {
    log << "Loading system " << import_name << "... " << std::flush;
  }
  std::replace(import_name.begin(), import_name.end(), '.', '/');
  std::string ext = ".skool";
  reader.readFile(import_name + ext);
  if (verbose) {
    log << "Finished." << std::endl;
  }
}

std::string Parser::findSystemName(const std::string& s) {
  std::string name;
  size_t dot = s.find_first_of('.');
  while (dot != std::string::npos) {
    name = s.substr(0, dot);
    if (reader.prm()->isSystem(name)) return name;
    dot = s.find('.', dot + 1);
  }
  std::string msg = "could not find any system in ";
  error(msg + s);
  throw msg + s;
}

std::string Parser::findInstanceName(const std::string& s, const std::string& sys) {
  std::string name = s.substr(sys.length() + 1);
  size_t dot = name.find('.');
  name = name.substr(0, dot);
  if (reader.prm()->getSystem(sys).exists(name)) return name;
  std::string msg = "could not find any instance in ";
  error(msg + name);
  throw msg + name;
}

std::string Parser::findAttributeName(const std::string& s, const gum::prm::Instance& instance) {
  size_t dot = s.find_last_of('.');
  if (dot != std::string::npos) {
    std::string name = s.substr(dot + 1, std::string::npos);
    if (instance.exists(name)) return name;
  }
  std::string msg = "could not find any attribute in ";
  error(msg + s);
  throw msg + s;
}

void Parser::addObservation(const std::string& left_val, const std::string& right_val) {
  const gum::prm::System& sys = reader.prm()->getSystem (findSystemName(left_val));
  const gum::prm::Instance& instance = sys.get(findInstanceName(left_val, sys.name()));
  const gum::prm::Attribute& attr = instance.get(findAttributeName(left_val, instance));
  gum::prm::PRMInference::Chain chain = std::make_pair(&instance, &attr);
  if (not inf) {
    generateInfEngine(sys);
  }
  if (inf->hasEvidence(chain)) {
    std::string msg = " is already observed";
    warning(left_val + msg);
  }
  gum::Potential<gum::prm::prm_float> e;
  e.add(chain.second->type().variable());
  gum::Instantiation i(e);
  bool found = false;
  for (i.setFirst(); not i.end(); i.inc()) {
    if (chain.second->type().variable().label(i.val(chain.second->type().variable())) == right_val) {
      e.set(i, (gum::prm::prm_float) 1.0);
      found = true;
    } else {
      e.set(i, (gum::prm::prm_float) 0.0);
    }
  }
  if (not found) {
    std::string msg = " is not a label of ";
    error(right_val + msg + left_val);
    throw right_val + msg + left_val;
  }
  try {
    inf->addEvidence(chain, e);
    //affichage en demande
    if (verbose) {
      log << "Added evidence " << right_val << " over attribute " << left_val << std::endl;
    }
  } catch (gum::OperationNotAllowed & ex) {
    std::stringstream sBuff;
    sBuff << " someting went wrong when adding evidence " << right_val;
    sBuff << " over " << left_val;
    error(sBuff.str());
    throw sBuff.str();
  }
}

void Parser::removeObervation(const std::string name) {
  const gum::prm::System& sys = reader.prm()->getSystem (findSystemName(name));
  const gum::prm::Instance& instance = sys.get(findInstanceName(name, sys.name()));
  const gum::prm::Attribute& attr = instance.get(findAttributeName(name, instance));
  gum::prm::PRMInference::Chain chain = std::make_pair(&instance, &attr);
  if (inf->hasEvidence(chain)) {
    inf->removeEvidence(chain);
    if (verbose) {
      log << "Removed evidence over attribute " << name << std::endl;
    }
  } else {
    std::string msg = " was not observed";
    warning(name + msg);
  }
}

void Parser::generateInfEngine(const gum::prm::System& sys) {
  if (inf) {
    warning("an inference engine has already been defined");
    delete inf;
  }
  if (engine == "SVED") {
    inf = new gum::prm::SVED(*(reader.prm()), sys);
  } else if (engine == "GRD") {
    typedef gum::prm::prm_float p_f;
    gum::BayesNetInference<p_f>* bn_inf = 0;
    gum::BayesNet<p_f>* bn = new gum::BayesNet<p_f>();
    gum::BayesNetFactory<p_f> bn_factory(bn);
    if (verbose) {
      log << "Grounding the network... " << std::flush;
    }
    sys.groundedBN(bn_factory);
    if (verbose) {
      log << "Finished." << std::endl;
    }
    if (bn_engine == "VE") {
      bn_inf = new gum::VariableElimination<p_f>(*bn);
    } else if (bn_engine == "VEBB") {
      bn_inf = new gum::VEWithBB<p_f>(*bn);
    } else if (bn_engine == "lazy") {
      bn_inf = new gum::LazyPropagation<p_f>(*bn);
    } else {
      bn_inf = new gum::ShaferShenoyInference<p_f>(*bn);
    }
    gum::prm::GroundedInference* grd_inf = new gum::prm::GroundedInference(*(reader.prm()), sys);
    grd_inf->setBNInference(bn_inf);
    inf = grd_inf;
  } else {
    inf = new gum::prm::SVE(*(reader.prm()), sys);
  }
}

void Parser::query(const std::string& name) {
  if (not isInSyntaxMode()) {
    const gum::prm::System& sys = reader.prm()->getSystem (findSystemName(name));
    const gum::prm::Instance& instance = sys.get(findInstanceName(name, sys.name()));
    const gum::prm::Attribute& attr = instance.get(findAttributeName(name, instance));
    gum::prm::PRMInference::Chain chain = std::make_pair(&instance, &attr);
    gum::Potential<gum::prm::prm_float> m;
    for (size_t i = 0; i < 80; ++i) {
      log << "#";
    }
    log << std::endl;
    if (not inf) {
      log << "Building the inference engine... " << std::flush;
      generateInfEngine(sys);
      log << "Finished." << std::endl;
    }
    try {
      gum::Timer timer;
      timer.reset();
      log << "Starting inference over query: " << name << "... " << std::flush;
      inf->marginal(chain, m);
      log << "Finished." << std::endl;
      double t = timer.step();
      log << "Time in seconds (accuracy ~0.001): " << t << std::endl;
    } catch (gum::Exception& e) {
      std::string msg = "something went wrong while infering: ";
      error(msg + e.getContent());
      delete inf;
      throw msg + e.getContent();
    }
    gum::Instantiation j (m);
    for (j.setFirst (); not j.end (); j.inc ()) {
      log << attr.type().variable().label(j.val(attr.type().variable())) << ": " << m.get(j) << std::endl;
    }
    for (size_t i = 0; i < 80; ++i) {
      log << "#";
    }
    log << std::endl << std::endl;
    delete inf;
  }
}

bool Parser::isInSyntaxMode() const {
  return __syntax_flag;
}

void Parser::setSyntaxMode(bool f) {
  __syntax_flag = f;
}



