#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>
#include <lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

typedef const LV2UI_Descriptor* (* lv2ui_descriptor_t)(uint32_t index);
typedef const LV2_Descriptor* (* lv2_descriptor_t)(uint32_t index);

int main(int argc, const char **argv)
{
    void *handle;
    char *error;
    lv2ui_descriptor_t f;

    handle = dlopen ("/usr/local/lib/lv2/lsp-plugins.lv2/lsp-plugins-gtk2.so", RTLD_NOW);
    if (!handle) {
        fputs (dlerror(), stderr);
        exit(1);
    }

    f = reinterpret_cast<lv2ui_descriptor_t>(dlsym(handle, "lv2ui_descriptor"));
    if ((error = dlerror()) != NULL)  {
        fputs(error, stderr);
        exit(1);
    }
    
    printf("%p\n", f);
    const LV2UI_Descriptor *d = f(0);
    printf("%p\n", d);

    if (d != NULL)
    	printf ("%s\n", d->URI);
    dlclose(handle);
    
    return 0;
}

