#include <Python.h>
#include "ppt_helper_interface.h"

void ppt_to_tool(enum to_tool_codes code, void* arg) {
}

static PyObject *
start_trace(PyObject *self, PyObject *args) {

  ppt_to_tool(START_TRACE, 0);

  Py_RETURN_NONE;
}

static PyObject *
stop_trace(PyObject *self, PyObject *args) {
  char const** p;
  int i;

  ppt_to_tool(STOP_TRACE, &p);

  PyObject* l = PyList_New(0);
  for (i = 0; p[i]; i++) {
    PyList_Append(l, Py_BuildValue("s", p[i]));
  }

  return l;
}

static PyMethodDef Methods[] = {
    {"start_trace",  start_trace, METH_VARARGS,
     "start tracing native execution"},
    {"stop_trace",  stop_trace, METH_VARARGS,
     "stop tracing native execution"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initpinhelper(void)
{
    (void) Py_InitModule("pinhelper", Methods);
}
