require 'socket'
require_relative 'seven_segment_display'
require_relative 'libMoxaSerialPort'
require_relative 'seven_segment_display.conf'

sp = MoxaSerialPort.new( '192.168.2.19', 4001 )

myTestObj = SevenSegmentDisplayLibrary5Led.new( sp, 0x08, false )

puts "Showing Open"
myTestObj.show_open
sleep 5
puts "Showing full"
myTestObj.show_full
sleep 5
puts "Showing closed"
myTestObj.show_closed
sleep 5
puts "Showing 42"
myTestObj.show_number( 42 )
sleep 5
puts "Showing 666"
myTestObj.show_number( 666 )
sleep 5
puts "Showing 9999"
myTestObj.show_number( 9999 )
sleep 5
puts "Showing Blank"
myTestObj.show_blank
sleep 1
puts "Showing 10000"
begin
    myTestObj.show_number( 10000 )
rescue NumberOutOfRangeException => e
    puts "Caught NumberOutOfRangeException: #{e}"
end

sp.close
