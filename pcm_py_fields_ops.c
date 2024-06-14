/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  PCMPy - Lib
  Python Extension for  Oracle BRM
  Copyright Cassio Jemael 2024
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

#include <Python.h>
#include "pcm_py.h"

#include <libxml/parser.h>
#include <libxml/tree.h>


PyObject * pcm_get_custom_opcode(PyObject * self , PyObject *args)
{
    char                *opcode_str;

    char                msg[BUFSIZ * 4];
    PyObject            *field_return = NULL;

    pcm_opcode          *retorno;

    PIN_ERR_CLEAR_ERR(EBUFP);
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP,  __PROGRAM__ "pcm_get_custom_opcode called.");


    // check args
    if (!PyArg_ParseTuple(args, "s", &opcode_str)) 
    {
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);

        // retrieve error description
        PyObject *str_exc_value = PyObject_Str(value);
        const char *error_message = PyString_AsString(str_exc_value);
        
        sprintf(msg, "%s : %s \n",  __PROGRAM__ " pcm_get_custom_opcode parameter error", error_message);
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
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , __PROGRAM__ " pcm_get_custom_opcode for  %s ", opcode_str );   

    retorno = pcm_opcode_search(opcodeList, globalSizeCustomOps, opcode_str);
	if (retorno==NULL)
	{        
		PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_NOT_FOUND, 0, 0, 0);        		
        sprintf(msg, __PROGRAM__ " pcm_get_custom_opcode: opcode not found  %s ", opcode_str );
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
        fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );                
            
        PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
        return NULL;
	}	
    return Py_BuildValue("l", retorno->number);
}


PyObject * pcm_get_field_spec(PyObject * self , PyObject *args)
{
    char                *field_str = NULL;
    field_spec          *fields = NULL;

    char                msg[BUFSIZ * 4];
    PyObject            *field_return = NULL;


    PIN_ERR_CLEAR_ERR(EBUFP);
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP,  __PROGRAM__ "pcm_get_field_spec called.");

    // check args
    if (!PyArg_ParseTuple(args, "s", &field_str)) 
    {
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);

        // retrieve error description
        PyObject *str_exc_value = PyObject_Str(value);
        const char *error_message = PyString_AsString(str_exc_value);
        
        sprintf(msg, "%s : %s \n",  __PROGRAM__ " pcm_get_field_spec parameter error", error_message);
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
    pcm_log_message(PIN_ERR_LEVEL_DEBUG, EBUFP , __PROGRAM__ " pcm_get_field_spec for  %s ", field_str );   

    

    fields = (field_spec *) malloc(sizeof(field_spec));
	fields->name=NULL;
	fields->descr=NULL;
	fields->type=NULL;
    if (pcm_array_find_custom_field( field_str, &CUSTOM_FLD_OP_TABLE, &fields) == -1)
	{	
		if (getFieldSpec(field_str, &fields) == -1)
		{	
            PIN_SET_ERR(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_NOT_FOUND, 0, 0, 0);        		
            sprintf(msg, __PROGRAM__ " pcm_get_field_spec: field not found  %s ", field_str );
            pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
            fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );                
                
            PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );      
            return NULL;

		
		}		
	}
    

    if (fields->type != NULL)
	{        
        field_return = field_spec_to_pyobject(fields);
        
        if (!field_return) {
            Py_DECREF(field_return);
            return NULL;
        }

        
        if (fields != NULL && fields->name != NULL)
		free(fields->name);
        if (fields != NULL && fields->descr != NULL)
            free(fields->descr);
        if (fields != NULL && fields->type != NULL)
            free(fields->type);

        
        free(fields);

        return field_return;
    }

    if (fields != NULL && fields->name != NULL)
		free(fields->name);
	if (fields != NULL && fields->descr != NULL)
		free(fields->descr);
	if (fields != NULL && fields->type != NULL)
		free(fields->type);

    if (fields!=NULL) 
        free(fields);

    return NULL;

}





/*======================================================================================================================

FUNCOES PRINCIPAIS

======================================================================================================================*/


