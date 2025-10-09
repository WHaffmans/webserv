#include <webserv/client/Client.hpp>

#include <webserv/http/HttpHeaders.hpp> // for HttpHeaders
#include <webserv/log/Log.hpp>          // for Log, LOCATION
#include <webserv/router/Router.hpp>    // for Router
#include <webserv/server/Server.hpp>    // for Server
#include <webserv/socket/Socket.hpp>    // for Socket

#include <cstdint>    // for uint8_t
#include <functional> // for ref, reference_wrapper
#include <map>        // for map
#include <string>     // for basic_string, to_string, operator+, operator<=>
#include <utility>    // for pair, move

#include <sys/types.h> // for ssize_t

Client::Client(std::unique_ptr<Socket> socket, Server &server)
    : httpRequest_(std::make_unique<HttpRequest>(this)), httpResponse_(std::make_unique<HttpResponse>()),
      client_socket_(std::move(socket)), server_(std::ref(server))
{
    Log::trace(LOCATION);
    Log::info("New client connected, fd: " + std::to_string(client_socket_->getFd()));
}

Client::~Client()
{
    Log::trace(LOCATION);
    Log::info("Client disconnected, fd: " + std::to_string(client_socket_->getFd()));
    server_.removeFromEpoll(*client_socket_);
};

int Client::getStatusCode() const
{
    Log::trace(LOCATION);
    return statusCode_;
}

void Client::setStatusCode(int code)
{
    Log::trace(LOCATION);
    statusCode_ = code;
}

void Client::request()
{
    Log::trace(LOCATION);
    char buffer[bufferSize_] = {}; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    ssize_t bytesRead = client_socket_->recv(
        buffer, sizeof(buffer) - 1); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    if (bytesRead < 0)
    {
        Log::error("Read error");
        return;
    }
    if (bytesRead == 0)
    {
        Log::info("Client closed connection, fd: " + std::to_string(client_socket_->getFd())); // TODO weird
        server_.removeClient(*this); // CRITICAL: RETURN IMMEDIATELY
        return;
    }

    buffer[bytesRead] = '\0'; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    httpRequest_->receiveData(static_cast<const char *>(buffer), static_cast<size_t>(bytesRead));

    if (httpRequest_->getState() == HttpRequest::State::Complete
        || httpRequest_->getState() == HttpRequest::State::ParseError)
    {
        Log::info("Received complete request",
                  {
                      {"request_method", httpRequest_->getMethod()},
                      {"request_target", httpRequest_->getTarget()},
                      {"http_version", httpRequest_->getHttpVersion()},
                      {"headers", httpRequest_->getHeaders().toString()},
                      {"body", httpRequest_->getBody()},
                      {"state", std::to_string(static_cast<uint8_t>(httpRequest_->getState()))},
                  });
        server_.responseReady(client_socket_->getFd());
    }
    else
    {
        Log::debug("Received partial request",
                   {
                       {"current_state", std::to_string(static_cast<uint8_t>(httpRequest_->getState()))},
                       {"buffer_length", std::to_string(bytesRead)},
                   });
    }
}

bool Client::isResponseReady() const
{
    Log::trace(LOCATION);
    // todo: poll the httpResponse_ object
    return httpResponse_->isComplete();
}

std::vector<uint8_t> Client::getResponse() const
{
    Log::trace(LOCATION);

    const Router &router = server_.getRouter();
    static_cast<void>(router); // Suppress unused variable warning
    auto response = Router::handleRequest(*httpRequest_);
    return response->toBytes();
}