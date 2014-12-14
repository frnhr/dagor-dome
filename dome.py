"""Dagor dome rotation interface version 1.0.

Usage:
    dome.py status [-v | --verbose]
    dome.py calibration [-r | --reset]
    dome.py goto <azimuth>
    dome.py azimuth set <azimuth>
    dome.py azimuth get
    dome.py park
    dome.py door (open|close)
    dome.py home (set|get)
    dome.py (up|down|stop)
    dome.py reset
    dome.py -h | --help
    dome.py --version

Commands:
    status         Dome status (azimuth \\n rotating or not \\n calibrated, during calibration or uncalibrated)
    calibration    Start calibration.
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
    -h --help      Show this screen or description of specific command.
    --version      Show version."""
  
from docopt import docopt
import serial
import sys
import dome_config

def read_serial(ser):
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
    ser.write(str + '\n')
    return read_serial(ser)
    

def _main(args):
    ser = serial.Serial()
    ser.port = dome_config.PORT
    ser.baudrate = dome_config.BAUDRATE
    ser.timeout = dome_config.TIMEOUT
    ser.setDTR(args['-r'] or args['--reset'] or args['reset']); #don't reset unless explicitly specified
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
    elif args['calibration']:
        write_serial(ser, 'cs\n')
    elif args['goto']:
        write_serial(ser, 'ds' + args['<azimuth>']);
    elif args['azimuth'] and args['set']:
        write_serial(ser, 'force' + args['<azimuth>']);
    elif args['azimuth'] and args['get']:
        print '\n'.join(write_serial(ser, 'dg'))
    elif args['park']:
        write_serial(ser, 'dp')
    elif args['door']:
        if args['open']:
            write_serial(ser, 'do')
        else:
            write_serial(ser, 'dc')
    elif args['home']:
        if args['set']:
            write_serial(ser, 'hs')
        else:
            print '\n'.join(write_serial(ser, 'hg'))
    elif args['up']:
        write_serial(ser, 'up')
    elif args['down']:
        write_serial(ser, 'down')
    elif args['stop']:
        write_serial(ser, 'stop')
    
    ser.close()
    sys.exit(0)

if __name__ == '__main__':
    args = docopt(__doc__, version=__doc__.split('\n'[0]))
    
    try:
        _main(args)
    except Exception as e:
        print(e)
        sys.exit(1)
