#include <iostream>

#include "SimplePocoHandler.h"

int main(void)
{
    SimplePocoHandler handler("localhost", 5672);

    AMQP::Connection connection(&handler, AMQP::Login("guest", "guest"), "/");
    AMQP::Channel channel(&connection);

    channel.onReady([&]()
    {
        std::cout<<"here"<<std::endl;
        if(handler.connected())
        {
            channel.publish("", "hello", std::string("Hello World!"));
            std::cout << " [x] Sent 'Hello World!'" << std::endl;
            handler.quit();
        }
    });

    handler.loop();
    return 0;
}
