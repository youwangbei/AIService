#include "../../include/http/HttpContext.h"

using namespace muduo;
using namespace muduo::net;

namespace http
{

// 解析HTTP请求，从缓冲区读取并解析为完整的HTTP请求对象
bool HttpContext::parseRequest(Buffer *buf, Timestamp receiveTime)
{
    bool ok = true;
    bool hasMore = true;
    while (hasMore)
    {
        if (state_ == ExpectRequestLine)
        {
            const char *crlf = buf->findCRLF();
            if (crlf)
            {
                ok = processRequestLine(buf->peek(), crlf);
                if (ok)
                {
                    request_.setReceiveTime(receiveTime);
                    buf->retrieveUntil(crlf + 2);
                    state_ = ExpectHeaders;
                }
                else
                {
                    hasMore = false;
                }
            }
            else
            {
                hasMore = false;
            }
        }
        else if (state_ == ExpectHeaders)
        {
            const char *crlf = buf->findCRLF();
            if (crlf)
            {
                const char *colon = std::find(buf->peek(), crlf, ':');
                if (colon < crlf)
                {
                    request_.addHeader(buf->peek(), colon, crlf);
                }
                else if (buf->peek() == crlf)
                { 
                    if (request_.method() == HttpRequest::Post || 
                        request_.method() == HttpRequest::Put)
                    {
                        std::string contentLength = request_.getHeader("Content-Length");
                        if (!contentLength.empty())
                        {
                            request_.setContentLength(std::stoi(contentLength));
                            if (request_.contentLength() > 0)
                            {
                                state_ = ExpectBody;
                            }
                            else
                            {
                                state_ = GotAll;
                                hasMore = false;
                            }
                        }
                        else
                        {
                            ok = false;
                            hasMore = false;
                        }
                    }
                    else
                    {
                        state_ = GotAll; 
                        hasMore = false;
                    }
                }
                else
                {
                    ok = false;
                    hasMore = false;
                }
                buf->retrieveUntil(crlf + 2);
            }
            else
            {
                hasMore = false;
            }
        }
        else if (state_ == ExpectBody)
        {
            if (buf->readableBytes() < request_.contentLength())
            {
                hasMore = false;
                return true;
            }

            std::string body(buf->peek(), buf->peek() + request_.contentLength());
            request_.setBody(body);

            buf->retrieve(request_.contentLength());

            state_ = GotAll;
            hasMore = false;
        }
    }
    return ok;
}

// 处理HTTP请求行，解析方法、路径、查询参数和版本
bool HttpContext::processRequestLine(const char *begin, const char *end)
{
    bool succeed = false;
    const char *start = begin;
    const char *space = std::find(start, end, ' ');
    if (space != end && request_.setMethod(start, space))
    {
        start = space + 1;
        space = std::find(start, end, ' ');
        if (space != end)
        {
            const char *argumentStart = std::find(start, space, '?');
            if (argumentStart != space)
            {
                request_.setPath(start, argumentStart);
                request_.setQueryParameters(argumentStart + 1, space);
            }
            else
            {
                request_.setPath(start, space);
            }

            start = space + 1;
            succeed = ((end - start == 8) && std::equal(start, end - 1, "HTTP/1."));
            if (succeed)
            {
                if (*(end - 1) == '1')
                {
                    request_.setVersion("HTTP/1.1");
                }
                else if (*(end - 1) == '0')
                {
                    request_.setVersion("HTTP/1.0");
                }
                else
                {
                    succeed = false;
                }
            }
        }
    }
    return succeed;
}

} // namespace http
