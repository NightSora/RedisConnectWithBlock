#include "redisManager.h"
#include <string.h>
#include <set>

#define SUCCESS 0
#define NIL 1
#define ERR -1

using namespace std;

class Redis:public RedisManager{
public:
Redis():RedisManager(){};
~Redis(){};

int fixKey(const std::string &cmd,const std::string &key);

template<class T>
int insertOne(const std::string &cmd,const std::string &key,T &insert);

template<class T>
int insertSet(const std::string &cmd,const std::string &key,std::set<T> &insert);

template<class T>
int fetchOne(const std::string &cmd,const std::string &key,T &dst);

template<class T>
int fetchSet(const std::string &cmd,const std::string &key,std::set<T> &dst);

};



int Redis::fixKey(const std::string &cmd,const std::string &key){
	int nRet = 0;
	std::vector<string> command;
	RedisResult DstResult;
	command.push_back(cmd);
	command.push_back(key);
	nRet = this->query(command,DstResult);

	if(REDIS_RESULT_SUCCESS == nRet && DstResult.type != REDIS_REPLY_ERROR){
		return SUCCESS;
	}
	return ERR;
}


template<class T>
int Redis::insertOne(const std::string &cmd,const std::string &key,T &insert){
	int nRet = 0;
	std::vector<string> command;
	RedisResult DstResult;
	command.push_back(cmd);
	command.push_back(key);
	command.push_back(string(reinterpret_cast<char *>(&insert),sizeof(T)));
	nRet = this->query(command,DstResult);

	if(REDIS_RESULT_SUCCESS == nRet && DstResult.type != REDIS_REPLY_ERROR){
		return SUCCESS;
	}
	return ERR;
}


template<class T>
int Redis::insertSet(const std::string &cmd,const std::string &key,std::set<T> &insert){
	int nRet = 0;
	std::vector<string> command;
	RedisResult DstResult;
	command.push_back(cmd);
	command.push_back(key);
	typedef typename std::set<T>::iterator ct_iter;
	
	for(ct_iter iter = insert.begin();iter != insert.end();++iter ){
		command.push_back(string( reinterpret_cast<const char*>( &(*iter) ),sizeof(T) ));
	}

	nRet = this->query(command,DstResult);

	if(REDIS_RESULT_SUCCESS == nRet && DstResult.type != REDIS_REPLY_ERROR){
		return SUCCESS;
	}
	return ERR;
}


template<class T>
int Redis::fetchOne(const std::string &cmd,const std::string &key,T &dst){
	int nRet = 0;
	std::vector<string> command;
	RedisResult DstResult;
	command.push_back(cmd);
	command.push_back(key);
	nRet = this->query(command,DstResult);
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
int Redis::fetchSet(const std::string &cmd,const std::string &key,std::set<T> &dst){
	int nRet = 0;
	std::vector<string> command;
	RedisResult DstResult;
	command.push_back(cmd);
	command.push_back(key);
	nRet = this->query(command,DstResult);
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
				dst.insert(tobj);
			}
			return SUCCESS;
		}
	}
	return ERR;
}
