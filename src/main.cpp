#include  <iostream>
#include  <unordered_map>
#include "server/triehttprouter.h"

int main()
{
    TrieHttpRouter router;

    router.registerRoute(http::verb::get,
                         "/users/{uuid:str}/customers/{customer_id:int}",
                         [](RouteParams routeParams,
                            QueryParams) {
                             std::cout << "GET METHOD" << std::endl;
                             auto uuid = routeParams.at("uuid").toString();
                             int customerId = routeParams.at("customer_id").toInt();
                             std::cout << "Route params:" << " uuid: " << uuid << " " << customerId
                                 << std::endl;
                         });

    router.registerRoute(http::verb::post,
                         "/users/{uuid:str}/customers/{customer_id:int}",
                         [](RouteParams routeParams,
                            QueryParams) {
                             std::cout << "POST METHOD" << std::endl;
                             auto uuid = routeParams.at("uuid").toString();
                             int customerId = routeParams.at("customer_id").toInt();
                             std::cout << "Route params:" << " uuid: " << uuid << " " << customerId
                                 << std::endl;
                         });

    router.registerRoute(http::verb::get,
                         "/users/{uuid:str}/customers/{customer_uuid:str}",
                         [](RouteParams routeParams,
                            QueryParams queryParams) {
                             auto uuid = routeParams.at("uuid").toString();
                             auto customerUuid = routeParams.at("customer_uuid").
                                 toString();
                             std::cout << "Route params:" << " uuid: " << uuid << " customer uuid: "
                                 << customerUuid << std::endl;
                             auto bookName = queryParams.at("book_name").toString();
                             int64_t authorId = queryParams.at("author_id").toInt64();
                             std::cout << "Query params:" << " book name: " << bookName
                                 << " author id: " << authorId << std::endl;
                         });

    router.processRoute(http::verb::get, "/users/all/customers/432");
    router.processRoute(http::verb::post, "/users/all/customers/432");
    router.processRoute(http::verb::get,
                        "/users/all/customers/hello?book_name=\"Hello world\",author_id=12");

    return 0;
}