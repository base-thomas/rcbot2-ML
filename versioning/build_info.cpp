#include "build_info.h"

// Arch detection borrowed from AMTL: https://github.com/alliedmodders/amtl/blob/master/amtl/am-platform.h#L67

#if defined(__x86_64) || defined(__x86_64__) || defined(__amd64__) || defined(__amd64) || \
    (defined(_MSC_VER) && defined(_M_X64))
#    define RCBOT_ARCH_X64
#    define RCBOT_ARCH_TAG "x86-64"
#elif defined(__i686) || defined(__i686__) || defined(__i386) || defined(__i386__) || \
    defined(i386) || (defined(_MSC_VER) && defined(_M_IX86))
#    define RCBOT_ARCH_X86
#    define RCBOT_ARCH_TAG "x86"
#elif defined(__aarch64__) || defined(_M_ARM64)
#    define RCBOT_ARCH_ARM64
#    define RCBOT_ARCH_TAG "ARM64"
#elif defined(__arm__) || defined(_M_ARM)
#    define RCBOT_ARCH_ARM
#    define RCBOT_ARCH_TAG "ARM"
#else
#    define RCBOT_ARCH_UNKNOWN
#    define RCBOT_ARCH_TAG "UNKNOWN-ARCH"
#endif

// generated with versioning/generate.py, AMBuild handles this
#include <build_version_auto.h>

const char* build_info::authors = "Cheeseh, RoboCop, nosoop, pongo1231, caxanga334";
const char* build_info::url = "http://rcbot.bots-united.com/";

const char* build_info::long_version = "APGRoboCop/rcbot2@" GIT_COMMIT_HASH;
const char* build_info::short_version = "2.0-alpha3 " RCBOT_ARCH_TAG " (apg)-" GIT_COMMIT_SHORT_HASH;

const char* build_info::date = __DATE__;