

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


class QuickMemoryManager:
    def __init__(self, size, file_path):
        self._ram = size * [0]
        self._size = size
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