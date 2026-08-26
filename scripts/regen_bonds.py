#!/usr/bin/env python3

import datetime
import os
import re
import sys

HEADER = os.path.join(os.path.dirname(__file__), "..", "config/boards/shields/enki42/proteus_bonds.h")


def addr_macro(name):
    a = bytearray(os.urandom(6))
    a[5] |= 0xC0
    mac = ":".join("%02X" % b for b in reversed(a))
    vals = ", ".join("0x%02X" % b for b in a)
    return "/* MAC %s */\n#define %s { \\\n\t.type = BT_ADDR_LE_RANDOM, \\\n\t.a = { .val = { %s } } \\\n}" % (mac, name, vals)


def ltk_macro(name):
    k = os.urandom(16)
    row = lambda bs: ", ".join("0x%02x" % b for b in bs)
    return "#define %s { \\\n\t%s, \\\n\t%s  \\\n}" % (name, row(k[:8]), row(k[8:]))


def sub(text, name, new):
    pat = r"(/\* MAC [^\n]*\*/\n)?#define %s \{.*?\n\}" % name
    text, n = re.subn(pat, lambda m: new, text, flags=re.S)
    if n != 1:
        sys.exit("%s: found %d times, expected 1" % (name, n))
    return text


text = open(HEADER).read()

for name in ("PROTEUS_ADDR_DONGLE_INIT", "PROTEUS_ADDR_LEFT_INIT", "PROTEUS_ADDR_RIGHT_INIT"):
    text = sub(text, name, addr_macro(name))
for name in ("PROTEUS_LTK_DONGLE_LEFT_INIT", "PROTEUS_LTK_DONGLE_RIGHT_INIT"):
    text = sub(text, name, ltk_macro(name))

text = re.sub(r"Generated \d{4}-\d{2}-\d{2}", "Generated " + str(datetime.date.today()), text, count=1)

open(HEADER, "w").write(text)
print("rewrote", HEADER)
