require_relative 'led_sign_server'
require_relative 'version'
require_relative 'http_status_codes'
require_relative 'signserver.conf'
require_relative 'seven_segment_display'
require_relative 'libMoxaSerialPort'
require_relative 'libMoxaMock'
require_relative 'file_logger'
require_relative 'rate_data'
require_relative 'exceptions'

include WEBrick

def send_to_ledsignage( command )
    puts "#{__method__} -- Executing command: #{command}"
    `#{command}`
end

class LedSignServerRate < LedSignServer
    #
    # The following is the expected JSON:
    # {
    #   "currentRateName": "Special event rate",
    #   "currentRateAmount": "5.00",
    # }
    #
    def handle_request( request )
        super( request ) # run the base class implementation first
        if( @parsed_request.has_key?( 'currentRateAmount' ) )
            return show_rate_on_sign( request, @parsed_request );
        else
            return $status_codes.bad_request
        end
    end
    def show_rate_on_sign( request, parsed_request )
        $rate_data.keys.each do | led_id |

            begin 
                $rate_data[ led_id ].parse( parsed_request )
                $rate_data[ led_id ].show
            rescue ParseException => pe
                $logger.write1 "Caught ParseException"
                return $status_codes.bad_request
            end
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

def initialize_rate_data( )
    $rate_data = Hash.new
    $seven_segment_displays.keys.each do | led_id |
        $rate_data[ led_id ] = RateData.new( $logger, led_id, $seven_segment_displays[ led_id ], SCROLL_TIMER )
    end
    puts "rate_data =  #{$rate_data}"
end

# To support multiple signs, $seven_segment_displays
# holds the total signs.

$logger = FileLogger.new( "#{LOG_FILE_LOCATION}/log.txt" )

$logger.write1 "Starting LED server (RATE). #{VERSION}"

verify_configuration()

serialPort = nil
if( MOXA_TEST_MODE )
    serialPort = MoxaSerialPortMock.new( $logger, MOXA_IP_ADDRESS, MOXA_IP_PORT )
else
    serialPort = MoxaSerialPort.new( $logger, MOXA_IP_ADDRESS, MOXA_IP_PORT )
end

initialize_led_signs( serialPort )

initialize_rate_data( )

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

server.mount "/rate", LedSignServerRate

# This handles the CTRL+C
trap( 'INT' ) do 
    puts "Good-bye!"
    server.stop 
end

server.start
