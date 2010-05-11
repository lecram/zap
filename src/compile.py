#! /usr/bin/python

## Copyright 2010 by Marcel Rodrigues <marcelgmr@gmail.com>
##
## This file is part of zap.
##
## zap is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## zap is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with zap.  If not, see <http://www.gnu.org/licenses/>.

"""
Z Compiler
"""

# Python 2.x

import os.path

EXPR_END = '/'

DELETE = '\xDE'

STRING_END = '\x00'
LIST_END = '\x00'
DICT_END = '\x00'

CALL_START = '\xF0'
CALL_END = '\xF1'

BLOCK_START = '\xB0'
BLOCK_END = '\xB1'

BLOCK_EXIT = '\xBE'

END = '\x01'
BREAK = '\x02'
CONTINUE = '\x03'
RETURN = '\x04'

NONE = '\x01'
BOOL = '\x02'
BYTE = '\x03'
WORD = '\x04'
BYTEARRAY = '\x05'
BIGNUM = '\x06'
LIST = '\x07'
DICT = '\x08'

IFBLOCK = '\x01'
ELIFBLOCK = '\x02'
ELSEBLOCK = '\x03'
WHILEBLOCK = '\x04'
DEFBLOCK = '\x05'

FALSE = '\x00'
TRUE = '\x01'

SQUOTE = '\x01'
DQUOTE = '\x02'

ASGN_END = '\x00'
CASGN_START = '\x01'
CASGN_END = '\x02'

def quoteok(str):
    derr = (str.count('"') - str.count('\\"')) % 2
    if derr:
        return False
    serr = (str.count("'") - str.count("\\'")) % 2
    if serr:
        return False
    return True

def ssplit(str, sub):
    split = str.split(sub)
    if str.count('"') + str.count("'") == 0:
        return split
    special = []
    while split:
        item = ""
        while True:
            item += split.pop(0)
            if quoteok(item):
                break
            item += sub
        special.append(item)
    return special

def isnumber(s):
    if not s:
        return False
    for c in s:
        if not (ord('0') <= ord(c) <= ord('9')):
            return False
    return True

def cplword(s):
    n = int(s)
    cpl = ""
    while n >= 256:
        cpl = chr(n % 256) + cpl
        n /= 256
    cpl = chr(n) + cpl
    L = len(cpl)
    if L > 4:
        raise Exception("Word out of range.")
    cpl = chr(0) * (4 - L) + cpl
    return cpl

def cplstr(s):
    quote = s[0]
    end = 0
    while True:
        end = s.find(quote, end + 1)
        if end < 0:
            raise Exception('Expected quote (%s).' % quote)
        if s[end - 1] != '\\':
            break
    rest = s[end + 1:]
    cpl = ""
    i = 1
    while i < end:
        c = s[i]
        if c == '\\':
            e = s[i + 1]
            if e in ("'", '"', '\\'):
                cpl += e
                i += 1
            elif e == 'x':
                h = s[i + 2:i + 4]
                cpl += chr(int(h, 16))
                i += 3
            elif e == 'n':
                cpl += '\n'
                i += 1
            else:
                cpl += '\\'
        else:
            cpl += c
        i += 1
    return cpl, rest

def uncomment(line):
    sq = dq = False
    i = 0
    imax = len(line)
    while i < imax:
        c = line[i]
        if c == '#' and not (sq or dq):
            break
        elif c == "'":
            sq = not sq
        elif c == '"':
            dq = not dq
        elif c == '\\':
            i += 1
        i += 1
    return line[:i]

def cplexpr(expr):
    # print 'Compiling "%s" ...' % expr
    i = 0
    temp = ""
    c = expr[i]
    if c == '<' and '>' in expr[2:5]:
        # Compile Byte.
        n = expr[1:expr.find('>')]
        for d in n:
            if ord(d) < ord('0') or ord(d) > ord('9'):
                break
        else:
            b = int(n)
            if b > 255:
                raise Exception('Byte out of range.')
            cpl = BYTE + chr(b)
            rest = expr[expr.find('>') + 1:]
            return cpl, rest
    while True:
        if c == '(':
            # Compile Function.
            if not temp:
                raise Exception('Call without name.')
            cpl = CALL_START + temp + STRING_END
            rest = expr[i + 1:].strip()
            while True:
                x, expr = cplexpr(rest.strip())
                cpl += x
                c = expr[0]
                rest = expr[1:]
                if c == ')':
                    break
                elif c == EXPR_END:
                    raise Exception("Unexpected line break.")
                elif c != ',':
                    raise Exception('Expected "," or ")".')
            cpl += CALL_END
            return cpl, rest
        elif c == '[':
            # Compile List.
            if temp:
                raise Exception('Syntax Error.')
            cpl = LIST
            rest = expr[i + 1:].strip()
            while True:
                x, expr = cplexpr(rest.strip())
                cpl += x
                c = expr[0]
                rest = expr[1:]
                if c == ']':
                    break
                elif c == EXPR_END:
                    raise Exception("Unexpected line break.")
                elif c != ',':
                    raise Exception('Expected "," or "]".')
            cpl += LIST_END
            return cpl, rest
        elif c == '{':
            # Compile Dict.
            if temp:
                raise Exception("Syntax error.")
            cpl = DICT
            rest = expr[i + 1:].strip()
            while True:
                x, expr = cplexpr(rest.strip())
                cpl += x
                c = expr[0]
                rest = expr[1:]
                if c != ':':
                    raise Exception('Expected ":".')
                x, expr = cplexpr(rest.strip())
                cpl += x
                c = expr[0]
                rest = expr[1:]
                if c == '}':
                    break
                elif c == EXPR_END:
                    raise Exception("Unexpected line break.")
                elif c != ',':
                    raise Exception('Expected "," or "}".')
            cpl += DICT_END
            return cpl, rest
        elif c == "'":
            # Compile Byte from character.
            if temp:
                raise Exception("Syntax error.")
            x, rest = cplstr(expr.strip())
            L = len(x)
            if L == 0:
                raise Exception("Empty char.")
            elif L > 1:
                raise Exception("Char out of range.")
            cpl = BYTE + x
            c = rest[0]
            return cpl, rest
        elif c == '"':
            # Compile ByteArray from string.
            if temp:
                raise Exception("Syntax error.")
            x, rest = cplstr(expr.strip())
            L = len(x)
            if L == 0:
                raise Exception("Empty string.")
            length = cplword(str(L))
            cpl = BYTEARRAY + length + x
            c = rest[0]
            return cpl, rest
        elif c in (',', ')', ']', '}', ':', EXPR_END):
            temp = temp.strip()
            # Try to Compile None/Bool Literal.
            if temp == "NONE":
                # Compile None.
                cpl = NONE
            elif temp == "TRUE":
                # Compile Bool Literal.
                cpl = BOOL + TRUE
            elif temp == "FALSE":
                # Compile Bool Literal.
                cpl = BOOL + FALSE
            elif isnumber(temp):
                # Compile Word.
                cpl = WORD + cplword(temp)
            # Compile Reference.
            else:
                cpl = temp + STRING_END
            rest = expr[i:]
            return cpl, rest
        else:
            i += 1
            if i >= len(expr):
                raise Exception("Unexpected line break.")
            temp += c
            c = expr[i]

