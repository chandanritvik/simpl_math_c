#include <Python.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/md5.h>

// Helper function to compute MD5 hash
void compute_md5(const char* input, char* output) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)input, strlen(input), digest);
    
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(&output[i * 2], "%02x", digest[i]);
    }
}

// The main function to perform the MEXC API bypass logic
static PyObject* mexc_api_bypass(PyObject* self, PyObject* args) {
    const char* url;
    const char* auth_token;
    PyObject* data_dict;
    
    // Parse arguments from Python (url, data dictionary, auth token)
    if (!PyArg_ParseTuple(args, "sOs", &url, &data_dict, &auth_token)) {
        return NULL;
    }
    
    // Ensure data_dict is a dictionary
    if (!PyDict_Check(data_dict)) {
        PyErr_SetString(PyExc_TypeError, "The 'data' argument must be a dictionary.");
        return NULL;
    }
    
    // Get the current timestamp in milliseconds
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long long timestamp = (long long)(ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
    
    // Modify timestamp for bypass logic
    long long modified_timestamp = timestamp - 50;

    // Add "ts" field to data dictionary
    PyObject* py_timestamp = PyLong_FromLongLong(modified_timestamp);
    PyDict_SetItemString(data_dict, "ts", py_timestamp);
    Py_DECREF(py_timestamp);

    // Convert data dictionary to JSON string
    PyObject* json_module = PyImport_ImportModule("json");
    if (!json_module) {
        PyErr_SetString(PyExc_ImportError, "Failed to import 'json' module.");
        return NULL;
    }
    PyObject* dumps_func = PyObject_GetAttrString(json_module, "dumps");
    if (!dumps_func || !PyCallable_Check(dumps_func)) {
        PyErr_SetString(PyExc_AttributeError, "'json.dumps' is not callable.");
        return NULL;
    }
    PyObject* json_args = PyTuple_Pack(1, data_dict);
    PyObject* data_str_obj = PyObject_CallObject(dumps_func, json_args);
    Py_DECREF(json_args);
    Py_DECREF(dumps_func);
    Py_DECREF(json_module);

    if (!data_str_obj) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to serialize data dictionary to JSON.");
        return NULL;
    }

    const char* data_str = PyUnicode_AsUTF8(data_str_obj);

    // Generate first MD5 hash
    char first_md5_input[256];
    snprintf(first_md5_input, sizeof(first_md5_input), "%s%lld", auth_token, modified_timestamp);
    char first_md5[33];
    compute_md5(first_md5_input, first_md5);

    // Use only part of the first MD5
    char first_md5_trimmed[26];
    strncpy(first_md5_trimmed, first_md5 + 7, 25);
    first_md5_trimmed[25] = '\0';

    // Generate final MD5 signature
    char final_md5_input[1024];
    snprintf(final_md5_input, sizeof(final_md5_input), "%lld%s%s", modified_timestamp, data_str, first_md5_trimmed);
    char signature[33];
    compute_md5(final_md5_input, signature);

    // Print the generated headers and signature (For simplicity, we are not sending the HTTP request in C)
    printf("Generated Headers:\n");
    printf("Authorization: %s\n", auth_token);
    printf("Content-Type: application/json\n");
    printf("User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36\n");
    printf("x-mxc-nonce: %lld\n", modified_timestamp);
    printf("x-mxc-sign: %s\n", signature);

    // Cleanup
    Py_DECREF(data_str_obj);

    Py_RETURN_NONE;
}

// Define the module's methods
static PyMethodDef MexcApiMethods[] = {
    {"mexc_api_bypass", mexc_api_bypass, METH_VARARGS, "Perform the MEXC API bypass logic"},
    {NULL, NULL, 0, NULL}
};

// Define the module
static struct PyModuleDef mexc_api_module = {
    PyModuleDef_HEAD_INIT,
    "mexc_api",  // Module name
    NULL,        // Module documentation
    -1,          // Module keeps state in global variables
    MexcApiMethods
};

// Module initialization function
PyMODINIT_FUNC PyInit_mexc_api(void) {
    return PyModule_Create(&mexc_api_module);
}
