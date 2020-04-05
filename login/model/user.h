#ifndef H_USER_H_
#define H_USER_H_

#include <vector>
#include <iostream>
#include "util/mysqlutil.h"

using namespace std;
using namespace util;
using namespace util::db;

namespace model {
    class User final{
    public:
        User();
        ~User();

        bool Get(int64_t id);
        bool GetByAccount(string account);
        vector<User> Get(int offset,int limit);

        /**
         * 插入新的用户
         */
        bool Create();

        /**
         * 更新对应的用户
         */
        bool Update();

        inline void set_account(const string& account) {
            if("" == account) {
                return ;
            }

            this->m_account = account;
        }

        inline void set_pass(const string& pass) {
            if("" == pass) {
                return ;
            }

            this->m_pass = pass;
        }

        inline void set_token(const string& token) {
            this->m_token = token;
        }
        
        inline void set_ctime(const int64_t ctime) {
            this->m_ctime = ctime;
        } 

        inline int64_t id() const {
            return this->m_id;
        }

        inline string account() const{
            return this->m_account;
        }

        inline string pass() const {
            return this->m_pass;
        }

        inline string token() const {
            return this->m_token;
        }

        inline int64_t ctime() const {
            return this->m_ctime;
        }

        string toString() const;
        friend ostream & operator<<(ostream &,const User &);
    private:
        int64_t m_id;
        string m_account;
        string m_pass;
        string m_token;
        int64_t m_ctime;
    };
}

#endif