#include "Common.h"
#include <string>


#define REDIS_RESULT_SUCCESS 0
#define REDIS_RESULT_NULL 20
#define REDIS_CONTEXT_NULL 40
#define REDIS_RESULT_ERROR 80

//redis返回的CPP风格包装
typedef struct redisResult{
	int type;
	std::string binary;
	long long integer;
	std::vector<struct redisResult> arr;
}RedisResult;

//命令输入包装,自动维护生命周期，妈妈再也不怕我内存泄露了
class RedisArgv{
	public:
		RedisArgv(std::vector<std::string> &Command);
		~RedisArgv();

		char **buffs;
		int argv_size;
		size_t *argvlen;
};

//二进制安全的redis命令执行包装类
class RedisCommand{
public:
	//执行redis命令，二进制安全,返回结构放在dst中
	static int queryCommand(redisContext *c,std::vector<std::string> &Command,RedisResult *dst);
private:
	//将redisReply组合成redisResult
	static void ComplexResult(const redisReply* reply,RedisResult &dst);

	
};
