#include "server.h"
#include "json.hpp"
using json = nlohmann::json;

Server::Server(int port)  {
    _port = port;
    memset(_client, 0, sizeof(_client));
}

Status Server::start() {
    listen_port();
    if (!_status.ok()) return _status;
    
    int max_fd;

    while (true) {
        FD_ZERO(&_fdsr);  
        FD_SET(_fd, &_fdsr);  

        for (int i = 0; i < _max_client_size; i++)
            if (_client[i])
                FD_SET(_client[i]->fd(), &_fdsr);

        max_fd = _fd;
        int client_max_fd = cal_client_max_fd();
        if (client_max_fd > max_fd) max_fd = client_max_fd;

        if(-1 == select(max_fd+1, &_fdsr, NULL, NULL, NULL)) {  
            _status.set_ok(false);
            _status.set_msg("select error");
            return _status;  
        } else {
            printf("select ok\n");
        }

        // receive data or client close 
        for (int i = 0; i < _max_client_size; i++)
            if (_client[i] && FD_ISSET(_client[i]->fd(), &_fdsr))
                process_client(_client[i]);
             

        // new client connect
        if (FD_ISSET(_fd, &_fdsr)) {
            accept_client();
            if (!_status.ok()) return _status;
        }
    } // end while
    return _status;
}

void Server::listen_port() {
    if (!_status.ok()) return;

    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd == -1) {
        _status.set_ok(false);
        _status.set_msg("create socket error");
        return;
    } else {
        printf("create socket ok\n");
    }

    struct sockaddr_in socksrv;
    bzero(&socksrv, sizeof(socksrv));
    socksrv.sin_family = AF_INET; 
    socksrv.sin_addr.s_addr = htonl(INADDR_ANY);  
    socksrv.sin_port = htons(_port);
    int result = bind(_fd, (struct sockaddr*)&socksrv, sizeof(socksrv)); 
    if(result == -1) {  
        _status.set_ok(false);
        _status.set_msg("bind error");   
        return;
    } else {
        printf("bind ok\n");
    }

    result = listen(_fd, _max_client_size);
    if (result == -1) {
        _status.set_ok(false);
        _status.set_msg("listen error");     
        return;
    } else {
        printf("listen ok\n");
    }
}

int Server::cal_client_max_fd() {
    int result = -1;
    for (int i = 0; i < _max_client_size; i++) {
        if (_client[i] && _client[i]->fd() > result) {
            result = _client[i]->fd();
        }
    }
    return result;
}

void Server::accept_client() {
    struct sockaddr_in client_addr;
    socklen_t sin_size;
    int new_fd = accept(_fd, (struct sockaddr *)&client_addr, &sin_size);  
    if (new_fd <= 0) {
        _status.set_ok(false);
        _status.set_msg("accept error");
        return;
    } else {
        printf("accept client ok\n");
    }

    int have_find = false;
    for (int i = 0; i < _max_client_size; i++) {
        if (_client[i] == NULL) {
            have_find = true;
            _client[i] = new Client(client_addr, new_fd, i, this);// todo
            printf("store in %d\n", i);
            break;
        }
    }
    if (!have_find) {
        _status.set_ok(false);
        _status.set_msg("exceed max client number");
        return;
    }
}

void Server::process_client(Client* client_ptr) {
    int nread = recv(client_ptr->fd(), _buf, sizeof(_buf), 0);  
      
    if (nread <= 0) {        // client close  
        printf("client[%d] close\n", client_ptr->idx());  
        close(client_ptr->fd());  
        FD_CLR(client_ptr->fd(), &_fdsr);
        _client[client_ptr->idx()] = NULL;
        delete client_ptr;
    } else {        // receive data
        json recv_json;
        _buf[nread] = '\0';
        try {
            recv_json = json::parse(_buf, _buf+nread);
        } catch (std::exception e) {
            printf("json parse error\n");
            printf("client[%d] send:%s\n", client_ptr->idx(), _buf);
            return;
        }
        printf("client[%d] send:%s\n", client_ptr->idx(), recv_json.dump(4).c_str());  
        try {
            client_ptr->transfer(recv_json);
        } catch (std::exception e) {
            printf("transfer error\n");
            return;
        }
    }
}

json Server::client_list(Client* client_ptr) {
    json result;
    int cnt = 0;
    result["self"] = {{"fd", client_ptr->fd()}, {"name", client_ptr->name()}};
    for (int i = 0; i < _max_client_size; i++) {
        if (_client[i]) {
            json tmp = {{"fd", _client[i]->fd()}, {"name", _client[i]->name()}};
            tmp["playing"] = _client[i]->playing();
            tmp["state"] = _client[i]->state_string();
            result["list"].push_back(tmp);
        }
    }
    return result;
}

Client* Server::find_client_ptr_by_fd(int fd) {
    for (int i = 0; i < _max_client_size; i++) {
        if (_client[i] && _client[i]->fd() == fd) {
            return _client[i];
        }
    }
    return NULL;
}