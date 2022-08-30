#include <mymuduo/TcpServer.h>
#include <string>
#include <mymuduo/Logger.h>
#include <functional>

class Echoserver 
{
public:
    Echoserver(EventLoop *loop,const InetAddress &addr,const std::string name) :server_(loop,addr,name),loop_(loop) 
    {
        //注册回调函数
        server_.setConnectionCallback(std::bind(&Echoserver::onConnection,this,std::placeholders::_1));
        //设置合适的loop线程数量
        server_.setMessageCallback(std::bind(&Echoserver::onMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
        //设置合适的线程数量
        server_.setThreadNum(4);
    }
    void start() {
        server_.start();
    }
private:
    //连接建立或者断开的回调
    void onConnection(const TcpConnectionPtr &conn) 
    {
        if(conn->connected()) 
        {
            LOG_INFO("conn UP: %s",conn->peerAddress().toIpPort().c_str());
        }else {
            LOG_INFO("conn Down: %s",conn->peerAddress().toIpPort().c_str());
        }
    }
    //可读写事件的回调
    void onMessage(const TcpConnectionPtr &conn,Buffer *buf,Timestamp time) 
    {
        std::string msg = buf ->retrieveAllAsString();
        conn->send(msg);
        conn->shutdown();
    }
    EventLoop *loop_;
    TcpServer server_;
};

int main ()
{
    EventLoop loop;
    InetAddress addr(8000);
    Echoserver server(&loop,addr,"Echoserver_01");
    server.start();
    loop.loop();//启动mainloop的底层Poller
    return 0;
}