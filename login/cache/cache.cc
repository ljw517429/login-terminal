#include <iostream>
#include "cache.h"

using namespace std;

namespace cache {
    CacheItem::CacheItem():
        m_token("") {
    }

    CacheItem::~CacheItem() {

    }

    string CacheItem::toString() const {
        string str("token: " + this->m_token);

        return str;
    }

    ostream & operator<<(ostream &out,const CacheItem &cache) {
        cout<< cache.toString() <<endl;
        
        return out;
    }

    bool Cache::GetCacheByAccount(const string& account,string* cache) {
        if("" == account || nullptr == cache) {
            return false;
        }

        if(nullptr == RedisPool::Pool()) {
            return false;
        }

        shared_ptr<RedisUtil> redis = RedisPool::Pool()->redis();
        if(nullptr == redis) {
            return false;
        }

        string temp = redis->Get(account).toString();
        if("" == temp) {
            return false;
        }

        *cache = temp;

        return true;
    }

    bool Cache::UpdateCacheByAccount(const string& account,const string& cache) {
        if("" == account || "" == cache) {
            return false;
        }

        if(nullptr == RedisPool::Pool()) {
            return false;
        }

        shared_ptr<RedisUtil> redis = RedisPool::Pool()->redis();
        if(nullptr == redis) {
            return false;
        }

        cout << "redis :" << account << " : " << cache << endl;
        redis->Set(account, cache);

        return true;
    }
}