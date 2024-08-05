/*
  Udp.cpp - UDP class for Raspberry Pi
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
#include <Arduino.h>
#include "WiFiUdp.h"
#include <new>  
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/inet.h>
#include <errno.h>
#include "tal_log.h"
#include "tal_network.h"

#undef write
#undef read



WiFiUDP::WiFiUDP()
: udp_server(-1)
, server_port(0)
, remote_port(0)
, tx_buffer(0)
, tx_buffer_len(0)
, rx_buffer(0)
{}

WiFiUDP::~WiFiUDP(){
   stop();
}

uint8_t WiFiUDP::begin(IPAddress address, uint16_t port){
  stop();

  server_port = port;

  tx_buffer = (char *)malloc(1460);
  if(!tx_buffer){
    PR_ERR("could not create tx buffer: %d\r\n", errno);
    return 0;
  }

  if ((udp_server = tal_net_socket_create(PROTOCOL_UDP)) == -1){
    PR_ERR("could not create socket: %d\r\n", errno);
    return 0;
  }

  int yes = 1;
  if (tal_net_setsockopt(udp_server,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
      PR_ERR("could not set socket option: %d\r\n", errno);
      stop();
      return 0;
  }
  uint32_t tmpIP = static_cast<uint32_t>(address);
  TUYA_IP_ADDR_T serverIP = (TUYA_IP_ADDR_T)UNI_HTONL(tmpIP);
  if(tal_net_bind(udp_server,serverIP,server_port) == -1){
    PR_ERR("could not bind socket: %d", errno);
    stop();
    return 0;
  }
  tal_net_set_block(udp_server,0);
  return 1;
}

uint8_t WiFiUDP::begin(uint16_t p){
  return begin(IPAddress(static_cast<uint32_t>INADDR_ANY), p);
}

uint8_t WiFiUDP::beginMulticast(IPAddress a, uint16_t p){
  if(begin(IPAddress(static_cast<uint32_t>INADDR_ANY), p)){
    if(!a){
      struct ip_mreq mreq;
      mreq.imr_multiaddr.s_addr = (in_addr_t)a;
      mreq.imr_interface.s_addr = INADDR_ANY;
      if (tal_net_setsockopt(udp_server, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
          PR_ERR("could not join igmp: %d", errno);
          stop();
          return 0;
      }
      multicast_ip = a;
    }
    return 1;
  }
  return 0;
}

void WiFiUDP::stop(){
  if(tx_buffer){
    free(tx_buffer);
    tx_buffer = NULL;
  }
  tx_buffer_len = 0;
  if(rx_buffer){
    cbuf *b = rx_buffer;
    rx_buffer = NULL;
    delete b;
  }
  if(udp_server == -1)
    return;
  if(!multicast_ip){
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = (in_addr_t)multicast_ip;
    mreq.imr_interface.s_addr = (in_addr_t)0;
    tal_net_setsockopt(udp_server, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    multicast_ip = IPAddress(static_cast<uint32_t>INADDR_ANY);
  }
  tal_net_close(udp_server);
  udp_server = -1;
}

int WiFiUDP::beginMulticastPacket(){
  if(!server_port || multicast_ip == IPAddress(static_cast<uint32_t>INADDR_ANY))
    return 0;
  remote_ip = multicast_ip;
  remote_port = server_port;
  return beginPacket();
}

int WiFiUDP::beginPacket(){
  if(!remote_port)
    return 0;

  // allocate tx_buffer if is necessary
  if(!tx_buffer){
    tx_buffer = (char *)malloc(1460);
    if(!tx_buffer){
      PR_ERR("could not create tx buffer: %d", errno);
      return 0;
    }
  }

  tx_buffer_len = 0;

  // check whereas socket is already open
  if (udp_server != -1)
    return 1;
 
  if ((udp_server = tal_net_socket_create(PROTOCOL_UDP)) == -1){
    PR_ERR("could not create socket: %d", errno);
    return 0;
  }

  tal_net_set_block(udp_server,0);
  return 1;
}

int WiFiUDP::beginPacket(IPAddress ip, uint16_t port){
  remote_ip = ip;
  remote_port = port;
  return beginPacket();
}

int WiFiUDP::beginPacket(const char *host, uint16_t port){
  TUYA_IP_ADDR_T addr;
  int res = tal_net_gethostbyname(host,&addr);
  if (res != OPRT_OK){
    PR_ERR("could not get host from dns: %d", errno);
    return 0;
  }
  return beginPacket(IPAddress((const uint8_t*)addr), port);
}

int WiFiUDP::endPacket(){
  int sent = tal_net_send_to(udp_server, tx_buffer, tx_buffer_len,(uint32_t)remote_ip,remote_port);
  if(sent < 0){
    PR_ERR("could not send data: %d", errno);
    return 0;
  }
  return 1;
}

size_t WiFiUDP::write(uint8_t data){
  if(tx_buffer_len == 1460){
    endPacket();
    tx_buffer_len = 0;
  }
  tx_buffer[tx_buffer_len++] = data;
  return 1;
}

size_t WiFiUDP::write(const uint8_t *buffer, size_t size){
  size_t i;
  for(i=0;i<size;i++)
    write(buffer[i]);
  return i;
}

int WiFiUDP::parsePacket(){
  if(rx_buffer)
    return 0;
  TUYA_IP_ADDR_T ip;
  uint16_t port;
  int len;
  char *buf = (char *)malloc(1460);
  if(!buf) {
    return 0;
  }
  if ((len = tal_net_recvfrom(udp_server, buf, 1460,&ip,&port)) == -1){
    free(buf);
    if(errno == EWOULDBLOCK){
      return 0;
    }
    PR_ERR("could not receive data: %d", errno);
    return 0;
  }
  remote_ip = IPAddress(ip);
  remote_port = port;
  if (len > 0) {
    rx_buffer = new(std::nothrow) cbuf(len);
    rx_buffer->write(buf, len);
  }
  free(buf);
  return len;
}

int WiFiUDP::available(){
  if(!rx_buffer) return 0;
  return rx_buffer->available();
}

int WiFiUDP::read(){
  if(!rx_buffer) return -1;
  int out = rx_buffer->read();
  if(!rx_buffer->available()){
    cbuf *b = rx_buffer;
    rx_buffer = 0;
    delete b;
  }
  return out;
}

int WiFiUDP::read(unsigned char* buffer, size_t len){
  return read((char *)buffer, len);
}

int WiFiUDP::read(char* buffer, size_t len){
  if(!rx_buffer) return 0;
  int out = rx_buffer->read(buffer, len);
  if(!rx_buffer->available()){
    cbuf *b = rx_buffer;
    rx_buffer = 0;
    delete b;
  }
  return out;
}

int WiFiUDP::peek(){
  if(!rx_buffer) return -1;
  return rx_buffer->peek();
}

void WiFiUDP::flush(){
  if(!rx_buffer) return;
  cbuf *b = rx_buffer;
  rx_buffer = 0;
  delete b;
}

IPAddress WiFiUDP::remoteIP(){
  return remote_ip;
}

uint16_t WiFiUDP::remotePort(){
  return remote_port;
}
