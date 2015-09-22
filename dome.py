"""Dagor dome rotation interface version 1.1.

Usage:
    dome.py status [-v | --verbose]
    dome.py calibrate [-r | --reset]
    dome.py goto <azimuth>
    dome.py azimuth set <azimuth>
    dome.py azimuth get
    dome.py park
    dome.py door (open|close|stop)
    dome.py home (set|get)
    dome.py (up|down|stop)
    dome.py reset
    dome.py -h | --help
    dome.py --version

Commands:
    status         Dome status (azimuth \\n rotating or not \\n calibrated, during calibration or uncalibrated)
    calibrate      Start calibration.
    goto           Move dome to specified azimuth.
    azimuth set    Set current position as specified azimuth.
    azimuth get    Get current azimuth.
    park           Move dome to home azimuth.
    door           Open or close door.
    home set       Set home to current azimuth.
    home get       Get home azimuth.
    up             Start rotation of the dome (south-east-north-west)
    down           Start rotation of the dome (south-wast-north-east)
    stop           Stop rotation of the dome.
    reset          Reset Arduino.

Options:
    -h --help      Show this screen.
    --version      Show version.
"""
  
from docopt import docopt
import serial
import sys
from os import system
import dome_config


def read_serial(ser):
    """ Read data from serial port. Raise exception if error code is returned.
    
    @param ser: PySerial Serial() object
    @return: Returned data
    """
    
    raw_reply = ser.readline().strip()
    
    try:
        status, lines = raw_reply.split(' ')
    except ValueError:
        status, lines = (raw_reply, 0)
    
    reply = [ser.readline().strip() for _ in range(int(lines))]
    
    if status=='ok':
        return reply
    else:
        raise Exception(reply)


def write_serial(ser, str):
    """ Write to serial port, and then parse return message.
    
    @param ser: PySerial Serial() object
    @param str: String to be sent.
    @return: Parsed return message.
    """
    
    ser.write(str + '\n')
    return read_serial(ser)
    

def _main(args):
    ser = serial.Serial()
    ser.port = dome_config.PORT
    ser.baudrate = dome_config.BAUDRATE
    ser.timeout = dome_config.TIMEOUT

    ser.open()
    
    if args['status']:
        reply = write_serial(ser, 'status')
        if args['-v'] or args['--verbose']:
            position, rotation, calibration = reply
            print 'Azimuth: {}'.format(position)
            print 'Rotation: ' + {'0': 'Stop', '1': 'Down', '-1': 'Up'}[rotation]
            print 'Calibration: ' + {'0': 'In progress', '1': 'Done', '-1': 'Not calibrated'}[calibration]
        else:
            print '|'.join(reply)
    elif args['calibrate']:
        write_serial(ser, 'calibrate\n')
    elif args['goto']:
        write_serial(ser, 'goto' + args['<azimuth>'])
    elif args['azimuth']:
        if args['set']:
            write_serial(ser, 'force' + args['<azimuth>'])
        elif args['get']:
            print '\n'.join(write_serial(ser, 'current_azimuth'))
        else:
            _unknown_command()
    elif args['park']:
        write_serial(ser, 'park')
    elif args['door']:
        if args['open']:
            write_serial(ser, 'door_open')
        elif args['stop']:
            write_serial(ser, 'door_stop')
        elif args['close']:
            write_serial(ser, 'door_close')
        else:
            _unknown_command()
    elif args['home']:
        if args['set']:
            write_serial(ser, 'set_as_home')
        elif args['get']:
            print '\n'.join(write_serial(ser, 'home_azimuth'))
        else:
            _unknown_command()
    elif args['up']:
        write_serial(ser, 'up')
    elif args['down']:
        write_serial(ser, 'down')
    elif args['stop']:
        write_serial(ser, 'stop')
    else:
        _unknown_command()
    
    ser.close()
    
def _unknown_command():
    print __doc__
    sys.exit(1)

if __name__ == '__main__':
    args = docopt(__doc__, version=__doc__.split('\n'[0]))
    
    try:
        _main(args)
        sys.exit(0)
    except Exception as e:
        print(e)
        sys.exit(1)