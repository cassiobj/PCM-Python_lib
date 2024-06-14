/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  PCMPy - Lib
  Python Extension for  Oracle BRM
  Copyright Cassio Jemael 2024
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


#include <Python.h>
#include "pcm_py.h"





int pcm_init_log(void)
{
    
    
    if ( (PCM_PY_LOG_FILE_POINTER = fopen(PCM_PY_LOG_FILE, "a")) == NULL)
    {
        fprintf(stderr, __PROGRAM__ " Error opening %s: %s\n", PCM_PY_LOG_FILE, strerror(errno));
        return -1;
    }
    
    
    return 0;
}


void pcm_log_message(int level, pin_errbuf_t *errbuf, const char *format, ...) {

    if (PCM_PY_LOG_FILE_POINTER == NULL) {
        fprintf(stderr, "Log file is not initialized.\n");
        return;
    }

    // Obter a hora atual
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[20];
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Imprimir o nível de log
    const char *level_str = "";
    switch (level) {
        case PIN_ERR_LEVEL_DEBUG:
            level_str = "DEBUG";
            break;
        case PIN_ERR_LEVEL_WARNING:
            level_str = "WARNING";
            break;
        case PIN_ERR_LEVEL_ERROR:
            level_str = "ERROR";
            break;
    }

    // Imprimir a mensagem de log principal
    va_list args;
    va_start(args, format);
    fprintf(PCM_PY_LOG_FILE_POINTER, "%s|%s| ", time_str, level_str);
    vfprintf(PCM_PY_LOG_FILE_POINTER, format, args);
    fprintf(PCM_PY_LOG_FILE_POINTER, "\n");
    va_end(args);

    // Se houver um errbuf, imprimir suas informações também
    if (errbuf->pin_err != 0)  {
        fprintf(PCM_PY_LOG_FILE_POINTER, "  Error Location: %d\n", errbuf->location);
        fprintf(PCM_PY_LOG_FILE_POINTER, "  Error Class: %d\n", errbuf->pin_errclass);
        fprintf(PCM_PY_LOG_FILE_POINTER, "  Error Code: %d\n", errbuf->pin_err);
        fprintf(PCM_PY_LOG_FILE_POINTER, "  Field: %d\n", errbuf->field);
        fprintf(PCM_PY_LOG_FILE_POINTER, "  Record ID: %d\n", errbuf->rec_id);
        fprintf(PCM_PY_LOG_FILE_POINTER, "  Line Number: %d\n", errbuf->line_no);
        fprintf(PCM_PY_LOG_FILE_POINTER, "  File Name: %s\n", errbuf->filename);
        fprintf(PCM_PY_LOG_FILE_POINTER, "  Facility: %d\n", errbuf->facility);
        fprintf(PCM_PY_LOG_FILE_POINTER, "  Message ID: %d\n", errbuf->msg_id);
        fprintf(PCM_PY_LOG_FILE_POINTER, "  Error Time (seconds): %d\n", errbuf->err_time_sec);
        fprintf(PCM_PY_LOG_FILE_POINTER, "  Error Time (microseconds): %d\n", errbuf->err_time_usec);
        fprintf(PCM_PY_LOG_FILE_POINTER, "  Version: %d\n", errbuf->version);
        fprintf(PCM_PY_LOG_FILE_POINTER, "  Args Pointer: %p\n", errbuf->argsp);
        fprintf(PCM_PY_LOG_FILE_POINTER, "  Next Pointer: %p\n", errbuf->nextp);
    }

    fflush(PCM_PY_LOG_FILE_POINTER);
}

char *pcm_dump_ebuf(pin_errbuf_t *errbuf, char *line )
{
    char *return_val;
    char msg[BUFSIZ*4];
    int tam=0;

    memset(msg, 0, (BUFSIZ*4 ) );
    if (errbuf->pin_err != 0)  {
        sprintf(msg, " %s\n Error Location: %d \n"
                    "  Error Class: %d \n"
                    "  Error Code: %d \n"
                    "  Field: %d \n"
                    "  Record ID: %d \n"
                    "  Line Number: %d \n"
                    "  File Name: %s \n"
                    "  Facility: %d \n"
                    "  Message ID: %d \n"
                    "  Error Time (seconds): %d \n"
                    "  Error Time (microseconds): %d \n"
                    "  Version: %d \n"
                    "  Args Pointer: %p \n"
                    "  Next Pointer: %p \n"
                    , line
                    , errbuf->location
                    , errbuf->pin_errclass
                    , errbuf->pin_err
                    , errbuf->field
                    , errbuf->rec_id
                    , errbuf->line_no
                    , errbuf->filename
                    , errbuf->facility
                    , errbuf->msg_id
                    , errbuf->err_time_sec
                    , errbuf->err_time_usec
                    , errbuf->version
                    , errbuf->argsp
                    , errbuf->nextp
                    );

        tam = strlen(msg) + 1;
        return_val = (char *) malloc (sizeof(char) * tam);
        memset(return_val, 0, tam);

        strcpy(return_val, msg);                

        return return_val;
    }
    
    return (char *)NULL;
}


void pcm_read_pinconf(void)
{
	char		logfile[256];
	int32			*loglevel_p;
	int32			loglevel = 0;
	char		*c_ptr = (char *)NULL;
	///char		*program = __PROGRAM__;
	int32		err;

    EBUFP = (pin_errbuf_t *) malloc(sizeof(pin_errbuf_t));
    PIN_ERR_CLEAR_ERR(EBUFP); 
    
	PIN_ERR_SET_PROGRAM(__PROGRAM__);
	PIN_ERR_SET_LEVEL(PIN_ERR_LEVEL_ERROR);

    

	memset(logfile, 0, 256);
	strcpy(logfile, "PCMPy.pinlog");
	pin_conf(__PROGRAM__, "logfile", PIN_FLDT_STR, (caddr_t *)&(c_ptr), &(EBUFP)->pin_err);
	if (c_ptr != (char *)NULL) {
		pin_strlcpy(logfile, c_ptr, sizeof(logfile));
		pin_free(c_ptr);
		c_ptr = (char *)NULL;
	}
    sprintf(PCM_PY_LOG_FILE, "%s", logfile);
    PIN_ERR_SET_LOGFILE(PCM_PY_LOG_FILE);


	pin_conf(__PROGRAM__, "loglevel", PIN_FLDT_INT, (caddr_t *)&loglevel_p, &(EBUFP)->pin_err);
	if (loglevel_p != (int32 *)NULL) {
		loglevel = *loglevel_p;
		free(loglevel_p);
	}
	PIN_ERR_SET_LEVEL(loglevel);
    PCM_PY_LOG_LEVEL = loglevel;

	PIN_ERR_SET_LEVEL(PCM_PY_LOG_LEVEL);
    
}
