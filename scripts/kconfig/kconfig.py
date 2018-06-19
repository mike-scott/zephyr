#!/usr/bin/env python3
# Modified from: https://github.com/ulfalizer/Kconfiglib/blob/master/examples/merge_config.py
import argparse
import sys
import textwrap

from kconfiglib import Kconfig, Symbol, BOOL, STRING, TRISTATE, TRI_TO_STR

# Warnings that won't be turned into errors (but that will still be printed),
# identified by a substring of the warning. The warning texts from Kconfiglib
# are guaranteed to not change.
WARNING_WHITELIST = (
    # Warning generated when a symbol with unsatisfied dependencies is being
    # selected. These should be investigated, but whitelist them for now.
    "unsatisfied direct dependencies",

    # Assignments to certain undefined symbols. These are only defined for ARC
    # and x86, respectively, but are set in some "shared" .conf files.
    "undefined symbol ARC_INIT",  # Issue #7977
    "undefined symbol SSE",       # Issue #7978
)

def fatal(warning):
    # Returns True if 'warning' is not whitelisted and should be turned into an
    # error

    for wl_warning in WARNING_WHITELIST:
        if wl_warning in warning:
            return False

    # Only allow enabled (printed) warnings to be fatal
    return enabled(warning)


def enabled(warning):
    # Returns True if 'warning' should be printed

    # Some prj.conf files seem to deliberately override settings from the board
    # configuration (e.g. samples/bluetooth/hci_usb/prj.conf, with GPIO=y).
    # Disable the warning about a symbol being assigned more than once.
    return "set more than once" not in warning


def main():
    parse_args()

    print("Parsing Kconfig tree in {}".format(args.kconfig_root))
    kconf = Kconfig(args.kconfig_root, warn_to_stderr=False)

    # Enable warnings for assignments to undefined symbols
    kconf.enable_undef_warnings()

    # This script uses alldefconfig as the base. Other starting states could be set
    # up here as well. The approach in examples/allnoconfig_simpler.py could
    # provide an allnoconfig starting state for example.

    print("Using {} as base".format(args.conf_fragments[0]))
    for config in args.conf_fragments[1:]:
        print("Merging {}".format(config))
    # Create a merged configuration by loading the fragments with replace=False
    for config in args.conf_fragments:
        kconf.load_config(config, replace=False)


    # Print warnings for symbols whose actual value doesn't match the assigned
    # value
    for sym in kconf.defined_syms:
        # Was the symbol assigned to?
        if sym.user_value is not None:
            verify_assigned_value(sym)

    # We could roll this into the loop below, but it's nice to always print all
    # warnings, even if one of them turns out to be fatal
    for warning in kconf.warnings:
        if enabled(warning):
            print(warning, file=sys.stderr)

    # Turn all warnings except for explicity whitelisted ones into errors. In
    # particular, this will turn assignments to undefined Kconfig variables
    # into errors.
    #
    # A warning is generated by this script whenever a symbol gets a different
    # value than the one it was assigned. Keep that one as just a warning for
    # now as well.
    for warning in kconf.warnings:
        if fatal(warning):
            sys.exit("Error: Aborting due to non-whitelisted Kconfig "
                     "warning '{}'.\nNote: If this warning doesn't point "
                     "to an actual problem, you can add it to the "
                     "whitelist at the top of {}."
                     .format(warning, sys.argv[0]))


    # Write the merged configuration
    kconf.write_config(args.dotconfig)

    # Write the C header
    kconf.write_autoconf(args.autoconf)


# Message printed when a promptless symbol is assigned (and doesn't get the
# assigned value)
PROMPTLESS_HINT = """
This symbol has no prompt, meaning assignments in configuration files have no
effect on it. It can only be set indirectly, via Kconfig defaults (e.g. in a
Kconfig.defconfig file) or through being 'select'ed or 'imply'd (note: try to
avoid Kconfig 'select's except for trivial promptless "helper" symbols without
dependencies, as it ignores dependencies and forces symbols on).
"""

# Message about where to look up symbol information
SYM_INFO_HINT = """
You can check symbol information (including dependencies) in the 'menuconfig'
interface (see the Application Development Primer section of the manual), or in
the Kconfig reference at
http://docs.zephyrproject.org/reference/kconfig/CONFIG_{}.html (which is
updated regularly from the master branch). See the 'Setting configuration
values' section of the Board Porting Guide as well.
"""[1:]  # Remove initial newline for nicer textwrap output when joining texts

PROMPTLESS_HINT_EXTRA = "It covers Kconfig.defconfig files."

def verify_assigned_value(sym):
    # Verifies that the value assigned to 'sym' "took" (matches the value the
    # symbol actually got), printing a warning otherwise

    # Tristate values are represented as 0, 1, 2. Having them as
    # "n", "m", "y" is more convenient here, so convert.
    if sym.type in (BOOL, TRISTATE):
        user_value = TRI_TO_STR[sym.user_value]
    else:
        user_value = sym.user_value

    if user_value != sym.str_value:
        msg = "warning: {} was assigned the value '{}' but got the " \
              "value '{}'. " \
              .format(name_and_loc(sym), user_value, sym.str_value)

        if promptless(sym):
            msg += PROMPTLESS_HINT

        msg += SYM_INFO_HINT.format(sym.name)

        if promptless(sym):
            msg += PROMPTLESS_HINT_EXTRA

        # Use a large fill() width to try to avoid linebreaks in the symbol
        # reference link
        print(textwrap.fill(msg, 100), file=sys.stderr)


def name_and_loc(sym):
    # Helper for printing the name and Kconfig file location(s) for a symbol

    if not sym.nodes:
        return sym.name + " (undefined)"

    return "{} (defined at {})".format(
        sym.name,
        ", ".join("{}:{}".format(node.filename, node.linenr)
                  for node in sym.nodes))


def promptless(sym):
    # Returns True if 'sym' has no prompt. Since the symbol might be defined in
    # multiple locations, we need to check all locations.

    for node in sym.nodes:
        if node.prompt:
            return False

    return True


def parse_args():
    global args

    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    parser.add_argument("kconfig_root")
    parser.add_argument("dotconfig")
    parser.add_argument("autoconf")
    parser.add_argument("conf_fragments", metavar='conf', type=str, nargs='+')

    args = parser.parse_args()


if __name__ == "__main__":
    main()
