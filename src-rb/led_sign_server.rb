# Base class
require 'webrick'
require 'webrick/https'
require 'json'

class LedSignServer < WEBrick::HTTPServlet::AbstractServlet

    # Not meant to be used
    def do_GET( request, response )
        puts "#{__method__}"
        HTTPAuth.basic_auth( request, response, "VPCRealm" ) {|user,pass|
        user == 'locomobi' && pass == '70canuck' 
        }
        response.body = "Authenticated OK"
 
        status, content_type, body = handle_request( request )
        
        response = set_response( response, status, content_type, body )

    end

    # This does the real work
    def do_POST( request, response )
        puts "#{__method__}"
        # This next block does the authentication. If the authentication fails,
        # then the code does not continue past the authentication part.
        HTTPAuth.basic_auth( request, response, "VPCRealm" ) { |user, pass| 
            user == 'locomobi' && pass == 'firefighters'
        }
        status = handle_request( request ) 
        response = set_response( response, status.status, status.content_type, status.body );
    end
    private
    def handle_request( request )
        $logger.write1 "#{__method__}::request looks like: \n\*****\n#{request}\n******"
        @parsed_request = JSON.parse( request.body )
        $logger.write1 "#{__method__}::parsed request: \"#{@parsed_request}\""
    end
    def set_response( response, status, content_type, body )
        response.status = status
        response[ 'Content-Type' ] = content_type
        response[ 'Access-Control-Allow-Origin' ] = '*'
        response[ 'Access-Control-Request-Method' ] = '*'
        response.body = body
        return response
    end
end