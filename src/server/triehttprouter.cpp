#include "triehttprouter.h"

TrieHttpRouter::TrieHttpRouter()
    : root(std::make_unique<Node>())
{
}

void TrieHttpRouter::registerRoute(http::verb method, std::string_view pattern,
                                   CallBack callback)
{
    if (!methodNamesByVerb.contains(method)) {
        return;
    }
    std::string_view methodString = methodNamesByVerb[method];
    std::vector<std::string_view> splitPatternVector;
    splitPatternVector.push_back(methodString);
    auto splitUri = split(pattern, '/');
    splitPatternVector.insert(splitPatternVector.end(), splitUri.begin(), splitUri.end());
    NodePtr current = root.get();
    for (const auto &segment : splitPatternVector) {
        if (segment.find("{") != std::string_view::npos &&
            segment.find("}") != std::string_view::npos) {
            if (auto childNode = registerAsDynamic(segment, current)) {
                current = childNode;
            }
        } else {
            registerAsStatic(segment, current);
            current = current->staticChildren[segment].get();
        }
    }
    current->method = std::move(method);
    current->callBack = std::move(callback);
}

void TrieHttpRouter::processRoute(http::verb method, std::string_view route)
{
    QueryParams queryParams = processQueryParams(route);
    auto questionIdx = route.find('?');
    std::string_view routWithoutQuery = route;
    if (questionIdx != std::string_view::npos) {
        routWithoutQuery = route.substr(0, questionIdx);
    }
    if (!methodNamesByVerb.contains(method)) {
        return;
    }
    std::string_view methodString = methodNamesByVerb[method];
    std::vector<std::string_view> splitRoute;
    splitRoute.push_back(methodString);
    auto splitUri = split(routWithoutQuery, '/');
    splitRoute.insert(splitRoute.end(), splitUri.begin(), splitUri.end());
    NodePtr current = root.get();
    RouteParams routeParams;
    for (const auto &segment : splitRoute) {
        if (current->staticChildren.contains(segment)) {
            current = current->staticChildren[segment].get();
        } else if (auto dynamicChild =
            checkByDynamicChild(segment, current, routeParams)) {
            current = dynamicChild;
        } else {
            return;
        }
    }
    if (current && current->callBack) {
        current->callBack(std::move(routeParams), std::move(queryParams));
    }
}

std::vector<std::string_view> TrieHttpRouter::split(std::string_view segment,
                                                    char splitCharacter) const
{
    std::vector<std::string_view> result;
    size_t start{0};
    size_t end = segment.find(splitCharacter, start);
    while (end != std::string_view::npos) {
        if (end - start > 0) {
            result.push_back(segment.substr(start, end - start));
        }
        start = end + 1;
        end = segment.find(splitCharacter, start);
    }
    result.push_back(segment.substr(start));
    return result;
}

TrieHttpRouter::NodePtr TrieHttpRouter::registerAsDynamic(std::string_view segment, NodePtr current)
{
    auto segmentWithoutBrackets = segment.substr(1, segment.length() - 2);
    auto splitWithoutBrackets = split(segmentWithoutBrackets, ':');
    if (splitWithoutBrackets.size() != 2) {
        return nullptr;
    }
    std::string_view typeString = splitWithoutBrackets[1];
    if (!dynamicTypeByString.contains(typeString)) {
        return nullptr;
    }
    DynamicType dynamicType = dynamicTypeByString[typeString];
    std::string_view fieldName = splitWithoutBrackets[0];
    for (const auto &child : current->dynamicChildren) {
        if (child && child->dynamicType == dynamicType &&
            child->dynamicFieldName == fieldName) {
            return child.get();
        }
    }
    auto result = std::make_unique<Node>();
    NodePtr rawPtr = result.get();
    result->dynamicType = dynamicType;
    result->dynamicFieldName = fieldName;
    current->dynamicChildren.push_back(std::move(result));
    return rawPtr;
}

void TrieHttpRouter::registerAsStatic(std::string_view segment, NodePtr current)
{
    if (!current->staticChildren.contains(segment)) {
        current->staticChildren[segment] = std::make_unique<Node>();
    }
}

TrieHttpRouter::NodePtr TrieHttpRouter::checkByDynamicChild(std::string_view routeSegment,
                                                            NodePtr current,
                                                            RouteParams &queryParams)
{
    if (current->dynamicChildren.empty()) {
        return nullptr;
    }
    for (const auto &child : current->dynamicChildren) {
        if (child) {
            bool ok = false;
            switch (child->dynamicType) {
            case DynamicType::Integer: {
                int intValue{0};
                ok = parseInt(routeSegment, intValue);
                queryParams[child->dynamicFieldName] = VariantValue{intValue};
                break;
            }
            case DynamicType::Double: {
                double doubleValue{0};
                ok = parseDouble(routeSegment, doubleValue);
                queryParams[child->dynamicFieldName] = VariantValue{doubleValue};
                break;
            }
            case DynamicType::String:
                ok = true;
                queryParams[child->dynamicFieldName] = VariantValue{routeSegment};
                break;
            }
            if (ok) {
                return child.get();
            }
        }
    }
    return nullptr;
}

QueryParams TrieHttpRouter::processQueryParams(std::string_view route)
{
    QueryParams result;
    auto questionPosition = route.find('?');
    if (questionPosition != std::string::npos) {
        std::string_view querySubString = route.substr(questionPosition + 1);
        auto splitQuery = split(querySubString, ',');
        for (const auto &queryParam : splitQuery) {
            auto splitParam = split(queryParam, '=');
            if (splitParam.size() == 2) {
                result[splitParam[0]] = VariantValue(splitParam[1]);
            }
        }
    }
    return result;
}

bool TrieHttpRouter::parseInt(std::string_view segment, int &value)
{
    try {
        value = std::stoi(std::string(segment));
        return true;
    } catch (const std::exception &) {
        return false;
    }
}

bool TrieHttpRouter::parseDouble(std::string_view segment, double &value)
{
    try {
        value = std::stod(std::string(segment));
        return true;
    } catch (const std::exception &) {
        return false;
    }
}