def cplasgn(asgns):
    cpl = ""
    for asgn in asgns:
        if asgn[0] == '(':
            cpl += CASGN_START
            for ref in asgn[1:-1].split(','):
                cpl += ref.strip() + STRING_END
            cpl += CASGN_END
        else:
            cpl += asgn + STRING_END
    cpl += ASGN_END
    return cpl

def compile(src, cpl=None):
    name, ext = os.path.splitext(src)
    if ext != '.z':
        print "Not a zap Source File."
        return
    if cpl is None:
        cpl = name + '.zbc'
    srcf = open(src, 'r')
    cplf = open(cpl, 'wb')
    identlevel = 0
    identstr = ""
    for linum, line in enumerate(srcf):
        # Ignore comments.
        line = uncomment(line)
        # Ignore blank/comment-only lines.
        if not line.strip():
            continue
        # Verify Identation.
        ident = line[:line.index(line.lstrip())]
        if identstr:
            level = len(ident) / len(identstr)
            if level > identlevel or ident != identstr * level:
                print "Incorrect identation in line %d." % (linum + 1)
                break
            while level < identlevel:
                # Compile BlockEnd.
                identlevel -= 1
                # cpl = BLOCK_END # deprecated.
                cpl = BLOCK_EXIT + END
                cplf.write(cpl)
        else:
            if identlevel:
                if not ident:
                    print "Incorrect identation in line %d." % (linum + 1)
                    break
                else:
                    identstr = ident
        line = line.strip()
        split = line.split(' ')
        cmd = split.pop(0)
        if cmd == "del":
            # Compile del.
            name = split[0]
            cplf.write(DELETE + name + STRING_END)
        elif cmd == "break":
            # Compile break.
            if split:
                level = chr(int(split[0]))
            else:
                level = '\x00'
            cplf.write(BLOCK_EXIT + BREAK + level)
        elif cmd == "continue":
            # Compile continue.
            if split:
                level = chr(int(split[0]))
            else:
                level = '\x00'
            cplf.write(BLOCK_EXIT + CONTINUE + level)
        elif line[-1] == ':':
            # Compile BlockStart.
            identlevel += 1
            cpl = BLOCK_START
            split = line[:-1].split(' ')
            cmd = split.pop(0)
            expr = ' '.join(split)
            if cmd == "while":
                # Compile while.
                cpl += WHILEBLOCK
                ecpl, rest = cplexpr(expr + EXPR_END)
                if rest != EXPR_END:
                    print "Extra caracters in line %d." % (linum + 1)
                    break
                cplf.write(cpl + ecpl)
            elif cmd == "if":
                # Compile if.
                cpl += IFBLOCK
                ecpl, rest = cplexpr(expr + EXPR_END)
                if rest != EXPR_END:
                    print "Extra caracters in line %d." % (linum + 1)
                    break
                cplf.write(cpl + ecpl)
            elif cmd == "elif":
                # Compile elif.
                cpl += ELIFBLOCK
                ecpl, rest = cplexpr(expr + EXPR_END)
                if rest != EXPR_END:
                    print "Extra caracters in line %d." % (linum + 1)
                    break
                cplf.write(cpl + ecpl)
            elif cmd == "else":
                cpl += ELSEBLOCK
                cplf.write(cpl)
        else:
            # Compile Statement.
            # Get Assignments and Expression.
            asgns = [h.strip() for h in ssplit(line, ' := ')]
            expr = asgns.pop()
            # Compile Expression.
            cpl, rest = cplexpr(expr + EXPR_END)
            if rest != EXPR_END:
                print "Extra caracters in line %d." % (linum + 1)
                break
            cplf.write(cpl)
            # Compile Assignments.
            cplf.write(cplasgn(asgns))
    cplf.write(BLOCK_EXIT + END)
    srcf.close()
    cplf.close()


if __name__ == "__main__":
    import sys

    if len(sys.argv) == 2:
        src = sys.argv[1]
        print "Compiling \"%s\"..." % src
        compile(src)
        print "OK."
    else:
        print "<< zap compiler >>\n\nUsage:\npython compile.py app.z"
