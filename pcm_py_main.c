/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  PCMPy - Lib
  Python Extension for  Oracle BRM
  Copyright Cassio Jemael 2024
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */



#include <Python.h>
#include "pcm_py.h"

#include <libxml/parser.h>
#include <libxml/tree.h>



PyObject * pcm_execute_op(PyObject * self , PyObject *args)
{
	
    pin_flist_t    *ret_flist = NULL;
    pin_flist_t    *in_flistpp = NULL;

    char           *in_flist_str = NULL;   
    PyObject       *in_flist_obj = NULL;
    char                 msg[BUFSIZ * 5];
    char            *out_flist = NULL;
    char            *out_xml = NULL;
    int             len = 0;

    int                 opcode;
    int                 flag;
    xml_t xml_buf;
	// char *retorno;
    

	PIN_ERR_CLEAR_ERR(EBUFP);
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP,  __PROGRAM__ "pcm_op called.");

    // check args
    if (!PyArg_ParseTuple(args, "iOi", &opcode, &in_flist_obj, &flag)) 
    {
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);

        // retrieve error description
        PyObject *str_exc_value = PyObject_Str(value);
        const char *error_message = PyString_AsString(str_exc_value);
        
        sprintf(msg, "%s : %s \n",  __PROGRAM__ " pcm_op parameter error", error_message);
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

    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP,  __PROGRAM__ "pcm_op opcode %d flag %d and flist  ", opcode, flag);


    PyObject *nargs = PyTuple_New(1);    
    PyTuple_SetItem(nargs, 0, PyLong_FromLong(ARRAY_CONN[keyConn].DATABASE ) );
    if ( connect_cm(self, nargs ) == NULL)
        return NULL;
    Py_DECREF(nargs);

    
     // check if argument is str
    if (PyString_Check(in_flist_obj)) 
    {        
        in_flist_str = PyString_AsString(in_flist_obj);
        if (!in_flist_str) 
        {          

            sprintf(msg, "%s \n",  __PROGRAM__ " pcm_op(): failed to convert string");
            PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_CONV_UNICODE, 0, 0, 0);        
            pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP,  pcm_dump_ebuf(EBUFP, msg) );

            PyErr_SetString(PCMPyError, msg );
            return NULL;
        }

        // Processa in_flist_str como uma string        
        pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP,  __PROGRAM__ "pcm_op opcode %d flag %d and flist string %s ", opcode, flag, in_flist_str);
        PIN_STR_TO_FLIST(in_flist_str, ARRAY_CONN[keyConn].DATABASE , &in_flistpp, EBUFP);
    } 
    // Verifica se o segundo argumento é um dicionário
    else if (PyDict_Check(in_flist_obj)) 
    {
        // Converte o dicionário para flist
        in_flistpp = PIN_FLIST_CREATE(EBUFP);


        if (pcm_convert_array(in_flist_obj, 0, &in_flistpp) <0) return NULL;
    } 
    // Argumento não é um tipo esperado
    else 
    {        
        sprintf(msg, "%s \n",  __PROGRAM__ " pcm_op(): Second argument must be a string or dictionary");
        PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_CONV_UNICODE, 0, 0, 0);        
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP,  pcm_dump_ebuf(EBUFP, msg) );

        PyErr_SetString(PCMPyError, msg );
        return NULL;
    }
    

    

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_op flist in: ",in_flistpp);
    if( PIN_ERR_IS_ERR( EBUFP ))
	{   
        sprintf(msg, __PROGRAM__ " Error converting input flist ");
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
		fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );
			
            
        PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
        return NULL;

    }
	

	PCM_OP( ARRAY_CONN[keyConn].CTXP , opcode, flag, in_flistpp, &ret_flist, EBUFP);
	if( PIN_ERR_IS_ERR( EBUFP ))
	{
		sprintf(msg, __PROGRAM__ " Error executing opcode %d, flag %d for flist ", opcode, flag);
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
		fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );

        if (in_flistpp != NULL)    
	        PIN_FLIST_DESTROY_EX(&in_flistpp, EBUFP);
        	
            
        PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
        return NULL;
	}

    if (PCM_PY_LOG_LEVEL >= PIN_ERR_LEVEL_DEBUG)
    {
        PIN_FLIST_TO_STR(ret_flist, &out_flist, &len, EBUFP);
        pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP, __PROGRAM__" opcode flist out: %s", out_flist);
        free(out_flist);
        out_flist = NULL;
    }

    PIN_FLIST_TO_XML( ret_flist, PIN_XML_BY_NAME, 0, &xml_buf.strp, &xml_buf.strsize, "root", EBUFP );
	if( PIN_ERR_IS_ERR( EBUFP ))
	{   
        sprintf(msg, __PROGRAM__ " Error converting xml for output flist ");
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
		fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );

        if (ret_flist != NULL)
            PIN_FLIST_DESTROY_EX(&ret_flist, EBUFP);
        if (in_flistpp != NULL)    
	        PIN_FLIST_DESTROY_EX(&in_flistpp, EBUFP);
            
        PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
        return NULL;

    }
    
    
   

	out_xml = (char *) calloc ( (xml_buf.strsize + 1), sizeof(char));
	memset(out_xml, 0, (xml_buf.strsize + 1));
	strncpy(out_xml, xml_buf.strp, xml_buf.strsize);


    if (ret_flist != NULL)
        PIN_FLIST_DESTROY_EX(&ret_flist, EBUFP);
    if (in_flistpp != NULL)    
        PIN_FLIST_DESTROY_EX(&in_flistpp, EBUFP);

	return Py_BuildValue("s", out_xml);

}




