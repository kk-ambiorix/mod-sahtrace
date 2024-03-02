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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>

#include <debug/sahtrace.h>
#include <debug/sahtrace_macros.h>

#include <amxc/amxc.h>
#include <amxp/amxp.h>
#include <amxd/amxd_dm.h>
#include <amxo/amxo.h>

#include <amxd/amxd_object.h>
#include <amxc/amxc_macros.h>
#include <amxo/amxo_save.h>

#include "mod_main.h"

#define UNUSED __attribute__((unused))

static char* identity = NULL;
amxo_parser_t* trace_parser = NULL;

int _main(int reason,
          amxd_dm_t* dm,
          amxo_parser_t* parser);

static sah_trace_type mod_sahtrace_type_to_id(const char* type, bool daemon) {
    static const char* types[] = {
        "syslog",
        "stdout",
        "stderr",
        "file",
        "auto",
        NULL
    };
    sah_trace_type type_id = TRACE_TYPE_SYSLOG;
    int i = 0;

    for(i = 0; types[i] != NULL; i++) {
        if(strcmp(type, types[i]) == 0) {
            break;
        }
    }

    if(types[i] != NULL) {
        if(i == 4) {
            type_id = daemon? TRACE_TYPE_SYSLOG:TRACE_TYPE_STDERR;
        } else {
            type_id = (sah_trace_type) i;
        }
    }

    return type_id;
}

static uint32_t mod_sahtrace_get_default_log_level(void) {
    uint32_t default_log_level = GET_UINT32(&trace_parser->config, "default_log_level");
    return default_log_level != 0 ? default_log_level : 200;
}

static uint32_t mod_sahtrace_get_default_trace_zone_level(void) {
    uint32_t default_trace_zone_level = GET_UINT32(&trace_parser->config, "default_trace_zone_level");
    return default_trace_zone_level != 0 ? default_trace_zone_level : 200;
}

static amxc_var_t* mod_sahtrace_get(amxc_var_t* opts,
                                    amxc_var_t* sahtrace_opts,
                                    const char* opt_name,
                                    const char* alt_name) {
    amxc_var_t* var = amxc_var_get_key(sahtrace_opts,
                                       opt_name,
                                       AMXC_VAR_FLAG_DEFAULT);
    if(var == NULL) {
        var = amxc_var_get_key(opts,
                               alt_name,
                               AMXC_VAR_FLAG_DEFAULT);
    }

    return var;
}

static void mod_sahtrace_open(const char* name,
                              const char* type_name,
                              amxc_var_t* file_var,
                              bool daemon) {
    sah_trace_type type_id = mod_sahtrace_type_to_id(type_name, daemon);
    if(type_id != TRACE_TYPE_FILE) {
        sahTraceOpen(name, type_id);
    } else {
        if(amxc_var_type_of(file_var) == AMXC_VAR_ID_CSTRING) {
            const char* file_name = amxc_var_constcast(cstring_t, file_var);
            sahTraceOpenFile(name, file_name);
        }
    }
}

static void mod_sahtrace_init(amxc_var_t* options) {
    const char* name = GET_CHAR(options, "name");
    const char* type_name = "auto";
    amxc_var_t* sahtrace_opts = amxc_var_get_key(options,
                                                 "sahtrace",
                                                 AMXC_VAR_FLAG_DEFAULT);
    amxc_var_t* type_var = mod_sahtrace_get(options, sahtrace_opts,
                                            "type", "log-type");
    amxc_var_t* level_var = mod_sahtrace_get(options, sahtrace_opts,
                                             "level", "log-level");
    amxc_var_t* file_var = mod_sahtrace_get(options, sahtrace_opts,
                                            "file", "log-file");
    bool daemon = GET_BOOL(options, "daemon");
    int level = mod_sahtrace_get_default_log_level();

    if((type_var != NULL) &&
       ( amxc_var_type_of(type_var) == AMXC_VAR_ID_CSTRING)) {
        type_name = amxc_var_constcast(cstring_t, type_var);
    }

    if(identity == NULL) {
        identity = strdup(name == NULL ? "mod_sahtrace" : name);
        mod_sahtrace_open(identity, type_name, file_var, daemon);
    }

    if(level_var != NULL) {
        level = amxc_var_dyncast(uint32_t, level_var);
    }
    sahTraceSetLevel(level);
}


