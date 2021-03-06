#define TENYR_PLUGIN 0
#include "plugin.h"
#include "common.h"

int tenyr_plugin_host_init(void *libhandle)
{
    void *ptr = dlsym(libhandle, "tenyr_plugin_init");
    plugin_init *init = ALIASING_CAST(plugin_init,ptr);
    if (init) {
        struct tenyr_plugin_ops ops = {
            .fatal = fatal_,
            .debug = debug_,
        };

        init(&ops);
    }

    return 0;
}

