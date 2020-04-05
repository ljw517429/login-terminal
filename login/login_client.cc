#include <iostream>
#include <memory>
#include <string>
#include <stdio.h>
#include <fstream>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <unordered_map>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include "util/configmodule.h"
#include "util/validate.h"
#include "util/json.hpp"
#include "util/const.h"
#include "protos/login.grpc.pb.h"


using namespace std;
using namespace util;
using json = nlohmann::json;

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

using login::SignInRequest;
using login::SignInResponse;
using login::SignUpRequest;
using login::SignUpResponse;
using login::HeartRequest;
using login::HeartResponse;
using login::Login;

#define TIME_WHEEL_SIZE 8
class LoginClient;
typedef void (*func)(int seconds, LoginClient* client);
struct timer_node {
    struct timer_node *next;
    int rotation;
    func proc;
    int seconds;
    LoginClient* client;
};

struct timer_wheel {
    struct timer_node* slot[TIME_WHEEL_SIZE];
    int current;
};

struct timer_wheel timer = {{0}, 0};

vector<string> split(const string &input) {
    istringstream buffer(input);
    vector<string> ret;

    copy(istream_iterator<string>(buffer), istream_iterator<string>(), back_inserter(ret));
    return ret;
}

void tick(int signo) {
    // 使用二级指针删进行单链表的删除
    struct timer_node **cur = &timer.slot[timer.current];
    while (*cur) {
        struct timer_node *curr = *cur;
        if (curr->rotation > 0) {
            curr->rotation--;
            cur = &curr->next;
        } else {
            curr->proc(curr->seconds, curr->client);
            *cur = curr->next;
            free(curr);
        }
    }
    timer.current = (timer.current + 1) % TIME_WHEEL_SIZE;
    alarm(1);
}

class LoginClient {
public:
    LoginClient(shared_ptr<Channel> channel)
         : stub_(Login::NewStub(channel)) 
    {
        this->m_account = "";
        this->m_pass    = "";
        this->m_token   = "";
        this->m_state   = false;
    }

    string OnSignIn(void) {
        SignInRequest request;
        request.set_account(this->m_account);
        request.set_pass(this->m_pass);
        request.set_sign(Validate::GenLoginSing(&request));

        SignInResponse reply;
        ClientContext context;

        Status status = stub_->OnSignIn(&context,request,&reply);

        if(status.ok()) {
            cout<<"OnSignIn Status  : " << reply.status()<<endl;
            cout<<"OnSignIn Token   : " << reply.token()<<endl;
            this->m_token = reply.token();
            this->m_state = true;
            this->AddTimer(10);
            return reply.message();
        }else {
            cout<<status.error_code() << ": "<< status.error_message() << endl;
            this->m_state = false;
            return "RPC failed";
        }
    }

    string OnSignUp(void)  {
        SignUpRequest request;
        request.set_account(this->m_account);
        request.set_pass(this->m_account);
        request.set_sign(Validate::GenSignUpSign(&request));

        SignUpResponse reply;
        ClientContext context;
        Status status = stub_->OnSignUp(&context,request,&reply);

        if(status.ok()) {
            cout<<"OnSignup Status : " << reply.status() << endl;
            return reply.message();
        }else {
            cout << status.error_code() << " : " << status.error_message() << endl;
            return "RPC Fail!";
        }
    }
    
    string OnHeart(int seconds) {
        HeartRequest request;
        request.set_account(this->m_account);
        request.set_token(this->m_token);

        HeartResponse reply;
        ClientContext context;

        Status status = stub_->OnHeart(&context,request,&reply);

        if(status.ok()) {
            cout<<"OnHeart Status : " << reply.status() <<endl;
            if (Const::StatusCode::OK == reply.status()) {
                this->AddTimer(seconds);
            }else {
                this->m_state = false;
                cout << "OnHeart Message : " << reply.message() << endl;
                cout << "You Logout!" << endl;
            }
            return reply.message();
        }else {
            cout<< status.error_code() << " : " << status.error_message() <<endl;
            this->m_state = false;
            return "RPC Fail";
        }
    }

    static void callback(int seconds, LoginClient* client) {
        client->OnHeart(seconds);
    }

    void AddTimer(int seconds){
        int pos = (seconds + timer.current) % TIME_WHEEL_SIZE;
        struct timer_node *node = (struct timer_node *)malloc(sizeof(struct timer_node));

        node->next = timer.slot[pos];
        timer.slot[pos] = node;
        node->rotation = seconds / TIME_WHEEL_SIZE;
        node->seconds = seconds;
        node->client  = this;
        node->proc = callback;
    }

    bool   m_state;
    string m_account;
    string m_pass;
    string m_token;

private:
    unique_ptr<Login::Stub> stub_;
    
};

vector<string> quits = {"exit", "quit", "bye"};
string _cliPrompt = "cli> ";
string exitPrompt = "Au revoir.\n";
string helpPrompt = "Usage:cmds={signin | signup }\n";

int main(int argc,const char* argv[]) {
    
    signal(SIGALRM, tick);
    alarm(1); // 1s的周期心跳
    
    if(argc != 2) {
        return 0;
    }

    config::ConfigModule conf;
    bool ret = conf.onLoad(argv[1]);
    if (!ret) {
        return 0;
    }

    string endpoint = conf.getStringByKey("server_host") + ":" + conf.getStringByKey("server_port");
    LoginClient client(grpc::CreateChannel(endpoint, grpc::InsecureChannelCredentials()));
    cout << endpoint << endl << helpPrompt;
    client.m_account = conf.getStringByKey("account");
    client.m_pass    = conf.getStringByKey("pass");
    
    string line, cmd;
    while (1) {
        cout << _cliPrompt;
        getline(cin, line);
        auto elts = split(line);
        if (elts.empty()) {
            continue;
        }
        string cmd = elts[0];
        if (quits.end() != find(quits.begin(), quits.end(), cmd)) {
            break;
        }

        if (0 == cmd.compare("signin")) {
            client.OnSignIn();
        } else if (0 == cmd.compare("signup")) {
            client.OnSignUp();
        } else {
            cout<<"invalild input!"<<endl;
            cout<<helpPrompt<<endl;
        }
    }
    
    return 0;
}
