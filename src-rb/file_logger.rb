require 'thread'

class FileLogger
    @@write_mutex = Mutex.new
    def initialize( filename )
        begin
            @fd = File.open( filename, 'a' )
        rescue IOError => e
            puts "Error opening file: #{e}"
        end
    end
    def write1( message )
        begin
            time_now = Time.now.ctime
            full_message = "#{time_now} - #{message}"
            puts full_message
            @@write_mutex.synchronize do
                @fd.write( "#{full_message}\r\n" )
                @fd.flush
            end
        rescue IOError => e
            puts "Error: #{e}"
            @fd.close if fd
        end
    end
    def write( client, message )
        self.write1 "client: #{client.peeraddr[2]}:#{client.peeraddr[1]} - #{message}"
    end
end