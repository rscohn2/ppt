#include <Python.h>
#include "ppt_helper_interface.h"

void ppt_to_tool(enum to_tool_codes code, void* arg) {
}

static PyObject *
trace_method(PyObject *self, PyObject *args) {
  PyObject* o;
  PyObject* method_name_o;
  PyObject* module_name_o;
  struct trace_method_info i = {0,0,0};

  if (!PyArg_ParseTuple(args, "O", &o)) {
    return NULL;
  }
  Py_INCREF(o);
  
  i.address = PyCFunction_GET_FUNCTION(o);
  method_name_o = PyObject_GetAttrString(o,"__name__");
  if (method_name_o) {
    i.method_name = PyString_AsString(method_name_o);
    Py_DECREF(method_name_o);
  }
  module_name_o = PyObject_GetAttrString(o,"__module__");
  if (module_name_o) {
    i.module_name = PyString_AsString(module_name_o);
    Py_DECREF(module_name_o);
  }

  ppt_to_tool(TRACE_METHOD, &i);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
test(PyObject *self, PyObject *args) {
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
experiments(PyObject *self, PyObject *args) {
  PyObject* o;

  if (!PyArg_ParseTuple(args, "O", &o)) {
    return NULL;
  }

  PyObject* repr = PyObject_Repr(o);
  printf("repr: %s\n",PyString_AsString(repr));

  PyObject* name = PyObject_GetAttrString(o,"__name__");
  printf("name: %s\n",PyString_AsString(name));
  PyObject* module = PyObject_GetAttrString(o,"__module__");
  printf("module: %s\n",PyString_AsString(module));
  void* meth = PyCFunction_GET_FUNCTION(o);
  printf("meth: %p\n",meth);
  printf("&trace_method: %p\n",&trace_method);

  PyObject* frame = (PyObject*)PyEval_GetFrame();
  PyObject* code = PyObject_GetAttrString(frame, "f_code");
  PyObject* bname = PyObject_GetAttrString (code, "co_name");
  printf("bname: %s\n",PyString_AsString(bname));

  Py_RETURN_NONE;
}

static PyMethodDef Methods[] = {
    {"trace_method",  trace_method, METH_VARARGS,
     "trace a method"},
    {"test",  test, METH_VARARGS,
     "test function."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initpinhelper(void)
{
    (void) Py_InitModule("pinhelper", Methods);
}
