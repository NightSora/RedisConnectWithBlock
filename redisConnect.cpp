#include "redisConnect.h"
#include <string.h>


RedisConnect::RedisConnect(){
	this->_context = NULL;
	memset(&(this->cfg),0,sizeof(RedisCfg));
}

RedisConnect::~RedisConnect(){
	freeConnect();
}

redisContext* RedisConnect::GetContext() const{
	return this->_context;
}

void RedisConnect::freeConnect(){
	if(NULL != this->_context){
		printf("Connection %s@%d close!\n",this->cfg.host,this->cfg.port);
		redisFree(this->_context);
		this->_context = NULL;
	}
}

int RedisConnect::Connect(const RedisCfg& cfg){
	this->freeConnect();
	memcpy(&(this->cfg),&cfg,sizeof(RedisCfg));
	return this->_Connect();
}

int RedisConnect::reConnect(){
	this->freeConnect();
	return this->_Connect();
}

int RedisConnect::_Connect(){
	redisContext *c = redisConnectWithTimeout(this->cfg.host, this->cfg.port, this->cfg.timeout);
	 if (c == NULL || c->err) {
        if (c) {
            printf("Connection error: %d %s\n", c->errstr);
            redisFree(c);
            return REDIS_CONNECT_ERR;
        } else {
            printf("Connection error: can't allocate redis context\n");
            return REDIS_CONNECT_NOTALLOC;
        }
    }
    printf("Connection %s@%d connect!\n",this->cfg.host,this->cfg.port);
    this->_context = c;
    return REDIS_CONNECT_SUCCESS;
}

