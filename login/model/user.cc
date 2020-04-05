#include <iostream>
#include "user.h"


using namespace std;

namespace model {
    User::User():
        m_id(-1),
        m_account(""),
        m_pass(""),
        m_token(""){
    }

    User::~User(){

    }

    bool User::Get(int64_t id) {
        if(0 > id) {
            return false;
        }

        shared_ptr<MysqlUtil> mysql = MysqlPool::Pool()->mysql();
        if(nullptr == mysql) {
            return false;
        }

        stringstream stream;  
        stream<<id;;  
        string temp = stream.str(); 

        vector<map<string,Value> > users = mysql->Selection("SELECT * FROM t_account_user_info WHERE id=" + temp + " limit 1");
        vector<map<string,Value> >::const_iterator it = users.begin();
        if(it != users.end()) {
            map<string,Value> value = *it;
            map<string,Value>::const_iterator tit = value.begin();
            while(tit != value.end()) {
                if("dwId" == tit->first) {
                    this->m_id = Value(tit->second).toInt();
                }
                if("strAccount" == tit->first) {
                    this->m_account = Value(tit->second).toString();
                }
                if("strPassword" == tit->first) {
                    this->m_pass = Value(tit->second).toString();
                }
                if("strToken" == tit->first) {
                    this->m_token = Value(tit->second).toString();
                }
                if("ddCreatime" == tit->first) {
                    this->m_ctime = Value(tit->second).toInt();
                }

                tit++;
            }

            it++;
        }else {
            return false;
        }

        return true;
    }

    vector<User> User::Get(int offset,int limit) {
        vector<User> users;
        if(0 > offset || 0 > limit) {
            return users;
        }

        shared_ptr<MysqlUtil> mysql = MysqlPool::Pool()->mysql();
        if(nullptr == mysql) {
            return users;
        }

        vector<map<string,Value> > temp = mysql->Selection("SELECT * FROM t_account_user_info WHERE id=");
        vector<map<string,Value> >::const_iterator it = temp.begin();
        while(it != temp.end()) {
            map<string,Value> value = *it;
            map<string,Value>::const_iterator tit = value.begin();
            while(tit != value.end()) {
                User user;

                if("dwId" == tit->first) {
                    this->m_id = Value(tit->second).toInt();
                }
                if("strAccount" == tit->first) {
                    user.m_account = Value(tit->second).toString();
                }
                if("strPassword" == tit->first) {
                    user.m_pass = Value(tit->second).toString();
                }
                if("strToken" == tit->first) {
                    user.m_token = Value(tit->second).toString();
                }
                if("ddCreatime" == tit->first) {
                    user.m_ctime = Value(tit->second).toInt();
                }

                users.push_back(user);

                tit++;
            }

            it++;
        }


        return users;
    }

    bool User::GetByAccount(string account) {
        if("" == account) {
            return false;
        }

        shared_ptr<MysqlUtil> mysql = MysqlPool::Pool()->mysql();
        if(nullptr == mysql) {
            return false;
        }

        vector<map<string,Value>> temp = mysql->Selection("SELECT * FROM t_account_user_info WHERE strAccount='" + account + "' limit 1");
        vector<map<string,Value>>::const_iterator it = temp.begin();
        if(it != temp.end()) {
            map<string,Value> value = *it;
            map<string,Value>::const_iterator tit = value.begin();
            while(tit != value.end()) {
                if("dwId" == tit->first) {
                    this->m_id = Value(tit->second).toInt();
                }
                if("strAccount" == tit->first) {
                    this->m_account = Value(tit->second).toString();
                }
                if("strPassword" == tit->first) {
                    this->m_pass = Value(tit->second).toString();
                }
                if("strToken" == tit->first) {
                    this->m_token = Value(tit->second).toString();
                }
                if("ddCreatime" == tit->first) {
                    this->m_ctime = Value(tit->second).toInt();
                }

                ++tit;
            }
        }else {
            return false;
        }

        return true;
    }

    bool User::Update() {
        if(0 > this->m_id) {
            return false;
        }

        bool isCan = false;
        string sql("UPDATE t_account_user_info SET ");

        if("" != this->m_token) {
            isCan = true;
            sql += "strToken='" + this->m_token + "'";
        }

        if(!isCan) {
            return false;
        }

        stringstream stream;  
        stream<<this->m_id;;  
        string temp=stream.str(); 

        sql += " WHERE id=" + temp;
        shared_ptr<MysqlUtil> mysql = MysqlPool::Pool()->mysql();
        if(nullptr == mysql) {
            return false;
        }

        mysql->Exec(sql); // return  rows

        return true;
    }

    bool User::Create() {
        if("" == this->m_account) {
            return false;
        }
        stringstream stream;  
        stream<<this->m_ctime;;  
        string temp=stream.str(); 

        string sql("INSERT INTO t_account_user_info(strAccount,strPassword,ddCreatime) VALUE('");
        sql += this->m_account + "','";
        sql += this->m_pass + "','";
        sql += temp + "')";

        shared_ptr<MysqlUtil> mysql = MysqlPool::Pool()->mysql();
        if(nullptr == mysql) {
            return false;
        }
        cout<<sql<<endl;
        mysql->Exec(sql);; // return  rows

        return true;
    }

    string User::toString() const{
        const string str = "\nstrAccount: " + this->m_account +
            "\nstrPassword: " + this->m_pass +
            "\nstrToken: " + this->m_token;
            
        return str;
    }

    ostream & operator<<(ostream &out,const User &user) {
        out<<"m_id: "<< user.id();
        out<<"\nm_account: " << user.account();
        out<<"\nm_pass: "<< user.pass();
        out<<"\nm_token: "<< user.token();
        out<<"\nm_ctime: "<< user.ctime();

        return out;
    }
}