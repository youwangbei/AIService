#include "../../include/ssl/SslConfig.h"

namespace ssl
{
// 构造SSL配置，初始化默认配置项
SslConfig::SslConfig()
    : version_(SSLVersion::TLS_1_2)
    , cipherList_("HIGH:!aNULL:!MDS")
    , verifyClient_(false)
    , verifyDepth_(4)
    , sessionTimeout_(300)
    , sessionCacheSize_(20480L)
{
}

}
