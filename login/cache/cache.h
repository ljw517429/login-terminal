#ifndef H_CACHE_H_
#define H_CACHE_H_

#include <string>
#include <iostream>
#include "util/redisutil.h"

using namespace util;
using namespace util::db;

namespace cache {
    /**
     * 缓存项信息
     */
    class CacheItem {
    public:
        CacheItem();
        ~CacheItem();

        string toString() const;
        string toJsonStr() const;

        inline void set_token(const string& token) {
            if("" == token) {
                return ;
            } 

            this->m_token = token;
        }

        inline string token() const {
            return this->m_token;
        }

        friend ostream & operator<<(ostream &out,const CacheItem &cache);
    private:
        string m_token;
    };

    class Cache{
    public:
        /**
         * 读取缓存信息
         * 
         * @param account
         * @param cache
         */
        static bool GetCacheByAccount(const string& account,string* cache);

        /**
         * 更新或者写入缓存信息
         * 
         * @param account
         * @param cache
         */
        static bool UpdateCacheByAccount(const string& account,const string& cache);
    };
}

#endif