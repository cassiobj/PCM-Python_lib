/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  PCMPy - Lib
  Python Extension for  Oracle BRM
  Copyright Cassio Jemael 2024
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


#include <Python.h>
#include "pcm_py.h"

#include <libxml/parser.h>
#include <libxml/tree.h>


PyObject * pcm_open_transaction(PyObject * self , PyObject *args)
{

    char                    msg[BUFSIZ];
	char                    *flist_entrada = NULL;
    long                    pcm_op_flag = 0;

    size_t                  s_flist_entrada = 0;
    char                    *trans_flist;
    poid_t		            *x_poid = NULL;
    pin_flist_t 	        *out_flistp = NULL;
    char                    *out_flist_str = NULL;
    int                     len = 0;


    PIN_ERR_CLEAR_ERR(EBUFP);
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP,  __PROGRAM__ "pcm_open_transaction called.");


    // check args
    if (!PyArg_ParseTuple(args, "l|s", &pcm_op_flag, &flist_entrada)) 
    {
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);

        // retrieve error description
        PyObject *str_exc_value = PyObject_Str(value);
        const char *error_message = PyString_AsString(str_exc_value);
        
        sprintf(msg, "%s : %s \n",  __PROGRAM__ " pcm_open_transaction parameter error", error_message);
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

    
    if (flist_entrada!=NULL)
        pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , __PROGRAM__ " pcm_open_transaction for flag %ld , input flist  %s ", pcm_op_flag, flist_entrada );   
    else
        pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , __PROGRAM__ " pcm_open_transaction for flag %ld  ", pcm_op_flag );       


    if ((pcm_op_flag != PCM_TRANS_OPEN_READONLY &&
	    pcm_op_flag != PCM_TRANS_OPEN_READWRITE &&
		pcm_op_flag != PCM_TRANS_OPEN_GLOBALTRANSACTION &&
		pcm_op_flag != PCM_OPFLG_USE_GIVEN_POID_DB_NUMBER &&
		pcm_op_flag != PCM_TRANS_OPEN_LOCK_OBJ &&
		pcm_op_flag != PCM_TRANS_OPEN_LOCK_DEFAULT &&
		pcm_op_flag !=  PCM_OPFLG_LOCK_NONE	) || 
		(flist_entrada != NULL && pcm_op_flag == PCM_TRANS_OPEN_LOCK_OBJ) 
	)
	{    	
        sprintf(msg, __PROGRAM__" pcm_open_transaction invalid flag:%ld \n", pcm_op_flag);
        PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_NOT_SUPPORTED, 0, 0, 0);        
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP,  pcm_dump_ebuf(EBUFP, msg) );

        PyErr_SetString(PCMPyError, msg );
        return Py_BuildValue("l", -1);
	}		

    PyObject *nargs = PyTuple_New(1);    
    PyTuple_SetItem(nargs, 0, PyLong_FromLong(ARRAY_CONN[keyConn].DATABASE ) );
    if ( connect_cm(self, nargs ) < 0)
    {
        sprintf(msg, __PROGRAM__" pcm_open_transaction connecting database :%ld \n", pcm_op_flag);
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP,  pcm_dump_ebuf(EBUFP, msg) );
        return Py_BuildValue("l", -1);
    }
    Py_DECREF(nargs);

    PIN_ERR_CLEAR_ERR(EBUFP);
    if (flist_entrada == NULL)
    {
        x_poid = PIN_POID_CREATE(ARRAY_CONN[keyConn].DATABASE , "/dummy", -1, EBUFP);
	
		ARRAY_CONN[keyConn].trans_flistp = PIN_FLIST_CREATE(EBUFP);
		PIN_FLIST_FLD_SET(ARRAY_CONN[keyConn].trans_flistp, PIN_FLD_POID, x_poid, EBUFP);
		if( PIN_ERR_IS_ERR( EBUFP ))
		{
            if (ARRAY_CONN[keyConn].trans_flistp)
			    PIN_FLIST_DESTROY_EX(&(ARRAY_CONN[keyConn]).trans_flistp, EBUFP);
			
            sprintf(msg,  __PROGRAM__" pcm_open_transaction error creating flist. \n");
            PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_CREATE, 0, 0, 0);        
            pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP,  pcm_dump_ebuf(EBUFP, msg) );

            PyErr_SetString(PCMPyError, msg );
            return Py_BuildValue("l", -1);
		}


	}
	else
	{
		trans_flist = (char *) calloc ( (s_flist_entrada + 1), sizeof(char));
		memset(trans_flist, 0, (s_flist_entrada + 1));
		
		sprintf(trans_flist, flist_entrada);
		PIN_STR_TO_FLIST(trans_flist, ARRAY_CONN[keyConn].DATABASE , &(ARRAY_CONN[keyConn]).trans_flistp, EBUFP);
		if( PIN_ERR_IS_ERR( EBUFP ))
		{
			
            sprintf(msg,  __PROGRAM__"  pcm_open_transaction: error converting flist string to flist %s. \n", trans_flist);
            PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_CANON_CONV, 0, 0, 0);        
            pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP,  pcm_dump_ebuf(EBUFP, msg) );

            PyErr_SetString(PCMPyError, msg );
            return Py_BuildValue("l", -1);		
		}
	}


    

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_open_transaction flist in: ", ARRAY_CONN[keyConn].trans_flistp);		

    if (PCM_PY_LOG_LEVEL >= PIN_ERR_LEVEL_DEBUG)
    {
        PIN_FLIST_TO_STR(ARRAY_CONN[keyConn].trans_flistp, &out_flist_str, &len, EBUFP);
        pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP, __PROGRAM__" pcm_open_transaction flist int: %s", out_flist_str);
        free(out_flist_str);
        out_flist_str = NULL;
    }


	PCM_OP( ARRAY_CONN[keyConn].CTXP , PCM_OP_TRANS_OPEN, pcm_op_flag, ARRAY_CONN[keyConn].trans_flistp, &out_flistp, EBUFP);
	if( PIN_ERR_IS_ERR( EBUFP ))
	{
        if (ARRAY_CONN[keyConn].trans_flistp != NULL)
		    PIN_FLIST_DESTROY_EX(&(ARRAY_CONN[keyConn]).trans_flistp, EBUFP);

        if (out_flistp != NULL)
        PIN_FLIST_DESTROY_EX(&out_flistp, EBUFP);		
        
        sprintf(msg, __PROGRAM__"  pcm_open_transaction: error executing opcode. \n");
        PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_OPCODE, 0, 0, 0);        
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP,  pcm_dump_ebuf(EBUFP, msg) );

        PyErr_SetString(PCMPyError, msg );
        return Py_BuildValue("l", -1);				
	}
    else
    {
        PIN_FLIST_TO_STR(out_flistp, &out_flist_str, &len, EBUFP);
        pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP, __PROGRAM__" pcm_open_transaction flist out: %s", out_flist_str);
        free(out_flist_str);
    }	
    
   	if (out_flistp != NULL)
        PIN_FLIST_DESTROY_EX(&out_flistp, EBUFP);		

    return Py_BuildValue("l", 0);
}



