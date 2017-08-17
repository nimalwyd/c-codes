require_relative 'led_sign_server'
require_relative 'version'
require_relative 'http_status_codes'
require_relative 'signserver.conf'
require_relative 'seven_segment_display'
require_relative 'libMoxaSerialPort'
require_relative 'libMoxaMock'
require_relative 'file_logger'
require_relative 'occupancy_data'
require_relative 'exceptions'

include WEBrick

def send_to_ledsignage( command )
    puts "#{__method__} -- Executing command: #{command}"
    `#{command}`
end



# API for occupancy
class LedSignServerOccupancy < LedSignServer
    def initialize( server )
        super( server )
        @current_vacancy = Hash.new
        @show_mode = "BLANK"
    end
    private
    #
    # The following is the expected JSON:
    # {
    #   "currentOccupancy": "100",
    #   "currentVacancy": "0",
    #   "fullMessage": "Lot Full",
    #   "openMessage": "",
    #   "closedMessage": "",
    #   "ledId" : "1"
    # }
    #
    def handle_request( request )
        super( request ) # class the base class implementation first
        if( @parsed_request.has_key?( 'currentOccupancy' ) )
            return show_occupancy_on_sign( request, @parsed_request );
        else
            return $status_codes.bad_request
        end
    end

    private
    #
    # The following is the expected JSON:
    # {
    #   "currentOccupancy": "100",
    #   "currentVacancy": "0",
    #   "fullMessage": "Lot Full",
    #   "openMessage": "",
    #   "closedMessage": "",
    #   "ledId: "1111"
    # }
    #
    def show_occupancy_on_sign( request, parsed_request )
        led_id = parsed_request[ 'ledId' ]
        
        puts "got led_id => #{led_id}"
        if( $occupancy_data.has_key?( led_id ) )
            begin 
                $occupancy_data[ led_id ].parse( parsed_request )
                $occupancy_data[ led_id ].show
            rescue ParseException => pe
                $logger.write1 "Caught ParseException"
                return $status_codes.bad_request
            end
        else
            return $status_codes.bad_request
        end

        request.query do | key, value |
            puts 'key: #{key} - value: #{value}' 
        end

        return $status_codes.successful
    end
end

def verify_configuration
    if( LED_SIGN_ADDRESSES == nil || 
        LED_SIGN_IDS == nil ||
        LED_SIGN_ADDRESSES.count != LED_SIGN_IDS.count )
        $logger.write1( "CONFIGURATION error - exiting" )
        Kernel.exit( -1 )
    end
end

def initialize_led_signs( serialPort )
    $seven_segment_displays = Hash.new
    if( LED_TYPE == '5LED' )
        $logger.write1 "5-LED signs are configured"
        for i in 0..LED_SIGN_ADDRESSES.count do
            $seven_segment_displays[ LED_SIGN_IDS[ i ]  ] = SevenSegmentDisplayLibrary5Led.new( serialPort, LED_SIGN_ADDRESSES[ i ], false )
            $logger.write1 "Configured: #{LED_SIGN_IDS[ i ]} ==> #{LED_SIGN_ADDRESSES[ i ]}" 
        end
    else
        $logger.write1 "4-LED signs are configured"
        $logger.write1 "Found #{LED_SIGN_ADDRESSES.count} signs in configuration"
        for i in 0...LED_SIGN_ADDRESSES.count do
            $seven_segment_displays[ LED_SIGN_IDS[ i ] ] = SevenSegmentDisplayLibrary.new( serialPort, LED_SIGN_ADDRESSES[ i ], false )
            $logger.write1 "Configured: #{LED_SIGN_IDS[ i ]} ==> #{LED_SIGN_ADDRESSES[ i ]}"
        end
    end
    puts "seven_segment_displays = #{$seven_segment_displays}"
end

def initialize_occupancy_data( )
    $occupancy_data = Hash.new
    $seven_segment_displays.keys.each do | led_id |
        $occupancy_data[ led_id ] = OccupancyData.new( $logger, led_id, $seven_segment_displays[ led_id ], SHOW_MODE, SCROLL_TIMER )
    end
    puts "occupany_data =  #{$occupancy_data}"
end

# To support multiple signs, $seven_segment_displays
# holds the total signs.

$logger = FileLogger.new( "#{LOG_FILE_LOCATION}/log.txt" )

$logger.write1 "Starting LED server. #{VERSION}"

verify_configuration()

serialPort = nil
if( MOXA_TEST_MODE )
    serialPort = MoxaSerialPortMock.new( MOXA_IP_ADDRESS, MOXA_IP_PORT )
else
    serialPort = MoxaSerialPort.new( MOXA_IP_ADDRESS, MOXA_IP_PORT )
end

initialize_led_signs( serialPort )

initialize_occupancy_data( )

puts "============"
$seven_segment_displays.each do |ledid, ss|
    puts ledid
end
puts "============"

$status_codes = HttpStatusCodeDictionary.new
root = File.expand_path '~/public'

# Configure the password file 
config = { :Realm => 'VPCRealm' }

htpasswd = WEBrick::HTTPAuth::Htpasswd.new 'signpasswd'
htpasswd.set_passwd config[ :Realm ], 'locomobi', 'password'
htpasswd.flush

config[ :UserDB ] = htpasswd

basic_auth = WEBrick::HTTPAuth::BasicAuth.new config

# Use a self-signed certificate
cert_name = [
    %w[CN localhost],
]

# Start the server
server = WEBrick::HTTPServer.new(
    :BindAddress => '0.0.0.0',
    :Port => SERVER_PORT,
    :DocumentRoot => root,
#    :SSLEnable => true,
#    :SSLCertName => cert_name
)

server.mount "/occupancy", LedSignServerOccupancy

# This handles the CTRL+C
trap( 'INT' ) do 
    puts "Good-bye!"
    server.stop 
end

server.start
