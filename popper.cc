/* Adapted from Boost.Redis examples (https://github.com/boostorg/redis/tree/develop/example) */

#include <boost/redis/connection.hpp>
#include <boost/asio/deferred.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/consign.hpp>
#include <iostream>

namespace asio = boost::asio;
using boost::redis::request;
using boost::redis::response;
using boost::redis::config;
using boost::redis::connection;

#if defined(BOOST_ASIO_HAS_CO_AWAIT)

auto co_main(config cfg, const std::string &queue_name) -> asio::awaitable<void> 
{
    auto conn = std::make_shared<connection>(co_await asio::this_coro::executor);
    conn->async_run(cfg, {}, asio::consign(asio::detached, conn));

    while (true) {
        request req;
        req.push("BRPOP", queue_name, 0);

        response<std::vector<std::string>> resp;
        co_await conn->async_exec(req, resp, asio::deferred);

        if (auto reply = std::get<0>(resp); !reply->empty()) {
            std::cout << "Popped data: " << reply->at(1) << " from: " << reply->at(0) << std::endl;
        } else {
            std::cout << "Queue is empty" << std::endl;
        }
    }

    conn->cancel();
}

#endif // defined(BOOST_ASIO_HAS_CO_AWAIT)