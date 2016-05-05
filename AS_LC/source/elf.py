"""A [toy] ARM (Little-endian, 32-bit) ELF executable loader."""
__author__ = 'jpjuecks'

import struct

from .quick_memory_manager import Memory

ELF_HEADER = struct.Struct("<4s5B7x2H5I6H")
EH_MAGIC = 0
EH_CLASS = 1
EH_DATA = 2
EH_MACHINE = 7
EH_ENTRY = 9
EH_PHOFF = 10
EH_PHENTSIZE = 14
EH_PHENTNUM = 15

ET_EXEC = 2

PROGRAM_HEAADER = struct.Struct("<8I")
PH_TYPE = 0
PH_OFFSET = 1
PH_VADDR = 2
PH_FILESZ = 4
PH_MEMSZ = 5

PT_LOAD = 1

SIM_BOOT = struct.Struct("<I4xI12xI")


class ElfFile(object):
    def __init__(self, filename):
        # Read the entire file into a 'bytes'
        with open(filename, 'rb') as fd:
            self._raw = fd.read()

        # Parse as a 32-bit ELF header
        fields = ELF_HEADER.unpack_from(self._raw)

        # Simple sanity checks
        assert (fields[EH_MAGIC] == b"\x7fELF"), "{0} is *not* an ELF file!".format(filename)
        assert (fields[EH_CLASS] == 1), "{0} is not a 32-bit ELF file!".format(filename)
        assert (fields[EH_DATA] == 1), "{0} uses big-endian data formatting!".format(filename)
        assert (fields[EH_MACHINE] == 0x28), "{0} is not an ARM binary!".format(filename)

        # Extract entry point
        self._entry = fields[EH_ENTRY]

        # Find all program header entries
        ph_table_size = fields[EH_PHENTSIZE] * fields[EH_PHENTNUM]
        ph_table_start = fields[EH_PHOFF]
        ph_table_end = ph_table_start + ph_table_size
        ph_table_data = self._raw[ph_table_start:ph_table_end]
        self._segments = [ph for ph in PROGRAM_HEAADER.iter_unpack(ph_table_data) if ph[PH_TYPE] == PT_LOAD]

    @property
    def entry(self):
        return self._entry

    @property
    def segments(self):
        for _, src_off, dst_addr, _, src_size, dst_size, flags, _ in self._segments:
            chunk = self._raw[src_off:src_off + src_size]
            if dst_size > src_size:
                chunk += b'\0'*(dst_size - src_size)
            yield (dst_addr, flags, chunk)

    @property
    def level(self):
        """Determine the simulator level required to run this ELF file."""
        # Does any program header load at address 0?
        boots = [ph for ph in self._segments if ph[PH_VADDR] == 0]
        if boots and (boots[0][PH_MEMSZ] >= SIM_BOOT.size):
            # Grab that header
            ph = boots[0]
            reset, swi, irq = SIM_BOOT.unpack_from(self._raw, ph[PH_OFFSET])

            # Heuristic scan
            if (reset != 0) and (reset != 0xef000000):
                # If reset is non-zero, that's a level 3
                return 3
            elif (irq != 0) and (irq != 0xef000000):
                # If IRQ is non-zero, that's a level 2
                return 2
            elif (swi != 0) and (swi != 0xef000000):
                # If SWI is non-zero, that's a level 1
                return 1
            else:
                # Weird...
                return 0
        else:
            # Must be C-level...
            return 0

    def load(self, mem: Memory) -> None:
        for addr, _, data in self.segments:
            mem.blit(addr, data)
