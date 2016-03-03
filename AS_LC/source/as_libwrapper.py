from ctypes import *
import os


def version_number():
    VersionNo = 1
    return VersionNo


def _supported_library_versions():
    allowed_versions = {}
    version_list = []
    allowed_versions["0"] = version_list
    allowed_versions["0"].append("2")
    return allowed_versions

supported_library_versions=_supported_library_versions()

def print_support_statement():
    print("SUPPORTED VERSIONS OF ASAPI ARE:")
    printstring = ""
    for major in supported_library_versions:
        printstring = printstring + major + "."
        frist = True
        for minor in supported_library_versions[major]:
            if not frist:
                # print("," + minor)
                printstring = printstring + "," + minor
            else:
                frist = False
                # print(minor)
                printstring = printstring + minor
        print(printstring + ";")


def is_version_supported(major, minor):
    if minor in supported_library_versions[major]:
        return True
    return False

# def _load_lib_from_relative_path(rel_path):
#    # code / concepts used from
#    # http://stackoverflow.com/questions/2980479/python-ctypes-loading-dll-from-from-a-relative-path
#    lib_abs_path = os.path.dirname(os.path.abspath(__file__)) + os.path.sep + rel_path
#    return cdll.LoadLibrary(lib_abs_path)


class SimulatorInstance:

    def __init__(self, lib_path, memory_amount, cache_amount, trace_level, log_level, trace_callback, log_callback):
        self._cache_amount = cache_amount
        self._trace_level = trace_level
        self._log_level = log_level
        self._trace_callback = trace_callback
        self._log_callback = log_callback
        self._memory_amount = memory_amount
        self._lib_path = lib_path
        self._simulator = cdll.LoadLibrary(lib_path)
        major_minor = self._determine_versioning(self._simulator)
        self._current_error = "None"
        self._major = major_minor[0]
        self._minor = major_minor[1]
        self._do_version_specific_things()
        if self._current_error != "None":
            return

    def get_error(self):
        return self._current_error

    def _determine_versioning(self, lib):
        self._simulator.asapi_major_version.restype = c_int
        self._simulator.asapi_minor_version.restype = c_int
        return (str(self._simulator.asapi_major_version()), str(self._simulator.asapi_minor_version()))

    def _do_version_specific_things(self):
        self._current_error = self._validate_parameters()
        if self._current_error != "None":
            return
        self._as_status_map = self._select_status_map()
        self._configure_lib_functions()

    def _validate_parameters(self):
        parameter_validator = self._select_parameter_validator()
        if parameter_validator is None:
            return "UNSUPPORTED_API_VERSION"
        else:
            return parameter_validator()

    def _select_parameter_validator(self):
        if is_version_supported(self._major, self._minor):
            print("ASAPI VERSION " + self._major + "." + self._minor + " IS SUPPORTED. BEGINNING LIBRARY SETUP.")
        else:
            print("ASAPI VERSION " + self._major + "." + self._minor + " IS NOT SUPPORTED")
            print_support_statement()
            print("ABORTING!")
            return None
        if self._major == "0":
            return self._parameter_validator_0

    def _parameter_validator_0(self):
            if self._memory_amount % 8 != 0:
                return "INVALID_RAM_AMOUNT"
            if self._cache_amount % 8 != 0:
                return "INVALID_CACHE_AMOUNT"
            if self._trace_level not in [0,1,2]:
                return "INVALID_TRACE_LEVEL"
            if self._log_level < 0 or self._log_level > 10:
                return "INVALID_DEBUG_LEVEL"
            return "None"

    def _select_status_map(self):
        if self._major == "0":
            return {
                0: "OK",
                10: "BAD_MALLOC",
                11: "BAD_IO",
                20: "NOT_IMPLEMENTED",
                21: "NULL_PTR",
                22: "BAD_CONST",
                30: "BAD_INSTRUCTION",
                31: "BAD_ALIGNMENT",
                32: "SWI_ZERO",
                50: "CACHE_DISABLED"
            }

    def _configure_lib_functions(self):
        if self._major == "0":
            self._configure_lib_functions_0()

    def _configure_lib_functions_0(self):
        #  configuring as_create_simulator
        self._simulator.as_create_simulator.restype = c_int32
        mmio_load_hook_type = CFUNCTYPE(c_int32, c_void_p, c_int32)
        mmio_store_hook_type = CFUNCTYPE(c_int32, c_void_p, c_int32, c_int32)
        self._simulator.as_create_simulator.argtypes = [POINTER(c_void_p), c_void_p, mmio_load_hook_type,
                                                        mmio_store_hook_type]

        # configuring as_destroy_simulator
        self._simulator.as_destroy_simulator.restype = c_int32
        self._simulator.as_destroy_simulator.argtypes = [c_void_p]

        # configuring
        # ARMSIM_STATUS as_set_loglevel(ARMSIM_CTX *ctx, ARMSIM_LOGLEVEL level);
        self._simulator.as_set_loglevel.restype = c_int32
        self._simulator.as_set_loglevel.argtypes = [c_void_p, c_int32]

        # configuring
        # ARMSIM_STATUS as_set_tracelevel(ARMSIM_CTX *ctx, ARMSIM_TRACELEVEL level);
        self._simulator.as_set_tracelevel.restype = c_int32
        self._simulator.as_set_tracelevel.argtypes = [c_void_p, c_int32]

        # configuring
        # ARMSIM_STATUS as_set_debug_log_hook(ARMSIM_CTX *ctx,
        # ARMSIM_LOG_HOOK hook,
        # void *uctx);
        as_log_hook = CFUNCTYPE(c_int32, c_void_p, c_char_p)
        self._simulator.as_set_debug_log_hook.restype = c_int32
        self._simulator.as_set_debug_log_hook.argtypes = [c_void_p, as_log_hook, c_void_p]

        # configuring
        # ARMSIM_STATUS as_set_trace_log_hook(ARMSIM_CTX *ctx,
        # ARMSIM_LOG_HOOK hook,
        # void *uctx);
        self._simulator.as_set_trace_log_hook.restype = c_int32
        self._simulator.as_set_trace_log_hook.argtypes = [c_void_p, as_log_hook, c_void_p]

        # configuring
        # ARMSIM_STATUS as_get_ram_word(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD *pval);
        self._simulator.as_get_ram_word.restype = c_int32
        self._simulator.as_get_ram_word.argtypes = [c_void_p, c_int32, POINTER(c_int32)]

        # configuring
        # ARMSIM_STATUS as_set_ram_word(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD value);
        self._simulator.as_set_ram_word.restype = c_int32
        self._simulator.as_set_ram_word.argtypes = [c_void_p, c_int32, c_int32]

        # configuring
        # ARMSIM_STATUS as_get_reg(ARMSIM_CTX *ctx, ARM_REG reg, ARM_WORD *pval);
        self._simulator.as_get_reg.restype = c_int32
        self._simulator.as_get_reg.argtypes = [c_void_p, c_int32, POINTER(c_int32)]

        # configuring
        # ARMSIM_STATUS as_set_reg(ARMSIM_CTX *ctx, ARM_REG reg, ARM_WORD value);
        self._simulator.as_set_reg.restype = c_int32
        self._simulator.as_set_reg.argtypes = [c_void_p, c_int32, c_int32]

        # configuring
        # ARMSIM_STATUS as_get_mode(ARMSIM_CTX *ctx, ARM_MODE *pmd);
        self._simulator.as_get_mode.restype = c_int32
        self._simulator.as_get_mode.argtypes = [c_void_p, POINTER(c_int32)]

        # configuring
        # ARMSIM_STATUS as_set_mode(ARMSIM_CTX *ctx, ARM_MODE mode);
        self._simulator.as_set_mode.restype = c_int32
        self._simulator.as_set_mode.argtypes = [c_void_p, c_int32]

        # configuring
        # ARMSIM_STATUS as_get_banked_reg(ARMSIM_CTX *ctx, ARM_BANKED_REG reg, ARM_WORD *pval);
        self._simulator.as_get_banked_reg.restype = c_int32
        self._simulator.as_get_banked_reg.argtypes = [c_void_p, c_int32, POINTER(c_int32)]

        # configuring
        # ARMSIM_STATUS as_set_banked_reg(ARMSIM_CTX *ctx, ARM_BANKED_REG reg, ARM_WORD value);
        self._simulator.as_set_banked_reg.restype = c_int32
        self._simulator.as_set_banked_reg.argtypes = [c_void_p, c_int32, c_int32]

        # configuring
        # ARMSIM_STATUS as_reset_cpu(ARMSIM_CTX *ctx);
        self._simulator.as_reset_cpu.restype = c_int32
        self._simulator.as_reset_cpu.argtypes = [c_void_p]

        # configuring
        # ARMSIM_STATUS as_cache_enable(ARMSIM_CTX *ctx, ARM_ADDRESS cache_limit);
        self._simulator.as_cache_enable.restype = c_int32
        self._simulator.as_cache_enable.argtypes = [c_void_p, c_int32]

        # configuring
        # ARMSIM_STATUS as_cache_invalidate(ARMSIM_CTX *ctx, ARM_ADDRESS address);
        self._simulator.as_cache_invalidate.restype = c_int32
        self._simulator.as_cache_invalidate.argtypes = [c_void_p, c_int32]

        # configuring
        # ARMSIM_STATUS as_notify_event(ARMSIM_CTX *ctx, ARMSIM_EVENT event);
        self._simulator.as_notify_event.restype = c_int32
        self._simulator.as_notify_event.argtypes = [c_void_p, c_int32]

        # configuring
        # ARMSIM_STATUS as_execute(ARMSIM_CTX *ctx, size_t cycles, size_t *finished);
        self._simulator.as_execute.restype = c_int32
        self._simulator.as_execute.argteypes = [c_void_p, c_int32, POINTER(c_int32)]


