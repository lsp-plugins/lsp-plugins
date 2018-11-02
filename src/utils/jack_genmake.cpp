#include <core/types.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <metadata/metadata.h>
#include <plugins/plugins.h>

namespace lsp
{
    static int gen_cpp_file(const char *path, const plugin_metadata_t *meta, const char *plugin_name, const char *cpp_name)
    {
        char fname[PATH_MAX], cppfile[PATH_MAX];

        // Replace all underscores
        strncpy(cppfile, cpp_name, PATH_MAX);
        cppfile[PATH_MAX-1] = '\0';
        for (char *p=cppfile; *p != '\0'; ++p)
            if (*p == '_')
                *p = '-';

        snprintf(fname, PATH_MAX, "%s/%s", path, cppfile);

        printf("Generating source file %s\n", fname);

        // Generate file
        FILE *out = fopen(fname, "w");
        if (out == NULL)
        {
            int code = errno;
            fprintf(stderr, "Error creating file %s, code=%d\n", fname, code);
            return -1;
        }

        // Write to file
        fprintf(out,   "//------------------------------------------------------------------------------\n");
        fprintf(out,   "// File:            %s\n", cpp_name);
        fprintf(out,   "// JACK Plugin:     %s %s - %s [JACK]\n", LSP_ACRONYM, meta->name, meta->description);
        fprintf(out,   "// JACK UID:        '%s'\n", meta->lv2_uid);
        fprintf(out,   "// Version:         %d.%d.%d\n",
                LSP_VERSION_MAJOR(meta->version),
                LSP_VERSION_MINOR(meta->version),
                LSP_VERSION_MICRO(meta->version)
            );
        fprintf(out,   "//------------------------------------------------------------------------------\n\n");

        // Write code
        fprintf(out,   "// Pass Plugin UID for factory function\n");
        fprintf(out,   "#define JACK_PLUGIN_UID     \"%s\"\n\n", meta->lv2_uid);

        fprintf(out,   "// Include factory function implementation\n");
        fprintf(out,   "#include <container/jack/main.h>\n\n");

        // Close file
        fclose(out);

        return 0;
    }

    int gen_makefile(const char *path)
    {
        char fname[PATH_MAX];
        snprintf(fname, PATH_MAX, "%s/Makefile", path);
        printf("Generating makefile %s\n", fname);

        // Generate file
        FILE *out = fopen(fname, "w");
        if (out == NULL)
        {
            int code = errno;
            fprintf(stderr, "Error creating file %s, code=%d\n", fname, code);
            return -2;
        }

        fprintf(out, "# Auto generated makefile, do not edit\n\n");

        fprintf(out, "FILES                   = $(patsubst %%.cpp, %%, $(wildcard *.cpp))\n");
        fprintf(out, "FILE                    = $(@:%%=%%.cpp)\n");
        fprintf(out, "\n");

        fprintf(out, ".PHONY: all\n\n");

        fprintf(out, "all: $(FILES)\n\n");

        fprintf(out, "$(FILES):\n");
        fprintf(out, "\t@echo \"  $(CC) $(FILE)\"\n");
        fprintf(out, "\t@$(CC) -o $(@) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) $(EXE_FLAGS) $(DL_LIBS)\n\n");

        fprintf(out, "install: $(FILES)\n");
        fprintf(out, "\t@$(INSTALL) $(FILES) $(TARGET_PATH)/");

        // Close file
        fclose(out);

        return 0;
    }

    int gen_make(const char *path)
    {
        // Generate list of plugins as CPP-files
        int code = 0;

        #define MOD_PLUGIN(x)  \
            if (code == 0) \
            { \
                if (x::metadata.ui_resource != NULL) \
                  code = gen_cpp_file(path, &x::metadata, #x, LSP_ARTIFACT_ID "-" #x ".cpp"); \
            }

        #include <metadata/modules.h>

        // Generate makefile
        if (code == 0)
            code = gen_makefile(path);

        return code;
    }
}

#ifndef LSP_IDE_DEBUG
int main(int argc, const char **argv)
{
    if (argc <= 0)
        fprintf(stderr, "required destination path");
    return lsp::gen_make(argv[1]);
}
#endif /* LSP_IDE_DEBUG */
