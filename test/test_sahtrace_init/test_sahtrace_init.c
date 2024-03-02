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
#include <stdio.h>
#include <setjmp.h>
#include <stdarg.h>
#include <cmocka.h>
#include <string.h>

#include <debug/sahtrace.h>

#include <amxc/amxc.h>
#include <amxp/amxp.h>
#include <amxd/amxd_dm.h>
#include <amxd/amxd_object_function.h>
#include <amxo/amxo.h>
#include <amxo/amxo_save.h>


#include "test_sahtrace_init.h"
#include "mod_main.h"

#define UNUSED __attribute__((unused))

const char* FILE_PATH = "mqtt/mqtt_config.odl"; // writing in current dir
const char* DIR_PATH = "mqtt/mqtt_config.odl";  // writing in current dir
const char* NAME = "MQTT";

int _main(int reason,
          amxd_dm_t* dm,
          amxo_parser_t* parser);

void init_config(amxd_dm_t* dm, amxo_parser_t* parser) {
    amxc_var_t* var_dir = NULL;
    amxc_var_t* var_path = NULL;

    amxd_dm_init(dm);
    amxo_parser_init(parser);

    var_dir = amxo_parser_claim_config(parser, "config-storage-dir");
    amxc_var_set(cstring_t, var_dir, DIR_PATH);
    var_path = amxo_parser_claim_config(parser, "config-storage-file");
    amxc_var_set(cstring_t, var_path, FILE_PATH);
}

bool check_zone_in_config(amxo_parser_t* parser, const char* zone, const uint32_t level) {
    amxc_var_t* var_zones = amxo_parser_claim_config(parser, "trace-zones");
    amxc_var_for_each(var_zone, var_zones) {
        const uint32_t config_level = amxc_var_constcast(uint32_t, var_zone);
        const char* config_zone = amxc_var_key(var_zone);
        if((config_level == level) && (strcmp(config_zone, zone) == 0)) {
            return true;
        }
    }
    return false;
}

bool call_trace_zone(const char* zone, const uint32_t level) {
    amxc_var_t args;
    amxd_object_t* object = NULL;
    amxd_function_t* function = NULL;
    amxc_var_t* ret = NULL;

    amxc_var_init(&args);
    amxc_var_set_type(&args, AMXC_VAR_ID_HTABLE);
    amxc_var_add_key(cstring_t, &args, "zone", zone);
    amxc_var_add_key(uint32_t, &args, "level", level);
    amxd_status_t retval = set_trace_zone(object, function, &args, ret);

    amxc_var_clean(&args);
    return (retval == amxd_status_ok);
}

