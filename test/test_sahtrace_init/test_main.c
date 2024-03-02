/****************************************************************************
**
** SPDX-License-Identifier: BSD-2-Clause-Patent
**
** SPDX-FileCopyrightText: Copyright (c) 2023 SoftAtHome
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
** this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice,
** this list of conditions and the following disclaimer in the documentation
** and/or other materials provided with the distribution.
**
** Subject to the terms and conditions of this license, each copyright holder
** and contributor hereby grants to those receiving rights under this license
** a perpetual, worldwide, non-exclusive, no-charge, royalty-free, irrevocable
** (except for failure to satisfy the conditions of this license) patent license
** to make, have made, use, offer to sell, sell, import, and otherwise transfer
** this software, where such license applies only to those patent claims, already
** acquired or hereafter acquired, licensable by such copyright holder or contributor
** that are necessarily infringed by:
**
** (a) their Contribution(s) (the licensed copyrights of copyright holders and
** non-copyrightable additions of contributors, in source or binary form) alone;
** or
**
** (b) combination of their Contribution(s) with the work of authorship to which
** such Contribution(s) was added by such copyright holder or contributor, if,
** at the time the Contribution is added, such addition causes such combination
** to be necessarily infringed. The patent license shall not apply to any other
** combinations which include the Contribution.
**
** Except as expressly stated above, no rights or licenses from any copyright
** holder or contributor is granted under this license, whether expressly, by
** implication, estoppel or otherwise.
**
** DISCLAIMER
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
** OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
** USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <cmocka.h>

#include "test_sahtrace_init.h"

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_sahtrace_default_init),
        cmocka_unit_test(test_sahtrace_main_does_not_fail_when_reason_unknown),
        cmocka_unit_test(test_sahtrace_no_name_available),
        cmocka_unit_test(test_sahtrace_invalid_type_string),
        cmocka_unit_test(test_sahtrace_invalid_type),
        cmocka_unit_test(test_sahtrace_config_in_sahtrace_table),
        cmocka_unit_test(test_sahtrace_set_level),
        cmocka_unit_test(test_sahtrace_open_file),
        cmocka_unit_test(test_sahtrace_invalid_file_name),
        cmocka_unit_test(test_sahtrace_can_activate_log_zones_list),
        cmocka_unit_test(test_sahtrace_can_activate_trace_zones_list),
        cmocka_unit_test(test_sahtrace_can_activate_log_zones_table),
        cmocka_unit_test(test_sahtrace_can_activate_trace_zones_table),
        cmocka_unit_test(test_sahtrace_zones_is_not_valid_var_type),
        cmocka_unit_test(test_sahtrace_trace_zone_list_takes_global_trace_level),
        cmocka_unit_test(test_sahtrace_set_zone_adds_zone_trace),
        cmocka_unit_test(test_sahtrace_set_zone_adds_zone_to_config),
        cmocka_unit_test(test_sahtrace_set_zone_keeps_zone_after_reboot),
        cmocka_unit_test(test_sahtrace_set_zone_wrong_level),
        cmocka_unit_test(test_sahtrace_list_zones),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
