SERVER_PORT = '2222'
LED_SERIAL_PORT_NAME = "/dev/ttyUSB0" # NOT USED ANYMORE
LED_SERIAL_PORT_BAUDRATE = 9600 # NOT USED ANYMORE
LED_SIGN_IDS = [ '1111', '2222' ]
LED_SIGN_ADDRESSES = [ 0x08, 0x04 ]
LED_TYPE = '5LED'
SCROLL_TIMER = 3
# OCCUPANCY/VACANCY MODE
SHOW_MODE = 'OCCUPANCY' # 'OCCUPANCY' for showing occupancy. 'VACANCY' for showing vacancy
# LOG FILE
LOG_FILE_LOCATION = '/var/lib/ledpark'
# MOXA SECTION
MOXA_IP_ADDRESS = '192.168.1.141'
MOXA_IP_PORT = 4001
MOXA_TEST_MODE = true # false for real moxa, true for testing