#ifndef __cxxtest__AgrumErrorFormatter_h__
#define __cxxtest__AgrumErrorFormatter_h__

//
// The AgrumErrorFormatter is a TestListener that
// prints reports of the errors to an output
// stream.  Since we cannot rely ou the standard
// iostreams, this header defines a base class
// analog to std::ostream.
//

#include <cstdlib>

#include <agrum/config.h>

#include <agrum/base/core/timer.h>

#include <cxxtest/TestListener.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/ValueTraits.h>

namespace CxxTest {

  class OutputStream {
    public:
    virtual ~OutputStream() {}

    virtual void flush() {};

    virtual OutputStream& operator<<(unsigned /*number*/) { return *this; }

    virtual OutputStream& operator<<(const char* /*string*/) { return *this; }

    typedef void (*Manipulator)(OutputStream&);

    virtual OutputStream& operator<<(Manipulator m) {
      m(*this);
      return *this;
    }

    static void endl(OutputStream& o) { (o << "\n").flush(); }
  };

  class AgrumErrorFormatter: public TestListener {
    private:
    gum::Timer* __clock;
    double      __totalTime;
    int         __nbrTest;

    public:
    AgrumErrorFormatter(OutputStream* o, const char* preLine = ":", const char* postLine = "") :
        _dotting(true), _reported(false), _o(o), _preLine(preLine), _postLine(postLine) {}

    int run() {
      TestRunner::runAllTests(*this);
      return tracker().failedTests();
    }

    void enterWorld(const WorldDescription& /*desc*/) {
      __clock = new gum::Timer();
      gum::initRandom(GUM_RANDOMSEED);
      __totalTime = 0.0;
      (*_o) << endl
            << "======================" << endl
            << "Agrum Test Unit Module" << endl
            << "======================" << endl
            << endl
            << "Running " << totalTests << endl;
      _o->flush();
      _dotting  = true;
      _reported = false;
    }

    static void totalTests(OutputStream& o) {
      char                    s[WorldDescription::MAX_STRLEN_TOTAL_TESTS];
      const WorldDescription& wd = tracker().world();
      o << wd.strTotalTests(s) << (wd.numTotalTests() == 1 ? " test" : " tests");
    }

    void enterSuite(const SuiteDescription& t) {
      __nbrTest = 0;
      ((*_o) << "\n" << t.file() << " : ").flush();
      _reported = false;
      __clock->reset();
    }

    void leaveSuite(const SuiteDescription&) {
      double step = __clock->step();
      __totalTime += step;
      ((*_o) << " [" << (unsigned int)(1000.0 * step) << " ms]").flush();
    }

    void enterTest(const TestDescription&) { _reported = false; }

    void leaveTest(const TestDescription&) {
      __nbrTest++;

      if (!tracker().testFailed()) {
        if (__nbrTest % 5) ((*_o) << ".").flush();
        else ((*_o) << "#").flush();

        _dotting = true;
      }
    }

    void leaveWorld(const WorldDescription& desc) {
      delete (__clock);
      newLine();
#ifdef GUM_DEBUG_MODE
      gum::__debug__::_atexit_();
#endif   // GUM_DEBUG_MODE

      (*_o) << endl
            << "## Profiling : " << (unsigned int)(1000.0 * __totalTime) << " ms ##" << endl;

      (*_o) << "Failed " << tracker().failedTests() << " of " << totalTests << endl;

      if (!tracker().failedTests()) {
        (*_o) << endl
              << "==================================" << endl
              << "Agrum Test Unit Module: 100% OK !" << endl
              << "==================================" << endl
              << endl;
        return;
      }

      newLine();

      unsigned numPassed = desc.numTotalTests() - tracker().failedTests();
      (*_o) << "Success rate: " << (numPassed * 100 / desc.numTotalTests()) << "%" << endl;
    }

    void trace(const char* file, unsigned line, const char* expression) {
      stop(file, line) << "Trace: " << expression << endl;
    }

    void warning(const char* file, unsigned line, const char* expression) {
      stop(file, line) << "Warning: " << expression << endl;
    }

    void failedTest(const char* file, unsigned line, const char* expression) {
      stop(file, line) << "Error: Test failed " << expression << endl;
    }

    void failedAssert(const char* file, unsigned line, const char* expression) {
      stop(file, line) << "Error: Assertion failed: " << expression << endl;
    }

    void failedAssertEquals(const char* file,
                            unsigned    line,
                            const char* xStr,
                            const char* yStr,
                            const char* x,
                            const char* y) {
      stop(file, line) << "Error: Expected (" << xStr << " == " << yStr << "), found (" << x
                       << " != " << y << ")" << endl;
    }

