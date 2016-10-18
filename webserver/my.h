#ifndef __MY_H__
#define __MY_H__


enum http_method {
  HTTP_GET ,
  HTTP_UNSUPPORTED ,
};
typedef struct
{
  enum http_method method ;
  int major_version ;
  int minor_version ;
  char * target ;
} http_request ;


#endif
