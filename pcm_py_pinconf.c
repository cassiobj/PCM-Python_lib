/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  PCMPy - Lib
  Python Extension for  Oracle BRM
  Copyright Cassio Jemael 2024
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


#include <Python.h>
#include "pcm_py.h"

#include <libxml/parser.h>
#include <libxml/tree.h>





PyObject * pcm_pin_conf(PyObject * self , PyObject *args)
{
    char                msg[BUFSIZ * 4];

    poid_t              * poid_p = 0;
    int64               db = 0;
    char                *program = NULL;
	char                *token = NULL;
	char                *type = NULL;
    char 				*retorno = NULL;
	int  value=0;
	long double 		decimal = 0;

	pin_poid_id_t		poid_id;
	char				*poid_type;
	


    PIN_ERR_CLEAR_ERR(EBUFP);
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP,  __PROGRAM__ "pcm_pin_conf called.");

    // check args
    if (!PyArg_ParseTuple(args, "sss", &program, &token, &type)) 
    {
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);

        // retrieve error description
        PyObject *str_exc_value = PyObject_Str(value);
        const char *error_message = PyString_AsString(str_exc_value);
        
        sprintf(msg, "%s : %s \n",  __PROGRAM__ " pcm_pin_conf parameter error", error_message);
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
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , __PROGRAM__ " pcm_pin_conf find for - %s %s ", program, token );


	if (strncmp(type, "STR", 3) == 0)
	{
		if (pcm_get_string_pin_conf(program, token, &retorno) == -1)
		{			
			sprintf(msg, __PROGRAM__" pcm_pin_conf: error getting STR pin.conf information for - %s %s ", program, token );
			pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
			
				
				
			PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
			return NULL;
		}
		else
		{			
			return Py_BuildValue("s", retorno);
		}		
	}
	else if (strncmp(type, "INT", 3) == 0)
	{
		if (pcm_get_int_pin_conf(program, token, &value) == -1)
		{
			sprintf(msg, __PROGRAM__" pcm_pin_conf: error getting INT pin.conf information for - %s %s ", program, token );
			pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
							
				
			PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
			return NULL;
		}
		else
		{
			return Py_BuildValue("l", value);
		}		
	}
	else if (strncmp(type, "DECIMAL", 7) == 0)
	{
		if (pcm_get_decimal_pin_conf(program, token, &decimal) == -1)
		{
			sprintf(msg, __PROGRAM__" pcm_pin_conf: error getting DECIMAL pin.conf information for - %s %s ", program, token );
			pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
						
				
			PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
			return NULL;
		}
		else
		{			
			return Py_BuildValue("d", (double) decimal);
		}		
	}
	else if (strncmp(type, "POID", 4) == 0)
	{
		


		if (pcm_get_string_pin_conf(program, token, &retorno) == -1)
		{
			sprintf(msg, __PROGRAM__" pcm_pin_conf: error getting POID pin.conf information for - %s %s ", program, token );
			pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
			
				
				
			PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
			return NULL;
		}
		else
		{
			
						
			PyObject *dict = PyDict_New();
			
			poid_p = PIN_POID_FROM_STR(retorno, NULL, EBUFP );

			if( PIN_ERR_IS_ERR( EBUFP ))
			{	
        
			   PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_UNKNOWN_POID, 0, 0, 0);        		
        	   sprintf(msg, __PROGRAM__ " pcm_get_string_pin_conf: error converting poid for %s %s ", program,  token );
        	   pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
        
            
		       PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );     
			   return NULL;
			}
			
			PyDict_SetItemString(dict, "db", PyLong_FromLong( (int64) PIN_POID_GET_DB(poid_p) ));
			PyDict_SetItemString(dict, "type", PyUnicode_FromString((char *)PIN_POID_GET_TYPE(poid_p)) );			
			PyDict_SetItemString(dict, "id", PyLong_FromLong( (pin_poid_id_t) PIN_POID_GET_ID(poid_p) ));
			
			
			PIN_POID_DESTROY(poid_p, NULL);

			return dict;
		}		
	}
	else
	{		
		sprintf(msg, __PROGRAM__" pcm_pin_conf: invalid type data - %s ", type );
		pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
			
			
		PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
		return NULL;
	}

	return NULL;

}





