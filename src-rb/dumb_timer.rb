class DumbTimer
    attr_reader :led_mode
    def initialize( timeout, seven_segment_display )
        @timeout = timeout
        @seven_segment_display = seven_segment_display
        @led_mode = "BLANK"        
        start_timer
    end
    def show_closed
        @led_mode = "CLOSED"
    end
    def show_open
        @led_mode = "OPEN" if( @led_mode != "OPEN" || @led_mode != "NUM" )
    end
    def show_full
        @led_mode = "FULL"
    end
    def set_number( number )
        @number_to_display = number
    end
    private
    def start_timer
        return if @timer_thread != nil
        @timer_thread = Thread.new do
            loop do
                IO.select( nil, nil, nil, @timeout )

                if( @led_mode == "CLOSED" )
                    @seven_segment_display.show_closed
                elsif( @led_mode == "FULL" )
                    @seven_segment_display.show_full
                elsif( @led_mode == "NUM" )
                    @seven_segment_display.show_number @number_to_display
                    @led_mode = "OPEN"
                elsif( @led_mode == "OPEN" )
                    @seven_segment_display.show_open
                    @led_mode = "NUM"
                else
                    puts "doing nothing because @led_mode=#{@led_mode}"
                end # case
            end # loop
        end # thread
    end # method start_timer
end