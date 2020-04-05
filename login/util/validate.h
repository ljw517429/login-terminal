#include <iostream>
#include <memory>
#include <string>

#include "md5.h"
#include "protos/login.grpc.pb.h"


using namespace std;

using login::SignInRequest;
using login::SignInResponse;
using login::SignUpRequest;
using login::SignUpResponse;

/**
 * 验证签名的正确信息以及获取签名
 */
class Validate {
public:
    /**
     * 根据登录信息产生签名信息
     * 
     * @param request
     */
    static const string GenLoginSing(const SignInRequest* request) {
        if(nullptr == request) {
            return "";
        }

        string info(
            "&account=" + request->account() +
            "&pass=" + request->pass()
        );

        return MD5(info).toStr();
    }

    /**
     * 验证签名是否正确
     * 
     * @param request
     */
    static bool IsValidateLogin(const SignInRequest* request) {
        if(nullptr == request) {
            return false;
        }

        const string sign = GenLoginSing(request);
        if(sign == request->sign()) {
            return true;
        }

        return false;
    }

    /**
     *  更新注册信息产生签名信息
     * 
     * @param request
     */
    static const string GenSignUpSign(const SignUpRequest* request) {
        if(nullptr == request) {
            return "";
        }

        string info(
            "account=" + request->account() +
            "&pass=" + request->pass()
        );

        return MD5(info).toStr();
    }

    /**
     *  验证注册的签名信是否正确
     * 
     * @param request
     */
    static bool IsValidateSignUp(const SignUpRequest* request) {
        if(nullptr == request) {
            return false;
        }

        const string sign = GenSignUpSign(request);
        if(sign == request->sign()) {
            return true;
        }

        return false;
    }

    /**
     * 产生登录响应的签名信息
     * 
     * @param reply
     */
    static const string GenLoginReplySign(SignInResponse* reply) {
        if(nullptr == reply) {
            return "";
        }

        const string info("token=" + reply->token());
        string sign(MD5(info).toStr());

        reply->set_sign(sign);

        return sign;
    }

    /**
     *  产生注册响应的签名信息
     * 
     * @param reply
     */
    static const string GenSignUpReplySign(SignUpResponse* reply) {
        if(nullptr == reply) {
            return "";
        }

        const string info("token=" + reply->token());
        string sign(MD5(info).toStr());

        reply->set_sign(sign);

        return sign;
    }
};