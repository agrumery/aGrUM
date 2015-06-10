/* EXCEPTION HANDLING */

%exception {
  try {
    $action
  } catch ( std::bad_cast& e ) {
    PyErr_SetString ( PyExc_RuntimeError, "C++ Bad Cast" );
    SWIG_fail;
  } catch ( gum::NotFound& e ) {
    PyErr_SetString ( PyExc_IndexError, e.errorContent().c_str() );
    SWIG_fail;
  } catch ( gum::DefaultInLabel& e ) {
    gum::DefaultInLabel* ecopy = new gum::DefaultInLabel ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__DefaultInLabel,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__DefaultInLabel ), err );
    SWIG_fail;
  } catch ( gum::IdError& e ) {
    gum::IdError* ecopy = new gum::IdError ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__IdError,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__IdError ), err );
    SWIG_fail;
  } catch ( gum::IOError& e ) {
    gum::IOError* ecopy = new gum::IOError ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__IOError,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__IOError ),
      err );
    SWIG_fail;
  } catch ( gum::OperationNotAllowed& e ) {
    gum::OperationNotAllowed* ecopy = new gum::OperationNotAllowed ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__OperationNotAllowed,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__OperationNotAllowed ),
      err );
    SWIG_fail;
  } catch ( gum::InvalidDirectedCycle& e ) {
    gum::InvalidDirectedCycle* ecopy = new gum::InvalidDirectedCycle ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__InvalidDirectedCycle,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__InvalidDirectedCycle ), err );
    SWIG_fail;
  } catch ( gum::InvalidNode& e ) {
    gum::InvalidNode* ecopy = new gum::InvalidNode ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__InvalidNode,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__InvalidNode ), err );
    SWIG_fail;
  } catch ( gum::InvalidEdge& e ) {
    gum::InvalidEdge* ecopy = new gum::InvalidEdge ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__InvalidEdge,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__InvalidEdge ), err );
    SWIG_fail;
  } catch ( gum::DuplicateElement& e ) {
    gum::DuplicateElement* ecopy = new gum::DuplicateElement ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__DuplicateElement,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__DuplicateElement ), err );
    SWIG_fail;
  } catch ( gum::OutOfLowerBound& e ) {
    gum::OutOfLowerBound* ecopy = new gum::OutOfLowerBound ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__OutOfLowerBound,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__OutOfLowerBound ), err );
    SWIG_fail;
  } catch ( gum::OutOfUpperBound& e ) {
    gum::OutOfUpperBound* ecopy = new gum::OutOfUpperBound ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__OutOfUpperBound,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__OutOfUpperBound ), err );
    SWIG_fail;
  } catch ( gum::OutOfBounds& e ) {
    gum::OutOfBounds* ecopy = new gum::OutOfBounds ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__OutOfBounds,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__OutOfBounds ), err );
    SWIG_fail;
  } catch ( gum::UnknownLabelInDatabase& e ) {
    gum::UnknownLabelInDatabase* ecopy = new gum::UnknownLabelInDatabase ( e );
    PyObject* err = SWIG_NewPointerObj ( ecopy,
    SWIGTYPE_p_gum__UnknownLabelInDatabase,
    1 );
    PyErr_SetObject (
      SWIG_Python_ExceptionType ( SWIGTYPE_p_gum__UnknownLabelInDatabase ), err );
    SWIG_fail;
  } catch ( gum::Exception& e ) {
    PyErr_SetString ( PyExc_Exception, e.errorContent().c_str() );
    SWIG_fail;
  } catch ( std::exception& e ) {
    PyErr_SetString ( PyExc_Exception, e.what() );
    SWIG_fail;
  }
}


/* CLASS EXTENSIONS */
%extend gum::Exception {
  const char * __str__() {
    return self->errorContent().c_str();
  }
}
