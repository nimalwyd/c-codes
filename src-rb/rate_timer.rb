class RateTimer
    attr_reader :led_mode
    def initialize( timeout, seven_segment_display )
        @timeout = timeout
        @seven_segment_display = seven_segment_display
        @led_mode = "BLANK"
        @rate_amount = 0
        start_timer
    end
    def set_rate( rate_to_display )
        @led_mode = "OPEN"
        @rate_amount = rate_to_display
    end
    private
    def start_timer
        return if @timer_thread != nil
        @timer_thread = Thread.new do
            loop do
                IO.select( nil, nil, nil, @timeout )

                if( @led_mode == "OPEN" )
                    puts "showing"
                    @seven_segment_display.show_number @rate_amount
                else
                    puts "blanking"
                    @seven_segment_display.show_blank
                end # case
            end # loop
        end # thread
    end # method start_timer
end