PyObject * connect_cm(PyObject * self ,PyObject *args) 
{
    PIN_ERR_SET_PROGRAM(__PROGRAM__);
    char msg [BUFSIZ *2];
    int64       db=1;
    
    PIN_ERR_CLEAR_ERR(EBUFP); 
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , __PROGRAM__" pcm_connect: function called" );

    
    // check args
    if (!PyArg_ParseTuple(args, "|l", &db)) 
    {
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);

        // retrieve error description
        PyObject *str_exc_value = PyObject_Str(value);
        const char *error_message = PyString_AsString(str_exc_value);
        
        sprintf(msg, "%s : %s \n",  __PROGRAM__ " pcm_op parameter error", error_message);
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
    if ( db <=0 ) db = 1;

	/***********************************************************
	* Open the database context 
	***********************************************************/
    ARRAY_CONN[keyConn].DATABASE = db;
    sprintf(msg, __PROGRAM__  "Connecting context %d database %lld \n\n", keyConn,  ARRAY_CONN[keyConn].DATABASE);
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , msg);
	ARRAY_CONN[keyConn].test=0;
    PIN_ERR_CLEAR_ERR(EBUFP); 
	if ( ARRAY_CONN[keyConn].CTXP == NULL)
	{        
		PCM_CONNECT(&(ARRAY_CONN[keyConn]).CTXP, &(ARRAY_CONN[keyConn]).DATABASE, EBUFP);
		if( PIN_ERR_IS_ERR( EBUFP ))
		{			
			pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , __PROGRAM__"  pcm_connect: error connection" );
			fprintf(stderr, pcm_dump_ebuf(EBUFP, __PROGRAM__"  pcm_connect: error connection" ) );
			
            
            PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, __PROGRAM__"  pcm_connect: error connection" ) );      
            Py_RETURN_FALSE;
            
		}
		ARRAY_CONN[keyConn].test=1;
	}
	

	if( PIN_ERR_IS_ERR( EBUFP ))
	{		
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , __PROGRAM__"  pcm_connect: error connection" );
		Py_RETURN_FALSE;
	}
   
    Py_RETURN_TRUE;
}


