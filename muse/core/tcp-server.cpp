#include <muse/tcp-server.hpp>

#include <muse/event_loop_thread_pool.hpp>
#include <muse/event_loop.hpp>
#include <muse/tcp-connection.hpp>
#include <muse/work-queue.hpp>
#include <muse/acceptor.hpp>
#include <muse/log.hpp>
#include <muse/assert.hpp>

#include <functional>
#include <format>

namespace muse {
    TcpServer::TcpServer(
        EventLoop *loop, 
        net::SocketAddr const &addr, 
        std::string const &name,
        Acceptor *acceptor,
        EventLoopThreadPool *tpool)
        : loop_(loop)
        , addr_(addr)
        , acceptor_(acceptor)
        , thread_pool_(tpool)
    {
        // std::bind_front requires at least C++20
        acceptor_->set_new_connection_handler(std::bind_front(&Self::new_connection, this));
    }

    auto TcpServer::create(EventLoop *loop, std::string const &name,
                           net::SocketAddr const &addr, Self *out) -> MuseStatus {
        auto acceptor = static_cast<Acceptor *>(malloc(sizeof(Acceptor)));
        if (!acceptor) {
            return MuseStatus::OutOfMem;
        }

        auto status = Acceptor::create(loop, addr, acceptor);
        if (MuseStatus::Ok != status) {
            return status;
        }

        auto thread_pool = new EventLoopThreadPool();
        if (!acceptor) {
            return MuseStatus::OutOfMem;
        }

        new (out) Self(loop, addr, name, acceptor, thread_pool);
        return MuseStatus::Ok;
    }

    auto TcpServer::start() -> void {
        // Under debug mode, we always expect this function never duplicated to be enter.
        DEBUG_ASSERT(!flags_.fetch_test(Flags::Running));

        if (!flags_.fetch_test(Flags::Running)) {
            flags_.fetch_or(Flags::Running);

            thread_pool_->start();
            WorkQueue::commit_task(std::bind(&Acceptor::listen, acceptor_.get()), 0);
        }
    }

    auto TcpServer::new_connection(NativeHandle handle, net::SocketAddr const &peer) -> void {
        auto const cid = cid_gen_.gen();

        char name[64];
        auto last = std::format_to(name, "{}|{}:{}\0", name_, addr_str_, cid);

        // For construction in std::string, passing the range from |name| to |last| is faster than just |name|.

        TcpConnectionPtr connection;
        auto status = TcpConnection::create(loop_, {name, last}, handle, addr_, peer, &connection);
        if (MuseStatus::Ok != status) {
            log::error("Failed to create TcpConnection(CID={}, Addr={})", cid, peer.to_string());
            return;
        }

        connection->set_message_handler(on_connection_message_);
        connection->set_connection_handler(on_connection_established_or_destroied_);
        connection->set_close_handler(std::bind(&TcpConnection::on_disconnected, connection));

        loop_->work_queue().commit_task(std::bind(&TcpConnection::on_connected, connection), 0);
        connections_[name] = connection;
    }

    auto TcpServer::remove_connection(TcpConnectionPtr const &connection) -> void {

    }

} // namespace muse
