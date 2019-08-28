#include <core/types.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <metadata/metadata.h>
#include <plugins/plugins.h>
#include <utils/common.h>

#include <container/vst/defs.h>

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

        // Generate file name
        snprintf(fname, PATH_MAX, "%s/%s", path, cppfile);
        printf("Generating source file %s\n", fname);

        // Generate file
        FILE *out = fopen(fname, "w");
        if (out == NULL)
        {
            int code = errno;
            fprintf(stderr, "Error creating file %s, code=%d\n", fname, code);
            return -2;
        }

        // Write to file
        VstInt32 uid        = vst_cconst(meta->vst_uid);
        fprintf(out,   "//------------------------------------------------------------------------------\n");
        fprintf(out,   "// File:            %s\n", cppfile);
        fprintf(out,   "// VST Plugin:      %s %s - %s\n", LSP_ACRONYM, meta->description, meta->name);
        fprintf(out,   "// VST UID:         '%s' (%ld)\n", meta->vst_uid, long(uid));
        fprintf(out,   "// Version:         %d.%d.%d\n",
                LSP_VERSION_MAJOR(meta->version),
                LSP_VERSION_MINOR(meta->version),
                LSP_VERSION_MICRO(meta->version)
            );
        fprintf(out,   "//------------------------------------------------------------------------------\n\n");

        // Write code
        fprintf(out,   "// Pass UID for factory function\n");
        fprintf(out,   "#define VST_PLUGIN_UID      %ld\n\n", long(uid));

        fprintf(out,   "// Include factory function implementation\n");
        fprintf(out,   "#include <container/vst/main.h>\n\n");

        // Close file
        fclose(out);

        return 0;
    }

    static int gen_makefile(const char *path)
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

        fprintf(out, "FILES                   = $(patsubst %%.cpp, %%.so, $(wildcard *.cpp))\n");
        fprintf(out, "FILE                    = $(@:%%.so=%%.cpp)\n");
        fprintf(out, "\n");

        fprintf(out, ".PHONY: all\n\n");

        fprintf(out, "all: $(FILES)\n\n");

        fprintf(out, "$(FILES):\n");
        fprintf(out, "\t@echo \"  $(CXX) $(FILE)\"\n");
        fprintf(out, "\t@$(CXX) -o $(@) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) $(FILE) $(SO_FLAGS) $(DL_LIBS)\n\n");

        // Close file
        fclose(out);

        return 0;
    }

    int gen_vst_make(const char *path)
    {
        // Generate list of plugins as CPP-files
        int code = 0;

        #define MOD_PLUGIN(plugin, ui)  \
            if ((code == 0) && (plugin::metadata.vst_uid != NULL)) \
                code = gen_cpp_file(path, &plugin::metadata, #plugin, LSP_ARTIFACT_ID "-vst-" #plugin ".cpp");

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
    return lsp::gen_vst_make(argv[1]);
}
#endif /* LSP_IDE_DEBUG */