PyObject * pcm_commit_transaction(PyObject * self )
{
    char                    msg[BUFSIZ];
	char                    *flist_entrada = NULL;
    long                    pcm_op_flag = 0;

    size_t                  s_flist_entrada = 0;
    char                    *trans_flist;
    poid_t		            *x_poid = NULL;
    pin_flist_t 	        *out_flistp = NULL;
    char                    *out_flist_str = NULL;
    int                     len = 0;

    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP,  __PROGRAM__ "pcm_commit_transaction called.");


    if (PCM_PY_LOG_LEVEL >= PIN_ERR_LEVEL_DEBUG)
    {
        
        PIN_FLIST_TO_STR(ARRAY_CONN[keyConn].trans_flistp, &out_flist_str, &len, EBUFP);
        pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP, __PROGRAM__" pcm_commit_transaction commiting flist opened: %s", out_flist_str);
        free(out_flist_str);
        out_flist_str = NULL;
    }

    	
    PCM_OP( ARRAY_CONN[keyConn].CTXP , PCM_OP_TRANS_COMMIT, 0, ARRAY_CONN[keyConn].trans_flistp, &out_flistp, EBUFP);
    if( PIN_ERR_IS_ERR( EBUFP ))	
	{
	
        if (ARRAY_CONN[keyConn].trans_flistp != NULL)
		    PIN_FLIST_DESTROY_EX(&(ARRAY_CONN[keyConn]).trans_flistp, EBUFP);

        if (out_flistp != NULL)
            PIN_FLIST_DESTROY_EX(&out_flistp, EBUFP);		
        
        sprintf(msg, __PROGRAM__"  pcm_commit_transaction: error executing opcode. \n");
        PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_OPCODE, 0, 0, 0);        
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP,  pcm_dump_ebuf(EBUFP, msg) );

        PyErr_SetString(PCMPyError, msg );
        return Py_BuildValue("l", -1);				

	}
	
    if (PCM_PY_LOG_LEVEL >= PIN_ERR_LEVEL_DEBUG)
    {
        PIN_FLIST_TO_STR(out_flistp, &out_flist_str, &len, EBUFP);
        pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP, __PROGRAM__" pcm_commit_transaction commiting flist opened: %s", out_flist_str);
        free(out_flist_str);
        out_flist_str = NULL;
    }

	
    if (ARRAY_CONN[keyConn].trans_flistp != NULL)
        PIN_FLIST_DESTROY_EX(&(ARRAY_CONN[keyConn]).trans_flistp, EBUFP);

    if (out_flistp != NULL)
        PIN_FLIST_DESTROY_EX(&out_flistp, EBUFP);		

    return Py_BuildValue("l", 0);			
}



