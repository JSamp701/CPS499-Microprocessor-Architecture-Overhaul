import argparse
import os
import lc_source.as_libwrapper as lw


def create_and_configure_parser():
    parser = argparse.ArgumentParser()
    parser.description = "A command line runner for CPS310 ARMSimulator library projects."
    parser.add_argument("library", help="path to library file to simulate")
    parser.add_argument("-l", "--load", help="name of elf file to load into memory; required if no --test flag",
                        metavar="elf-name", dest="load_file_name")
    parser.add_argument("-m", "--mem", help="amount of ram (Bytes) to simulate; must be multiple of 8",
                        default=256, metavar="ram-amount",
                        dest="amount_of_ram", type=int)
    parser.add_argument("--test", help="sanity test the library code", action="store_true",
                        dest="do_api_test")
    parser.add_argument("-c","--cache", help="enable caching with amount of cache space (Bytes)",
                        default=0, type=int, metavar="cache-amount", dest="amount_to_cache")
    parser.add_argument("-t", "--trace", help="specify filename for tracing", metavar="trace-name",
                        dest="trace_file_name", default="trace.log")
    parser.add_argument("--trace-level", help="specify level for tracing", metavar="trace-level",
                        dest="trace_level", default=2, type=int)
    parser.add_argument("--log", help="specify filename for logging", metavar="log-name",
                        dest="log_file_name")
    parser.add_argument("--log-level", help="specify level for logging", metavar="log-level",
                        dest="log_level", default=1, type=int)
    parser.add_argument("-s", "--script", help="specify filename for interrupt scripting", metavar="script-name",
                        dest="script_file_name")
    return parser


class QuickLogger:
    def log(self,astring):
        self._logger(astring)

    def _tostdout(self, astring):
        print(astring)

    def _nothing(self):
        # print("QuickLogger cleanned up")
        self._closer = self._nothing

    def _tofile(self, astring):
        self._file.write(astring + "\n")
        self._file.flush()
        print("got here")

    def _closefile(self):
        self._file.flush()
        self._file.close()
        # print("QuickLogger cleanned up")

    def __init__(self, filename):
        if(filename is not None):
            self._filename=filename
            self._file = open(filename, 'w')
            self._logger = self._tofile
            self._closer = self._closefile
        else:
            self._logger = self._tostdout
            self._closer = self._nothing

    def cleanup(self):
        self._closer()


if __name__ == "__main__":
    myparser = create_and_configure_parser()
    nspace = myparser.parse_args()
    # print("HI!")
    # print(nspace)
    if nspace.load_file_name is not None or nspace.do_api_test:
        ql_logger = QuickLogger(nspace.log_file_name)
        ql_tracer = QuickLogger(nspace.trace_file_name)
        lib_abs_path = os.path.dirname(os.path.abspath(__file__)) + os.path.sep + nspace.library
        simulator = lw.SimulatorInstance(lib_path=nspace.library, memory_amount=nspace.amount_of_ram,
                                         cache_amount=nspace.amount_to_cache,
                                         trace_level=nspace.trace_level,
                                         log_level=nspace.log_level,
                                         trace_callback=ql_tracer.log, log_callback=ql_logger.log)
        if nspace.load_file_name is not None:
            elf_file_path = os.path.dirname(os.path.abspath(__file__)) + os.path.sep + nspace.load_file_name
        elif nspace.do_api_test:
            print("NOT IMPLEMENTED YET")
        ql_logger.cleanup()
        ql_tracer.cleanup()
    else:
        myparser = create_and_configure_parser()
        myparser.parse_args(["-h"])
