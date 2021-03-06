"""Dagor dome rotation interface version 1.0.

Usage:
    dome.py status
    dome.py calibration [-r | --reset]
    dome.py azimuth set <azimuth> [-f | --force]
    dome.py azimuth get
    dome.py park
    dome.py door (open|close)
    dome.py home (set|get)
    dome.py manual (up|down|stop)
	dome.py reset
    dome.py -h | --help
    dome.py --version

Commands:
    status         Dome status (azimuth \\n rotating or not \\n calibrated, during calibration or uncalibrated)
    calibration    Start calibration.
    azimuth set    Move dome to specified azimuth.
    azimuth get    Get current azimuth.
    park           Move dome to home azimuth.
    door           Open or close door.
    home set       Set home to current azimuth.
    home get       Get home azimuth.
    manual         Start or stop rotation of dome.
	reset          Reset Arduino Mega.

Options:
    -h --help      Show this screen or description of specific command.
    --version      Show version.
    -f --force     Replace current azimuth with new one without moving dome.
	-r --reset     Reset Arduino before executing command."""
  
from docopt import docopt
import serial

def _main(args):
    ser = serial.Serial()
	ser.port = '/dev/ttyACM0'
	ser.baudrate = 115200
	ser.timeout = 1
	ser.setDTR(args['-r'] or args['--reset'] or args['reset']); #don't reset unless explicitly specified
	ser.open()
	
    if args['status']:
	    ser.write('status\n')
		print ser.readline()
	elif args['calibration']:
	    ser.write('cs\n')
	elif args['azimuth'] and args['set']:
        if args['--force']:
            ser.write('force' + args['<azimuth>'] + '\n');
        else:
            ser.write('ds' + args['<azimuth>'] + '\n');
            if ser.readline()=='ec':
                raise Exception('Calibration is not done')
	elif args['azimuth'] and args['get']:
	    ser.write('dg\n')
		reply = ser.readline()
        if reply=='ec':
            raise Exception('Calibration is not done')
        else:
            print reply
    elif args['park']:
        ser.write('dp\n')
        if ser.readline()=='ec':
            raise Exception('Calibration is not done')
    elif args['door']:
        if args['open']:
            ser.write('do\n')
        else:
            ser.write('dc\n')
    elif args['home']:
        if args['set']:
            ser.write('hs\n')
            if ser.readline()=='ec':
                raise Exception('Calibration is not done')
        else:
            ser.write('hg\n')
            print ser.readline()
    elif args['manual']:
        if args['up']:
            ser.write('up\n')
        elif args['down']:
            ser.write('down\n')
        else:
            ser.write('stop\n')
    
	ser.close()
	exit(0)

if __name__ == '__main__':
    args = docopt(__doc__, version=__doc__.split('\n'[0])

    if len(sys.argv) == 1 or args['-h'] or args['--help']:
        print __doc__.strip()
        exit(0)

    try:
        _main(args)
    except:
        raise
        #exit_('ERROR')