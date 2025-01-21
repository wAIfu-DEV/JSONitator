#include "stdio.h"
#include "time.h"

// Uncomment for call stack trace
// #define JSON_PARSER_DEBUG 1

// Uncomment to supress error messages
// #define JSON_PARSER_SILENT 1

#include "JSONitator.h"

char *file_read(const char *file_path)
{
    FILE *f = fopen(file_path, "rb");

    if (f == NULL)
        return NULL;

    fseek(f, 0, SEEK_END);
    uint64_t fsize = ftell(f);
    rewind(f);

    char *file_buff = malloc(sizeof(char) * fsize);
    uint64_t read_size = fread(file_buff, sizeof(char), fsize, f);

    if (fsize != read_size)
    {
        printf("Failed to read entirety of file.\n");
        return NULL;
    }

    fclose(f);
    return file_buff;
}

int main(void)
{
    const char *FILE_PATH = "in.json";
    char *file_buff = file_read(FILE_PATH);

    if (file_buff == NULL)
    {
        printf("Failed to read file.\n");
        return 1;
    }

    //==========================================================================
    // Parse JSON file
    //==========================================================================

    JSON *json = json_parse(file_buff);

    // json_parse can fail
    if (json == NULL)
    {
        printf("json is null.\n");
        return 1;
    }
    free(file_buff);

    //==========================================================================
    // Stringify JSON struct
    //==========================================================================

    char *stringified = json_stringify(json);

    // json_stringified can fail
    if (stringified == NULL)
    {
        printf("Failed to stringify json struct.\n");
        return 1;
    }

    FILE *fstream = fopen("out.json", "w");

    if (fstream == NULL)
    {
        printf("Failed to open out.json file.\n");
        return 1;
    }

    size_t len = strlen(stringified);
    fwrite(stringified, sizeof(char), len, fstream);
    fclose(fstream);
    free(stringified);

    //==========================================================================
    // Access into JSON objects
    //==========================================================================

    // Equivalent to JS: json["behaviour"]
    JSON *shallow_val = json_object_get(json, "behaviour");

    // Equivalent to JS: json["_"]["user_name"]["default"]
    // Works for both object and array access (index must be stringified)
    JSON *nested_val = json_get_deep(json, 3, (const char *[3]){"_", "user_name", "default"});

    // access functions might fail, handle the error case
    if (nested_val == NULL)
    {
        printf("Failed to access: json[\"_\"][\"user_name\"][\"default\"]\n");
        return 1;
    }

    // Example with array access:
    JSON *nested_val2 = json_get_deep(json, 4, (const char *[4]){"moderation", "blacklisted_chatters", "value", "0"});

    if (nested_val2 == NULL)
    {
        printf("Failed to access: json[\"moderation\"][\"blacklisted_chatters\"][\"value\"][0]\n");
        return 1;
    }

    printf("Deep value type: \"%s\"\n", json_type_to_str(nested_val->type));

    // Here deep_val->type == VAL_STRING
    // to extract the data, use typed json_value functions
    char *default_username = json_value_string(nested_val);
    char *blacklisted = json_value_string(nested_val2);

    printf("json[\"_\"][\"user_name\"][\"default\"] == \"%s\"\n", default_username);
    printf("json[\"moderation\"][\"blacklisted_chatters\"][\"value\"][0] == \"%s\"\n", blacklisted);

    JSON *nested_obj = json_get_deep(json, 2, (const char *[2]){"llm", "temperature"});

    if (nested_obj == NULL)
    {
        printf("Failed to access: json[\"llm\"][\"temperature\"]\n");
        return 1;
    }

    // Print a JSON object
    printf("json[\"llm\"][\"temperature\"] == ");
    json_print(nested_obj);

    // Clear the memory allocated during parsing
    // Only call on the direct output of json_parse
    json_free(json);
    return 0;
}