/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  PCMPy - Lib
  Python Extension for  Oracle BRM
  Copyright Cassio Jemael 2024
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


#include <Python.h>
#include "pcm_py.h"

#include <libxml/parser.h>
#include <libxml/tree.h>



PyObject * pcm_get_poid_db(PyObject * self , PyObject *args)
{
    char                msg[BUFSIZ * 4];

    poid_t              * poid_p = 0;
    int64               db = 0;
    char                *poid_str = NULL;
    

        PIN_ERR_CLEAR_ERR(EBUFP);
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP,  __PROGRAM__ "pcm_get_poid_db called.");

    // check args
    if (!PyArg_ParseTuple(args, "s", &poid_str)) 
    {
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);

        // retrieve error description
        PyObject *str_exc_value = PyObject_Str(value);
        const char *error_message = PyString_AsString(str_exc_value);
        
        sprintf(msg, "%s : %s \n",  __PROGRAM__ " pcm_get_poid_db parameter error", error_message);
        PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_ARG, 0, 0, 0);        
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP,  pcm_dump_ebuf(EBUFP, msg) );

        PyErr_SetString(PCMPyError, msg );

        // Free Memory
        Py_XDECREF(type);
        Py_XDECREF(value);
        Py_XDECREF(traceback);
        Py_XDECREF(str_exc_value);
        
        return NULL;
    }
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , __PROGRAM__ " pcm_get_poid_db from  %s ", poid_str );

    poid_p = PIN_POID_FROM_STR(poid_str, NULL , EBUFP);    
    if( PIN_ERR_IS_ERR( EBUFP ))
	{
        sprintf(msg, __PROGRAM__ " pcm_get_poid_db error getting type from poid for parameter from %s ", poid_str );
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
		fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );
			
            
        PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
        return NULL;
    }
    

    db = PIN_POID_GET_DB(poid_p);

    PIN_POID_DESTROY(poid_p, NULL);

    return Py_BuildValue("l", db);
}




PyObject * pcm_get_poid_type(PyObject * self , PyObject *args)
{
    char                msg[BUFSIZ * 4];

    poid_t              * poid_p = 0;
    char                *poid_type = NULL;
    char                *poid_str = NULL;
    

    PIN_ERR_CLEAR_ERR(EBUFP);
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP,  __PROGRAM__ "pcm_get_poid_type called.");

    // check args
    if (!PyArg_ParseTuple(args, "s", &poid_str)) 
    {
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);

        // retrieve error description
        PyObject *str_exc_value = PyObject_Str(value);
        const char *error_message = PyString_AsString(str_exc_value);
        
        sprintf(msg, "%s : %s \n",  __PROGRAM__ " pcm_get_poid_type parameter error", error_message);
        PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_ARG, 0, 0, 0);        
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP,  pcm_dump_ebuf(EBUFP, msg) );

        PyErr_SetString(PCMPyError, msg );

        // Free Memory
        Py_XDECREF(type);
        Py_XDECREF(value);
        Py_XDECREF(traceback);
        Py_XDECREF(str_exc_value);
        
        return NULL;
    }
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , __PROGRAM__ " pcm_get_poid_type from  %s ", poid_str );

    poid_p = PIN_POID_FROM_STR(poid_str, NULL , EBUFP);
    if( PIN_ERR_IS_ERR( EBUFP ))
	{
        sprintf(msg, __PROGRAM__ " pcm_get_poid_type error getting type from poid for parameter from %s ", poid_str );
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
		fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );
			
            
        PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
        return NULL;
    }

    poid_type = (char *)PIN_POID_GET_TYPE(poid_p);

    PIN_POID_DESTROY(poid_p, NULL);

    return Py_BuildValue("s", poid_type);
}



