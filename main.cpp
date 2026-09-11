#include "./deps/crow_all.h"

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return "Please visit <a href='https://zigref.org/'>https://zigref.org/</a>.";
    });

    CROW_ROUTE(app, "/status")([](){
        return "Everything operational";
    });

    app.port(8080).multithreaded().run();

    return 0;
}