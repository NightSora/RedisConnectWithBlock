#include "redisCommand.h"
#include <string>
#include <sstream>
#include <string.h>

using namespace std;

std::vector<std::string> string2eslpvector(const std::string& str){
	const char *cstr = str.c_str();
	vector<string> arr_cmds;
	stringstream ostr;

	int len = strlen(cstr);
	for(int i=0;i<len;++i){
		if(' ' == str[i] || len-1 == i){
			if(len-1 == i){
				ostr << str[i];
			}

			string cmd = ostr.str();
			if(cmd.size() >0 ){
				arr_cmds.push_back(cmd);
			}
			ostr.str(""); 
			continue;
		}
		ostr << str[i];
	}
	return arr_cmds;
}; 

RedisArgv::RedisArgv(std::vector<std::string> &Command){
	//动态分配....我也不想
	this->argv_size = Command.size();
	this->buffs  = new char*[this->argv_size];
	this->argvlen= new size_t[this->argv_size];
	int i=0;

	for(std::vector<std::string>::iterator iter=Command.begin();iter<Command.end();++iter){
		this->buffs[i] = (char*) ((*iter).data()); 
		this->argvlen[i] = (*iter).size();
		++i;
	}

}

RedisArgv::~RedisArgv(){
	//赶紧DELETE掉
	delete this->buffs;
	delete this->argvlen;
}

void RedisCommand::ComplexResult(const redisReply* reply,RedisResult &dst){
	if(NULL == reply){
		return ;
	}
	dst.type = reply->type;
	switch(reply->type){
		case REDIS_REPLY_INTEGER:
			dst.integer = reply->integer;
		break;
		case REDIS_REPLY_STRING:
		case REDIS_REPLY_STATUS:
		case REDIS_REPLY_ERROR:
			dst.binary = std::string(reply->str,reply->len);
		break;
		case REDIS_REPLY_ARRAY:
			RedisResult t;
			for(int i=0;i<reply->elements;++i){
				RedisCommand::ComplexResult(reply->element[i],t);
				dst.arr.push_back(t);
			}
		break;
	}

}

int RedisCommand::queryCommand(redisContext *c,std::vector<std::string> &Command,RedisResult *dst){
	if(NULL == c){
		return REDIS_CONTEXT_NULL;
	}
	//把命令序列封装到RedisArgv
	RedisArgv argv(Command);
	//执行命令
	redisReply *reply = static_cast<redisReply *>(redisCommandArgv(c,argv.argv_size,(const char**) (argv.buffs) ,(const size_t*)(argv.argvlen) ) );

	//执行失败的处理
	//将错误码错误值封装到RedisResult
	if(NULL == reply){
		if(0 != c->err){
			//printf("redis Context Err %s",c->errstr);
			//组合失败信息到返回引用结构体中
			if(NULL != dst){
				dst->binary=std::string(c->errstr);
				dst->type = c->err;
			}
			return REDIS_RESULT_ERROR;
		}
		return REDIS_RESULT_NULL;
	}

	//组合返回到RedisResult对象中
	if(NULL != dst){
		RedisCommand::ComplexResult(reply,*dst);
	}
	//释放redisReply
	freeReplyObject(reply);
	return REDIS_RESULT_SUCCESS;
}