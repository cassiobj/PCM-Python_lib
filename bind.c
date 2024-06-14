/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  PCMPy - Lib
  Python Extension for  Oracle BRM
  Copyright Cassio Jemael 2024
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

#include <Python.h>
#include "pcm_py.h"

char connect_cm_docs[] = "Function responsable for connect in CM.";
char PCMPy_docs[] = "Oracle BRM PCM Functions. ";


PyObject *PCMPyError;

PyMethodDef PCMPy_funcs[] = {
	{	"pcm_connect",              (PyCFunction)connect_cm,                METH_VARARGS,       connect_cm_docs},
    {	"pcm_set_context",          (PyCFunction)set_context,		        METH_VARARGS,	    "Change context for connection. 0 to 15"},
    {	"pcm_op",		            (PyCFunction)pcm_execute_op,	        METH_VARARGS,	    "Execute opcode"},
    {	"pcm_create_poid",          (PyCFunction)pcm_create_poid,	        METH_VARARGS,	    "Create poid register number"},
    {	"pcm_get_poid_number",      (PyCFunction)pcm_get_poid_number,       METH_VARARGS,	    "get id from poid"},
    {	"pcm_get_poid_type",        (PyCFunction)pcm_get_poid_type,         METH_VARARGS,	    "get type from poid"},
    {	"pcm_get_poid_db",          (PyCFunction)pcm_get_poid_db,           METH_VARARGS,	    "get db from poid"},
    {	"pcm_get_field_spec",       (PyCFunction)pcm_get_field_spec,        METH_VARARGS,       "Search for field specification"},
    {	"pcm_get_custom_opcode",    (PyCFunction)pcm_get_custom_opcode,     METH_VARARGS,       "Search for opcode number"},
    {	"pcm_open_transaction",     (PyCFunction)pcm_open_transaction,      METH_VARARGS,       "Open Transaction"},
    {	"pcm_commit_transaction",   (PyCFunction)pcm_commit_transaction,    METH_NOARGS,        "Commit Transaction"},
    {	"pcm_abort_transaction",    (PyCFunction)pcm_abort_transaction,     METH_NOARGS,        "Abort Transaction"},
    {	"pcm_pin_conf",             (PyCFunction)pcm_pin_conf,              METH_VARARGS,        "Get pin.conf information"},
        

	{	NULL}
};



// Tabela de métodos
static PyMethodDef PCMPy_errors_func[] = {
    {"error", NULL, METH_VARARGS, "PCMPy Object Error"},
    {NULL, NULL, 0, NULL}  // Sentinela
};




PyMODINIT_FUNC initPCMPy(void) {

    
    PyObject *m = NULL;
    
    
    (void) Py_InitModule("PCMPy", PCMPy_funcs);    
    
    // Starting exception object
    //m = Py_InitModule3("PCMPyError", PCMPy_errors_func, "Error Exception");
    m = Py_InitModule("PCMPy", PCMPy_errors_func);
    if (m != NULL)
    {

        PCMPyError = PyErr_NewException("PCMPy.error", NULL, NULL);
        
        Py_XINCREF(PCMPyError);
        
        if (PyModule_AddObject(m, "error", PCMPyError) < 0) {
            Py_XDECREF(PCMPyError);
            Py_CLEAR(PCMPyError);
            Py_DECREF(m);            
            // return NULL;
        }
    }


    pcm_init_constants(m);

    pcm_read_pinconf();
    if (pcm_init_log() < 0)
    {
        fprintf(stderr, "Fatal Error opening log file \n");        
    }

    pcm_load_and_sort_field_spec(m );
            
}


