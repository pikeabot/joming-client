# include "game_client.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using json = nlohmann::json;

namespace gamephysics {
    struct PositionVector {
        float x;
        float y;
        float z;
    };
    struct VelocityVector {
        float x;
        float y;
        float z;
    };

    void to_json(json& j, const PositionVector& p) {
        j = json{ {"x", p.x}, {"y", p.y}, {"z", p.z} };
    }

    void from_json(const json& j, PositionVector& p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
        j.at("z").get_to(p.x);
    }

    void to_json(json& j, const VelocityVector& p) {
        j = json{ {"x", p.x}, {"y", p.y}, {"z", p.z} };
    }

    void from_json(const json& j, VelocityVector& p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
        j.at("z").get_to(p.x);
    }
} // namespace ns



int main() {
    int client_socket;
    sockaddr_in server_address;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    gamephysics::PositionVector p = {0, 10, 188};
    json j = p;
    std::cout << j << std::endl;
    auto j_str = to_string(j);
    send(client_socket, j_str.data(), j_str.size(), 0);
    
    char buffer[1024] = {0};
    read(client_socket, buffer, sizeof(buffer));
    std::cout << "Server replied: " << buffer << std::endl;

    char next_message[1024] = "\\q";
    std::cout << next_message << std::endl;
    send(client_socket, next_message, strlen(next_message), 0);
 
    close(client_socket);

    return 0;
}
