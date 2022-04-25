# Unit tests for Python bindings to
# Messages(TM) by Xplicit Computing, Inc.

import unittest
import sys


def msgv3_init():
    "Generate Vector64 message with 3 values."
    msg = vector.Vector64()
    msg.name = "Position|Value"
    msg.components = 3
    msg.values[:] = [1, 2, 3]
    return msg


def msgv6_init():
    "Generate Vector64 message with 6 values."
    msg = vector.Vector64()
    msg.name = "Position|Value"
    msg.components = 3
    msg.values[:] = [1, 2, 3, 4, 5, 6]
    return msg


class TestMessages(unittest.TestCase):
    def test_values_basic(self):
        msg = vector.Vector64()
        msg.name = "Position|Value"
        msg.components = 3
        msg.values.append(1)
        msg.values.append(2)
        msg.values.append(3)
        self.assertEqual(len(msg.values), 3)
        lll = msg.values
        self.assertEqual(msg.values, [1, 2, 3])

    def test_values_append_loop(self):
        msg = msgv3_init()
        other = [7, 8, 9]
        for val in other:
            msg.values.append(val)
        self.assertEqual(len(msg.values), 6)
        self.assertEqual(msg.values, [1, 2, 3, 7, 8, 9])

    def test_values_array_asgn(self):
        msg = msgv3_init()
        msg.values[:] = [3, 2, 1]
        self.assertEqual(msg.values, [3, 2, 1])
        msg.values[:] = [1, 2, 3, 4, 5, 6]
        self.assertEqual(msg.values, [1, 2, 3, 4, 5, 6])

    def test_values_extend(self):
        msg = msgv6_init()
        msg.values.extend([7, 8, 9])
        self.assertEqual(msg.values, [1, 2, 3, 4, 5, 6, 7, 8, 9])

    @unittest.expectedFailure
    def test_values_fail(self):
        msg = msgv3_init()
        print("expect fail: values do not match.")
        self.assertEqual(msg.values, [3, 2, 1])  # reversed order

    @unittest.expectedFailure
    def test_values_asgn(self):
        try:
            msg = msgv3_init()
            msg.values = [1, 2, 3]
        except Exception:
            print("expect fail: exception during value assign.")
        self.assertEqual(msg.values, [3, 2, 1])  # reversed order


if __name__ == "__main__":
    sys.path.insert(1, "../../bindings/python")
    # path within Messages Git repo
    import vector_pb2 as vector

    unittest.main()
