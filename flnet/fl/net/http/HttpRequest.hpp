#pragma once

#include "fl/net/http/HttpUrl.hpp"

namespace Forward {

    template<class Body>
    class HttpRequestWrapper
    {
    private:
        http::request<Body> request_;

    public:
        HttpRequestWrapper()
            : request_{} {}

        HttpRequestWrapper(http::status status, int version)
            : request_(status, version) {}
        HttpRequestWrapper(http::verb method, std::string_view target, int version)
            : request_(method, target, version) {}

        HttpRequestWrapper(http::request<Body>&& request)
            : request_(std::move(request)){}
        HttpRequestWrapper(http::request<Body> const& request) 
            : request_(request) {}

        constexpr auto const& base() const &
        {
            return request_;
        }
        constexpr auto& base() &
        {
            return request_;
        }
        constexpr auto&& base() &&
        {
            return std::move(request_);
        }
        constexpr auto&& base() const&&
        {
            return std::move(request_);
        }

        HttpUrl Url() const 
        {
            return HttpUrl(request_.target());
        }
        HttpQuery Query() const 
        {
            HttpQuery query(Url().Query());

            if (query.IsValid())
                return query;

            query.SetQuery(request_.body());
            return query;
        }
        
        void Clear() 
        {
            request_ = {};
        }

        constexpr operator http::request<Body>&() &
        {
            return &request_;
        }
        constexpr operator http::request<Body>&&() &&
        {
            return std::move(request_);
        }
        constexpr operator http::request<Body> && () const&&
        {
            return std::move(request_);
        }
        constexpr operator http::request<Body> const&() const&
        {
            return request_;
        }

        constexpr operator http::message_generator()
        {
            return std::move(request_);
        }
    };

    using HttpRequest = HttpRequestWrapper<http::string_body>;
    using HttpRequestFile = HttpRequestWrapper<http::file_body>;
    using HttpRequestEmpty = HttpRequestWrapper<http::empty_body>;
}