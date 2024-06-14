/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  PCMPy - Lib
  Python Extension for  Oracle BRM
  Copyright Cassio Jemael 2024
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */



#include <Python.h>
#include "pcm_py.h"

#include <libxml/parser.h>
#include <libxml/tree.h>



void ltrim( char *s )
{
    int i = 0;
    int j = 0;
    char *p;

    while( (' ' == s[i] || '\t' == s[i]) && s[i] )
        i++;

    j = strlen( s+i );
    p =  (char *) malloc(j+1);

    //strncpy( s, s+i, j );
    strncpy( p, s+i, j );
    p[j] = '\0';
    //strcpy(s, p);
    s=p;
    //s[j] = '\0';
}


void rtrim( char *s )
{
    int i = 0, t=0;

    i= t = strlen( s );


    while( (' ' == s[i-1] || '\t' == s[i-1]) && (i-1) > 0 )
        i--;

    if (t!=i) s[i] = '\0';
}

void alltrim( char *s )
{
    rtrim( s );
    ltrim( s );
}






long pcm_get_file_size(FILE *file) {
    long current_position = ftell(file);

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, current_position, SEEK_SET);

    return size;
}



long int findReg(char *source, char *item,unsigned long start,long end, int regLen, int itemLen)
{
  unsigned long mid=0;
  long int positionIni,positionEnd,positionMid;
  char t1[256+1],t2[256+1],t3[256+1];

  mid = ((start+end) %2 == 0) ? (start+end)/2 : (start+end-1)/2;

  positionIni=start*regLen;
  positionEnd=(end)*regLen;
  positionMid=mid*regLen;

  strncpy(t1,source+positionIni,itemLen);
  t1[itemLen]=0;

  strncpy(t2,source+positionEnd,itemLen);
  t2[itemLen]=0;

  strncpy(t3,source+positionMid,itemLen);
  t3[itemLen]=0;


  if (strcmp(t1,item)==0)
  {
    return positionIni;
  }
  if (strcmp(t2,item)==0)
  {
     return positionEnd;
  }
  if (strcmp(t3,item)==0)
  {
    return positionMid;
  }
  if ((positionIni==0) && (positionMid==0) && (positionEnd==0))
    return -1;
  if ((positionIni==positionMid) && (positionEnd-regLen==positionIni))
    return -1;
  if (strcmp(t3,item)>0)
    return (findReg(source,item,start,mid,regLen,itemLen));
  else
    if (strcmp(t3,item)<0)
      return (findReg(source,item,mid,end,regLen,itemLen));
  return -1;
}


void remove_single_quotes(char *str) {
    int len = strlen(str);
    int i, j = 0;

    for (i = 0; i < len; i++) {
        if (str[i] != '\'') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0'; // Adiciona o caractere nulo no final da string
}
