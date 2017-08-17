require_relative 'exceptions'
require_relative 'dumb_timer'

class OccupancyData
    attr_reader :led_id
    attr_reader :current_vacancy
    attr_reader :current_occupancy
    attr_reader :full
    attr_reader :closed
    attr_reader :open
    attr_reader :show_mode
    attr_reader :associated_seven_segment_display

    def initialize( logger, led_id, seven_segment_display, show_mode, blink_time )
        @logger = logger
        @led_id = led_id
        #@associated_seven_segment_display = seven_segment_display
        @show_mode = show_mode
        
        @logger.write1 "OccupancyData constructor"
        @timer = DumbTimer.new( blink_time, seven_segment_display )
    end

    def parse( parsed_request )
        @led_id = parsed_request[ 'ledId' ]
        occupancy = parsed_request[ 'currentOccupancy' ]
        vacancy = parsed_request[ 'currentVacancy' ]

        full_message = parsed_request[ 'fullMessage' ]
        open_message = parsed_request[ 'openMessage' ]
        closed_message = parsed_request[ 'closedMessage' ]

        if( ( current_occupancy == nil || current_vacancy == nil ) && open_message == nil && full_message == nil && closed_message == nil )
            @logger.write1 "Parse error for led id: #{@led_id}"
            raise ParseException
        end

        @current_occupancy = occupancy.to_i
        @current_vacancy = vacancy.to_i
        @full = !full_message.empty?
        @closed = !closed_message.empty?
        @open = !open_message.empty?

        @logger.write1 "#{__method__}:: led_id=#{@led_id} | current_occupancy=#{@current_occupancy} | current_vacancy=#{@current_vacancy} | full=#{@full} | closed=#{@closed} | open=#{@open}"
    end

    def show
        if( @closed )
            @logger.write1 "Showing lot closed"
            @timer.show_closed
        elsif( @full )
            @logger.write1 "Showing lot full"
            @timer.show_full
        else
            @logger.write1 "Showing current occupancy"
            num_to_show = @current_occupancy if @show_mode == "OCCUPANCY"
            num_to_show = @current_vacancy if @show_mode == "VACANCY"
            @timer.set_number( num_to_show )
            @timer.show_open
        end
    end
end
    