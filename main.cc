/* Adapted from Boost.Redis examples (https://github.com/boostorg/redis/tree/develop/example) */

#include <boost/redis/src.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <iostream>

namespace asio = boost::asio;
using boost::redis::config;

#if defined(BOOST_ASIO_HAS_CO_AWAIT)

extern asio::awaitable<void> co_main(config, std::string const&);

auto main(int argc, char * argv[]) -> int
{
    try {
        config cfg;

        std::string queue_name = (argc > 1) ? argv[1] : "queue";
        cfg.addr.host = (argc > 2) ? argv[2] : "127.0.0.1";
        cfg.addr.port = (argc > 3) ? argv[3] : "6379";

        asio::io_context ioc;
        asio::co_spawn(ioc, co_main(cfg, queue_name), [](std::exception_ptr p) {
            if (p)
            std::rethrow_exception(p);
        });
        ioc.run();

        } catch (std::exception const& e) {
            std::cerr << "(main) " << e.what() << std::endl;
            return 1;
        }
}

#else // defined(BOOST_ASIO_HAS_CO_AWAIT)

auto main() -> int
{
    std::cout << "Requires coroutine support." << std::endl;
    return 0;
}

#endif // defined(BOOST_ASIO_HAS_CO_AWAIT)