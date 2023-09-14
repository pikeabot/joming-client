# include "game_client.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

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

    struct MotionVector {
        uint64_t t;
        float xp;
        float yp;
        float zp;
        float xv;
        float yv;
        float zv;
    };

    void to_json(json& j, const PositionVector& p) {
        j = json{ {"x", p.x}, {"y", p.y}, {"z", p.z} };
    }

    void from_json(const json& j, PositionVector& p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
        j.at("z").get_to(p.x);
    }

    void to_json(json& j, const VelocityVector& v) {
        j = json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
    }

    void from_json(const json& j, VelocityVector& v) {
        j.at("x").get_to(v.x);
        j.at("y").get_to(v.y);
        j.at("z").get_to(v.x);
    }

    void to_json(json& j, const MotionVector& m) {
        j = json{ {"t", m.t}, {"xp", m.xp},{"yp", m.yp}, {"zp", m.zp}, {"xv", m.xv}, {"yv", m.yv}, {"zv", m.zv} };
    }

    void from_json(const json& j, MotionVector& m) {
        j.at("t").get_to(m.t);
        j.at("xp").get_to(m.xp);
        j.at("yp").get_to(m.yp);
        j.at("zp").get_to(m.xp);
        j.at("xv").get_to(m.xv);
        j.at("yv").get_to(m.yv);
        j.at("zv").get_to(m.xv);
    }
} // namespace ns



int main() {
    using namespace std::chrono;
    
    // define a 2D boundary the client can move within
    float x_bound = 500.0;
    float y_bound = 500.0;
    float heartbeat = 1000.0;
    
    int client_socket;
    sockaddr_in server_address;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    // 2D for now
    // get initial starting position and vector
    float xp = round(rand() / static_cast<float>(RAND_MAX) * 300.0);
    float yp = round(rand() / static_cast<float>(RAND_MAX) * 300.0);
    float zp = 0.0;
    gamephysics::PositionVector p0 = {xp, yp, zp };
    json jp = p0;
    std::cout << "Initial position: " << jp << std::endl;

    float xv = round(rand() / static_cast<float>(RAND_MAX) * 10.0);
    float yv = round(rand() / static_cast<float>(RAND_MAX) * 10.0);
    float zv = 0.0;
    gamephysics::VelocityVector v = {xv, yv, zv };
    json jv = v;
    std::cout << "Initial velocity: " << jv << std::endl;

    uint64_t ms0 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    float ms_to_start = 1000 - ms0 % 1000;
    ms0 = ms0 + ms_to_start;
    std::cout << "Initial time: " << ms0 << std::endl;
    for (;;) {
        sleep(1);
        uint64_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        if (ms - ms0 >=  heartbeat) {
            float dt = (ms - ms0)/heartbeat;
            
            //calculate next position after some delta time
            xp = round(xp + xv * dt);
            yp = round(yp + yv * dt);
            gamephysics::MotionVector mv = {ms, xp, yp, zp , xv, yv, zv };
            json jmv = mv;
            auto j_str = to_string(jmv);

            send(client_socket, j_str.data(), j_str.size(), 0);
            ms0 = ms;
    //        sleep(1);
            char buffer[1024] = {0};
            read(client_socket, buffer, sizeof(buffer));
            std::cout << "Server replied: " << buffer << std::endl;
            
        }
    }

    char next_message[1024] = "\\q";
    std::cout << next_message << std::endl;
    send(client_socket, next_message, strlen(next_message), 0);
 
    close(client_socket);

    return 0;
}
