#include "redisConnect.h"
#include "redisCommand.h"
#include <stdarg.h> 

using namespace std;

//redis管理类，自动管理redis连接生命周期，提供命令执行与数据返回接口
class RedisManager{
public:
	RedisManager();
	~RedisManager();
	int initialize(int args,...);//可以输入多个rediscfg配置
	//命令执行函数
	int query(std::vector<std::string> &command,RedisResult &tdst); //执行任意字符串命令

protected:
	//重新连接
	inline int _reConnect();
	vector<RedisCfg> arr_cfg;
	RedisConnect _conn;
};


