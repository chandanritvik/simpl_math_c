#include <Python.h>
#include <math.h>

static PyObject* calculate_expression(PyObject* self, PyObject* args) {
    double a = 2.0;  // default value
    if (!PyArg_ParseTuple(args, "|d", &a)) {  // optional argument
        return NULL;
    }
    
    double b = pow(2.0, 2.223);
    double c = a - b + a * b;
    
    return PyFloat_FromDouble(c);
}

static PyMethodDef SimpleMathMethods[] = {
    {"calculate_expression", calculate_expression, METH_VARARGS, "Calculate the expression"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef simplemath_module = {
    PyModuleDef_HEAD_INIT,
    "simple_math_c",
    NULL,
    -1,
    SimpleMathMethods
};

PyMODINIT_FUNC PyInit_simple_math_c(void) {
    return PyModule_Create(&simplemath_module);
}