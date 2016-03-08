import argparse
import os
import lc_source.as_libwrapper as lw
import lc_source.quick_memory_manager as qmm
import lc_source.quick_logger as ql


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


def _execute_non_scripted(simulator):
    status = simulator.execute(10)
    print(status)
    while status[0] == "OK" and status[1] == 10:
        status = simulator.execute(10)
        print(status)
    return

if __name__ == "__main__":
    myparser = create_and_configure_parser()
    nspace = myparser.parse_args()
    if nspace.load_file_name is not None or nspace.do_api_test:
        lib_abs_path = os.path.dirname(os.path.abspath(__file__)) + os.path.sep + nspace.library
        if nspace.load_file_name is not None:
            ql_logger = ql.QuickLogger(nspace.log_file_name)
            ql_tracer = ql.QuickLogger(nspace.trace_file_name)
            mman = None
            elf_file_path = os.path.dirname(os.path.abspath(__file__)) + os.path.sep + nspace.load_file_name
            execute_method = None
            if nspace.script_file_name is not None:
                print("SCRIPT INPUT NOT IMPLEMENTED YET")
                # execute_method = execute_scripted
            else:
                execute_method = _execute_non_scripted
            mman = qmm.QuickMemoryManager(nspace.amount_of_ram, elf_file_path)
            simulator = lw.SimulatorInstance(lib_path=nspace.library, memory_amount=nspace.amount_of_ram,
                                             cache_amount=nspace.amount_to_cache,
                                             trace_level=nspace.trace_level,
                                             log_level=nspace.log_level,
                                             raw_trace_callback=ql_tracer.log, raw_log_callback=ql_logger.log,
                                             raw_mmio_load_hook=mman.load, raw_mmio_store_hook=mman.store)
            error = simulator.get_error()
            if error != "None":
                print error
            execute_method(simulator)
            ql_logger.cleanup()
            ql_tracer.cleanup()
        elif nspace.do_api_test:
            print("API TESTS NOT IMPLEMENTED YET")
    else:
        myparser = create_and_configure_parser()
        myparser.parse_args(["-h"])
