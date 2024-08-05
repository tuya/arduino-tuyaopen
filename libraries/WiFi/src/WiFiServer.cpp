/*
  Server.cpp - Server class for Raspberry Pi
  Copyright (c) 2016 Hristo Gochkov  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "WiFiServer.h"
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include "tal_log.h"
#include "tal_network.h"

#undef write
#undef close

int WiFiServer::setTimeout(uint32_t seconds){
  struct timeval tv;
  tv.tv_sec = seconds;
  tv.tv_usec = 0;
  if(tal_net_setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
    return -1;
  return tal_net_setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval));
}

size_t WiFiServer::write(const uint8_t *data, size_t len){
  return 0;
}

void WiFiServer::stopAll(){}

WiFiClient WiFiServer::available(){
  if(!_listening)
    return WiFiClient();
  int client_sock;
  if (_accepted_sockfd >= 0) {
    client_sock = _accepted_sockfd;
    _accepted_sockfd = -1;
  }
  else {
    client_sock = tal_net_accept(sockfd,NULL,NULL);
  }
  if(client_sock >= 0){
    int val = 1;
    if(tal_net_setsockopt(client_sock, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, sizeof(int)) == 0) {
      val = _noDelay;
      if(tal_net_setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&val, sizeof(int)) == 0){
        return WiFiClient(client_sock);
      }
    }
  }
  return WiFiClient();
}

void WiFiServer::begin(uint16_t port){
    begin(port, 1);
}

void WiFiServer::begin(uint16_t port, int enable){
  if(_listening)
    return;
  if(port){
      _port = port;
  }
  sockfd = tal_net_socket_create(PROTOCOL_TCP);
  if (sockfd < 0)
    return;

  tal_net_setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
  uint32_t tmpIP = static_cast<uint32_t>(_addr);
  TUYA_IP_ADDR_T serverIP = (TUYA_IP_ADDR_T)UNI_HTONL(tmpIP);
  if(tal_net_bind(sockfd,serverIP,_port)< 0)
    return;
  if(tal_net_listen(sockfd , _max_clients) < 0)
    return;
  tal_net_set_block(sockfd,false);
  _listening = true;
  _noDelay = false;
  _accepted_sockfd = -1;
  PR_INFO("server begin ok!\n");
}

void WiFiServer::setNoDelay(bool nodelay) {
    _noDelay = nodelay;
}

bool WiFiServer::getNoDelay() {
    return _noDelay;
}

bool WiFiServer::hasClient() {
    if (_accepted_sockfd >= 0) {
      return true;
    }
    _accepted_sockfd = tal_net_accept(sockfd,NULL,NULL);
    if (_accepted_sockfd >= 0) {
      return true;
    }
    return false;
}

void WiFiServer::end(){

  tal_net_close(sockfd);
  sockfd = -1;
  _listening = false;
}

void WiFiServer::close(){
  end();
}

void WiFiServer::stop(){
  end();
}

