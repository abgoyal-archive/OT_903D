#!/usr/bin/python
import sys, os, string, re

header = """\
#include "android_charmap.h"

"""

kmap_header = """\
static const AKeyEntry  _%(name)s_keys[] =
{
   /* keycode                   base   caps    fn  caps+fn   number */
"""


kmap_footer = """\
};

static const AKeyCharmap  _%(name)s_charmap =
{
    _%(name)s_keys,
    %(count)d,
    "%(name)s"
};
"""


re_mapname = re.compile( r".*/(\w+).kcm" )
re_start = re.compile( r"(\w+)\s*(.*)" )
re_char  = re.compile( r"('.')\s*(.*)" )
re_hex   = re.compile( r"(0x\w+)\s*(.*)" )

specials = { 'COMMA': 'Comma',
             'PERIOD': 'Period',
             'AT': 'At',
             'LEFT_BRACKET': 'LeftBracket',
             'RIGHT_BRACKET': 'RightBracket',
             'SLASH': 'Slash',
             'BACKSLASH': 'Backslash',
             'GRAVE': 'Grave',
             'MINUS': 'Minus',
             'EQUALS': 'Equals',
             'SEMICOLON': 'Semicolon',
             'APOSTROPHE': 'Apostrophe',
             'SPACE': 'Space',
             'ENTER': 'Newline',
             'TAB': 'Tab',
             'STAR': 'Star',
             'POUND': 'Pound',
             'PLUS': 'Plus',
             'DEL': 'Del'
           }

entries = []

def match_char_or_hex(line):
    m = re_char.match(line)
    if not m:
        m = re_hex.match(line)
    return m

def quote(s):
    if s == "'''":
        s = "'\\''"
    elif s == "'\\'":
        s = "'\\\\'"
    return s

def process_line(line,result):
    m = re_start.match(line)
    if not m:
        print "bad bad line: " + line
        return -1
    keycode = m.group(1)
    line    = m.group(2)
    m = match_char_or_hex(line)
    if not m:
        print "character expected in: " + line
        return -1
    disp = quote(m.group(1))
    line = m.group(2)
    m = match_char_or_hex(line)
    if not m:
        print "character expected in: " + line
        return -1
    number = quote(m.group(1))
    line = m.group(2)
    m = match_char_or_hex(line)
    if not m:
        print "character expected in: " + line
        return -1
    base = quote(m.group(1))
    line = m.group(2)
    m = match_char_or_hex(line)
    if not m:
        print "character expected in: " + line
        return -1
    caps = quote(m.group(1))
    line = m.group(2)
    m = match_char_or_hex(line)
    if not m:
        print "character expected in: " + line
        return -1
    fn = quote(m.group(1))
    line = m.group(2)
    m = match_char_or_hex(line)
    if not m:
        print "character expected in: " + line
        return -1
    caps_fn = quote(m.group(1))

    if specials.has_key(keycode):
        keycode = specials[keycode]
    keycode = "kKeyCode" + keycode

    result.append( (keycode,base,caps,fn,caps_fn,number) )
    return 0

def process_file( file ):
    result = []
    fp = open(file,"rb")
    for line in fp.xreadlines():
        line = line.strip()
        if not line:   # skip empty lines
            continue
        if line[0] == '#' or line[0] == '[':  # skip
            continue
        if process_line(line,result) < 0:
            break
    fp.close()
    return result

class KMap:
    def __init__(self,name,results):
        self.name    = name
        self.results = results

    def dump(self):
        t = { 'name': self.name, 'count':len(self.results) }
        print kmap_header % t
        for item in self.results:
            print "    { %-22s, %5s, %5s, %5s, %6s, %5s }," % item
        print kmap_footer % t

kmaps = []

if len(sys.argv) < 2:
    print "usage: progname  charmap.kcm [charmap2.kcm ...] > charmap-tab.h"
else:
    genline = ""
    for filepath in sys.argv[1:]:
        m = re_mapname.match(filepath)
        if not m:
            print "%s is not a keyboard charmap name" % filepath
            os.exit(1)

        mapname = m.group(1)
        genline = genline + " " + mapname + ".kcm"

    for filepath in sys.argv[1:]:
        m = re_mapname.match(filepath)
        mapname = m.group(1)
        result = process_file( filepath )
        kmap = KMap(mapname,result)
        kmaps.append(kmap)

    print header + genline + header2
    for kmap in kmaps:
        kmap.dump()

    print "const AKeyCharmap*  android_charmaps[%d] = {" % len(kmaps),
    comma = ""
    for kmap in kmaps:
        print "%s&_%s_charmap" % (comma, kmap.name),
        comma = ", "
    print "};"
    print "const int           android_charmap_count = %d;" % len(kmaps)