static void mod_sahtrace_add_zones(amxc_var_t* options) {
    amxc_var_t* zones_var = mod_sahtrace_get(options, options,
                                             "trace-zones", "log-zones");
    amxc_var_t* sahtrace_opts = amxc_var_get_key(options,
                                                 "sahtrace",
                                                 AMXC_VAR_FLAG_DEFAULT);
    amxc_var_t* level_var = mod_sahtrace_get(options, sahtrace_opts,
                                             "level", "log-level");
    int level = mod_sahtrace_get_default_trace_zone_level();

    when_null(zones_var, exit);

    if(level_var != NULL) {
        level = amxc_var_dyncast(uint32_t, level_var);
    }

    if(amxc_var_type_of(zones_var) == AMXC_VAR_ID_LIST) {
        amxc_var_for_each(zone, zones_var) {
            const char* zone_name = amxc_var_constcast(cstring_t, zone);
            if(amxc_var_type_of(zone) != AMXC_VAR_ID_CSTRING) {
                continue;
            }
            sahTraceAddZone(level, zone_name);
        }
    }

    if(amxc_var_type_of(zones_var) == AMXC_VAR_ID_HTABLE) {
        const amxc_htable_t* zones = amxc_var_constcast(amxc_htable_t, zones_var);
        amxc_htable_for_each(it, zones) {
            const char* zone_name = amxc_htable_it_get_key(it);
            amxc_var_t* var_level = amxc_var_from_htable_it(it);
            uint32_t zone_level = amxc_var_dyncast(uint32_t, var_level);
            sahTraceAddZone(zone_level, zone_name);
        }
    }

exit:
    return;
}

static int mod_sahtrace_save_config(void) {
    const char* path = GETP_CHAR(&trace_parser->config, "config-storage-file");
    amxd_status_t status = amxd_status_unknown_error;
    amxc_var_t* trace_zones = NULL;
    amxc_var_t config;

    amxc_var_init(&config);
    amxc_var_set_type(&config, AMXC_VAR_ID_HTABLE);
    trace_zones = amxc_var_add_key(amxc_htable_t, &config, "trace-zones", NULL);
    amxc_var_copy(trace_zones, GET_ARG(&trace_parser->config, "trace-zones"));

    status = amxo_parser_save_config(trace_parser, path, &config, false);
    when_failed_trace(status, exit, ERROR, "Failed to save parser config file: %s", path);

exit:
    amxc_var_clean(&config);
    return status;
}

amxd_status_t set_trace_zone(UNUSED amxd_object_t* object,
                             UNUSED amxd_function_t* func,
                             amxc_var_t* args,
                             UNUSED amxc_var_t* ret) {
    const char* zone = GET_CHAR(args, "zone");
    uint32_t level = GET_UINT32(args, "level");
    uint32_t log_levels[] = {0, 100, 200, 300, 350, 400, 500};
    int length = sizeof(log_levels) / sizeof(log_levels[0]);
    amxd_status_t status = amxd_status_unknown_error;
    amxc_var_t trace_table;
    amxc_var_t* current_zone = NULL;
    amxc_var_t* var = amxo_parser_get_config(trace_parser, "trace-zones");
    amxc_var_init(&trace_table);

    if(var == NULL) {
        amxc_var_set_type(&trace_table, AMXC_VAR_ID_HTABLE);
    } else {
        amxc_var_copy(&trace_table, var);
    }

    for(int i = 0; i < length; i++) {
        if(level == log_levels[i]) {
            status = amxd_status_ok;
            break;
        }
    }
    when_failed_trace(status, exit, ERROR, "Invalid level value: %d", level);

    current_zone = GET_ARG(&trace_table, zone);
    if(current_zone == NULL) {
        amxc_var_add_key(uint32_t, &trace_table, zone, level);
    } else {
        amxc_var_set(uint32_t, current_zone, level);
    }

    status = amxo_parser_set_config(trace_parser, "trace-zones", &trace_table);
    when_failed_trace(status, exit, ERROR, "Failed to add zone to config: %s %d", zone, level);
    when_failed(mod_sahtrace_save_config(), exit);
    mod_sahtrace_add_zones(&trace_parser->config);
exit:
    amxc_var_clean(&trace_table);
    return status;
}


