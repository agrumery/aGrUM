/* EXCEPTION HANDLING */
%{
#define PYGUM_CATCH(GUMEXCEPTION)                                               \
  catch (gum::GUMEXCEPTION & e) {                                               \
     PyErr_SetString (SWIG_Python_ExceptionType(SWIGTYPE_p_gum__##GUMEXCEPTION),e.what().c_str()); \
  }

static void SetPythonizeAgrumException() {
  try {
    throw;
  }
  catch ( std::bad_cast& e ) {
    PyErr_SetString ( PyExc_RuntimeError, "C++ Bad Cast" );
  }
  PYGUM_CATCH(NotFound)
  PYGUM_CATCH(DefaultInLabel)
  PYGUM_CATCH(IdError)
  PYGUM_CATCH(IOError)
  PYGUM_CATCH(OperationNotAllowed)
  PYGUM_CATCH(InvalidDirectedCycle)
  PYGUM_CATCH(InvalidNode)
  PYGUM_CATCH(InvalidArc)
  PYGUM_CATCH(InvalidEdge)
  PYGUM_CATCH(DuplicateLabel)
  PYGUM_CATCH(DuplicateElement)
  PYGUM_CATCH(OutOfLowerBound)
  PYGUM_CATCH(OutOfUpperBound)
  PYGUM_CATCH(OutOfBounds)
  PYGUM_CATCH(UnknownLabelInDatabase)
  PYGUM_CATCH(FatalError)
  PYGUM_CATCH(UndefinedElement)
  PYGUM_CATCH(InvalidArgument)
  PYGUM_CATCH(MissingValueInDatabase)
  catch ( gum::Exception& e ) {
    PyErr_SetString ( PyExc_Exception, e.what().c_str() );
  }
  catch ( std::exception& e ) {
    PyErr_SetString ( PyExc_Exception, e.what() );
  }
}
%}

%exception {
  try {
    $action
  } catch (...) {
    SetPythonizeAgrumException();
    SWIG_fail;
  }
}

