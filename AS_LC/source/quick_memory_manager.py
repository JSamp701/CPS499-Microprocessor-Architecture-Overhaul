

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


class _ElfReader:
    def __init__(self, file_path):
        self._file_path = file_path
        efile = open(file_path, "r")
        self._file = efile
        header_string = efile.read(50)
        self._eheader = _ElfHeader(header_string)

class _ElfHeader:
    def __init__(self, header_string):
        self._header_string = header_string
        magic_string = header_string[0:3]
        good_magic_string = [0x7F, 0x45, 0x4C, 0x46]
        self.valid_eheader = False
        if magic_string != good_magic_string:
            self.valid_eheader = False
        else:
            self.valid_eheader = True




class QuickMemoryManager:
    def __init__(self, size, file_path):
        if size % 8 != 0:
            size = 32768
        size /= 8
        self._size = size
        self._ram = size * [0]
        self._mmio_load_address = size + 1
        self._mmio_store_address = size
        self._elf_file_path = file_path

    def load(self, address):
        if address > self._mmio_load_address or address == self._mmio_store_address:
            print("Invalid memory access: Load: " + address)
            return 0
        elif address == self._mmio_load_address:
            return self._external_load()
        else:
            return self._ram[address]

    def store(self, address, value):
        if address > self._mmio_store_address:
            print("Invalid memory access: Store: " + address)
        if address == self._mmio_store_address:
            self._external_store(value)
        else:
            self._ram[address] = value

    def _external_load(self):
        return getch()

    def _external_store(self, value):
        print(value)