PyObject * pcm_get_poid_number(PyObject * self , PyObject *args)
{


    char                msg[BUFSIZ * 4];

    poid_t              * poid_p = 0;
    char                *poid_str;
    pin_poid_id_t       return_value;;

    PIN_ERR_CLEAR_ERR(EBUFP);
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP,  __PROGRAM__ "pcm_get_poid_number called.");

    // check args
    if (!PyArg_ParseTuple(args, "s", &poid_str)) 
    {
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);

        // retrieve error description
        PyObject *str_exc_value = PyObject_Str(value);
        const char *error_message = PyString_AsString(str_exc_value);
        
        sprintf(msg, "%s : %s \n",  __PROGRAM__ " pcm_get_poid_number parameter error", error_message);
        PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_ARG, 0, 0, 0);        
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP,  pcm_dump_ebuf(EBUFP, msg) );

        PyErr_SetString(PCMPyError, msg );

        // Free Memory
        Py_XDECREF(type);
        Py_XDECREF(value);
        Py_XDECREF(traceback);
        Py_XDECREF(str_exc_value);
        
        return NULL;
    }
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , __PROGRAM__ " pcm_get_poid_number from  %s ", poid_str );

    poid_p = PIN_POID_FROM_STR(poid_str, NULL , EBUFP);
    if( PIN_ERR_IS_ERR( EBUFP ))
	{
        sprintf(msg, __PROGRAM__ " pcm_get_poid_number error getting number from poid for parameter from %s ", poid_str );
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
		fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );
			
            
        PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
        return NULL;
    }

    return_value = PIN_POID_GET_ID(poid_p);

    PIN_POID_DESTROY(poid_p, NULL);

    return Py_BuildValue("l", return_value);
    

}






PyObject * pcm_create_poid(PyObject * self , PyObject *args)
{
    pin_db_no_t         db = 0;
    pin_poid_type_t     type = NULL;
    pin_poid_id_t       id  = 0;

    

    char                msg[BUFSIZ * 4];

    poid_t              * poid_p = 0;
    char	            poid_buf[PCM_MAX_POID_TYPE + 1];
    int32	            maxpoidlen = 0;
    char                *poid_return;
    char                *poid_str;

    PIN_ERR_CLEAR_ERR(EBUFP);
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP,  __PROGRAM__ "pcm_create_poid called.");

    // check args
    if (!PyArg_ParseTuple(args, "lsl", &db, &type, &id)) 
    {
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);

        // retrieve error description
        PyObject *str_exc_value = PyObject_Str(value);
        const char *error_message = PyString_AsString(str_exc_value);
        
        sprintf(msg, "%s : %s \n",  __PROGRAM__ " pcm_create_poid parameter error", error_message);
        PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_ARG, 0, 0, 0);        
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP,  pcm_dump_ebuf(EBUFP, msg) );

        PyErr_SetString(PCMPyError, msg );

        // Free Memory
        Py_XDECREF(type);
        Py_XDECREF(value);
        Py_XDECREF(traceback);
        Py_XDECREF(str_exc_value);
        
        return NULL;
    }
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , __PROGRAM__ " pcm_create_poid parameter: db: %lld  type: %s  id: %lld ", db, type, id );

    poid_p = PIN_POID_CREATE(db, type, id, EBUFP);
    if( PIN_ERR_IS_ERR( EBUFP ))
	{
        sprintf(msg, __PROGRAM__ " Error creating poid for parameter: db: %lld  type: %s  id: %lld ", db, type, id );
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
		fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );
			
            
        PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
        return NULL;
    }

    memset(poid_buf, 0, (PCM_MAX_POID_TYPE + 1));
	poid_str = poid_buf;
	maxpoidlen = sizeof(poid_buf);

	PIN_POID_TO_STR(poid_p, &poid_str, &maxpoidlen, EBUFP);
    if( PIN_ERR_IS_ERR( EBUFP ))
	{
        sprintf(msg, __PROGRAM__ " Error converting poid to str from parameter: db: %lld  type: %s  id: %lld ", db, type, id );
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
		fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );
		            
        PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );    
        return NULL;
    }
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , __PROGRAM__ " pcm_create_poid poid created: %s", poid_str );
    

    /*
    poid_return  = (char *) calloc ( (strlen(poid_str) + 1), sizeof(char));
    strcpy(*poid_number, poid_str);
    */

    PIN_POID_DESTROY(poid_p, NULL);
    return Py_BuildValue("s", poid_str);

}