int pcm_get_string_pin_conf(char *program, char *token, char **retorno )
{
	char		*c_ptr = (char *)NULL;	
	int32		err;
	char 		buffer[BUFSIZ+1];

	memset(buffer, 0, BUFSIZ+1);
	
	pin_conf(program, token, PIN_FLDT_STR, (caddr_t *)&(c_ptr), &((EBUFP)->pin_err) );
	if (c_ptr != (char *)NULL) 
	{		
		
		pin_strlcpy(buffer, c_ptr, sizeof(buffer));
		
		*retorno = (char *) malloc (strlen(buffer) + 1);
		strcpy(*retorno, buffer);

		pin_free(c_ptr);
		
		c_ptr = (char *)NULL;
	}
	else
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_WARNING, __PROGRAM__" pcm_get_string_pin_conf: no value found", EBUFP);
		
        PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_NOT_FOUND, 0, 0, 0);        		
        sprintf(buffer, __PROGRAM__ " pcm_get_string_pin_conf: no value found for %s %s ", program,  token );
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , buffer );
        fprintf(stderr, pcm_dump_ebuf(EBUFP, buffer ) );                
            
        PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, buffer ) );     
		return -1;
	}

	
	return 0;
}



int pcm_get_decimal_pin_conf(char *program, char *token, long double *retorno )
{
	char		*c_ptr = (char *)NULL;	
	int32		err;
	char 		buffer[BUFSIZ+1];

	memset(buffer, 0, BUFSIZ+1);
	
	pin_conf(program, token, PIN_FLDT_STR, (caddr_t *)&(c_ptr), &((EBUFP)->pin_err) );
	if (c_ptr != (char *)NULL) 
	{		
		
		pin_strlcpy(buffer, c_ptr, sizeof(buffer));
		
		*retorno = atof(buffer);

		
		pin_free(c_ptr);
		
		c_ptr = (char *)NULL;
	}
	else
	{		
        PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_NOT_FOUND, 0, 0, 0);        		
        sprintf(buffer, __PROGRAM__ " pcm_get_decimal_pin_conf: no value found for %s : %s\n", program, token);	
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , buffer );
        fprintf(stderr, pcm_dump_ebuf(EBUFP, buffer ) );                
            
        PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, buffer ) );     
		return -1;
	}

	
	return 0;
}



int pcm_get_poid_pin_conf(char *program, char *token, char **retorno )
{
	char		*c_ptr = (char *)NULL;
	int32		err;
	char 		buffer[BUFSIZ+1];

	memset(buffer, 0, BUFSIZ+1);
	
	pin_conf(program, token, PIN_FLDT_STR, (caddr_t *)&(c_ptr), &((EBUFP)->pin_err) );
	if (c_ptr != (char *)NULL) 
	{		
		
		pin_strlcpy(buffer, c_ptr, sizeof(buffer));
		
		*retorno = (char *) malloc (strlen(buffer) + 1);
		strcpy(*retorno, buffer);

		pin_free(c_ptr);
		
		c_ptr = (char *)NULL;
	}
	else
	{		
        PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_NOT_FOUND, 0, 0, 0);        		
        sprintf(buffer, __PROGRAM__" pcm_get_poid_pin_conf: no value found for %s : %s\n", program, token);		
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , buffer );
        fprintf(stderr, pcm_dump_ebuf(EBUFP, buffer ) );                
            
        PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, buffer ) );     
		return -1;
	}

	
	return 0;
}


int pcm_get_int_pin_conf(char *program, char *token, int *retorno  )
{
	char		logfile[256];
	int32			*value_p;
	
	char		*c_ptr = (char *)NULL;
	int32		err;
    char 		buffer[BUFSIZ+1];

	
	pin_conf(program, token, PIN_FLDT_INT, (caddr_t *)&value_p, &((EBUFP)->pin_err) );
	if (value_p != (int32 *)NULL) {
		*retorno = *value_p;
		free(value_p);
	}
	else
	{		
        PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_NOT_FOUND, 0, 0, 0);        		
        sprintf(buffer, __PROGRAM__" pcm_get_int_pin_conf: no value found for %s : %s\n", program, token);		
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , buffer );
        fprintf(stderr, pcm_dump_ebuf(EBUFP, buffer ) );                
            
        PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, buffer ) );     
		return -1;
	}

	return 0;
	
	
}
