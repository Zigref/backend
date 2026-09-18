#include "./deps/crow_all.h"
#include <sqlite3.h>
#include <thread>
#include <shared_mutex>
#include <chrono>

std::vector<std::string> global_list;
std::shared_mutex list_mutex;

void prepare_statements()
{
    sqlite3 *db = nullptr;
    if (sqlite3_open_v2("./zigistry.db", &db, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK)
    {
        if (db)
        {
            sqlite3_close(db);
        }
        return;
    }

    const char *sql = "SELECT repo_id FROM packages";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sqlite3_close(db);
        return;
    }

    std::vector<std::string> new_list;
    new_list.reserve(10000);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char *text = sqlite3_column_text(stmt, 0);
        if (text)
        {
            new_list.push_back((const char *)text);
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    std::unique_lock lock(list_mutex);
    global_list = std::move(new_list);
}

int main()
{
    prepare_statements();

    std::thread([]() {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::hours(1));
            if (system("make download_database") == 0)
            {
                prepare_statements();
            }
        }
    }).detach();

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

        std::shared_lock lock(list_mutex);
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

    app.bindaddr("0.0.0.0").port(8080).multithreaded().run();

    return 0;
}