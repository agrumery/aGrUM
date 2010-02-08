#include <agrum/BN/BayesNet.h>
#include <agrum/graphs/diGraphListener.h>


class PythonBNListener : public gum::DiGraphListener {
	private:
		PyObject *__pyWhenNodeAdded;
		PyObject *__pyWhenNodeDeleted;
		PyObject *__pyWhenArcAdded;
		PyObject *__pyWhenArcDeleted;

		const gum::VariableNodeMap* __map;

		void __checkCallable(PyObject *pyfunc) {
			if (!PyCallable_Check(pyfunc)) {
				PyErr_SetString(PyExc_TypeError, "Need a callable object!");
			}
		}

	public:
		PythonBNListener(gum::DiGraph* g,const gum::VariableNodeMap* vnm):
            gum::DiGraphListener(g), __map(vnm)
        {
			__pyWhenArcAdded = __pyWhenArcDeleted = (PyObject *)0;
            __pyWhenNodeAdded = __pyWhenNodeDeleted = (PyObject *)0;
		}

		~PythonBNListener() {
			if (__pyWhenArcAdded) Py_DECREF(__pyWhenArcAdded);
			if (__pyWhenArcDeleted) Py_DECREF(__pyWhenArcDeleted);
			if (__pyWhenNodeAdded) Py_DECREF(__pyWhenNodeAdded);
			if (__pyWhenNodeDeleted) Py_DECREF(__pyWhenNodeDeleted);
		}

		virtual void whenNodeAdded( const void * source,gum::NodeId id) {
			// we could check if source==_digraph !!!
			if (__pyWhenNodeAdded) {
				PyObject *arglist = Py_BuildValue("(ls)", id,
                                                  (*__map)[id].name().c_str());
				PyEval_CallObject(__pyWhenNodeAdded,arglist);
				Py_DECREF(arglist);
			}
		}

    virtual void whenNodeDeleted( const void *,gum::NodeId id) {
			// we could check if source==_digraph !!!
			if (__pyWhenNodeDeleted) {
				PyObject *arglist = Py_BuildValue("(l)",id);
				PyEval_CallObject(__pyWhenNodeDeleted,arglist);
				Py_DECREF(arglist);
			}
		}

    virtual void whenArcAdded( const void *,gum::NodeId src,gum::NodeId dst) {
			// we could check if source==_digraph !!!
			if (__pyWhenArcAdded) {
				PyObject *arglist = Py_BuildValue("(ll)",src,dst);
				PyEval_CallObject(__pyWhenArcAdded,arglist);
				Py_DECREF(arglist);
			}
		}

    virtual void whenArcDeleted( const void *,gum::NodeId src,gum::NodeId dst) {
			// we could check if source==_digraph !!!
			if (__pyWhenArcDeleted) {
				PyObject *arglist = Py_BuildValue("(ll)",src,dst);
				PyEval_CallObject(__pyWhenArcDeleted,arglist);
				Py_DECREF(arglist);
			}
		}

		void setWhenArcAdded(PyObject *pyfunc) {
			__checkCallable(pyfunc);
			__pyWhenArcAdded=pyfunc;
			Py_INCREF(pyfunc);
		}

		void setWhenArcDeleted(PyObject *pyfunc) {
			__checkCallable(pyfunc);
			__pyWhenArcDeleted=pyfunc;
			Py_INCREF(pyfunc);
		}

		void setWhenNodeAdded(PyObject *pyfunc) {
			__checkCallable(pyfunc);
			__pyWhenNodeAdded=pyfunc;
			Py_INCREF(pyfunc);
		}

		void setWhenNodeDeleted(PyObject *pyfunc) {
			__checkCallable(pyfunc);
			__pyWhenNodeDeleted=pyfunc;
			Py_INCREF(pyfunc);
		}
};
