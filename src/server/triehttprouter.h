#ifndef HTTPROUTER_TRIEHTTPROUTER_H
#define HTTPROUTER_TRIEHTTPROUTER_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <boost/beast.hpp>

#include "variantvalue.h"

using RouteParams = std::unordered_map<std::string, VariantValue>;
using QueryParams = RouteParams;
using CallBack = std::function<void(RouteParams, QueryParams)>;

namespace http = boost::beast::http;

class TrieHttpRouter
{
public:
    TrieHttpRouter();

    void registerRoute(http::verb method, std::string_view pattern, CallBack callback);
    void processRoute(http::verb method, std::string_view route);

private:
    enum class DynamicType { Integer, Double, String };

    std::unordered_map<std::string_view, DynamicType> dynamicTypeByString{
        {"int", DynamicType::Integer},
        {"double", DynamicType::Double},
        {"str", DynamicType::String}
    };

    std::unordered_map<http::verb, std::string> methodNamesByVerb{
        {http::verb::get, "GET"},
        {http::verb::post, "POST"},
        {http::verb::put, "PUT"},
        {http::verb::head, "HEAD"},
        {http::verb::delete_, "DELETE"},
        {http::verb::connect, "CONNECT"},
        {http::verb::patch, "PATCH"},
    };

    struct Node
    {
        std::unordered_map<std::string, std::unique_ptr<Node> > staticChildren;
        std::vector<std::unique_ptr<Node> > dynamicChildren;
        DynamicType dynamicType;
        std::string dynamicFieldName;
        http::verb method;
        CallBack callBack;
    };

    using NodePtr = TrieHttpRouter::Node *;

    std::unique_ptr<Node> root;

    std::vector<std::string> split(std::string_view segment,
                                        char splitCharacter) const;
    NodePtr registerAsDynamic(std::string_view segment, NodePtr current);
    void registerAsStatic(std::string_view segment, NodePtr current);
    NodePtr checkByDynamicChild(std::string_view routeSegment,
                                NodePtr current,
                                RouteParams &queryParams);
    QueryParams processQueryParams(std::string_view route);
    bool parseInt(std::string_view segment, int &value);
    bool parseDouble(std::string_view segment, double &value);
};

#endif // HTTPROUTER_TRIEHTTPROUTER_H