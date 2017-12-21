#ifndef __COMMON_ALLOC_H__
#define __COMMON_ALLOC_H__
#define SAFE_DELETE_PTR(p) if(p){delete p; p = NULL;}
#define SAFE_DELETE_ARRAY(p) if(p){delete[] p; p = NULL;}
#endif
