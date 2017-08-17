require_relative 'exceptions'
require_relative 'rate_timer'
require 'sqlite3'

class RateData
    attr_reader :led_id
    attr_reader :current_rate
    attr_reader :current_rate_description
    attr_reader :associated_seven_segment_display

    def initialize( logger, led_id, seven_segment_display, update_time )
        @logger = logger
        @led_id = led_id
        @associated_seven_segment_display = seven_segment_display
        @db_name = '/var/lib/vpark/rate.db'
        @current_rate = 0
        @current_rate_description = "None"

        @logger.write1 "RateData::#{__method__}: led_id = #{@led_id} | update_time = #{update_time}"
        @timer = RateTimer.new( update_time, seven_segment_display )
        get_rate
    end
    
    def parse( parsed_request )
        rateName = parsed_request[ 'currentRateName' ]
        rateAmount = ( parsed_request[ 'currentRateAmount' ].to_f * 100 ).to_i
        @logger.write1 "#{__method__}:: Got rate: #{rateName} ==> #{rateAmount}"
        if( rateAmount > 0 && rateAmount < 99999 )
            # Store the rate in the DB and locally
            set_rate( rateAmount, rateName )
            # Show the rate
            show
        else
            @logger.write1 "Error - number out of range"
            raise ParseException
        end
    end
    
    def show
        @timer.set_rate @current_rate
    end
    
    def to_s
        "rate_data:: LED ID: #{@led_id}, ASS: #{@associated_seven_segment_display}, db_name: #{@db_name}, #{@current_rate}, #{@current_rate_description}"
    end
    
    private
    
    def set_rate( rate, rate_description )
        @current_rate = rate
        @current_rate_description = rate_description
        begin
            rate, description = read_from_db
            if( rate != @current_rate )
                db = SQLite3::Database.new @db_name
                db.execute( "INSERT INTO rate_table ( RateAmount, RateDescription )	VALUES ( #{@current_rate}, \'#{@current_rate_description}\' )" )
                db.close if db
            end
        rescue Exception => ex
            @logger.write1 "#{__method__}:: Got exception ==> #{ex}"
            db.close if db
        end
    end
    def get_rate
        puts "#{__method__} #{self}"
        rate, description = read_from_db
        if( rate != @current_rate )
            @current_rate = rate
            @current_rate_description = description
        end
        puts "#{__method__} #{self}"
        if( @current_rate != 0 )
            puts "current rate = #{@current_rate}"
            show
        else
            @logger.write1 "#{__method__}:: There is no data in DB (not showing rates yet!)"
        end
    end
    
    def read_from_db
        rate_amount = 0
        rate_description = "None"

        begin
            db = SQLite3::Database.new @db_name
            rates_from_db = db.execute( "SELECT RateAmount, RateDescription FROM rate_table WHERE Id = ( SELECT MAX( Id ) FROM rate_table )" )
            if( rates_from_db.count == 1 )
                rate_amount = rates_from_db[ 0 ][ 0 ] 
                rate_description = rates_from_db[ 0 ][ 1 ]
            end
            db.close if db
        rescue Exception => ex
            @logger.write1 "#{__method__}:: Got exception ==> #{ex}."
        end
        return [ rate_amount, rate_description ]
    end
end