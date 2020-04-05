## 编译

### 本地环境

\```

Ubuntu 16.04

Bazel: 2.2.0

Protobuf: 3.11.2

(对 `proto` 文件需要本地编译（`protos.sh`）脚本，这就需要本地编译安装 `protoc` 和关联的 `grpc_cpp_plugin` 插件。)

\```

### 第三方依赖

- [JSON](https://github.com/nlohmann/json) , 对自定义结构体的序列化和反序列化处理非常方便
- [MD5](https://github.com/joyeecheung/md5)，`MD5` 处理



### 目录结构

```shell
.
├── BUILD
├── cache						# redis 数据缓存操作接口
│   ├── BUILD
│   ├── cache.cc
│   └── cache.h
├── config.ini
├── hiredis						# redis 驱动
│   ├── adapters
│   │   ├── ae.h
│   │   ├── glib.h
│   │   ├── ivykis.h
│   │   ├── libevent.h
│   │   ├── libev.h
│   │   ├── libuv.h
│   │   ├── macosx.h
│   │   └── qt.h
│   ├── appveyor.yml
│   ├── async.c
│   ├── async.h
│   ├── BUILD
│   ├── CHANGELOG.md
│   ├── COPYING
│   ├── dict.c
│   ├── dict.h
│   ├── examples
│   │   ├── example-ae.c
│   │   ├── example.c
│   │   ├── example-glib.c
│   │   ├── example-ivykis.c
│   │   ├── example-libev.c
│   │   ├── example-libevent.c
│   │   ├── example-libuv.c
│   │   ├── example-macosx.c
│   │   ├── example-qt.cpp
│   │   └── example-qt.h
│   ├── fmacros.h
│   ├── hiredis.c
│   ├── hiredis.h
│   ├── Makefile
│   ├── net.c
│   ├── net.h
│   ├── read.c
│   ├── read.h
│   ├── README.md
│   ├── sdsalloc.h
│   ├── sds.c
│   ├── sds.h
│   ├── test.c
│   └── win32.h
├── login_client.cc				# 客户端代码
├── login_server.cc				# 服务端代码
├── model						# mysql 操作接口
│   ├── BUILD
│   ├── user.cc
│   └── user.h
├── protos
│   ├── BUILD
│   ├── login.grpc.pb.cc
│   ├── login.grpc.pb.h
│   ├── login.pb.cc
│   ├── login.pb.h
│   ├── login.proto
│   └── protoc.sh
├── README.md
├── table.sql
├── util
│   ├── BUILD
│   ├── configmodule.cc				# ini 配置文件解析处理
│   ├── configmodule.h
│   ├── const.h						# 自定义状态码
│   ├── datetime.cc
│   ├── datetime.h
│   ├── dbmodule.cc
│   ├── dbmodule.h
│   ├── imodule.cc
│   ├── imodule.h
│   ├── json.hpp					# json 处理
│   ├── log.cc
│   ├── log.h
│   ├── md5.cpp						# md5加密解密
│   ├── md5.h
│   ├── mysqlutil.cc
│   ├── mysqlutil.h
│   ├── redisutil.cc
│   ├── redisutil.h
│   ├── uuid.cc
│   ├── uuid.h
│   ├── validate.h
│   ├── value.cc
│   └── value.h
└── WORKSPACE
```



## 命令

### client

```shell
./client config.ini # 配置服务器地址和用户账户
```

#### signin 登录

```json
request:
	account
	pass
	sign

repsponse:
	state
	message
	token
	sign
```



#### signup 注册

```
request:
	account
	pass
	sign

repsponse:
	state
	message
	sign
```

