#include "./deps/crow_all.h"
#include <sqlite3.h>

std::vector<std::string> global_list;
void prepare_statements()
{
    global_list.clear();
    global_list.reserve(10000);

    sqlite3 *db = nullptr;
    if (sqlite3_open_v2("./zigistry.db", &db, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK)
    {
        if (db)
        {
            sqlite3_close(db);
        }
        exit(1);
    }

    const char *sql = "SELECT repo_id FROM packages";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sqlite3_close(db);
        exit(1);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char *text = sqlite3_column_text(stmt, 0);
        if (text)
        {
            global_list.push_back((const char *)text);
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

int main()
{
    prepare_statements();
    int i = 0;
    for (auto thing : global_list)
    {
        std::cout << thing << std::endl;
        std::cout << i << std::endl;
        i++;
    }

    std::cout << "completed" << std::endl;

    crow::Crow<crow::CORSHandler> app;

    app.get_middleware<crow::CORSHandler>().global()
        .origin("*")
        .methods(crow::HTTPMethod::Get)
        .methods(crow::HTTPMethod::Post)
        .methods(crow::HTTPMethod::Put)
        .methods(crow::HTTPMethod::Delete)
        .methods(crow::HTTPMethod::Patch);

    CROW_ROUTE(app, "/")([]()
                         { return "Please visit <a href='https://zigref.org/'>https://zigref.org/</a>."; });

    CROW_ROUTE(app, "/status")([]()
                               { return "Everything operational"; });

    CROW_ROUTE(app, "/search")([](const crow::request &req)
                               {
        const char *q__ = req.url_params.get("q");

        if (!q__)
        {
            return crow::response(404);
        }

        std::string q = q__;
        std::transform(q.begin(), q.end(), q.begin(), ::tolower);

        if (q.length() > 30)
        {
            return crow::response("length_too_long");
        }

        crow::json::wvalue::list results;

        for (const std::string &repo : global_list)
        {
            if (repo.find(q) != std::string::npos)
            {
                results.push_back(repo);

                if (results.size() == 20)
                {
                    break;
                }
            }
        }

        crow::json::wvalue response;
        response["results"] = std::move(results);

        return crow::response(response); });

    app.port(8080).multithreaded().run();

    return 0;
}