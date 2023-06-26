#pragma once

#include "fl/net/http/Core.hpp"

namespace fl {

    template<class Body>
    class HttpResponseWrapper
    {
        http::response<Body> response_;

    public:
        HttpResponseWrapper() {}

        HttpResponseWrapper(http::status status, int version)
            : response_(status, version) {}

        HttpResponseWrapper(http::response<Body>&& response)
            : response_(std::move(response)){}
        HttpResponseWrapper(http::response<Body> const& response) 
            : response_(response) {}

        auto& Base() & 
        {
            return response_;
        }
        auto&& Base() &&
        {
            return std::move(response_);
        }
        auto const& Base() const &
        {
            return response_;
        }

        void Clear() 
        {
            request_ = {};
        }

        operator http::message_generator() 
        {
            return std::move(response_);
        }

        template<typename T>
        static HttpResponseWrapper<http::string_body> Message(
            HttpResponseWrapper<http::string_body> res, 
            T const& msg) 
        {
            return MessageImpl(res, msg);
        }

    private: 
        static HttpResponseWrapper<http::string_body> MessageImpl(
            HttpResponseWrapper<http::string_body> res, 
            std::string const& msg) 
        {
            res.SetHeader(http::field::content_type, "text/plain");
            res.Body() = std::move(msg);
            return std::move(res);
        } 
        static HttpResponseWrapper<http::string_body> MessageImpl(
            HttpResponseWrapper<http::string_body> res, 
            boost::json::value const& msg) 
        {
            res.SetHeader(http::field::content_type, "application/json");
            res.Body() = boost::json::serialize(msg);
            return res;
        } 
    };

    using HttpResponse = HttpResponseWrapper<http::string_body>;
    using HttpResponseFile = HttpResponseWrapper<http::file_body>;
    using HttpResponseEmpty = HttpResponseWrapper<http::empty_body>;
}