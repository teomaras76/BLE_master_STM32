

#include "stdio.h"
#include "string.h"

const char MyEnv[] = "IOT_EMBDC_HOME=root\0IOT_EMBDC_LOGGING=root\0";
const char *__environ = MyEnv;


typedef struct {
	char *name;
	int	 linecount;
    char **line;
	
}
SmartFile_t;

char *device_content[]= {
"org=122mmi",
"domain=internetofthings.ibmcloud.com",
"type=stm32",
"id=C47F5101074F",
"auth-method=token",
"auth-token=4_LN-vEHcMK(-IfMXd",
"serverCertPath=hacksofar",
"useClientCertificates=0",
//rootCACertPath=$rootCACertPath if useClientCertificates=1
//clientCertPath=$clientCertPath if useClientCertificates=1
//clientKeyPath=$clientKeyPath if useClientCertificates=1
  0
}
;

char *null_values_content[]= {
"org=122mmi",
"type=stm32",
"id=C47F5101074F",
"auth-token=",
  0
};


SmartFile_t filelist[]=
{
	{"root/test/device.cfg",0, device_content} ,
	{"root/test/device_with_csc.cfg",0 , device_content},
	{"toot/test/null_values.cfg",0 , null_values_content} 
};




#if defined (__ICCARM__)
#define _RESTRICT  _Restrict
#elif defined (__CC_ARM) || defined ( __GNUC__ )  
#define _RESTRICT  __restrict
#endif

int fclose(FILE *f)
{
  return 0;
}

FILE * fopen(const char *_RESTRICT name, const char *_RESTRICT mode)
{
  for(int i=0;i < sizeof(filelist)/sizeof(SmartFile_t) ; i++)
  {
    if (strcmp(filelist[i].name,name)==0) 
    {
      if (filelist[i].linecount != 0)
      {
        // printf("WARNING: file already open %s\n",name);
        filelist[i].linecount = 0;
      }
      return (FILE*) &filelist[i];
    }
  }
  printf("failed to to open %s \n",name);	
  return (FILE*) 0;
}

char *(fgets)(char *buf, int n, FILE * fp)
{
  SmartFile_t *sfp= (SmartFile_t*) fp;
  
  if (sfp->line[sfp->linecount])
  {
    strncpy(buf,sfp->line[sfp->linecount],n);
    sfp->linecount++;
    return  buf;
  }
  return 0;
}

void sleep(int);

	

char *getenv(const char *name);

char *getenv(const char *name)
{ /* search environment list for named entry */
  const char *s = __environ;
  size_t n = strlen(name);

  if( s == 0 )
    return 0;

  for (; *s; s += strlen(s) + 1)
  { /* look for name match */
    if (!strncmp(s, name, n) && s[n] == '=')
      return ((char *)&s[n + 1]);
  }
  return (0);
}