//long pcm_load_and_sort_field_spec(char **tabela, pcm_opcode **opcodeList, int *totField, int *totOpcode)
long pcm_load_and_sort_field_spec( PyObject * m )
{
	int i =0;
	long size = 0;
    long tam = 0;
	FILE *file = NULL;
	char msg[BUFSIZ];
    char aux[BUFSIZ];
    char *PosI, *PosF;
    char *filename;

    char information[256];
    char value[256];
    
    
    globalSizeCustomFields = 0;
    globalSizeCustomOps = 0;


    if (pcm_get_string_pin_conf(__PROGRAM__, "py_ops_fields_extension_file", &filename ) == -1)
    {
        sprintf(msg, __PROGRAM__" Error getting information custom fields and opcodes file from pin.conf  - %s  py_ops_fields_extension_file\n", __PROGRAM__);
    	//fprintf(stderr, msg);
		//pin_set_err(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, (EBUFP)->pin_err, 0, 0, 0);
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_WARNING, msg , EBUFP);
		return -1;        
    }
    PIN_ERR_CLEAR_ERR(EBUFP); 

    file = fopen(filename, "r");
    if (file == NULL) {
    	sprintf(msg, __PROGRAM__" Error opening file %s: %s\n", filename, strerror(errno));
    	fprintf(stderr, msg);
		pin_set_err(EBUFP, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, errno, 0, 0, 0);
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , EBUFP);
		return -1;
	}

    size = floor(pcm_get_file_size(file)/(PCM_SIZE_LINE_CUSTOM_FIELD + 1)); //+1 bcz \n
    tam  = sizeof(char) * size * PCM_SIZE_LINE_CUSTOM_FIELD ;

    

    CUSTOM_FLD_OP_TABLE = (char *) malloc( tam + 1 );    
    memset(CUSTOM_FLD_OP_TABLE, 0, (tam + 1) );    
    
    
    

    // Carrega os dados do arquivo CSV para a array
    while (1) {
        char line[BUFSIZ];    
        if (fgets(line, sizeof(line), file) != NULL)
        {
            if (line[PCM_SIZE_LINE_CUSTOM_FIELD-2]=='F')
            {
                globalSizeCustomFields ++;
                strncat(CUSTOM_FLD_OP_TABLE, line, PCM_SIZE_LINE_CUSTOM_FIELD);                                
            }
            else
            {
                if (globalSizeCustomOps == 0)
                    opcodeList = (pcm_opcode *) malloc (sizeof(pcm_opcode));
                else
                    opcodeList  = (pcm_opcode *) realloc(opcodeList, (globalSizeCustomOps+1) * sizeof(pcm_opcode));

                memset(aux, 0, BUFSIZ);
                sprintf((opcodeList)[globalSizeCustomOps].opcode, "%-60.60s", line);
                rtrim((opcodeList)[globalSizeCustomOps].opcode);

                sprintf(aux, "%-6.6s", line + 71);
                (opcodeList)[globalSizeCustomOps].number = atol(aux);
                
                globalSizeCustomOps++;
            }
            sprintf(information, "%-60.60s", line);
            rtrim(information);

            sprintf(value, "%-60.60s", line + 71);
            rtrim(value);

            PyModule_AddIntConstant(m, information, atoi(value) );
        }
        else
            break;

        
    }
    
    fclose(file);

    

    return 0;
}


int compare_opcodes(const void *a, const void *b) {
    return strcmp(((pcm_opcode*)a)->opcode, ((pcm_opcode*)b)->opcode);
}


pcm_opcode *pcm_opcode_search(pcm_opcode *array, int size, const char *key) 
{
    int left = 0;
    int right = size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(array[mid].opcode, key);

        
        if (cmp == 0) {
            return &array[mid];
        } else if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return NULL; // Elemento não encontrado
}

