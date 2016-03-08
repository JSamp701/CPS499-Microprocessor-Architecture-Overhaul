import os

import lc_source.as_parser_maker as pm
import lc_source.as_libwrapper as lw
import lc_source.quick_memory_manager as qmm
import lc_source.quick_logger as ql


def _execute_non_scripted(simulator):
    status = simulator.execute(10)
    print(status)
    while status[0] == "OK" and status[1] == 10:
        status = simulator.execute(10)
        print(status)
    return

if __name__ == "__main__":
    myparser = pm.create_and_configure_parser("A command line runner for CPS310 ARMSimulator library projects.")
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
        myparser = pm.create_and_configure_parser("A command line runner for CPS310 ARMSimulator library projects.")
        myparser.parse_args(["-h"])

