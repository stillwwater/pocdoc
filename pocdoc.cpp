// Copyright (c) 2020 stillwwater
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>

#include "pocdoc.h"

static const char *Usage =
"pocdoc [options] cpp-files...\n\n"
"options:\n"
"  -o output_directory  The path to an output directory for compiled\n"
"                       markdown files. The directory must exist.\n\n"
"  -v                   Verbose output\n\n"
"  -include-private     Whether to include private member declarations.\n\n"
"  -no-toc              This option disables creating a table of contents\n"
"                       in the beginning of each markdown file.\n\n"
"  -trim-path path      Trims path from the beginning of all given c++\n"
"                       file names to use in the markdown output.\n";

std::tuple<pocdoc::Options, std::vector<std::string>>
parse_flags(int argc, char *argv[]) {
    pocdoc::Options opt;
    std::vector<std::string> tail;

    for (int i = 1; i < argc; ++i) {
        auto value = std::string{argv[i]};
        if (value == "-o") {
            opt.output_dir = argv[++i];
            continue;
        }
        if (value == "-v") {
            opt.verbose = true;
            continue;
        }
        if (value == "-trim-path") {
            opt.trim_path_prefix = argv[++i];
            continue;
        }
        if (value == "-include-private") {
            opt.include_private = true;
            continue;
        }
        if (value == "-no-toc") {
            opt.build_toc = false;
            continue;
        }
        tail.emplace_back(std::move(value));
    }
    return {opt, tail};
}

int main(int argc, char *argv[]) {
	if (argc <= 1 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "%s", Usage);
		return 1;
	}
    auto [opt, filenames] = parse_flags(argc, argv);

	if (filenames.size() == 0) {
		fprintf(stderr, "error: missing input\n");
		return 1;
	}

	if (opt.output_dir != "") {
		struct stat pathinfo;
		if (stat(opt.output_dir.c_str(), &pathinfo) != 0) {
			fprintf(stderr, "error: output directory '%s' does not exist\n",
				opt.output_dir.c_str());
			return 1;
		}

		if ((pathinfo.st_mode & S_IFDIR) == 0) {
			fprintf(stderr, "error: output path '%s' is not a directory\n",
				opt.output_dir.c_str());
			return 1;
		}
	}

	bool error = false;
    for (const auto &file : filenames) {
        if (!pocdoc::build_docs(file, opt)) {
            fprintf(stderr, "error: could not parse c++ source file: %s\n",
				file.c_str());
			error = true;
        }
    }
	return int(error);
}
