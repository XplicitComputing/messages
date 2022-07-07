#!/usr/bin/env python3

# Unit tests for Python bindings to
# Messages(TM) by Xplicit Computing, Inc.

import unittest
from copy import deepcopy
import hashlib
import tempfile
import os, sys


def msgv3_init():
    "Generate Vector64 message with 3 values."
    msgv3 = vector.Vector64()
    msgv3.name = "Position|Value"
    msgv3.components = 3
    msgv3.values[:] = [1, 2, 3]
    return msgv3


msgv3_sha1 = (
    "d9e4f31c22933a7f865a1ebe41e797cf24c4b464"  # SHA1 hexdigest of serialized msgv3
)


def msgv6_init():
    "Generate Vector64 message with 6 values."
    msgv6 = vector.Vector64()
    msgv6.name = "Position|Value"
    msgv6.components = 3
    msgv6.values[:] = [1, 2, 3, 4, 5, 6]
    return msgv6


msgv6_sha1 = (
    "2baee8d6c42c04b94a2d744c92186aded881ac55"  # SHA1 hexdigest of serialized msgv6
)


class TestMessages(unittest.TestCase):
    def test_values_basic(self):
        "Basic Vector64 creation and appending values"
        msg = vector.Vector64()
        msg.name = "Position|Value"
        msg.components = 3
        msg.values.append(1)
        msg.values.append(2)
        msg.values.append(3)
        self.assertEqual(len(msg.values), 3)
        self.assertEqual(msg.values, [1, 2, 3])

    def test_values_append_loop(self):
        "values appended in for-loop"
        msg = msgv3_init()
        other = [7, 8, 9]
        for val in other:
            msg.values.append(val)
        self.assertEqual(len(msg.values), 6)
        self.assertEqual(msg.values, [1, 2, 3, 7, 8, 9])

    def test_values_array_asgn(self):
        "values array assignment"
        msg = msgv3_init()
        msg.values[:] = [3, 2, 1]
        self.assertEqual(msg.values, [3, 2, 1])
        msg.values[:] = [1, 2, 3, 4, 5, 6]
        self.assertEqual(msg.values, [1, 2, 3, 4, 5, 6])

    def test_values_extend(self):
        "values array extend by list"
        msg = msgv6_init()
        msg.values.extend([7, 8, 9])
        self.assertEqual(msg.values, [1, 2, 3, 4, 5, 6, 7, 8, 9])

    def test_serialize(self):
        "serialize to string, deserialize from string, validate SHA1"

        # serialization
        msg1 = msgv3_init()
        serial = msg1.SerializeToString()
        s1 = hashlib.sha1()
        s1.update(serial)
        m1sha1 = s1.hexdigest()
        self.assertEqual(m1sha1, msgv3_sha1)  # match precomputed SHA1 hexdigest

        # deserialization
        msg2 = vector.Vector64()
        msg2.ParseFromString(serial)
        self.assertEqual(msg1, msg2)

    def test_serialize_file(self):
        "serialize to file, deserialize from file"

        # create file with zero length
        handle, path = tempfile.mkstemp(suffix=".xco", prefix="msgtest_")
        os.close(handle)
        fsize = os.stat(path).st_size
        self.assertEqual(fsize, 0)

        # create msg1 and write to file
        msg1 = msgv6_init()  # msg1 = vector.Vector64(), and more
        serial1 = msg1.SerializeToString()
        with open(path, "wb") as fout:
            fout.write(msg1.SerializeToString())

        # sanity check: validate against SHA1 for msgv6
        s1 = hashlib.sha1()
        s1.update(serial1)
        m1sha1 = s1.hexdigest()
        self.assertEqual(m1sha1, msgv6_sha1)

        # file size should match serialized msg1
        fsize = os.stat(path).st_size
        self.assertEqual(fsize, len(serial1))

        # create msg2 and read from file
        msg2 = vector.Vector64()
        with open(path, "rb") as fin:
            serial2 = fin.read()
        msg2.ParseFromString(serial2)

        # serialized stings should match, message structures should match
        self.assertEqual(serial1, serial2)
        self.assertEqual(msg1, msg2)

        # remove file
        os.remove(path)

    def test_copy_ref(self):
        "copy by reference - two references, same object"
        msg1 = msgv3_init()
        msg2 = msg1
        msg2.values.append(9)
        self.assertEqual(msg2.values[3], 9)
        self.assertEqual(len(msg1.values), 4)
        self.assertEqual(len(msg1.values), len(msg2.values))

    def test_copy_deep(self):
        "deep copies - two objects"
        msg1 = msgv3_init()
        msg2 = deepcopy(msg1)
        msg2.values.append(9)
        self.assertEqual(len(msg1.values), 3)
        self.assertEqual(len(msg2.values), 4)
        self.assertNotEqual(len(msg1.values), len(msg2.values))

    @unittest.expectedFailure
    def test_df1(self):
        "deliberate fail: values vs list mismatch"
        msg = msgv3_init()
        print(self.test_df1.__doc__)
        self.assertEqual(msg.values, [3, 2, 1])  # reversed order

    @unittest.expectedFailure
    def test_df2(self):
        "deliberate fail: exception during value assign"
        succ = None
        try:
            msg = msgv3_init()
            msg.values = [1, 2, 3]
            # should be msg.values[:] = ...
        except Exception:
            succ = False
        else:
            succ = True
        print(self.test_df2.__doc__)
        self.assertTrue(succ)


if __name__ == "__main__":
    sys.path.insert(1, "../../bindings/python")
    # path within Messages Git repo
    import vector_pb2 as vector

    unittest.main()
