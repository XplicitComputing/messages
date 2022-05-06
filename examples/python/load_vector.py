#!/usr/bin/env python3

about = "Load floating point data."

import argparse
import sys
import vector_pb2 as vector

channels = 2  # components


def load(msg, fname):
    "Deserialize 'msg' from file 'fname'."
    print("loading:", fname)
    try:
        with open(fname, "rb") as fin:
            serial = fin.read()
        msg.ParseFromString(serial)
    except Exception:
        exinfo = sys.exc_info()
        print(exinfo[1])  # error message
        return False

    print("data name:", msg.name)
    print("components:", msg.components)
    print("length:", len(msg.values) // msg.components)
    print("size:", len(msg.values))
    print("read", len(serial), "bytes from:", fname)

    return True


def main():
    parser = argparse.ArgumentParser(description=about)
    parser.add_argument("fname", type=str, help="output filename")

    args = parser.parse_args()

    # create a new protobuf message (see vector.proto for what's available)
    msg = vector.Vector64()

    # load msg from file fname
    result = load(msg, args.fname)
    if result == False:
        sys.exit(1)

    # do something else with msg, like load into a vector.
    # print(msg)      # list out all the values
    print("load complete.")


main()