void test_sahtrace_default_init(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxc_var_t* var_name = NULL;

    init_config(&dm, &parser);

    var_name = amxo_parser_claim_config(&parser, "name");
    amxc_var_set(cstring_t, var_name, NAME);


    assert_int_equal(_main(0, &dm, &parser), 0);
    assert_int_equal(_main(1, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_main_does_not_fail_when_reason_unknown(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxc_var_t* var_name = NULL;

    init_config(&dm, &parser);

    var_name = amxo_parser_claim_config(&parser, "name");
    amxc_var_set(cstring_t, var_name, "Test");

    assert_int_equal(_main(99, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_no_name_available(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;

    init_config(&dm, &parser);

    assert_int_equal(_main(0, &dm, &parser), 0);
    assert_int_equal(_main(1, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_invalid_type_string(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxc_var_t* var_type = NULL;

    init_config(&dm, &parser);

    var_type = amxo_parser_claim_config(&parser, "log-type");
    amxc_var_set(cstring_t, var_type, "invalid");

    assert_int_equal(_main(0, &dm, &parser), 0);
    assert_int_equal(_main(1, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_invalid_type(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxc_var_t* var_type = NULL;

    init_config(&dm, &parser);

    var_type = amxo_parser_claim_config(&parser, "log-type");
    amxc_var_set(bool, var_type, true);

    assert_int_equal(_main(0, &dm, &parser), 0);
    assert_int_equal(_main(1, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_config_in_sahtrace_table(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxc_var_t* sahtrace = NULL;

    init_config(&dm, &parser);

    sahtrace = amxo_parser_claim_config(&parser, "sahtrace");
    amxc_var_set_type(sahtrace, AMXC_VAR_ID_HTABLE);
    amxc_var_add_key(cstring_t, sahtrace, "type", "stdout");
    amxc_var_add_key(uint32_t, sahtrace, "level", 500);

    assert_int_equal(_main(0, &dm, &parser), 0);
    assert_int_equal(_main(1, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_set_level(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxc_var_t* var_type = NULL;

    init_config(&dm, &parser);

    var_type = amxo_parser_claim_config(&parser, "log-level");
    amxc_var_set(uint32_t, var_type, 300);

    assert_int_equal(_main(0, &dm, &parser), 0);
    assert_int_equal(sahTraceLevel(), 300);
    assert_int_equal(_main(1, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_open_file(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxc_var_t* var_name = NULL;
    amxc_var_t* var_type = NULL;
    amxc_var_t* var_file = NULL;

    init_config(&dm, &parser);

    var_name = amxo_parser_claim_config(&parser, "name");
    amxc_var_set(cstring_t, var_name, "Test");
    var_type = amxo_parser_claim_config(&parser, "log-type");
    amxc_var_set(cstring_t, var_type, "file");
    var_file = amxo_parser_claim_config(&parser, "log-file");
    amxc_var_set(cstring_t, var_file, "/tmp/test.log");

    assert_int_equal(_main(0, &dm, &parser), 0);
    assert_int_equal(_main(1, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_invalid_file_name(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxc_var_t* var_name = NULL;
    amxc_var_t* var_type = NULL;
    amxc_var_t* var_file = NULL;

    init_config(&dm, &parser);

    var_name = amxo_parser_claim_config(&parser, "name");
    amxc_var_set(cstring_t, var_name, "Test");
    var_type = amxo_parser_claim_config(&parser, "log-type");
    amxc_var_set(cstring_t, var_type, "file");
    var_file = amxo_parser_claim_config(&parser, "log-file");
    amxc_var_set(uint32_t, var_file, 100);

    assert_int_equal(_main(0, &dm, &parser), 0);
    assert_int_equal(_main(1, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_can_activate_log_zones_list(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxc_var_t* var_name = NULL;
    amxc_var_t* var_zones = NULL;
    sah_trace_zone_it* zone = NULL;

    init_config(&dm, &parser);

    var_name = amxo_parser_claim_config(&parser, "name");
    amxc_var_set(cstring_t, var_name, "Test");
    var_zones = amxo_parser_claim_config(&parser, "log-zones");
    amxc_var_set_type(var_zones, AMXC_VAR_ID_LIST);
    amxc_var_add(cstring_t, var_zones, "test_zone1");
    amxc_var_add(cstring_t, var_zones, "test_zone2");
    amxc_var_add(uint32_t, var_zones, 123);
    amxc_var_add(cstring_t, var_zones, "test_zone3");

    assert_int_equal(_main(0, &dm, &parser), 0);

    zone = sahTraceGetZone("test_zone1");
    assert_non_null(zone);
    assert_int_equal(sahTraceZoneLevel(zone), 200);
    zone = sahTraceGetZone("test_zone2");
    assert_non_null(zone);
    assert_int_equal(sahTraceZoneLevel(zone), 200);
    zone = sahTraceGetZone("test_zone3");
    assert_non_null(zone);
    assert_int_equal(sahTraceZoneLevel(zone), 200);

    assert_int_equal(_main(1, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_can_activate_trace_zones_list(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxc_var_t* log_level = NULL;
    amxc_var_t* zone_level = NULL;
    amxc_var_t* var_name = NULL;
    amxc_var_t* var_zones = NULL;
    sah_trace_zone_it* zone = NULL;
    #define ZONE_LEVEL 300

    init_config(&dm, &parser);

    log_level = amxo_parser_claim_config(&parser, "default_log_level");
    amxc_var_set(uint32_t, log_level, ZONE_LEVEL);
    zone_level = amxo_parser_claim_config(&parser, "default_trace_zone_level");
    amxc_var_set(uint32_t, zone_level, ZONE_LEVEL);
    var_name = amxo_parser_claim_config(&parser, "name");
    amxc_var_set(cstring_t, var_name, "Test");
    var_zones = amxo_parser_claim_config(&parser, "trace-zones");
    amxc_var_set_type(var_zones, AMXC_VAR_ID_LIST);
    amxc_var_add(cstring_t, var_zones, "test_zone1");
    amxc_var_add(cstring_t, var_zones, "test_zone2");
    amxc_var_add(uint32_t, var_zones, 123);
    amxc_var_add(cstring_t, var_zones, "test_zone3");

    assert_int_equal(_main(0, &dm, &parser), 0);

    zone = sahTraceGetZone("test_zone1");
    assert_non_null(zone);
    assert_int_equal(sahTraceZoneLevel(zone), ZONE_LEVEL);
    zone = sahTraceGetZone("test_zone2");
    assert_non_null(zone);
    assert_int_equal(sahTraceZoneLevel(zone), ZONE_LEVEL);
    zone = sahTraceGetZone("test_zone3");
    assert_non_null(zone);
    assert_int_equal(sahTraceZoneLevel(zone), ZONE_LEVEL);

    assert_int_equal(_main(1, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_can_activate_log_zones_table(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxc_var_t* var_name = NULL;
    amxc_var_t* var_zones = NULL;
    sah_trace_zone_it* zone = NULL;

    init_config(&dm, &parser);

    var_name = amxo_parser_claim_config(&parser, "name");
    amxc_var_set(cstring_t, var_name, "Test");
    var_zones = amxo_parser_claim_config(&parser, "log-zones");
    amxc_var_set_type(var_zones, AMXC_VAR_ID_HTABLE);
    amxc_var_add_key(uint32_t, var_zones, "test_zone1", 500);
    amxc_var_add_key(uint32_t, var_zones, "test_zone2", 400);
    amxc_var_add_key(uint32_t, var_zones, "test_zone3", 300);

    assert_int_equal(_main(0, &dm, &parser), 0);

    zone = sahTraceGetZone("test_zone1");
    assert_non_null(zone);
    assert_int_equal(sahTraceZoneLevel(zone), 500);
    zone = sahTraceGetZone("test_zone2");
    assert_non_null(zone);
    assert_int_equal(sahTraceZoneLevel(zone), 400);
    zone = sahTraceGetZone("test_zone3");
    assert_non_null(zone);
    assert_int_equal(sahTraceZoneLevel(zone), 300);

    assert_int_equal(_main(1, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_can_activate_trace_zones_table(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxc_var_t* var_name = NULL;
    amxc_var_t* var_zones = NULL;
    sah_trace_zone_it* zone = NULL;

    init_config(&dm, &parser);
    var_name = amxo_parser_claim_config(&parser, "name");
    amxc_var_set(cstring_t, var_name, "Test");
    var_zones = amxo_parser_claim_config(&parser, "trace-zones");
    amxc_var_set_type(var_zones, AMXC_VAR_ID_HTABLE);
    amxc_var_add_key(uint32_t, var_zones, "test_zone1", 500);
    amxc_var_add_key(uint32_t, var_zones, "test_zone2", 400);
    amxc_var_add_key(uint32_t, var_zones, "test_zone3", 300);

    assert_int_equal(_main(0, &dm, &parser), 0);

    zone = sahTraceGetZone("test_zone1");
    assert_non_null(zone);
    assert_int_equal(sahTraceZoneLevel(zone), 500);
    zone = sahTraceGetZone("test_zone2");
    assert_non_null(zone);
    assert_int_equal(sahTraceZoneLevel(zone), 400);
    zone = sahTraceGetZone("test_zone3");
    assert_non_null(zone);
    assert_int_equal(sahTraceZoneLevel(zone), 300);

    assert_int_equal(_main(1, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_zones_is_not_valid_var_type(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxc_var_t* var_name = NULL;
    amxc_var_t* var_zones = NULL;

    init_config(&dm, &parser);
    var_name = amxo_parser_claim_config(&parser, "name");
    amxc_var_set(cstring_t, var_name, "Test");
    var_zones = amxo_parser_claim_config(&parser, "log-zones");
    amxc_var_set(bool, var_zones, true);

    assert_int_equal(_main(0, &dm, &parser), 0);
    assert_int_equal(_main(1, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_trace_zone_list_takes_global_trace_level(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxc_var_t* var_name = NULL;
    amxc_var_t* var_zones = NULL;
    amxc_var_t* sahtrace = NULL;
    sah_trace_zone_it* zone = NULL;

    init_config(&dm, &parser);

    var_name = amxo_parser_claim_config(&parser, "name");
    amxc_var_set(cstring_t, var_name, "Test");
    var_zones = amxo_parser_claim_config(&parser, "trace-zones");
    amxc_var_set_type(var_zones, AMXC_VAR_ID_LIST);
    amxc_var_add(cstring_t, var_zones, "test_zone1");
    amxc_var_add(cstring_t, var_zones, "test_zone2");

    sahtrace = amxo_parser_claim_config(&parser, "sahtrace");
    amxc_var_set_type(sahtrace, AMXC_VAR_ID_HTABLE);
    amxc_var_add_key(cstring_t, sahtrace, "type", "stdout");
    amxc_var_add_key(uint32_t, sahtrace, "level", 200);

    assert_int_equal(_main(0, &dm, &parser), 0);

    zone = sahTraceGetZone("test_zone1");
    assert_non_null(zone);
    assert_int_equal(sahTraceZoneLevel(zone), 200);
    zone = sahTraceGetZone("test_zone2");
    assert_non_null(zone);
    assert_int_equal(sahTraceZoneLevel(zone), 200);

    assert_int_equal(_main(1, &dm, &parser), 0);

    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_set_zone_adds_zone_trace(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    const char* zone = "test_set_zone";
    const uint32_t level = 400;

    init_config(&dm, &parser);

    assert_int_equal(_main(0, &dm, &parser), 0);

    assert_true(call_trace_zone(zone, level));

    sah_trace_zone_it* it_zone = sahTraceGetZone(zone);
    assert_non_null(it_zone);
    assert_int_equal(sahTraceZoneLevel(it_zone), level);

    assert_int_equal(_main(1, &dm, &parser), 0);

    remove(FILE_PATH);
    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_set_zone_adds_zone_to_config(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    const char* zone = "test_set_zone";
    const uint32_t level = 400;

    init_config(&dm, &parser);

    assert_int_equal(_main(0, &dm, &parser), 0);

    assert_true(call_trace_zone(zone, level));
    assert_true(check_zone_in_config(&parser, zone, level));

    assert_int_equal(_main(1, &dm, &parser), 0);

    remove(FILE_PATH);
    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}

void test_sahtrace_set_zone_keeps_zone_after_reboot(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    const char* zone = "test_set_zone";
    const uint32_t level = 400;

    init_config(&dm, &parser);

    assert_int_equal(_main(0, &dm, &parser), 0);

    assert_true(call_trace_zone(zone, level));

    sah_trace_zone_it* it_zone = sahTraceGetZone(zone);
    assert_non_null(it_zone);
    assert_int_equal(sahTraceZoneLevel(it_zone), level);

    assert_int_equal(_main(1, &dm, &parser), 0);

    assert_int_equal(_main(0, &dm, &parser), 0);

    it_zone = sahTraceGetZone(zone);
    assert_non_null(it_zone);
    assert_int_equal(sahTraceZoneLevel(it_zone), level);

    assert_int_equal(_main(1, &dm, &parser), 0);

    remove(FILE_PATH);
    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}


void test_sahtrace_set_zone_wrong_level(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    const char* zone = "test_set_zone";
    const uint32_t level = 440;

    init_config(&dm, &parser);

    assert_int_equal(_main(0, &dm, &parser), 0);

    assert_false(call_trace_zone(zone, level));

    assert_int_equal(_main(1, &dm, &parser), 0);

    remove(FILE_PATH);
    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}


void test_sahtrace_list_zones(UNUSED void** state) {
    amxd_dm_t dm;
    amxo_parser_t parser;
    amxd_object_t* object = NULL;
    amxd_function_t* function = NULL;
    amxc_var_t* args = NULL;
    amxc_var_t ret;
    const char* zone1 = "test_set_zone1";
    const uint32_t level1 = 400;
    const char* zone2 = "test_set_zone2";
    const uint32_t level2 = 300;
    UNUSED bool z1_present = false;
    UNUSED bool z2_present = false;

    amxc_var_init(&ret);
    amxc_var_set_type(&ret, AMXC_VAR_ID_HTABLE);
    init_config(&dm, &parser);

    assert_int_equal(_main(0, &dm, &parser), 0);

    assert_true(call_trace_zone(zone1, level1));
    assert_true(call_trace_zone(zone2, level2));

    amxd_status_t retval = list_trace_zone(object, function, args, &ret);
    assert_true(retval == amxd_status_ok);

    assert_non_null(&ret);

    amxc_var_for_each(arg, &ret) {
        const char* zone = amxc_var_key(arg);
        uint32_t level = GET_UINT32(&ret, zone);

        if((strcmp(zone, zone1) == 0) && (level == level1)) {
            z1_present = true;
        }
        if((strcmp(zone, zone2) == 0) && (level == level2)) {
            z2_present = true;
        }

    }

    assert_true(z1_present && z2_present);
    assert_int_equal(_main(1, &dm, &parser), 0);

    remove(FILE_PATH);
    amxc_var_clean(&ret);
    amxo_parser_clean(&parser);
    amxd_dm_clean(&dm);
}
