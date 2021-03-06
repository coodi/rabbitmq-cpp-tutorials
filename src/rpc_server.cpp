#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>

#include "SimplePocoHandler.h"

int fib(int n)
{
    switch (n)
    {
    case 0:
        return 0;
    case 1:
        return 1;
    default:
        return fib(n - 1) + fib(n - 2);
    }
}

int main(void)
{
    SimplePocoHandler handler("localhost", 5672);

    AMQP::Connection connection(&handler, AMQP::Login("guest", "guest"), "/");

    AMQP::Channel channel(&connection);
    channel.setQos(1);

    channel.declareQueue("rpc_queue");
    channel.consume("").onReceived([&channel](const AMQP::Message &message,
            uint64_t deliveryTag,
            bool redelivered)
    {
        const auto body = std::string(message.body());//message.message();
        std::cout<<" [.] fib("<<body<<")"<<std::endl;

        auto tstr = std::to_string(fib(std::stoi(body)));
        AMQP::Envelope env(tstr.c_str(),tstr.size());
        env.setCorrelationID(message.correlationID());

        channel.publish("", message.replyTo(), env);
        channel.ack(deliveryTag);
    });

    std::cout << " [x] Awaiting RPC requests" << std::endl;
    handler.loop();
    return 0;
}
