import re

COMMENT = re.compile(';.*$')
LABEL_DEF = re.compile('^([a-zA-Z_]\w*):')
STATEMENT2 = re.compile('(\w+)\s+(\w+)$')
STATEMENT3 = re.compile('(\w+)\s+(\w+)\s*,\s*(\w+)$')

class Literal:
    """Just a number"""

    def __init__(self, val):
        self.val = val

    def value(self):
        return [self.val]

    def size(self):
        return 1

class Label:
    """A symbol that will be resolved to a position in the code (a
    number)."""

    def __init__(self, name):
        self.name = name
        self.pos = 'undefined'

    def define(self, pos):
        self.pos = pos

    def value(self):
        if self.pos == 'undefined':
            raise Exception("`%s' is undefined" % self.name)
        return [self.pos]

    def size(self):
        return 1

class OneByte:
    """An opcode represented in a single byte. May include a 4 bit
    immediate value encoded with the opcode."""

    def __init__(self, op, imm = None):
        self.op = op
        self.imm = imm

    def size(self):
        return 1

    def value(self):
        if self.imm:
            v = self.imm.value()[0]
            return [v[-1] + self.op[-1]]
        else:
            return [self.op]

class TwoByte:
    """An opcode represented in two bytes. The immediate value must be
    provided and is a full 8 bit thing that will be resolved to a
    number."""

    def __init__(self, op, imm):
        self.op = op
        self.imm = imm

    def size(self):
        return 2

    def value(self):
        val = [self.op]
        val.extend(self.imm.value())
        return val

# Instruction table
#
# The key tuples will be matched in order of decreasing
# specificity. Thus, it's fine to have both of these defined and the
# correct one will dispatch as appropriate:
#
# mov a, d
# mov a
#
INSTRUCTIONS = {
    ('nop'): lambda p, g: [OneByte('00')],
    ('mov', 'd'): lambda p, g: [TwoByte('01', p.parseImm(g.group(3)))],
    ('mov', 'a'): lambda p, g: [TwoByte('02', p.parseImm(g.group(3)))],
    ('mov', 'd', 'a'): lambda p, g: [OneByte('03')],
    ('mov', 'a', 'd'): lambda p, g: [OneByte('04')],
    ('add', 'd', 'a'): lambda p, g: [OneByte('05')],
    ('jmp'): lambda p, g: [TwoByte('06', p.parseImm(g.group(2)))],
    ('jg', 'd'): lambda p, g: [OneByte('07', p.parseImm(g.group(3)))],
    ('jg'): lambda p, g: [OneByte('08', p.parseImm(g.group(2)))],
    ('add', 'd'): lambda p, g: [TwoByte('09', p.parseImm(g.group(3)))],
    ('mov', 'm', 'd'): lambda p, g: [OneByte('0A')],
    ('mov', 'a', 'm'): lambda p, g: [OneByte('0B')],
    ('add', 'm', 'd'): lambda p, g: [OneByte('0C')],
    ('add', 'd', 'm'): lambda p, g: [OneByte('0D')]
}

class Parser:
    def __init__(self):
        self.labels = {}
        self.output = []

    def find_label(self, name):
        """Resolve NAME to a label, creating it if it doesn't already exist"""

        if name in self.labels: return self.labels[name]
        label = Label(name)
        self.labels[name] = label
        return label

    def emit(self, ops):
        """Include ops in the output"""
        self.output.extend(ops)

    def position(self):
        """Return the current position in the output (determined by the number
        and size of ops that have been emitted)"""
        pos = 0
        for item in self.output:
            pos = pos + item.size()

        return '%02d' % pos

    def parseImm(self, imm):
        """Parse a string as an immediate value. Can either be a number or a
        label."""
        try:
            imm = int(imm)
            return Literal('%02d' % imm)
        except:
            return self.find_label(imm)

    def dump(self):
        """Reeturn array of output as 2 character hex representations of the
        bytes"""
        parts = []
        for part in self.output:
            parts.extend(part.value())
        return parts

    def v2raw(self, f):
        f.write("v2.0 raw\n")
        f.write(' '.join([str(int(b)) for b in self.dump()]))
        f.write("\n")

    def read(self, f):
        """Read from F and extend the output with what we find there."""
        for line in f:
            line = re.sub(COMMENT, '', line)
            line = line.rstrip().lstrip().lower()
            if not line: continue

            m = LABEL_DEF.match(line)
            if m:
                label = m.group(1)
                self.find_label(label).define(self.position())
                continue

            m = STATEMENT3.match(line)
            if m:
                parts3 = m.group(1,2,3)
                parts2 = m.group(1,2)
                parts1 = m.group(1)
                if parts3 in INSTRUCTIONS:
                    self.emit(INSTRUCTIONS[parts3](self, m))
                elif parts2 in INSTRUCTIONS:
                    self.emit(INSTRUCTIONS[parts2](self, m))
                elif parts1 in INSTRUCTIONS:
                    self.emit(INSTRUCTIONS[parts1](self, m))
                else:
                    raise Exception("unrecognized statement3: `%s'" % m.group())
                continue

            m = STATEMENT2.match(line)
            if m:
                parts2 = m.group(1,2)
                parts1 = m.group(1)
                if parts2 in INSTRUCTIONS:
                    self.emit(INSTRUCTIONS[parts2](self, m))
                elif parts1 in INSTRUCTIONS:
                    self.emit(INSTRUCTIONS[parts1](self, m))
                else:
                    raise Exception("unrecognized statement3: `%s'" % m.group())
                continue

            raise Exception("couldn't parse `%s'" % line)


if __name__ == '__main__':
    import sys
    if len(sys.argv) != 3:
        sys.stderr.write("usage: %s infile outfile\n" % sys.argv[0])
        exit(1)

    infile = sys.argv[1]
    outfile = sys.argv[2]

    with open(infile) as f:
        p = Parser()
        p.read(f)

        with open(outfile, 'w') as of:
            p.v2raw(of)
