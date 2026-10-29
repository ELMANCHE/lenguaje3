#include "Database.h"
#include <iostream>

Database::Database() : conn(nullptr), connected(false) {}

Database::~Database() {
    disconnect();
}

bool Database::connect() {
    const char* conninfo = "postgresql://neondb_owner:npg_5lmIWvygPZ4L@ep-restless-breeze-b7ccky1m-pooler.c-13.us-east-1.aws.neon.tech/neondb?sslmode=require&channel_binding=require";
    conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Error de conexión a la base de datos: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        conn = nullptr;
        connected = false;
        return false;
    }

    connected = true;
    std::cout << "Conectado a PostgreSQL exitosamente" << std::endl;
    return true;
}

void Database::disconnect() {
    if (conn) {
        PQfinish(conn);
        conn = nullptr;
        connected = false;
    }
}

bool Database::savePlayer(const std::string& nombre, int puntaje) {
    if (!connected) {
        std::cerr << "No hay conexión a la base de datos" << std::endl;
        return false;
    }

    std::string query = "INSERT INTO jugadores (nombre, puntaje) VALUES ($1, $2)";
    const std::string puntajeTexto = std::to_string(puntaje);
    const char* paramValues[2] = {nombre.c_str(), puntajeTexto.c_str()};
    int paramLengths[2] = {
        static_cast<int>(nombre.length()),
        static_cast<int>(puntajeTexto.length())
    };
    int paramFormats[2] = {0, 0};

    PGresult* res = PQexecParams(conn, query.c_str(), 2, nullptr, paramValues, paramLengths, paramFormats, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Error al insertar jugador: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return false;
    }

    PQclear(res);
    std::cout << "Jugador guardado: " << nombre << " - " << puntaje << " puntos" << std::endl;
    return true;
}

std::vector<PlayerScore> Database::getTopPlayers(int limit) {
    std::vector<PlayerScore> players;

    if (!connected) {
        std::cerr << "No hay conexión a la base de datos" << std::endl;
        return players;
    }

    std::string query = "SELECT nombre, puntaje FROM jugadores ORDER BY puntaje DESC LIMIT " + std::to_string(limit);
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Error al obtener jugadores: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return players;
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        PlayerScore ps;
        ps.nombre = PQgetvalue(res, i, 0);
        ps.puntaje = std::stoi(PQgetvalue(res, i, 1));
        players.push_back(ps);
    }

    PQclear(res);
    return players;
}