int getFieldSpec(char *fieldName, field_spec **fields)
{
	char *retorno = NULL;

	pin_flist_t	*s_flistp = NULL;
	pin_flist_t	*fld_flistp = NULL;
	pin_flist_t	*r_flistp = NULL;
	poid_t		*search_poid;
	void		*vp = NULL;

	int 		*fieldType = NULL;
	int 		*fieldNumber = NULL;
	int 		*status = NULL;
	char		*descr = NULL;
	char        msg[BUFSIZ *  4];

	/*
r << XX 3
0 PIN_FLD_POID           POID [0] 0.0.0.1 /dd/fields -1 0
0 PIN_FLD_FIELD           ARRAY [0] allocated 20, used 6
1     PIN_FLD_FIELD_NUM         ENUM [0] 0
1     PIN_FLD_FIELD_NAME            STR [0] "PIN_FLD_DUE_T"
XX
xop PCM_OP_SDK_GET_FLD_SPECS 0 3
*/

	
	
	PIN_ERR_CLEAR_ERR(EBUFP);
	

	if ( ARRAY_CONN[keyConn].CTXP == NULL)
	{
		PCM_CONNECT(&(ARRAY_CONN[keyConn]).CTXP, &(ARRAY_CONN[keyConn]).DATABASE, EBUFP);
		if( PIN_ERR_IS_ERR( EBUFP ))
		{			
			pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , __PROGRAM__"  pcm_get_field_spec: error connection" );
			fprintf(stderr, pcm_dump_ebuf(EBUFP, __PROGRAM__"  pcm_get_field_spec: error connection" ) );
			
            
            PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, __PROGRAM__"  pcm_get_field_spec: error connection" ) );      
            return -1;
            
		}
		ARRAY_CONN[keyConn].test=1;
	}
	
	
	s_flistp = PIN_FLIST_CREATE(EBUFP);

	search_poid = PIN_POID_CREATE(  ARRAY_CONN[keyConn].DATABASE  , "/dd/fields", (int64)-1, EBUFP);

	PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, (void *)search_poid, EBUFP);
	
	
	fld_flistp = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_FIELD, 0, EBUFP);	
	PIN_FLIST_FLD_SET(fld_flistp, PIN_FLD_FIELD_NUM, (void *) vp, EBUFP);
	PIN_FLIST_FLD_SET(fld_flistp, PIN_FLD_FIELD_NAME, (char *) fieldName, EBUFP);



	PCM_OP( ARRAY_CONN[keyConn].CTXP , PCM_OP_SDK_GET_FLD_SPECS, 0, s_flistp, &r_flistp, EBUFP);

	if( PIN_ERR_IS_ERR( EBUFP ))
	{        
        sprintf(msg, __PROGRAM__ " getFieldSpec error getting field information for %s ", fieldName );
        pcm_log_message(PIN_ERR_LEVEL_ERROR, EBUFP , msg );
		fprintf(stderr, pcm_dump_ebuf(EBUFP, msg ) );
			
            
        PyErr_SetString(PCMPyError, pcm_dump_ebuf(EBUFP, msg ) );              
        PIN_FLIST_DESTROY_EX(&s_flistp, NULL);
	    PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
		return(-1);
	}
	

	// gettingg fields
	fld_flistp = PIN_FLIST_ELEM_GET(r_flistp, PIN_FLD_FIELD, 0, 0, EBUFP);
	
	if (fld_flistp != NULL)
	{		
		
		fieldType = PIN_FLIST_FLD_GET(fld_flistp, PIN_FLD_FIELD_TYPE, 1, EBUFP);
		(*fields)->type = (char *) calloc ( (strlen(PCM_FIELD_TYPE[*fieldType]) + 1), sizeof(char));

		sprintf((*fields)->type, "%s", PCM_FIELD_TYPE[*fieldType]);
		(*fields)->numberType = *fieldType;

		(*fields)->number = 0;
		fieldNumber = PIN_FLIST_FLD_GET(fld_flistp, PIN_FLD_FIELD_NUM, 1, EBUFP);
		if (fieldNumber!=NULL)
			(*fields)->number = *fieldNumber;

		(*fields)->status = 0;
		status = PIN_FLIST_FLD_GET(fld_flistp, PIN_FLD_STATUS, 1, EBUFP);
		if (status!=NULL)
			(*fields)->status = *status;

		(*fields)->descr = NULL;
		descr = PIN_FLIST_FLD_GET(fld_flistp, PIN_FLD_DESCR, 1, EBUFP);
		if (descr!=NULL)
		{
			(*fields)->descr = (char *) calloc ( (strlen(descr) + 1), sizeof(char));
			sprintf((*fields)->descr, "%s", descr);
		}
		(*fields)->name = NULL;
		
	}
	else
	{
		(*fields)->name = NULL;
		(*fields)->type = NULL;
		(*fields)->descr = NULL;
	}

	PIN_FLIST_DESTROY_EX(&s_flistp, NULL);
	PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
	
	return ( 0);


}


