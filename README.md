# JSONitator
## Lightweight JSON Parser

### What can it do?
1. Parse json files/strings of any sizes
2. Stringify JSON

```c
#include "JSONitator.h"

void main()
{
    const char* json_str = "{\"field\":\"value\"}";

    JSON* j = json_parse(json_str);
    JSON* entry = json_object_get(j,"field");
    char* value = json_value_string(entry);

    printf("Json: %s\n", json_stringify(j));
    printf("Value: %s\n", value);
    return 0;
}
// This is an example with 0 error checking, for real-world usage check example.c
```

### TODO
- [x] Water the plants
- [ ] Functions for creating JSON structs without parsing
- [ ] Check for hidden memory leaks
- [ ] More testing with malformed JSON strings