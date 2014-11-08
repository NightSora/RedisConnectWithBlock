#include <cstdio>
#include <time.h>
#include <set>
#include <vector>
#include <string>

#ifdef __cplusplus
extern "C"{
#endif
#include "hiredis/hiredis.h"
#ifdef __cplusplus
}
#endif

#define REDIS_RESULT_SUCCESS 0
#define REDIS_RESULT_NULL 20
#define REDIS_CONTEXT_NULL 40
#define REDIS_RESULT_ERROR 80

#define REDIS_CONNECT_SUCCESS 0
#define REDIS_CONNECT_NOTALLOC -10
#define REDIS_CONNECT_ERR -20