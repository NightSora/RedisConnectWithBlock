#include "redisManager.h"

#include <string.h>

RedisManager::RedisManager(){
	this->_conn = RedisConnect();
};
RedisManager::~RedisManager(){};

int RedisManager::_reConnect(){
	int nRet=0;
	vector<RedisCfg>::iterator iter;
	for(iter = this->arr_cfg.begin();iter < this->arr_cfg.end(); ++iter){
		nRet=this->_conn.Connect(*iter);
		if(REDIS_CONNECT_SUCCESS == nRet){
			return REDIS_CONNECT_SUCCESS;
		}
	}
	return nRet;
}

//初始化配置，可以输入多个cfg，在args输入数量
int RedisManager::initialize(int args,...){
	this->arr_cfg.clear();
	va_list ap;
	va_start(ap, args);
	for(int i=0;i<args;i++){
		this->arr_cfg.push_back(va_arg(ap,RedisCfg));
	}
	va_end(ap);
	return this->_reConnect();
};


int RedisManager::query(std::vector<std::string> &command,RedisResult &DstResult){
	int nRet = 0;
	nRet = RedisCommand::queryCommand(this->_conn.GetContext(),command,&DstResult);
	//如果是服务器连接断开则重新连接
	if(REDIS_CONTEXT_NULL == nRet  || REDIS_RESULT_NULL == nRet || (REDIS_RESULT_ERROR == nRet && REDIS_ERR_EOF == DstResult.type)){
		nRet = this->_reConnect();
		//连接成功则重新执行命令
		if(REDIS_CONNECT_SUCCESS == nRet){
			nRet = RedisCommand::queryCommand(this->_conn.GetContext(),command, &DstResult );
		}
	}
	return nRet;
}


int RedisManager::Command(const char* Command){
	
	int nRet = 0;
	std::vector<string> command=string2eslpvector(string(Command));
	RedisResult DstResult;

	nRet = this->query(command,DstResult);

	if(REDIS_RESULT_SUCCESS == nRet && DstResult.type != REDIS_REPLY_ERROR){
		return SUCCESS;
	}
	return ERR;
}

int RedisManager::Command(std::vector<string> &Command){
	
	int nRet = 0;
	RedisResult DstResult;

	nRet = this->query(Command,DstResult);

	if(REDIS_RESULT_SUCCESS == nRet && DstResult.type != REDIS_REPLY_ERROR){
		return SUCCESS;
	}
	return ERR;
}