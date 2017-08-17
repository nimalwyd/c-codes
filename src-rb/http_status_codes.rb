# This is a subset of the HTTP status codes

class HttpStatusCode
    attr_reader :status
    attr_reader :content_type
    attr_reader :body
    def initialize( status, body )
        @status = status;
        @content_type = "text/plain"
        @body = body
    end
end

class HttpStatusCodeDictionary
    def initialize()
        create_status_map
    end
    def successful( )
        return @status_dictionary[ "200" ];
    end
    def bad_request( )
        return @status_dictionary[ "400" ];
    end
    def unauthorized( )
        return @status_dictionary[ "401" ];
    end
    def forbidden( )
        return @status_dictionary[ "403" ];
    end
    def not_found( )
        return @status_dictionary[ "404" ];
    end
    private
    def create_status_map
        @status_dictionary = Hash.new
        @status_dictionary[ "200" ] = HttpStatusCode.new( "200", "Successful" )
        @status_dictionary[ "400" ] = HttpStatusCode.new( "400", "Bad request" )
        @status_dictionary[ "401" ] = HttpStatusCode.new( "401", "Unauthorized" )
        @status_dictionary[ "403" ] = HttpStatusCode.new( "403", "Forbidden" )
        @status_dictionary[ "404" ] = HttpStatusCode.new( "404", "Not found" )
    end
end
