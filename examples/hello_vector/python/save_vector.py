#!/usr/bin/env python3

about = "Generate and save floating point data."

import math
import datetime
import argparse
import sys
import vector_pb2 as vector

channels = 2  # components


def timestr():
    "Return the current date and time in ISO 8601 format down to seconds."
    now = datetime.datetime.now()
    return now.isoformat(timespec="seconds")


def fill_data(msg, args):
    "Fill data, in this case, with example sine sweeps on two channels."
    print("filling data... ", end="", flush=True)
    msg.name = args.fname  # use filename for name
    msg.components = channels
    N = args.Nsamps
    w_max = args.w_max
    for n in range(N):
        t = n / (N - 1)
        channel0 = math.sin(w_max * t * t)
        channel1 = math.cos(w_max * t * t)
        msg.values.extend([channel0, channel1])
    msg.comment = f"w_max={w_max} N={N} C={channels} {timestr()}"
    print("done.")


def save(msg, fname):
    "Serialize 'msg' and save as file 'fname'."
    try:
        serial = msg.SerializeToString()
        with open(fname, "wb") as sout:
            sout.write(serial)
    except Exception:
        exinfo = sys.exc_info()
        print(exinfo[1])  # error message
        return False

    print("data name:", msg.name)
    print("components:", msg.components)
    print("length:", len(msg.values) // msg.components)
    print("size:", len(msg.values))
    print("wrote", len(serial), "bytes to:", fname)

    return True


def main():
    parser = argparse.ArgumentParser(description=about)
    parser.add_argument(
        "-N", "--Nsamps", type=int, default=10000, help="number of sample per channel"
    )
    parser.add_argument(
        "-w", "--w_max", type=float, default=100.0, help="w_max frequency end of sweep"
    )
    parser.add_argument("fname", type=str, help="output filename")

    args = parser.parse_args()

    # create a new protobuf message (see vector.proto for what's available)
    msg = vector.Vector64()

    # fill buffer with data... in this case, sine sweeps
    fill_data(msg, args)

    # when ready to save or transmit
    result = save(msg, args.fname)
    if result == False:
        sys.exit(1)


main()
