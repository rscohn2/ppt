#include <Python.h>
#include "ppt_helper_interface.h"

void ppt_to_tool(enum to_tool_codes code, void* arg) {
}

static PyObject *
trace_method(PyObject *self, PyObject *args) {
  PyObject* o;
  if (!PyArg_ParseTuple(args, "O", &o)) {
    return NULL;
  }
  
  struct trace_method_info i;
  i.address = PyCFunction_GET_FUNCTION(o);
  i.method_name = PyString_AsString(PyObject_GetAttrString(o,"__name__"));
  i.module_name = PyString_AsString(PyObject_GetAttrString(o,"__module__"));
  ppt_to_tool(TRACE_METHOD, &i);
  Py_RETURN_NONE;
}

static PyObject *
test(PyObject *self, PyObject *args) {
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
  PyObject* ml = PyObject_GetAttrString(o,"__ml__");
  printf("ml: %p\n",ml);

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

void ppt_helper(int code, void* args) {
  switch(code) {
  default:
    fprintf(stderr, "ppt_helper unknown code: %d\n", code);
  }
}

PyMODINIT_FUNC
initpinpytrace(void)
{
    (void) Py_InitModule("pinpytrace", Methods);
}
