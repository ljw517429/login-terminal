cc_library(
    name = "util",
    srcs = [
        "datetime.cc",
        "dbmodule.cc",
        "log.cc",
        "redisutil.cc",
        "value.cc",
        "imodule.cc",
        "configmodule.cc",
        "mysqlutil.cc",
        "uuid.cc",
        "md5.cpp",
        "json.hpp"
        ],
    hdrs = [
        "datetime.h",
        "dbmodule.h",
        "log.h",
        "redisutil.h",
        "value.h",
        "imodule.h",
        "configmodule.h",
        "mysqlutil.h",
        "validate.h",
        "const.h",
        "uuid.h",
        "md5.h",
        "json.hpp"
        ],
    deps = ["//hiredis:hiredis"],
    linkopts = ["-lmysqlclient"],
    visibility = ["//visibility:public"],
)