class PythonLoadListener : public gum::Listener {
private:
	PyObject *__whenLoading;
public:
	void whenLoading(const void *buffer,int percent) {
		if (__whenLoading) {
			PyObject* arglist = Py_BuildValue("(i)",percent);
			PyEval_CallObject(__whenLoading,arglist);
			Py_DECREF(arglist);
		}
	}

	bool setPythonListener(PyObject* l) {
		if (! PyCallable_Check(l)) {
			//GUM_ERROR(gum::OperationNotAllowed,"Need a callable object !");
			return false;
		} else {
			__whenLoading=l;
			Py_INCREF(l);
			return true;
		}
	}

	PythonLoadListener() {
		__whenLoading=(PyObject*)0;
	}
	~PythonLoadListener() {
		if (__whenLoading) Py_DECREF(__whenLoading);
	}
};
