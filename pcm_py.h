/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  PCMPy - Lib
  Python Extension for  Oracle BRM
  Copyright Cassio Jemael 2024
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


#ifndef __PCP_PY_H__
#define __PCP_PY_H__


#define __PROGRAM__ "PCMPy"

#include <Python.h>

#include "pcm_defines.h"


typedef struct
{
	pcm_context_t	*CTXP;
	pin_db_no_t		DATABASE;
	int				test;	
	pin_flist_t	*trans_flistp;	
} PCM_ARRAY_CONN;

typedef struct
{
	char *name;
	char *descr;
	char *type;
	long number;
	int numberType;
	int status;	
	char  typeInformaton;
} field_spec ;

typedef struct {
	int32 chunksize;
	char *strp;
	int32 size;
	int32 strsize;
} xml_t;

typedef struct
{
	char opcode[61];
	long number;	
} pcm_opcode ;


static const char *const PCM_FIELD_TYPE[] = {
    "",
    "INT",
    "UINT",
	"ENUM",
	"NUM",
	"STR",
	"BUF",
	"POID",
	"TSTAMP",
	"ARRAY",
	"SUBSTRUCT",
	"OBJ",
	"BINSTR",
	"ERR",
	"DECIMAL",
	"TIME",
	"TEXTBUF",
	"ERR"
};


#define MAX_CONTEXT_CONN 16
#define MAX_DEPTH_FLIST 64

#define PCM_SIZE_LINE_CUSTOM_FIELD  80


// pcm_py_main.c
PyObject * connect_cm(PyObject * self ,PyObject *args);
PyObject * set_context(PyObject * self, PyObject *args);
PyObject * pcm_execute_op(PyObject * self , PyObject *args);
int pcm_convert_array(PyObject *data, int depth, pin_flist_t **in_flistpp) ;

// pcm_py_main.c
PyObject * pcm_create_poid(PyObject * self , PyObject *args);
PyObject * pcm_get_poid_number(PyObject * self , PyObject *args);
PyObject * pcm_get_poid_type(PyObject * self , PyObject *args);
PyObject * pcm_get_poid_db(PyObject * self , PyObject *args);

//pcm_py_fields_ops.c
PyObject * pcm_get_field_spec(PyObject * self , PyObject *args);
PyObject * pcm_get_custom_opcode(PyObject * self , PyObject *args);
long  int pcm_array_find_custom_field( char *key, char **tabela,  field_spec **fields);
int getFieldSpec(char *fieldName, field_spec **fields);
PyObject* field_spec_to_pyobject(field_spec *field) ;
long pcm_load_and_sort_field_spec( PyObject * m );
pcm_opcode *pcm_opcode_search(pcm_opcode *array, int size, const char *key);

// pcm_py_trnasactions.c
PyObject * pcm_open_transaction(PyObject * self , PyObject *args);
PyObject * pcm_commit_transaction(PyObject * self );
PyObject * pcm_abort_transaction(PyObject * self );

// pcm_py_constants
void pcm_init_constants(PyObject * m);

// pcm_py_pinconf.c
PyObject * pcm_pin_conf(PyObject * self , PyObject *args);
int pcm_get_string_pin_conf(char *program, char *token, char **retorno );
int pcm_get_decimal_pin_conf(char *program, char *token, long double *retorno );
int pcm_get_poid_pin_conf(char *program, char *token, char **retorno );
int pcm_get_int_pin_conf(char *program, char *token, int *retorno  );



// pcm_py_log.c
void pcm_read_pinconf(void);
int pcm_init_log(void);
void pcm_log_message(int level, pin_errbuf_t *errbuf, const char *format, ...);
char *pcm_dump_ebuf(pin_errbuf_t *errbuf, char *line );

//pcm_py_functions.c
void ltrim( char *s );
void rtrim( char *s );
void alltrim( char *s );
long pcm_get_field(void);
long int findReg(char *source, char *item,unsigned long start,long end, int regLen, int itemLen);
long pcm_get_file_size(FILE *file);
void remove_single_quotes(char *str) ;


static char   PCM_PY_LOG_FILE[BUFSIZ];
static FILE   *PCM_PY_LOG_FILE_POINTER;
static int    PCM_PY_LOG_LEVEL;

PyObject *PCMPyError;



PCM_ARRAY_CONN ARRAY_CONN[MAX_CONTEXT_CONN];

char        *CUSTOM_FLD_OP_TABLE;
static long globalSizeCustomFields;

static pcm_opcode  *opcodeList;
static long globalSizeCustomOps;



pin_errbuf_t	    *EBUFP;
static int keyConn;


#endif