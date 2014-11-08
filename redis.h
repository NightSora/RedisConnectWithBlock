#include "redisManager.h"
#include <string.h>
#include <set>
#include <stdarg.h> 


#include <sstream> 

#define SUCCESS 0
#define NIL 1
#define ERR -1

using namespace std;

class Redis:public RedisManager{
public:
Redis():RedisManager(){};
~Redis(){};

template<class T>
int insert(const char* cmd,const char* key,T &insert);

template<class T>
int fetch(const char* cmd,const char* key,T &dst);

template<class T>
int insert(const char* cmd,const char* key,std::vector<T> &insert);

template<class T>
int fetch(const char* cmd,const char* key,std::vector<T> &dst);

};


template<class T>
int Redis::insert(const char* cmd,const char* key,T &insert){
	int nRet = 0;
	std::vector<string> command;
	RedisResult DstResult;
	command.push_back(string(cmd));
	command.push_back(string(key));
	command.push_back(string(reinterpret_cast<char *>(&insert),sizeof(T)));
	nRet = this->query(RedisCommand::queryCommand,command,DstResult);

	if(REDIS_RESULT_SUCCESS == nRet && DstResult.type != REDIS_REPLY_ERROR){
		return SUCCESS;
	}
	return ERR;
}

template<class T>
int Redis::fetch(const char* cmd,const char* key,T &dst){
	int nRet = 0;
	std::vector<string> command;
	RedisResult DstResult;
	command.push_back(string(cmd));
	command.push_back(string(key));
	nRet = this->query(RedisCommand::queryCommand,command,DstResult);
	if(REDIS_RESULT_SUCCESS == nRet && DstResult.type != REDIS_REPLY_ERROR){
		if(REDIS_REPLY_NIL == DstResult.type){
			return NIL;
		}
		if(REDIS_REPLY_STRING == DstResult.type && sizeof(T) == DstResult.binary.size() ){
			memcpy(&dst,DstResult.binary.data(),DstResult.binary.size() );
			return SUCCESS;
		}
	}
	return ERR;
}


template<class T>
int Redis::insert(const char* cmd,const char* key,std::vector<T> &insert){
	int nRet = 0;
	std::vector<string> command;
	RedisResult DstResult;
	command.push_back(string(cmd));
	command.push_back(string(key));
	typedef typename std::vector<T>::iterator ct_iter;
	
	for(ct_iter iter = insert.begin();iter != insert.end();++iter ){
		command.push_back(string( reinterpret_cast<const char*>( &(*iter) ),sizeof(T) ));
	}

	nRet = this->query(RedisCommand::queryCommand,command,DstResult);

	if(REDIS_RESULT_SUCCESS == nRet && DstResult.type != REDIS_REPLY_ERROR){
		return SUCCESS;
	}
	return ERR;
}



template<class T>
int Redis::fetch(const char* cmd,const char* key,std::vector<T> &dst){
	int nRet = 0;
	std::vector<string> command;
	RedisResult DstResult;
	command.push_back(string(cmd));
	command.push_back(string(key));
	nRet = this->query(RedisCommand::queryCommand,command,DstResult);
	if(REDIS_RESULT_SUCCESS == nRet && DstResult.type != REDIS_REPLY_ERROR){
		if(REDIS_REPLY_NIL == DstResult.type){
			return NIL;
		}
		if(REDIS_REPLY_ARRAY == DstResult.type ){
			for(std::vector<RedisResult>::iterator iter = DstResult.arr.begin() ; iter < DstResult.arr.end() ;++iter ){
				RedisResult tmp = *iter;
				if(REDIS_REPLY_STRING !=tmp.type || tmp.binary.size() != sizeof(T) ){
					return ERR;
				}
				T tobj;
				memcpy(&tobj,tmp.binary.data(),tmp.binary.size());
				dst.push_back(tobj);
			}
			return SUCCESS;
		}
	}
	return ERR;
}
