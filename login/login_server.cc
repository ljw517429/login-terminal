#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <mysql/mysql.h>
#include <grpcpp/grpcpp.h>

#include "util/md5.h"
#include "util/configmodule.h"
#include "util/validate.h"
#include "util/const.h"
#include "model/user.h"
#include "cache/cache.h"
#include "util/redisutil.h"
#include "util/mysqlutil.h"
#include "util/json.hpp"
#include "util/uuid.h"
#include "protos/login.grpc.pb.h"

using namespace std;
using namespace util;
using namespace model;
using namespace cache;
using json = nlohmann::json;

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerCompletionQueue;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using login::SignInRequest;
using login::SignInResponse;
using login::SignUpRequest;
using login::SignUpResponse;
using login::HeartRequest;
using login::HeartResponse;
using login::ReplyInfo;
using login::Login;

/**
 *  RPC回调方法
 */
class LoginServiceImpl final : public Login::Service {
    /**
     *  用户登录
     * 
     * @param context
     * @param request
     * @param reply
     */
    Status OnSignIn(ServerContext* context,const SignInRequest* request, SignInResponse* reply) override {
        if("" == request->account()) {
            // 账号不能为空
            reply->set_status(Const::StatusCode::ACCOUNT_EMPTY);
            reply->set_message(Const::GetStatusMsg(Const::StatusCode::ACCOUNT_EMPTY));

            return Status::OK;
        }

        if("" == request->pass()) {
            // 密码不能为空
            reply->set_status(Const::StatusCode::PASS_EMPTY);
            reply->set_message(Const::GetStatusMsg(Const::StatusCode::PASS_EMPTY));

            return Status::OK;
        }

        if(!Validate::IsValidateLogin(request)) {
            reply->set_status(Const::StatusCode::SIGN_ERROR);
            reply->set_message(Const::GetStatusMsg(Const::StatusCode::SIGN_ERROR));

            return Status::OK;
        }

        // 查询redis中用户是否存在
        string cache("");
        CacheItem cacheItem;
        User user;
        bool ret = Cache::GetCacheByAccount(request->account(), &cache);
        if(ret) {
            // 已经登录在线,可以剔除,生成新的UUID,老客户端下线
            cout<<"Already Login: " << request->account() << "token: " << cache << endl;
        } else {
            //  redis 中不存在，则查MYSQL
            ret = user.GetByAccount(request->account());
            if(!ret) {
                reply->set_status(Const::StatusCode::ACCOUNT_NOTEXIST);
                reply->set_message(Const::GetStatusMsg(Const::StatusCode::ACCOUNT_NOTEXIST));

                return Status::OK;
            }

            string md5pass = MD5(request->pass()).toStr();
            if(md5pass != user.pass()) {
                reply->set_status(Const::StatusCode::LOGIN_ERROR);
                reply->set_message(Const::GetStatusMsg(Const::StatusCode::LOGIN_ERROR));

                return Status::OK;
            }
        }

        char buf[UUID4_LEN] = {0};
        UUID::uuid4_generate(buf);
        string token(request->account() + buf);
        cacheItem.set_token(token);
        Cache::UpdateCacheByAccount(request->account(), token);
        cout << "OnSignIn : " << request->account() << cacheItem << endl;

        ReplyInfo info;
        reply->set_status(Const::StatusCode::OK);
        reply->set_message(Const::GetStatusMsg(Const::StatusCode::OK));
        reply->set_token(token);

        return Status::OK;
    }

