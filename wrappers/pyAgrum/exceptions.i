/* EXCEPTION HANDLING */
%{
static void SetPythonizeAgrumException() {
  try {
    throw;
  } catch ( std::bad_cast& e ) {
    PyErr_SetString ( PyExc_RuntimeError, "C++ Bad Cast" );
  } catch ( gum::NotFound& e ) {
    PyErr_SetString ( PyExc_IndexError, e.errorContent().c_str() );
  } catch ( gum::DefaultInLabel& e ) {
    gum::DefaultInLabel* ecopy = new gum::DefaultInLabel ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__DefaultInLabel,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__DefaultInLabel ), err );
  } catch ( gum::IdError& e ) {
    gum::IdError* ecopy = new gum::IdError ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__IdError,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__IdError ), err );
  } catch ( gum::IOError& e ) {
    gum::IOError* ecopy = new gum::IOError ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__IOError,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__IOError ),
      err );
  } catch ( gum::OperationNotAllowed& e ) {
    gum::OperationNotAllowed* ecopy = new gum::OperationNotAllowed ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__OperationNotAllowed,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__OperationNotAllowed ),
      err );
  } catch ( gum::InvalidDirectedCycle& e ) {
    gum::InvalidDirectedCycle* ecopy = new gum::InvalidDirectedCycle ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__InvalidDirectedCycle,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__InvalidDirectedCycle ), err );
  } catch ( gum::InvalidNode& e ) {
    gum::InvalidNode* ecopy = new gum::InvalidNode ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__InvalidNode,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__InvalidNode ), err );
  } catch ( gum::InvalidArc& e ) {
    gum::InvalidArc* ecopy = new gum::InvalidArc ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__InvalidArc,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__InvalidArc ), err );
  } catch ( gum::InvalidEdge& e ) {
    gum::InvalidEdge* ecopy = new gum::InvalidEdge ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__InvalidEdge,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__InvalidEdge ), err );
  } catch ( gum::DuplicateLabel& e ) {
    gum::DuplicateLabel* ecopy = new gum::DuplicateLabel ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__DuplicateLabel,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__DuplicateLabel ), err );
  } catch ( gum::DuplicateElement& e ) {
    gum::DuplicateElement* ecopy = new gum::DuplicateElement ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__DuplicateElement,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__DuplicateElement ), err );
  } catch ( gum::OutOfLowerBound& e ) {
    gum::OutOfLowerBound* ecopy = new gum::OutOfLowerBound ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__OutOfLowerBound,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__OutOfLowerBound ), err );
  } catch ( gum::OutOfUpperBound& e ) {
    gum::OutOfUpperBound* ecopy = new gum::OutOfUpperBound ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__OutOfUpperBound,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__OutOfUpperBound ), err );
  } catch ( gum::OutOfBounds& e ) {
    gum::OutOfBounds* ecopy = new gum::OutOfBounds ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__OutOfBounds,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__OutOfBounds ), err );
  } catch ( gum::UnknownLabelInDatabase& e ) {
    gum::UnknownLabelInDatabase* ecopy = new gum::UnknownLabelInDatabase ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__UnknownLabelInDatabase,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__UnknownLabelInDatabase ), err );
  } catch ( gum::FatalError& e ) {
    gum::FatalError* ecopy = new gum::FatalError ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__FatalError,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__FatalError ), err );
  } catch ( gum::UndefinedElement& e ) {
    gum::UndefinedElement* ecopy = new gum::UndefinedElement ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__UndefinedElement,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__UndefinedElement ), err );
  } catch ( gum::InvalidArgument& e ) {
    gum::InvalidArgument* ecopy = new gum::InvalidArgument ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__InvalidArgument,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__InvalidArgument ), err );
  } catch ( gum::Exception& e ) {
    PyErr_SetString ( PyExc_Exception, e.errorContent().c_str() );
  } catch ( std::exception& e ) {
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

