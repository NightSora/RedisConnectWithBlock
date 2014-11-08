#include "Common.h"

#define REDIS_CONNECT_SUCCESS 0
#define REDIS_CONNECT_NOTALLOC -10
#define REDIS_CONNECT_ERR -20


typedef struct{
	char host[16];
	unsigned short port;
	struct timeval timeout;
}RedisCfg;

//连接管理类，维护redisConext的生命周期，连接方式为同步阻塞
class RedisConnect{
public:
	//初始化_context与cfg为NULL
	RedisConnect();
	//析构释放redisContext
	~RedisConnect();
	//从配置连接redis
	int Connect(const RedisCfg& cfg);
	//使用上次的连接配置重新连接
	int reConnect();
	//返回当前的redisContext指针
	redisContext *GetContext() const;
private:
	//安全释放redisContext连接
	void freeConnect();
	//连接方法
	int _Connect();
	//当前redis指针，默认初始化为NULL
	redisContext *_context;
	//保存连接配置
	RedisCfg cfg;
};
