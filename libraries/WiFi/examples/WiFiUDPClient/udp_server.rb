require 'socket'
include Socket::Constants

udp_socket = UDPSocket.new(AF_INET)

#bind 
udp_socket.bind("", 3333)
puts 'Server listening'

while true do
  message, sender = udp_socket.recvfrom(1024)
  puts message
end