    void failedAssertSameData(const char* file,
                              unsigned    line,
                              const char* xStr,
                              const char* yStr,
                              const char* sizeStr,
                              const void* x,
                              const void* y,
                              unsigned    size) {
      stop(file, line) << "Error: Expected " << sizeStr << " (" << size
                       << ") bytes to be equal at (" << xStr << ") and (" << yStr
                       << "), found:" << endl;
      dump(x, size);
      (*_o) << "     differs from" << endl;
      dump(y, size);
    }

    void failedAssertDelta(const char* file,
                           unsigned    line,
                           const char* xStr,
                           const char* yStr,
                           const char* dStr,
                           const char* x,
                           const char* y,
                           const char* d) {
      stop(file, line) << "Error: Expected (" << xStr << " == " << yStr << ") up to " << dStr
                       << " (" << d << "), found (" << x << " != " << y << ")" << endl;
    }

    void failedAssertDiffers(const char* file,
                             unsigned    line,
                             const char* xStr,
                             const char* yStr,
                             const char* value) {
      stop(file, line) << "Error: Expected (" << xStr << " != " << yStr << "), found (" << value
                       << ")" << endl;
    }

    void failedAssertLessThan(const char* file,
                              unsigned    line,
                              const char* xStr,
                              const char* yStr,
                              const char* x,
                              const char* y) {
      stop(file, line) << "Error: Expected (" << xStr << " < " << yStr << "), found (" << x
                       << " >= " << y << ")" << endl;
    }

    void failedAssertLessThanEquals(const char* file,
                                    unsigned    line,
                                    const char* xStr,
                                    const char* yStr,
                                    const char* x,
                                    const char* y) {
      stop(file, line) << "Error: Expected (" << xStr << " <= " << yStr << "), found (" << x
                       << " > " << y << ")" << endl;
    }

    void failedAssertRelation(const char* file,
                              unsigned    line,
                              const char* relation,
                              const char* xStr,
                              const char* yStr,
                              const char* x,
                              const char* y) {
      stop(file, line) << "Error: Expected " << relation << "( " << xStr << ", " << yStr
                       << " ), found !" << relation << "( " << x << ", " << y << " )" << endl;
    }

    void failedAssertPredicate(const char* file,
                               unsigned    line,
                               const char* predicate,
                               const char* xStr,
                               const char* x) {
      stop(file, line) << "Error: Expected " << predicate << "( " << xStr << " ), found !"
                       << predicate << "( " << x << " )" << endl;
    }

    void failedAssertThrows(const char* file,
                            unsigned    line,
                            const char* expression,
                            const char* type,
                            bool        otherThrown) {
      stop(file, line) << "Error: Expected (" << expression << ") to throw (" << type << ") but it "
                       << (otherThrown ? "threw something else" : "didn't throw") << endl;
    }

    void failedAssertThrowsNot(const char* file, unsigned line, const char* expression) {
      stop(file, line) << "Error: Expected (" << expression << ") not to throw, but it did" << endl;
    }

    protected:
    OutputStream* outputStream() const { return _o; }

    private:
    AgrumErrorFormatter(const AgrumErrorFormatter&);
    AgrumErrorFormatter& operator=(const AgrumErrorFormatter&);

    OutputStream& stop(const char* file, unsigned line) {
      newLine();
      reportTest();
      return (*_o) << file << _preLine << line << _postLine << ": ";
    }

    OutputStream& specialStop(const char* file, unsigned line) {
      newLine();
      reportTest();
      return (*_o) << file << "<" << line << "> ";
    }

    void newLine(void) {
      if (_dotting) {
        (*_o) << endl;
        _dotting = false;
      }
    }

    void reportTest(void) {
      if (_reported) return;

      (*_o) << "In " << tracker().suite().suiteName() << "::" << tracker().test().testName() << ":"
            << endl;

      _reported = true;
    }

    void dump(const void* buffer, unsigned size) {
      if (!buffer) dumpNull();
      else dumpBuffer(buffer, size);
    }

    void dumpNull() { (*_o) << "   (null)" << endl; }

    void dumpBuffer(const void* buffer, unsigned size) {
      unsigned dumpSize = size;

      if (maxDumpSize() && dumpSize > maxDumpSize()) dumpSize = maxDumpSize();

      const unsigned char* p = (const unsigned char*)buffer;

      (*_o) << "   { ";

      for (unsigned i = 0; i < dumpSize; ++i)
        (*_o) << byteToHex(*p++) << " ";

      if (dumpSize < size) (*_o) << "... ";

      (*_o) << "}" << endl;
    }

    static void endl(OutputStream& o) { OutputStream::endl(o); }

    bool          _dotting;
    bool          _reported;
    OutputStream* _o;
    const char*   _preLine;
    const char*   _postLine;
  };
}   // namespace CxxTest

#endif   //  __cxxtest__AgrumErrorFormatter_h__