PyObject * set_context(PyObject * self, PyObject *args)
{
    int key;
    char msg[BUFSIZ *4];

    // check args
    if (!PyArg_ParseTuple(args, "i", &key)) 
    {
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);

        // retrieve error description
        PyObject *str_exc_value = PyObject_Str(value);
        const char *error_message = PyString_AsString(str_exc_value);
        
        sprintf(msg, "%s : %s \n",  __PROGRAM__ " pcm_set_context parameter error", error_message);
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


	/***********************************************************
	* Open the database context
	***********************************************************/	
	if ( (key < 0) || 
		(key > (MAX_CONTEXT_CONN-1) ) 
	)
	{
		sprintf(msg, __PROGRAM__" pcm_set_context invalid value :%d \n Values must be between 0 and %d", key, (MAX_CONTEXT_CONN-1));
		PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_ARG, 0, 0, 0);
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP,  pcm_dump_ebuf(EBUFP, msg) );

        PyErr_SetString(PCMPyError, msg );

		return NULL;
        
	}

    keyConn = key;
	
    

    PyObject *nargs = PyTuple_New(1);    
    PyTuple_SetItem(nargs, 0, PyLong_FromLong(ARRAY_CONN[keyConn].DATABASE ) );
    connect_cm(self, nargs );
    Py_DECREF(nargs);
		
	return Py_BuildValue("i", key);

    

}



