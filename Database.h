#pragma once
#include <string>
#include <vector>
#include <libpq-fe.h>

struct PlayerScore {
    std::string nombre;
    int puntaje;
};

class Database {
private:
    PGconn* conn;
    bool connected;

public:
    Database();
    ~Database();

    bool connect();
    void disconnect();
    bool savePlayer(const std::string& nombre, int puntaje);
    std::vector<PlayerScore> getTopPlayers(int limit = 10);
};
