#include <Python.h>
#include <stdbool.h>
#include "two_bits_encoding.h"
static PyObject *


encode_module(PyObject *self, PyObject *args) {
    char *queryString;

    if (!PyArg_ParseTuple(args, "s", &queryString)){
        return PyErr_Format(PyExc_TypeError, "Wrong input: expected py string object");
    }
    //PySys_WriteStdout("Trying to encode \"%s\"\n", queryString);

/* DOES NOT WORK, value is improperly passed out from function

    uint64_t val = encode(queryString, &strLen);
    printf(">>%llu\n", val);
    PySys_WriteStdout("Encoded string of size %lu\n", strLen);
    PySys_WriteStdout(">>>%llu\n", val);
*/
    binaryWord_t val_bis;
    encode_bis(queryString, &val_bis);
    //PySys_WriteStdout("Encoded string of size %lu\n", strLen);
    //printf("%llu\n", val_bis);
    return Py_BuildValue("K", val_bis.value);
}

static PyObject *
decode_module(PyObject *self, PyObject *args) {
    char word[1024];

    /*
    Parser taken from https://stackoverflow.com/questions/52668426/python-c-api-check-if-int64-is-unsigned
    */

    int wordLen;
    PyObject* input_obj;
    if (!PyArg_ParseTuple(args, "Oi", &input_obj, &wordLen)){
        return PyErr_Format(PyExc_TypeError, "Wrong input: expected py object and an integer");
    }
    unsigned long long input = PyLong_AsUnsignedLongLong(input_obj);
    if(input == -1 && PyErr_Occurred()) {
        PyErr_Clear();
        return PyErr_Format(PyExc_TypeError, "Parameter must be an unsigned integer type, but got %s", Py_TYPE
        (input_obj)->tp_name);
    }
    binaryWord_t bWord;
    bWord.len   = wordLen;
    bWord.value = input;
    //PySys_WriteStdout("Trying to decode \"%llu\" (%d char long)\n", input, wordLen);
    //char *word = malloc( ( wordLen + 1 ) * sizeof(char) ); // No '\0'
    decode(bWord, word, false);
    //free(word);
    //PySys_WriteStdout("%s\n", word);
    return Py_BuildValue("s", word);// Seems to be ok w/ no '\0'
    Py_RETURN_NONE;
}

static PyObject *
truncate_module(PyObject *self, PyObject *args) {
    int newWordLen, wordLen;
    PyObject* input_obj;
    if ( !PyArg_ParseTuple(args, "Oii", &input_obj, &wordLen, &newWordLen) ){
            return PyErr_Format(PyExc_TypeError, "Wrong input: expected py object and and two integers");
    }

    unsigned long long input = PyLong_AsUnsignedLongLong(input_obj);
    if(input == -1 && PyErr_Occurred()) {
        PyErr_Clear();
        return PyErr_Format(PyExc_TypeError, "Parameter must be an unsigned integer type, but got %s", Py_TYPE
        (input_obj)->tp_name);
    }

    binaryWord_t bWord, bWordTrunc;
    bWord.value = input;
    bWord.len  =  wordLen;

    truncateBinaryLeft_bis(bWord, newWordLen, &bWordTrunc);
    return Py_BuildValue("K", bWordTrunc.value);
}



static PyMethodDef twoBitsMethods[] = {
    //...
    {"encode",  encode_module, METH_VARARGS,
     "Encode a ATCG string in a 2bit per char uint64"},
    {"decode",  decode_module, METH_VARARGS,
     "Decode a ATCG string from a 2bit per char uint64"},
     {"truncate",  truncate_module, METH_VARARGS,
     "truncate to left a ATCG string passed as a uint64"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef twobitsmodule = {
    PyModuleDef_HEAD_INIT,
    "twobits",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    twoBitsMethods
};

PyMODINIT_FUNC
PyInit_twobits(void)
{
    return PyModule_Create(&twobitsmodule);
}

int
main(int argc, char *argv[])
{
  wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    PyImport_AppendInittab("spam", PyInit_twobits);

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(program);

    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();

    /* Optionally import the module; alternatively,
       import can be deferred until the embedded script
       imports it. */
    PyImport_ImportModule("twobits");

    PyMem_RawFree(program);
    return 0;
}