int pcm_convert_array(PyObject *data, int depth, pin_flist_t **in_flistpp) 
{
    PyObject *key, *value;
    Py_ssize_t pos = 0;
    PyObject *repr;
    char chave[BUFSIZ];
    char *valor;
    int len = 0;
    char msg[BUFSIZ];
    int i;
    Py_ssize_t is;

    field_spec  *fields;


    if (PyDict_Check(data)) 
    {        
        // Percorre o dicionário
        
        while (PyDict_Next(data, &pos, &key, &value)) {
            // Obter a representação da chave como string
            
            repr = PyObject_Repr(key);
            sprintf(chave , "%s",  PyString_AsString(repr) ) ;            
            remove_single_quotes(chave);
            Py_XDECREF(repr);

            

            fields = (field_spec *) malloc(sizeof(field_spec));
            fields->name=NULL;
            fields->descr=NULL;
            fields->type=NULL;
            if (pcm_array_find_custom_field( chave, &CUSTOM_FLD_OP_TABLE, &fields) == -1)
            {	
                if (getFieldSpec(chave, &fields) == -1)
                {	
                    PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_NOT_FOUND, 0, 0, 0);        		
                    sprintf(msg, __PROGRAM__ " pcm_op(): field not found  %s ", chave );
                    pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
                    fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );                
                        
                    PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
                    return -1;                
                }		
            }
            
            if (strcmp(fields->type, "POID")==0)
            {
                poid_t   *auxPoid = NULL;

                repr = PyObject_Repr(value);
                
                len = (sizeof(char) * strlen(PyString_AsString(repr)) + 1);
                valor = (char *) malloc(len);
                memset(valor, 0, len);
                sprintf(valor, "%s", PyString_AsString(repr));
                remove_single_quotes(valor);
                Py_XDECREF(repr);
              

                pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , __PROGRAM__" pcm_op(): poid to convert %s", valor );

                auxPoid = PIN_POID_FROM_STR( valor, NULL , EBUFP)	;
                if( PIN_ERR_IS_ERR( EBUFP ))
                {     
                    sprintf(msg, __PROGRAM__ " pcm_op(): error converting poid  %s ", valor );
                    PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_UNKNOWN_POID, 0, 0, 0);
                    pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
                    fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );                
                        
                    PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
                    return -1;
                    
                }   

                PIN_FLIST_FLD_SET(*in_flistpp, PIN_MAKE_FLD(  PIN_FLDT_POID, fields->number ), auxPoid , EBUFP);            
                if( PIN_ERR_IS_ERR( EBUFP ))
                {     
                    sprintf(msg, __PROGRAM__ " pcm_op(): setting flist field  %s / %ld : %s ", chave,  fields->number, valor );
                    PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_UNKNOWN_POID, 0, 0, 0);
                    pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
                    fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );                
                        
                    PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
                    return -1;              
                    
                }            
                
                free(valor);
                valor = NULL;
                
            }
            else if (strcmp(fields->type, "STR")==0)
            {
                repr = PyObject_Repr(value);
                
                len = (sizeof(char) * strlen(PyString_AsString(repr)) + 1);
                valor = (char *) malloc(len);
                memset(valor, 0, len);
                sprintf(valor, "%s", PyString_AsString(repr));
                remove_single_quotes(valor);
                Py_XDECREF(repr);
                
                sprintf(msg, __PROGRAM__ " pcm_op: str to flist: %s", valor );
                pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , msg);
                PIN_FLIST_FLD_SET(*in_flistpp, PIN_MAKE_FLD(PIN_FLDT_STR, fields->number) , valor, EBUFP);          

                if( PIN_ERR_IS_ERR( EBUFP ))
                {     
                    sprintf(msg, __PROGRAM__ " pcm_op(): error setting str to flist: %s - %s", chave, valor );
                    PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_ARG, 0, 0, 0);
                    pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
                    fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );                
                        
                    PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
                    return -1;          
                    
                }
            }
            else if (strcmp(fields->type, "INT")==0)
            {

                int32   auxInt = PyLong_AsLong(value);
                
                sprintf(msg, __PROGRAM__ " pcm_op: INT to flist: %d", auxInt );
                pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , msg);
                PIN_FLIST_FLD_SET(*in_flistpp, PIN_MAKE_FLD(PIN_FLDT_INT, fields->number) , &auxInt, EBUFP);          
                if( PIN_ERR_IS_ERR( EBUFP ))
                {     
                    sprintf(msg, __PROGRAM__ " pcm_op(): error setting int to flist: %s - %d", chave, auxInt );
                    PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_ARG, 0, 0, 0);
                    pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
                    fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );                
                        
                    PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
                    return -1;            
                    
                }

            }
            else if (strcmp(fields->type, "ENUM")==0)
            {
                int32   auxInt = PyLong_AsLong(value);
                
                
                sprintf(msg, __PROGRAM__ " pcm_op: ENUM to flist: %d", auxInt );
                pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , msg);
                PIN_FLIST_FLD_SET(*in_flistpp, PIN_MAKE_FLD(PIN_FLDT_ENUM, fields->number) , &auxInt, EBUFP);          

                if( PIN_ERR_IS_ERR( EBUFP ))
                {     
                    sprintf(msg, __PROGRAM__ " pcm_op(): error setting enum to flist: %s - %d", chave, auxInt );
                    PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_ARG, 0, 0, 0);
                    pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
                    fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );                
                        
                    PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
                    return -1;          
                    
                }
                
            }
            else if (strcmp(fields->type, "NUM")==0)
            {
                int32   auxInt = PyLong_AsLong(value);
                
                
                sprintf(msg, __PROGRAM__ " pcm_op: NUM to flist: %d", auxInt );
                pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , msg);
                PIN_FLIST_FLD_SET(*in_flistpp, PIN_MAKE_FLD(PIN_FLDT_NUM, fields->number) , &auxInt, EBUFP);          
                if( PIN_ERR_IS_ERR( EBUFP ))
                {     
                    sprintf(msg, __PROGRAM__ " pcm_op(): error setting num to flist: %s - %d", chave, auxInt );
                    PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_ARG, 0, 0, 0);
                    pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
                    fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );                
                        
                    PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
                    return -1;          
                    
                }
                
            }
            else if (strcmp(fields->type, "TSTAMP")==0)
            {

                int32   auxInt = PyLong_AsLong(value);
                
                
                sprintf(msg, __PROGRAM__ " pcm_op: TSTAMP to flist: %d", auxInt );
                pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , msg);
                PIN_FLIST_FLD_SET(*in_flistpp, PIN_MAKE_FLD(PIN_FLDT_TSTAMP, fields->number) , &auxInt, EBUFP);          
                if( PIN_ERR_IS_ERR( EBUFP ))
                {     
                    sprintf(msg, __PROGRAM__ " pcm_op(): error setting tstamp to flist: %s - %d", chave, auxInt );
                    PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_ARG, 0, 0, 0);
                    pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
                    fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );                
                        
                    PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
                    return -1;        
                    
                }

            }
            else if ( strcmp(fields->type, "DECIMAL") == 0 )
            {          
                
                double  due_double = PyFloat_AsDouble(value);
                long double auxDouble = (long double) due_double;
                

                char aux[128];                        
                pin_decimal_t *value_dec  = NULL;
                

                sprintf(aux, "%.4Lf", (long double) auxDouble   ) ;

                sprintf(msg, __PROGRAM__ " pcm_op: DECIMAL to flist: %s", aux );
                pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , msg);

                value_dec = pbo_decimal_from_str(aux , EBUFP);
                if( PIN_ERR_IS_ERR( EBUFP ))
                {     
                    sprintf(msg, __PROGRAM__ " pcm_op(): error converting decimal value: %s - %s", chave, aux );
                    PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_ARG, 0, 0, 0);
                    pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
                    fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );                
                        
                    PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
                    return -1;        
                    
                }
                
                
                PIN_FLIST_FLD_SET(*in_flistpp, PIN_MAKE_FLD( PIN_FLDT_DECIMAL, fields->number), value_dec , EBUFP);
                 if( PIN_ERR_IS_ERR( EBUFP ))
                {     
                    sprintf(msg, __PROGRAM__ " pcm_op(): error setting decimal to flist: %s - %s", chave, aux );
                    PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_ARG, 0, 0, 0);
                    pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
                    fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );                
                        
                    PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
                    return -1;
                    
                }

                pbo_decimal_destroy(&value_dec);
            }
            else if (strcmp(fields->type, "ARRAY")==0 || strcmp(fields->type, "SUBSTRUCT")==0)
            {
                pin_flist_t       *arr_flistp = NULL;

                arr_flistp = PIN_FLIST_CREATE(EBUFP);

                
                pcm_convert_array(value, depth + 1, &arr_flistp);

                if ( strcmp(fields->type, "ARRAY") == 0)
                    PIN_FLIST_FLD_SET(*in_flistpp, PIN_MAKE_FLD( PIN_FLDT_ARRAY, fields->number), arr_flistp , EBUFP)
                else
                    PIN_FLIST_FLD_SET(*in_flistpp, PIN_MAKE_FLD( PIN_FLDT_SUBSTRUCT, fields->number), arr_flistp , EBUFP);

                if( PIN_ERR_IS_ERR( EBUFP ))
                {     
                    sprintf(msg, __PROGRAM__ " pcm_op(): error setting flist to flist: %s ", chave  );
                    PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_ARG, 0, 0, 0);
                    pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
                    fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );                
                        
                    PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
                    return -1;
                    
                }
            }
            else
            {
                sprintf(msg, __PROGRAM__ " pcm_op(): error field type %s cannot be used in array. Try use string flist ", chave  );
                PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_BAD_ARG, 0, 0, 0);
                pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
                fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );                
                    
                PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
                return -1;
            }
            
        }    
    } else if (PyList_Check(data)) {                
        for (is = 0; is < PyList_Size(data); is++) {
            PyObject *item = PyList_GetItem(data, is);
         
            pcm_convert_array(item, depth + 1, in_flistpp);
            

        }
        
    } else {
        // Para outros tipos de dados
        
        repr = PyObject_Repr(data);
        printf("%s\n", PyString_AsString(repr));
        Py_XDECREF(repr);
    }

    if (fields != NULL && fields->name != NULL)
		free(fields->name);
	if (fields != NULL && fields->descr != NULL)
		free(fields->descr);
	if (fields != NULL && fields->type != NULL)
		free(fields->type);

    if (fields!=NULL) 
        free(fields);

    return 1;
}