    /**
     * 用户请求注册回调, 仅仅是执行注册动作
     * 
     * @param context
     * @param request
     * @param reply
     */
    Status OnSignUp(ServerContext* context,const SignUpRequest* request, SignUpResponse* reply) override {
        if("" == request->account()) {
            reply->set_status(Const::StatusCode::ACCOUNT_EMPTY);
            reply->set_message(Const::GetStatusMsg(Const::StatusCode::ACCOUNT_EMPTY));

            return Status::OK;
        }

        if("" == request->pass()) {
            reply->set_status(Const::StatusCode::PASS_EMPTY);
            reply->set_message(Const::GetStatusMsg(Const::StatusCode::PASS_EMPTY));

            return Status::OK;
        }

        if(!Validate::IsValidateSignUp(request)) {
            reply->set_status(Const::StatusCode::SIGN_ERROR);
            reply->set_message(Const::GetStatusMsg(Const::StatusCode::SIGN_ERROR));

            return Status::OK;
        }

        // 查询redis 用户是否存在，存在返回已注册
        string cache("");
        CacheItem cacheItem;
        bool ret = Cache::GetCacheByAccount(request->account(),&cache);
        if(ret) {
            reply->set_status(Const::StatusCode::ACCOUNT_EXIST);
            reply->set_message(Const::GetStatusMsg(Const::StatusCode::ACCOUNT_EXIST));
            return Status::OK;
        }

        // 查找Mysql
        User user;
        bool isExist = user.GetByAccount(request->account());
        if(isExist) {
            //是否更新redis
            reply->set_status(Const::StatusCode::ACCOUNT_EXIST);
            reply->set_message(Const::GetStatusMsg(Const::StatusCode::ACCOUNT_EXIST));
            return Status::OK;
        }
        
        string md5pass = MD5(request->pass()).toStr();
        user.set_account(request->account());
        user.set_pass(md5pass);
        user.set_ctime(time(NULL)); // 首次注册时间

        ret = user.Create();
        if(!ret) {
            reply->set_status(Const::StatusCode::SIGNUP_FAILED);
            reply->set_message(Const::GetStatusMsg(Const::StatusCode::SIGNUP_FAILED));

            return Status::OK;
        }
        cout<<"OnSignup:"<<user<<"  " + user.pass()<<endl;

        ReplyInfo replyinfo;
        reply->set_status(Const::StatusCode::OK);
        reply->set_message(Const::GetStatusMsg(Const::StatusCode::OK));

        return Status::OK;
    }

    /**
     * 用户在线心跳保持以及实时消息响应
     * 
     * @param context
     * @param request
     * @param reply
     */
	Status OnHeart(ServerContext* context,const HeartRequest* request, HeartResponse* reply) override{

        string cache("");
        CacheItem cacheItem;
        bool ret = Cache::GetCacheByAccount(request->account(), &cache);
        if(!ret) {
            reply->set_status(Const::StatusCode::NOT_LOGIN);
            reply->set_message(Const::GetStatusMsg(Const::StatusCode::NOT_LOGIN));

            return Status::OK;
        }

        cacheItem.set_token(cache);
        if (request->token() != cacheItem.token()) {
            reply->set_status(Const::StatusCode::INFO_ERROR);
            reply->set_message(Const::GetStatusMsg(Const::StatusCode::INFO_ERROR));
            return Status::OK;
        }

		cout<<"Request Heart :" << request->account() << endl;
        //将对应的消息返回给用户

        // 更新缓存消息
        Cache::UpdateCacheByAccount(request->account(), cache);
		reply->set_status(Const::StatusCode::OK);
        reply->set_message(Const::GetStatusMsg(Const::StatusCode::OK));

		return Status::OK;
	}
};

void RunServer() {
    config::ConfigModule conf;
    bool ret = conf.onLoad("config.ini");
    if(!ret) {
        exit(1);

        return ;
    }

    string server_address = conf.getStringByKey("server_host") + ":" + conf.getStringByKey("server_port");

    ServerBuilder builder;
    LoginServiceImpl service;

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    unique_ptr<Server> server(builder.BuildAndStart());
    cout << "Server Listening on " << server_address << endl;

    server->Wait();
}

int main(int argc,const char* argv[]) {
    config::ConfigModule conf;
    conf.onLoad("config.ini");

    RedisPool* redis_pool = RedisPool::Pool();
    redis_pool->Init(&conf,conf.getIntByKey("redis_pool"));

    MysqlPool* mysql_pool = MysqlPool::Pool();
    mysql_pool->Init(&conf,conf.getIntByKey("mysql_pool"));

    RunServer();
    MysqlPool::Free();
    RedisPool::Free();
    
    return 0;
}