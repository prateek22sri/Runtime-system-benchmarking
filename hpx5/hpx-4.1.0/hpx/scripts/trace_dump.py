import struct
import sys


FMT_STR = 'IIQQQQQ' # int, int padding, uint64_t * 5
FMT_STR_LEN = struct.calcsize(FMT_STR)

if __name__ == '__main__':
    '''
-f to specify file name to read
-a to specity file with action names to read
-c to specify which column is actions
    '''
    import getopt

    # set defaults
    filename = ""
    actions_filename = ""
    actions_column = -1

    # parse arguments
    opts, junk = getopt.getopt(sys.argv[1:], "f:a:c:")
    if len(opts) > 0:
        for flag, arg in opts:
            if flag == '-f':
                filename = arg.strip()
            elif flag == '-a':
                actions_filename = arg.strip()
            elif flag == '-c':
                actions_column = int(arg)

    # open input files
    f = open(filename)
    if actions_filename != "":
        af = open(actions_filename)
    else:
        af = None

    # skip header
    f.seek(12)
    start_raw = f.read(4)
    start = struct.unpack('I', start_raw)[0]
    f.seek(start)

    # read log file rows and output
    s_raw = f.read(FMT_STR_LEN)
    while len(s_raw) == FMT_STR_LEN:
        s = struct.unpack(FMT_STR, s_raw)
        if af == None:
            print s[0], ',', s[2], ',', s[3], ',', s[4], ',', s[5], ',', s[6]
        else:
            print s[0], ',', s[2], ',', s[3], ',', s[4], ',', s[5], ',', s[6]
        s_raw = f.read(FMT_STR_LEN)
