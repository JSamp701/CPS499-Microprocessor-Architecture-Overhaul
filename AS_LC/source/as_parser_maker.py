import argparse

def create_and_configure_parser(description):
    parser = argparse.ArgumentParser()
    parser.description = description
    parser.add_argument("library", help="path to library file to simulate")
    parser.add_argument("-l", "--load", help="name of elf file to load into memory; required if no --test flag",
                        metavar="elf-name", dest="load_file_name")
    parser.add_argument("-m", "--mem", help="amount of ram (Bytes) to simulate; must be multiple of 8",
                        default=32768, metavar="ram-amount",
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