PyObject * pcm_abort_transaction(PyObject * self )
{
    char                    msg[BUFSIZ];
	char                    *flist_entrada = NULL;
    long                    pcm_op_flag = 0;

    size_t                  s_flist_entrada = 0;
    char                    *trans_flist;
    poid_t		            *x_poid = NULL;
    pin_flist_t 	        *out_flistp = NULL;
    char                    *out_flist_str = NULL;
    int                     len = 0;

    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP,  __PROGRAM__ "pcm_abort_transaction called.");


    if (PCM_PY_LOG_LEVEL >= PIN_ERR_LEVEL_DEBUG)
    {
        PIN_FLIST_TO_STR(ARRAY_CONN[keyConn].trans_flistp, &out_flist_str, &len, EBUFP);
        pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP, __PROGRAM__" pcm_abort_transaction commiting flist opened: %s", out_flist_str);
        free(out_flist_str);
        out_flist_str = NULL;
    }

    	
    PCM_OP( ARRAY_CONN[keyConn].CTXP , PCM_OP_TRANS_ABORT, 0, ARRAY_CONN[keyConn].trans_flistp, &out_flistp, EBUFP);
    if( PIN_ERR_IS_ERR( EBUFP ))	
	{
	
        if (ARRAY_CONN[keyConn].trans_flistp != NULL)
		    PIN_FLIST_DESTROY_EX(&(ARRAY_CONN[keyConn]).trans_flistp, EBUFP);

        if (out_flistp != NULL)
            PIN_FLIST_DESTROY_EX(&out_flistp, EBUFP);		
        
        sprintf(msg, __PROGRAM__"  pcm_abort_transaction: error executing opcode. \n");
        PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_OPCODE, 0, 0, 0);        
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP,  pcm_dump_ebuf(EBUFP, msg) );

        PyErr_SetString(PCMPyError, msg );
        return Py_BuildValue("l", -1);				

	}
	
    if (PCM_PY_LOG_LEVEL >= PIN_ERR_LEVEL_DEBUG)
    {
        PIN_FLIST_TO_STR(out_flistp, &out_flist_str, &len, EBUFP);
        pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP, __PROGRAM__" pcm_abort_transaction commiting flist opened: %s", out_flist_str);
        free(out_flist_str);
        out_flist_str = NULL;
    }

	
    if (ARRAY_CONN[keyConn].trans_flistp != NULL)
        PIN_FLIST_DESTROY_EX(&(ARRAY_CONN[keyConn]).trans_flistp, EBUFP);

    if (out_flistp != NULL)
        PIN_FLIST_DESTROY_EX(&out_flistp, EBUFP);		

    return Py_BuildValue("l", 0);				
}
