#include "../include/handlers/ChatEntryHandler.h"



// 处理入口页面请求
void ChatEntryHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{

    std::string reqFile;
    reqFile.append("../AIApps/resource/entry.html");
    FileUtil fileOperater(reqFile);
    if (!fileOperater.isValid())
    {
        LOG_WARN << reqFile << " not exist";
        fileOperater.resetDefaultFile(); // 404 NOT FOUND
    }

    std::vector<char> buffer(fileOperater.size());
    fileOperater.readFile(buffer); 
    std::string bufStr = std::string(buffer.data(), buffer.size());

    resp->setStatusLine(req.getVersion(), http::HttpResponse::Ok200, "OK");
    resp->setCloseConnection(false);
    resp->setContentType("text/html");
    resp->setContentLength(bufStr.size());
    resp->setBody(bufStr);
}
