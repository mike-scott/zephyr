#!/usr/bin/env python3
#
# Copyright (c) 2017 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0

import sys
import re
import argparse
import os
import json

table_template = """/* auto-generated by gen_syscalls.py, don't edit */

/* Weak handler functions that get replaced by the real ones unless a system
 * call is not implemented due to kernel configuration.
 */
%s

const _k_syscall_handler_t _k_syscall_table[K_SYSCALL_LIMIT] = {
\t%s
};
"""

list_template = """
/* auto-generated by gen_syscalls.py, don't edit */
#ifndef _ZEPHYR_SYSCALL_LIST_H_
#define _ZEPHYR_SYSCALL_LIST_H_

#ifndef _ASMLANGUAGE

#include <zephyr/types.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
\t%s
};

%s

#ifdef __cplusplus
}
#endif

#endif /* _ASMLANGUAGE */

#endif /* _ZEPHYR_SYSCALL_LIST_H_ */
"""

syscall_template = """
/* auto-generated by gen_syscalls.py, don't edit */

#ifndef _ASMLANGUAGE

#include <syscall_list.h>
#include <syscall_macros.h>

#ifdef __cplusplus
extern "C" {
#endif

%s

#ifdef __cplusplus
}
#endif

#endif
"""

handler_template = """
extern u32_t %s(u32_t arg1, u32_t arg2, u32_t arg3,
                u32_t arg4, u32_t arg5, u32_t arg6, void *ssf);
"""

weak_template = """
__weak ALIAS_OF(handler_no_syscall)
u32_t %s(u32_t arg1, u32_t arg2, u32_t arg3,
         u32_t arg4, u32_t arg5, u32_t arg6, void *ssf);
"""


def parse_args():
    global args
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument("-i", "--json-file", required=True,
                        help="Read syscall information from json file")
    parser.add_argument("-d", "--syscall-dispatch", required=True,
                        help="output C system call dispatch table file")
    parser.add_argument("-o", "--base-output", required=True,
                        help="Base output directory for syscall macro headers")
    args = parser.parse_args()


def main():
    parse_args()

    with open(args.json_file, 'r') as fd:
        syscalls = json.load(fd)

    invocations = {}
    ids = []
    table_entries = []
    handlers = []

    for fn, handler, inv, sys_id, entry in syscalls:
        if fn not in invocations:
            invocations[fn] = []

        invocations[fn].append(inv)
        ids.append(sys_id)
        table_entries.append(entry)
        handlers.append(handler)

    with open(args.syscall_dispatch, "w") as fp:
        table_entries.append("[K_SYSCALL_BAD] = handler_bad_syscall")

        weak_defines = "".join([weak_template % name for name in handlers])

        fp.write(table_template % (weak_defines, ",\n\t".join(table_entries)))

    # Listing header emitted to stdout
    ids.sort()
    ids.extend(["K_SYSCALL_BAD", "K_SYSCALL_LIMIT"])
    handler_defines = "".join([handler_template % name for name in handlers])
    sys.stdout.write(list_template % (",\n\t".join(ids), handler_defines))

    os.makedirs(args.base_output, exist_ok=True)
    for fn, invo_list in invocations.items():
        out_fn = os.path.join(args.base_output, fn)

        header = syscall_template % "\n\n".join(invo_list)

        with open(out_fn, "w") as fp:
            fp.write(header)


if __name__ == "__main__":
    main()