amxd_status_t list_trace_zone(UNUSED amxd_object_t* object,
                              UNUSED amxd_function_t* func,
                              UNUSED amxc_var_t* args,
                              amxc_var_t* ret) {
    amxd_status_t rc = amxd_status_unknown_error;
    amxc_var_set_type(ret, AMXC_VAR_ID_HTABLE);
    amxc_var_t* var = amxo_parser_get_config(trace_parser, "trace-zones");
    amxc_var_copy(ret, var);

    rc = amxd_status_ok;
    return rc;
}

int mod_sahtrace_add_sahtrace_functions(amxd_dm_t* dm) {
    amxd_status_t rc = amxd_status_unknown_error;
    amxd_function_t* set_function = NULL;
    amxd_function_t* list_function = NULL;
    amxd_object_t* object = amxd_object_get_base(amxd_dm_get_root(dm));
    amxd_object_fn_t set_impl = set_trace_zone;
    amxd_object_fn_t list_impl = list_trace_zone;
    amxc_var_t def_val;

    amxc_var_init(&def_val);
    amxc_var_set(uint32_t, &def_val, mod_sahtrace_get_default_trace_zone_level());

    when_null(object, exit);

    rc = amxd_function_new(&set_function, "set_trace_zone", AMXC_VAR_ID_NULL, set_impl);
    when_failed(rc, exit);

    rc = amxd_function_new(&list_function, "list_trace_zones", AMXC_VAR_ID_HTABLE, list_impl);
    when_failed(rc, exit);

    rc = amxd_function_new_arg(set_function, "zone", AMXC_VAR_ID_CSTRING, NULL);
    when_failed(rc, exit);
    amxd_function_arg_set_attr(set_function, "zone", amxd_aattr_in, true);
    rc = amxd_function_new_arg(set_function, "level", AMXC_VAR_ID_UINT32, &def_val);
    when_failed(rc, exit);
    amxd_function_arg_set_attr(set_function, "level", amxd_aattr_in, true);

    rc = amxd_object_add_function(object, set_function);
    when_failed(rc, exit);

    rc = amxd_object_add_function(object, list_function);
    when_failed(rc, exit);

exit:
    amxc_var_clean(&def_val);
    if(rc != amxd_status_ok) {
        amxd_function_delete(&set_function);
        amxd_function_delete(&list_function);
    }
    return rc;
}


int _main(int reason,
          amxd_dm_t* dm,
          amxo_parser_t* parser) {

    switch(reason) {
    case 0:     // START
        trace_parser = parser;
        amxd_object_t* root = amxd_dm_get_root(dm);
        const char* dir = GET_CHAR(&parser->config, "config-storage-dir");
        const char* path = GET_CHAR(&parser->config, "config-storage-file");

        if((dir != NULL) && (*dir != 0)) {
            struct stat stats;
            int rv = stat(dir, &stats);

            // Check for dir existence
            if((rv != 0) || !S_ISDIR(stats.st_mode)) {
                syslog(LOG_USER | LOG_WARNING, "Trace config folder not found: %s", dir);
            }
        }

        amxo_parser_parse_file(trace_parser, path, root);

        mod_sahtrace_init(&parser->config);
        mod_sahtrace_add_zones(&parser->config);
        mod_sahtrace_add_sahtrace_functions(dm);
        break;
    case 1:     // STOP
        sahTraceClose();
        free(identity);
        identity = NULL;
        break;
    default:
        break;
    }

    return 0;
}
