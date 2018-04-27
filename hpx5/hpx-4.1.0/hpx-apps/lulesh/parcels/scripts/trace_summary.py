import sys
import trace
import argparse

parser=argparse.ArgumentParser(description="Summarize lulesh trace output.")
parser.add_argument('--width', action='store', default=80, type=int,
                               help='width of output terminal')
parser.add_argument('--field', dest='fields', action='append', default=[],
                               choices=['absolute','offset','relative'], 
                               help='list of fields to summarize')
args=parser.parse_args()

if len(args.fields) == 0:
   args.fields.append('offset')
action=trace.width(args.width)
samples=trace.load(sys.stdin)
summary=trace.summarize(samples, args.fields)
print(summary.join(trace.events))
