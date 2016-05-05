# _Getch was retrieved from this answer
# http://stackoverflow.com/questions/510357/python-read-a-single-character-from-the-user#answer-510364
class _Getch:
    # Gets a single character from standard input.  Does not echo to the screen.
    def __init__(self):
        try:
            self.impl = _GetchWindows()
        except ImportError:
            self.impl = _GetchUnix()

    def __call__(self): return self.impl()


class _GetchUnix:
    def __init__(self):
        import tty, sys

    def __call__(self):
        import sys, tty, termios
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(sys.stdin.fileno())
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch


class _GetchWindows:
    def __init__(self):
        import msvcrt

    def __call__(self):
        import msvcrt
        return msvcrt.getch()


getch = _Getch()

# Code directly copied from Jordan Jueckstock's python ARMSimulator storage.py

import ctypes
import hashlib
import struct

# Precompiled byte packer/unpacker
_WORD_STRUCT = struct.Struct("<I")

# Well-known memory-mapped I/O ports
CONSOLE_WRITE_PORT = 0x00100000
CONSOLE_READ_PORT = CONSOLE_WRITE_PORT + 1

class Memory:
    """Byte-addressable virtual memory.

    Has support for half-word and word reads/writes.
    """
    def __init__(self, size: int=32768):
        self._s = size
        self._m = bytearray(b'\0'*size)
        self._rh = (None, lambda: 0)
        self._wh = (None, lambda v: None)

    def set_read_hook(self, addr: int, callback):
        self._rh = (addr, callback)

    def set_write_hook(self, addr: int, callback):
        self._wh = (addr, callback)

    def md5(self) -> str:
        """Compute MD5 of memory contents.

        Returns hex digest (32-chars)
        """
        return hashlib.md5(self._m).hexdigest()

    def blit(self, addr: int, data: bytes) -> None:
        """Copy block of member into RAM; does NOT trigger write-hook."""
        assert (0 <= addr), "Underflow bad."
        assert (addr + len(data) < self._s), "Overflow bad."
        self._m[addr:addr+len(data)] = data

    def read_byte(self, addr: int) -> int:
        # Trap read-hook even before bounds check
        if addr == self._rh[0]:
            return self._rh[1]()
        else:
            assert (0 <= addr < self._s), "Underflow/overflow bad."
            return self._m[addr]

    def write_byte(self, addr: int, value: int) -> None:
        # Trap write-hook before bounds check, even...
        if addr == self._wh[0]:
            self._wh[1](value & 0xff)
        else:
            assert (0 <= addr < self._s), "Underflow/overflow bad."
            self._m[addr] = value

    def read_word(self, addr: int) -> int:
        assert (0 <= addr), "Underflow bad."
        assert (addr + _WORD_STRUCT.size <= self._s), "Overflow bad."
        return _WORD_STRUCT.unpack_from(self._m, addr)[0]

    def write_word(self, addr: int, value: int) -> None:
        assert (0 <= addr), "Underflow bad."
        assert (addr + _WORD_STRUCT.size <= self._s), "Overflow bad."
        return _WORD_STRUCT.pack_into(self._m, addr, value)

### end copied code

from .elf import *

class QuickMemoryManager:
    def __init__(self, size: int, file_name):
        self._memory = Memory(size)
        self._elf_file_name = file_name
        self._elf_file = ElfFile(file_name)
        self._elf_file.load(self._memory)
        self._entry = self._elf_file.entry

    def load(self, address):
        # if address > self._mmio_load_address or address == self._mmio_store_address:
        #     print("Invalid memory access: Load: " + address)
        #     return 0
        # elif address == self._mmio_load_address:
        #     return self._external_load()
        # else:
        #     return self._ram[address]
        return self._memory.read_word(address)

    def store(self, address, value):
        # if address > self._mmio_store_address:
        #     print("Invalid memory access: Store: " + address)
        # if address == self._mmio_store_address:
        #     self._external_store(value)
        # else:
        #     self._ram[address] = value
        self._memory.write_word(address, value)

    def _external_load(self):
        return getch()

    def _external_store(self, value):
        print(value)

    @property
    def entry(self):
        return self._entry

    @property
    def level(self):
        return self._elf_file.level()