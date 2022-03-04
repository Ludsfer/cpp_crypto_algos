#pragma once

#include "OrderExecutor.h"

#include <ccapi_cpp/ccapi_macro.h>
#include <ccapi_cpp/ccapi_session.h>

#pragma once

#include <map>
#include <vector>
#include <tuple>

#include <atomic>

class CcexOrderExecutor : public OrderExecutor
{
private:
    inline static const std::map<OrderType, std::string> order_type_names_{
        {OrderType::limit, "Limit"}, {OrderType::market, "Market"}};
    inline static const std::map<Side, std::string> side_names_{
        {Side::buy, "Buy"}, {Side::sell, "Sell"}};

    std::string order_message_;

    std::string api_key_;
    std::string api_secret_;
    std::string pass_phrase_;
    int expiry_;
    std::string exchange_;
    inline static const std::map<std::string, std::tuple<std::string, std::string, std::string>> exchange_key_names_{{
        {CCAPI_EXCHANGE_NAME_FTX, {CCAPI_FTX_API_KEY, CCAPI_FTX_API_SECRET, ""}},
        {CCAPI_EXCHANGE_NAME_BITMEX, {CCAPI_BITMEX_API_KEY, CCAPI_FTX_API_SECRET, ""}},
        {CCAPI_EXCHANGE_NAME_COINBASE, 
            {CCAPI_COINBASE_API_KEY, CCAPI_COINBASE_API_SECRET, CCAPI_COINBASE_API_PASSPHRASE}},
    }};

    class CcexOrderHandler : public ccapi::EventHandler
    {
    private:
        std::atomic_flag ordered_;

    public:
        CcexOrderHandler() = default;
        bool processEvent(const ccapi::Event &event, ccapi::Session *session) override
        {
            std::cout << "Received an event:\n" + event.toStringPretty(2, 2) << std::endl;
            ordered_.test_and_set();
            ordered_.notify_one();
            return true;
        }
        void wait() const
        {
            std::cout << "Waiting for order event" << std::endl;
            ordered_.wait(false);
        }
    };

public:
    CcexOrderExecutor(
        const std::string &, int,
        const std::string &api_key,
        const std::string &api_secret,
        const std::string &pass_phrase = "");
    ~CcexOrderExecutor();
    void new_order(const std::string &symbol, Side side, double orderQty, OrderType type, double price = -1.0) override;
};
