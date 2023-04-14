import argparse
import os

parser = argparse.ArgumentParser()
parser.add_argument('-m', dest='method', type=str, help='encrypt||decrypt||check')
parser.add_argument('-t', dest='type', type=str, help='128||192||256')
parser.add_argument('-f', dest='filename', type=str, help='File To Use')
args = parser.parse_args()

if args.filename is not None:
    message = open(args.filename).read()
    pipe = os.popen("./AES-128_192_256 " + args.method + " "+ args.type+" " + args.filename)
    print("result:", pipe.read(), end='')