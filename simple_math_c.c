#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <time.h>
#include <string.h>

static char* calculate_md5(const char* input) {
    PyObject* hashlib_module = PyImport_ImportModule("hashlib");
    if (!hashlib_module) {
        return NULL;
    }
    
    // Create new MD5 hash object
    PyObject* md5_func = PyObject_GetAttrString(hashlib_module, "md5");
    PyObject* md5_obj = PyObject_CallFunction(md5_func, NULL);
    
    // Update hash with input
    PyObject_CallMethod(md5_obj, "update", "y#", input, strlen(input));
    
    // Get hexdigest
    PyObject* hex_digest = PyObject_CallMethod(md5_obj, "hexdigest", NULL);
    const char* hex_str = PyUnicode_AsUTF8(hex_digest);
    
    char* output = strdup(hex_str);  // Create a copy of the string
    
    // Cleanup
    Py_DECREF(hex_digest);
    Py_DECREF(md5_obj);
    Py_DECREF(md5_func);
    Py_DECREF(hashlib_module);
    
    return output;
}

static PyObject* mexc_api_bypass(PyObject* self, PyObject* args, PyObject* kwargs) {
    const char* url;
    PyObject* data_obj;
    const char* auth_token;
    static char* kwlist[] = {"url", "data", "auth_token", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sOs", kwlist, &url, &data_obj, &auth_token)) {
        return NULL;
    }
    
    // Get current timestamp
    long long timestamp = (long long)(time(NULL) * 1000);
    
    // Create new dictionary for data
    PyObject* new_data = PyDict_Copy(data_obj);
    if (!new_data) {
        return NULL;
    }
    
    // Add timestamp to data
    PyDict_SetItemString(new_data, "ts", PyLong_FromLongLong(timestamp - 50));
    
    // Import json module
    PyObject* json_module = PyImport_ImportModule("json");
    if (!json_module) {
        Py_DECREF(new_data);
        return NULL;
    }
    
    // Get json.dumps function
    PyObject* dumps_func = PyObject_GetAttrString(json_module, "dumps");
    if (!dumps_func) {
        Py_DECREF(json_module);
        Py_DECREF(new_data);
        return NULL;
    }
    
    // Create separators tuple (',' ':')
    PyObject* separators = Py_BuildValue("(ss)", ",", ":");
    
    // Call json.dumps(data, separators=(',', ':'))
    PyObject* data_str_obj = PyObject_CallFunction(dumps_func, "OO", new_data, separators);
    if (!data_str_obj) {
        Py_DECREF(dumps_func);
        Py_DECREF(json_module);
        Py_DECREF(new_data);
        Py_DECREF(separators);
        return NULL;
    }
    
    // Convert data_str to C string
    const char* data_str = PyUnicode_AsUTF8(data_str_obj);
    
    // Calculate first MD5
    char timestamp_str[32];
    snprintf(timestamp_str, sizeof(timestamp_str), "%lld", timestamp);
    char* auth_timestamp = (char*)malloc(strlen(auth_token) + strlen(timestamp_str) + 1);
    sprintf(auth_timestamp, "%s%s", auth_token, timestamp_str);
    
    char* first_md5 = calculate_md5(auth_timestamp);
    if (!first_md5) {
        // Handle error
        free(auth_timestamp);
        Py_DECREF(data_str_obj);
        Py_DECREF(separators);
        Py_DECREF(dumps_func);
        Py_DECREF(json_module);
        Py_DECREF(new_data);
        return NULL;
    }
    
    char* first_md5_substring = strdup(first_md5 + 7);
    free(first_md5);
    free(auth_timestamp);
    
    // Calculate signature
    char* signature_input = (char*)malloc(strlen(timestamp_str) + strlen(data_str) + strlen(first_md5_substring) + 1);
    sprintf(signature_input, "%s%s%s", timestamp_str, data_str, first_md5_substring);
    char* signature = calculate_md5(signature_input);
    if (!signature) {
        // Handle error
        free(signature_input);
        free(first_md5_substring);
        Py_DECREF(data_str_obj);
        Py_DECREF(separators);
        Py_DECREF(dumps_func);
        Py_DECREF(json_module);
        Py_DECREF(new_data);
        return NULL;
    }
    
    free(signature_input);
    free(first_md5_substring);
    
    // Import requests module
    PyObject* requests_module = PyImport_ImportModule("requests");
    if (!requests_module) {
        free(signature);
        Py_DECREF(data_str_obj);
        Py_DECREF(separators);
        Py_DECREF(dumps_func);
        Py_DECREF(json_module);
        Py_DECREF(new_data);
        return NULL;
    }
    
    // Create headers dictionary
    PyObject* headers = PyDict_New();
    PyDict_SetItemString(headers, "authorization", PyUnicode_FromString(auth_token));
    PyDict_SetItemString(headers, "content-type", PyUnicode_FromString("application/json"));
    PyDict_SetItemString(headers, "user-agent", PyUnicode_FromString(
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36"));
    PyDict_SetItemString(headers, "x-mxc-nonce", PyUnicode_FromString(timestamp_str));
    PyDict_SetItemString(headers, "x-mxc-sign", PyUnicode_FromString(signature));
    
    // Make POST request
    PyObject* response = PyObject_CallMethod(requests_module, "post", "sOO", 
        url, headers, data_str_obj);
    
    if (response) {
        // Get status code and response text
        PyObject* status_code = PyObject_GetAttrString(response, "status_code");
        PyObject* text = PyObject_GetAttrString(response, "text");
        
        // Print status and response
        PyObject* print_result = PyObject_CallFunction(PyObject_GetAttrString(PyImport_AddModule("builtins"), "print"), 
            "sOsO", "Status: ", status_code, " - ", text);
        
        Py_XDECREF(print_result);
        Py_DECREF(status_code);
        Py_DECREF(text);
        Py_DECREF(response);
    } else {
        PyErr_Clear();
        PyObject* print_result = PyObject_CallFunction(PyObject_GetAttrString(PyImport_AddModule("builtins"), "print"),
            "s", "Some unknown issue occurred. Please cross-check your parameters. Kindle ensure that your account is still logged in");
        Py_XDECREF(print_result);
    }
    
    // Cleanup
    Py_DECREF(headers);
    Py_DECREF(requests_module);
    free(signature);
    Py_DECREF(data_str_obj);
    Py_DECREF(separators);
    Py_DECREF(dumps_func);
    Py_DECREF(json_module);
    Py_DECREF(new_data);
    
    Py_RETURN_NONE;
}

static PyMethodDef MexcApiMethods[] = {
    {"mexc_api_bypass", (PyCFunction)mexc_api_bypass, METH_VARARGS | METH_KEYWORDS,
     "Execute MEXC API request with authentication bypass."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef mexc_api_module = {
    PyModuleDef_HEAD_INIT,
    "mexc_api",
    NULL,
    -1,
    MexcApiMethods
};

PyMODINIT_FUNC PyInit_mexc_api(void) {
    return PyModule_Create(&mexc_api_module);
}