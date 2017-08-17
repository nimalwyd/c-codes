require 'socket'

moxa_client = TCPSocket.new '192.168.2.19', 4001

bytes_to_send = [ 0x16, 0x16, 0x02, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x03 ]
puts "Attempting to send following bytes: "
bytes_to_send.each do |byte|
    puts "%02X" % [ byte ]
end

bytes_sent = moxa_client.write bytes_to_send.pack('C*') # this pack business is tres important

puts "Sent #{bytes_sent} bytes"