long  int pcm_array_find_custom_field( char *key, char **tabela,  field_spec **fields)
{
    long left = 0;
    long right = globalSizeCustomFields - 1;
    char aux[BUFSIZ];
    field_spec *prov;
    long int index =0 ;

    



    sprintf(aux, "%-60.60s", key);    
    if ( (index = findReg(*tabela, aux, 0, globalSizeCustomFields, PCM_SIZE_LINE_CUSTOM_FIELD, 60 )  ) >= 0)
    {
        int indReg = 0;

        

        // getting register type (last field)
        sprintf(aux, "%-1.1s", *tabela + index + (PCM_SIZE_LINE_CUSTOM_FIELD-2));        
        (*fields)->typeInformaton = aux[0];

        // getting name
        sprintf(aux, "%-60.60s", *tabela + index);
        rtrim(aux);
        (*fields)->name = (char *) calloc ( (strlen(aux) + 1), sizeof(char));
        sprintf((*fields)->name, "%s", aux);
        indReg +=61;

        

        sprintf(aux, "%-9.9s", *tabela + index + indReg);
        indReg+=10;

        

        if ((*fields)->typeInformaton == 'F')
        {
            rtrim(aux);
            (*fields)->type = (char *) calloc ( (strlen(aux) + 1), sizeof(char));
            memset((*fields)->type, 0, (strlen(aux) + 1));
            strcpy((*fields)->type, aux);

            (*fields)->numberType = -1;
            if (strcmp("UNUSED", aux) == 0)
                (*fields)->numberType = 0;
            if (strcmp("INT", aux) == 0)
                (*fields)->numberType = 1;
            if (strcmp("UINT", aux) == 0)
                (*fields)->numberType = 2;
            if (strcmp("ENUM", aux) == 0)
                (*fields)->numberType = 3;
            if (strcmp("NUM", aux) == 0)
                (*fields)->numberType = 4;
            if (strcmp("STR", aux) == 0)
                (*fields)->numberType = 5;
            if (strcmp("BUF", aux) == 0)
                (*fields)->numberType = 6;
            if (strcmp("POID", aux) == 0)
                (*fields)->numberType = 7;
            if (strcmp("TSTAMP", aux) == 0)
                (*fields)->numberType = 8;
            if (strcmp("ARRAY", aux) == 0)
                (*fields)->numberType = 9;
            if (strcmp("SUBSTRUCT", aux) == 0)
                (*fields)->numberType = 10;
            if (strcmp("OBJ", aux) == 0)
                (*fields)->numberType = 11;
            if (strcmp("BINSTR", aux) == 0)
                (*fields)->numberType = 12;
            if (strcmp("ERR", aux) == 0)
                (*fields)->numberType = 13;
            if (strcmp("DECIMAL", aux) == 0)
                (*fields)->numberType = 14;
            if (strcmp("TIME", aux) == 0)
                (*fields)->numberType = 15;
            if (strcmp("TEXTBUF", aux) == 0)
                (*fields)->numberType = 16;
            if (strcmp("ERRBUF", aux) == 0)
                (*fields)->numberType = 13;
        }    

        

        sprintf(aux, "%-6.6s", *tabela + index + indReg);
        indReg+=7;
        rtrim(aux);
        (*fields)->number = atol(aux);

        (*fields)->status = 3;
        (*fields)->descr=NULL;
        return index;
    } 

    return (-1); // Registro não encontrado
}


PyObject* field_spec_to_pyobject(field_spec *field) 
{
    PyObject *dict = PyDict_New();

    if (!dict) {
        return NULL;
    }

    if (field->descr != NULL)
        PyDict_SetItemString(dict, "descr", PyUnicode_FromString(field->descr));
    else
        PyDict_SetItemString(dict, "descr", PyUnicode_FromString(""));

    if (field->type != NULL)
        PyDict_SetItemString(dict, "type", PyUnicode_FromString(field->type));
    else
        PyDict_SetItemString(dict, "type", PyUnicode_FromString(""));

    PyDict_SetItemString(dict, "number", PyLong_FromLong(field->number));
    PyDict_SetItemString(dict, "numberType", PyLong_FromLong(field->numberType));
    PyDict_SetItemString(dict, "status", PyLong_FromLong(field->status));

    return dict;
}