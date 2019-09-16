#include <string.h>
#include <stdio.h>

#include "database.h"
#include "options.h"

#include "../c-rest-api/c-rest-api.h"

void free_json_array(JSONArray* arr);

void prog_info(Response* res, Request* req)
{
    elf_instruction_arr* prog_instr = db_get_program_execute_instruction_array();
    JSONArray* ja = ALLOCATE(JSONArray, 1);
    init_array(ja);
    JSONObject* jobj;
    for(size_t i = 0; i < prog_instr->size; i++){
        char offset[16]; // stores the hex address of the instruction
        jobj = ALLOCATE(JSONObject, 1);
        init_json(jobj);
        json_add_string_c(jobj, "symbols", prog_instr->list[i].symbols);
        printf("%lx: %s\n",prog_instr->list[i].offset, prog_instr->list[i].symbols);
        // send the offset as a string since the server cannot handle the offset
        // values properly for some reason
        snprintf(offset, 16, "0x%lx", prog_instr->list[i].offset);
        json_add_string_c(jobj, "offset", offset);
        JSONValue objval = json_value_object(jobj);
        json_array_append_value(ja, objval);
    }
    printf("len: %ld\n", prog_instr->size);
    JSONObject* fjson = ALLOCATE(JSONObject, 1);
    init_json(fjson);
    json_add_array_c(fjson, "info", ja);
    send_json(res, fjson);
    //free_json_array(ja);
    free_json(fjson);
}

void* restapi_main(void* arg)
{
    RestServer rs;
    init_server(&rs);
    if (option_verbose_output)
        set_server_option_verbose_output();
    add_url(&rs, "/proginfo", prog_info);
    run_server(&rs);
    return (void*)0;
}
