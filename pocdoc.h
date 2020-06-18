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

#ifndef POCDOC_H
#define POCDOC_H

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <memory>
#include <iostream>
#include <optional>
#include <tuple>
#include <fstream>
#include <cstdio>
#include <cassert>
#include <clang-c/Index.h>

namespace pocdoc {

bool isclass(const CXCursorKind &kind) {
	switch (kind) {
	case CXCursor_ClassDecl:
	case CXCursor_ClassTemplate:
	case CXCursor_ClassTemplatePartialSpecialization:
		return true;
	default:
		return false;
	}
}

bool iscontainer(const CXCursorKind &kind) {
	switch (kind) {
	case CXCursor_StructDecl:
	case CXCursor_UnionDecl:
	case CXCursor_ClassDecl:
	case CXCursor_ClassTemplate:
	case CXCursor_ClassTemplatePartialSpecialization:
	case CXCursor_EnumDecl:
	case CXCursor_EnumConstantDecl:
		return true;
	default:
		return false;
	}
}

bool isfunc(const CXCursorKind &kind) {
	switch (kind) {
	case CXCursor_CXXMethod:
	case CXCursor_FunctionDecl:
	case CXCursor_FunctionTemplate:
	case CXCursor_Constructor:
	case CXCursor_Destructor:
		return true;
	default:
		return false;
	}
}

bool isdecl(const CXCursorKind &kind) {
	if (iscontainer(kind) || isfunc(kind)) {
		return true;
	}
	switch (kind) {
	case CXCursor_FieldDecl:
	case CXCursor_UsingDeclaration:
	case CXCursor_TypedefDecl:
	case CXCursor_TypeAliasDecl:
	case CXCursor_TypeAliasTemplateDecl:
	case CXCursor_VarDecl:
		return true;
	default:
		return false;
	}
}

const char *decl_str(const CXCursorKind &kind) {
	switch (kind) {
	case CXCursor_StructDecl:  return "Struct";
	case CXCursor_UnionDecl:   return "Union";
	case CXCursor_EnumDecl:    return "Enum";

	case CXCursor_EnumConstantDecl:
		return "Enum Constant";

	case CXCursor_ClassDecl:
	case CXCursor_ClassTemplate:
		return "Class";

	case CXCursor_CXXMethod:
	case CXCursor_FunctionDecl:
	case CXCursor_FunctionTemplate:
		return "Function";

	case CXCursor_Constructor:      return "Constructor";
	case CXCursor_Destructor:       return "Destructor";
	case CXCursor_UsingDeclaration: return "Using";
	case CXCursor_TypedefDecl:      return "Typedef";

	case CXCursor_TypeAliasDecl:
	case CXCursor_TypeAliasTemplateDecl:
		return "Type Alias";

	case CXCursor_VarDecl:    return "Variable";
	case CXCursor_FieldDecl:  return "Field";

	default:
		// Kind should not be printed
		return nullptr;
	}
}

std::string get_qualified_name(const CXCursor &cursor) {
	auto kind = clang_getCursorKind(cursor);
	if (kind == CXCursor_TranslationUnit || kind == CXCursor_FirstInvalid) {
		return "";
	}
	auto spelling = clang_getCursorSpelling(cursor);
	std::string name(clang_getCString(spelling));
	clang_disposeString(spelling);

	auto res = get_qualified_name(clang_getCursorSemanticParent(cursor));
	if (res == "") {
		return name;
	}
	return res + "::" + name;
}

template<typename... Args>
void ltrim(std::string &str, int count, Args... ch) {
	if (count == -1) {
		count = str.size() + 1;
	}
	str.erase(str.begin(), std::find_if(str.begin(), str.end(),
		[ch..., &count](char c) {
			bool match = ((c == ch) || ...);
			if (match) --count;
			return (!match) || count < 0;
		}));
}

template<typename... Args>
void ltrim(std::string &str, Args... ch) {
	ltrim(str, -1, ch...);
}

template<typename... Args>
void rtrim(std::string &str, int count, Args... ch) {
	if (count == -1) {
		count = str.size() + 1;
	}
	str.erase(std::find_if(str.rbegin(), str.rend(),
		[ch..., &count](char c) {
			bool match = ((c == ch) || ...);
			if (match) --count;
			return (!match) || count < 0;
		}).base(), str.end());
}

template<typename... Args>
void rtrim(std::string &str, Args... ch) {
	rtrim(str, -1, ch...);
}

struct SourceRange {
	unsigned line_start, line_end;
};

struct Node;

using QualifiedName = std::string;
using NodeMap = std::map<QualifiedName, std::unique_ptr<Node>>;

struct Node {
	std::string name;
	std::string qualified_name;

	CXCursorKind kind;
	CX_CXXAccessSpecifier access;

	SourceRange decl_range;
	std::optional<SourceRange> doc_range;

	NodeMap children;

	Node() = default;

	Node(const std::string &name, const std::string &qname,
	     CXCursorKind kind, CX_CXXAccessSpecifier access,
	     SourceRange decl_range, std::optional<SourceRange> doc_range)
		: name{name}
		, qualified_name{qname}
		, kind{kind}
		, access{access}
		, decl_range{decl_range}
		, doc_range{doc_range} {}
};

struct Options {
	bool include_private = false;
	bool build_toc = true;
	bool verbose = false;
	std::string output_dir;
	std::string trim_path_prefix;
};

class Header {
public:
	std::string filename;

	Header(const std::string &filename,
	       std::vector<std::string> &&lines, Options opt)
		: filename{filename}, lines{std::move(lines)}, options{opt} {}

	void parse(CXTranslationUnit tu);

	const std::vector<std::string> &build();

	void insert(const CXCursor &cursor, std::unique_ptr<Node> node);
	std::unique_ptr<Node> *find(NodeMap &declmap,
	                            const QualifiedName &name) const;

	std::string parse_source(unsigned start, unsigned end, int indent) const;
	std::string parse_comment(const SourceRange &range) const;

private:
	std::optional<SourceRange> find_doc(unsigned linenum) const;

	void append_child_nodes(const std::unique_ptr<Node> &parent, int indent);

	void append_decl(const char *pre, const char *kind,
	                 const std::unique_ptr<Node> &node);

	void append_fields(const std::unique_ptr<Node> &parent);

	void build(NodeMap &declmap, int depth);

	void build_toc(std::vector<std::string> &toc,
	               NodeMap &declmap, int depth) const;

	template<typename... Args>
	void append(const char *fmt, Args... args);
	void append(const char *s);

	std::vector<std::string> compiled;
	std::vector<std::string> lines;
	NodeMap declarations;
	Options options;
};

template<typename... Args>
void Header::append(const char *fmt, Args... args) {
	char sbuf[512];
	snprintf(sbuf, sizeof(sbuf)-1, fmt, args...);
	sbuf[sizeof(sbuf)-1] = 0;
	compiled.emplace_back(std::string{sbuf});
}

void Header::append(const char *s) {
	compiled.emplace_back(s);
}

std::unique_ptr<Node> *Header::find(NodeMap &map,
                                    const QualifiedName &name) const {
	if (map.find(name) != map.end()) {
		return &map[name];
	}
	for (auto &kv : map) {
		auto &parent = kv.second;
		auto *node = find(parent->children, name);
		if (node != nullptr) {
			return node;
		}
	}
	return nullptr;
}

void Header::insert(const CXCursor &cursor, std::unique_ptr<Node> node) {
	if (declarations.find(node->qualified_name) != declarations.end()) {
		// Duplicare declaration, bias the documented declaration,
		// if neither node is documented, bias the most recent declaration
		auto &current = declarations[node->qualified_name];
		if (current->doc_range) {
			return;
		}
		declarations[node->qualified_name] = std::move(node);
		return;
	}
	auto parent = clang_getCursorSemanticParent(cursor);
	if (isfunc(clang_getCursorKind(parent))) {
		// Prevents variables declared inside functions to be added
		return;
	}
	if (iscontainer(clang_getCursorKind(parent))) {
		auto parent_name = get_qualified_name(parent);
		auto &parent_node = *find(declarations, parent_name);
		parent_node->children.insert({node->qualified_name, std::move(node)});
		return;
	}
	declarations.insert({node->qualified_name, std::move(node)});
}

void Header::parse(CXTranslationUnit tu) {
	auto tu_cursor = clang_getTranslationUnitCursor(tu);
	clang_visitChildren(tu_cursor, [](auto cursor, auto, auto cdata) {
		auto self = reinterpret_cast<Header *>(cdata);
		auto loc = clang_getCursorLocation(cursor);

		if (clang_Location_isFromMainFile(loc) == 0) {
			return CXChildVisit_Continue;
		}
		auto kind = clang_getCursorKind(cursor);

		if (!clang_isDeclaration(kind) || !isdecl(kind)) {
			return CXChildVisit_Recurse;
		}

		auto access = clang_getCXXAccessSpecifier(cursor);
		auto src_range = clang_getCursorExtent(cursor);
		auto loc_start = clang_getRangeStart(src_range);
		auto loc_end = clang_getRangeEnd(src_range);

		unsigned line_start, line_end;
		clang_getSpellingLocation(loc_start, nullptr,
		                          &line_start, nullptr, nullptr);

		clang_getSpellingLocation(loc_end, nullptr,
		                          &line_end, nullptr, nullptr);

		auto name = clang_getCursorSpelling(cursor);
		auto qname = get_qualified_name(cursor);

		if (self->options.verbose) {
			printf("%s [%d-%d]: %s %s\n", self->filename.c_str(),
			                              line_start, line_end,
			                              decl_str(kind),
			                              qname.c_str());
		}

		std::string name_str{clang_getCString(name)};

		SourceRange decl_range{line_start, line_end};
		auto doc_range = self->find_doc(line_start);
		auto node = std::make_unique<Node>(name_str, qname, kind, access,
		                                   decl_range, doc_range);

		self->insert(cursor, std::move(node));
		clang_disposeString(name);
		return CXChildVisit_Recurse;
	}, this);
}

std::optional<SourceRange> Header::find_doc(unsigned linenum) const {
	if (--linenum < 0 || linenum >= lines.size()) {
		return {};
	}
	std::string line;
	unsigned start;
	unsigned end = linenum;
	bool found_comment = false;

	for (int repeat = 0; linenum >= 0 && repeat < 2; --linenum) {
		line = lines[linenum];
		ltrim(line, ' ', '\t');
		if (line.rfind("//", 0) != 0) {
			if (found_comment) {
				start = linenum + 1;
				break;
			}
			--end;
			++repeat;
			continue;
		}
		found_comment = true;
	}

	if (!found_comment) {
		return {};
	}
	return SourceRange{start, end};
}

std::string Header::parse_source(unsigned start, unsigned end,
                                 int indent) const {
	assert(start < lines.size() && end < lines.size());
	std::string indent_str(indent, ' ');
	std::string result;

	for (unsigned i = (start - 1); i < end; ++i) {
		auto line = lines[i];
		ltrim(line, '\t');
		rtrim(line, ';');

		if (line.rfind('{') != std::string::npos) {
			// The end position is the true ending of the declaration
			// including the funciton body if it has any. We only want
			// the declaration signature so we stop early if an opening
			// brace is reached.
			rtrim(line, '{', ' ');
			result += indent_str + line;
			break;
		}

		result += indent_str + line;
		if (i < (end - 1)) {
			result.push_back('\n');
		}
	}
	return result;
}

std::string Header::parse_comment(const SourceRange &range) const {
	std::string line;
	std::string comment;

	for (unsigned ln = range.line_start; ln <= range.line_end; ++ln) {
		line = lines[ln];
		ltrim(line, ' ','\t');
		ltrim(line, 3, '/');
		ltrim(line, 1, ' ');
		if (line.empty()) {
			comment.push_back('\n');
			comment.push_back('\n');
			continue;
		}
		if (ln < range.line_end) {
			line.push_back(' ');
		}
		comment += line;
	}
	return comment;
}

void Header::append_child_nodes(const std::unique_ptr<Node> &parent,
                                int indent) {
	// Nodes are sorted so they appear in the same order as they
	// were declared in the source. Only when displaying the source
	// of classes, structs, unions and enums.
	std::vector<Node *> sorted_nodes;
	for (const auto &kv : parent->children) {
		const auto &node = kv.second;
		sorted_nodes.push_back(node.get());
	}
	std::sort(sorted_nodes.begin(), sorted_nodes.end(),
		[](const Node *lhs, const Node *rhs) {
			return lhs->decl_range.line_start < rhs->decl_range.line_start;
		});

	std::unordered_set<unsigned> parsed_decl;
	auto access = CX_CXXInvalidAccessSpecifier;

	for (size_t i = 0; i < sorted_nodes.size(); ++i) {
		const auto *node = sorted_nodes[i];
		auto [line_start, line_end] = node->decl_range;

		if (!options.include_private && node->access == CX_CXXPrivate) {
			continue;
		}

		// Avoid duplicating lines that include multiple declarations
		// as to not output 'float x, y;' twice for example.
		if (parsed_decl.find(line_start) != parsed_decl.end()) {
			continue;
		}
		parsed_decl.insert(line_start);
		auto formatted = parse_source(line_start, line_end, indent);

		auto semi = node->kind == CXCursor_EnumConstantDecl
		          ? "" : ";";
		auto lf = node->kind == CXCursor_EnumConstantDecl
		          ? "" : "\n";

		if (node->access != access) {
			// public is default for structs so it is not included
			if (node->access == CX_CXXPublic && isclass(parent->kind)) {
				append("public:\n");
			} else if (node->access == CX_CXXProtected) {
				append("protected:\n");
			} else if (node->access == CX_CXXPrivate) {
				append("private:\n");
			}
			access = node->access;
		}

		append("%s%s%s\n", formatted.c_str(), semi, lf);
	}
	if (compiled.size() == 0) {
		// Really should not happen
		return;
	}
	auto &last = compiled.back();
	if (last.size() > 1 && last[last.size() - 2] == '\n') {
		// Remove extra line ending
		last.pop_back();
	}
}

void Header::append_decl(const char *pre, const char *kind,
                         const std::unique_ptr<Node> &node) {
	auto [line_start, line_end] = node->decl_range;
	auto formatted = parse_source(line_start, line_end, 0);

	auto semi = node->kind == CXCursor_EnumConstantDecl
	          ? "" : ";";

	append("%s %s `%s`\n\n", pre, kind, node->qualified_name.c_str());
	append("```cpp\n");
	append("%s%s\n", formatted.c_str(), semi);
	append("```\n");
}

void Header::append_fields(const std::unique_ptr<Node> &parent) {
	bool has_fields = false;
	append("#### Member Variables\n");

	for (const auto &kv : parent->children) {
		const auto &node = kv.second;
		if (node->kind != CXCursor_FieldDecl || !node->doc_range) {
			continue;
		}
		auto comment = parse_comment(node->doc_range.value());
		append("* `%s`  %s\n", node->name.c_str(), comment.c_str());
		has_fields = true;
	}
	if (!has_fields) {
		compiled.pop_back();
		return;
	}
	append("\n");
}

const std::vector<std::string> &Header::build() {
	append("# %s\n\n", filename.c_str());
	build(declarations, 0);

	if (options.build_toc) {
		std::vector<std::string> toc;
		build_toc(toc, declarations, 0);
		toc.push_back("\n---\n\n");
		compiled.insert(compiled.begin() + 1, toc.begin(), toc.end());
	}
	return compiled;
}

void Header::build_toc(std::vector<std::string> &toc,
                       NodeMap &declmap, int depth) const {
	static char buffer[512];
	for (const auto &kv : declmap) {
		const auto &node = kv.second;
		if (!node->doc_range && !iscontainer(node->kind)) {
			continue;
		}

		if (node->kind == CXCursor_FieldDecl) {
			// Cannot have a link to a member variable
			continue;
		}

		auto kstr = decl_str(node->kind);
		if (kstr == nullptr || node->name == "") {
			continue;
		}
		auto link = std::string{kstr} + "-" + node->qualified_name;
		auto n = sprintf(buffer, "%s* [%s](#%s)\n",
		                 std::string(depth*4, ' ').c_str(),
		                 node->name.c_str(),
		                 link.c_str());
		buffer[n] = '\0';
		toc.emplace_back(std::string{buffer});
		if (node->children.size() > 0) {
			build_toc(toc, node->children, depth + 1);
		}
	}
}

void Header::build(NodeMap &declmap, int depth) {
	for (const auto &kv : declmap) {
		auto &node = kv.second;
		auto kstr = decl_str(node->kind);

		if (kstr == nullptr) {
			// Non printable declaration
			continue;
		}

		if (!options.include_private && node->access == CX_CXXPrivate) {
			continue;
		}

		auto [line_start, line_end] = node->decl_range;

		// Checking if start != end ensures we don't try to add children
		// to a one line declaration. For example take 'enum Enum {A, B};',
		// since the child declarations already appear on the same line
		// where the parent was declared we don't want to include them again.
		if (iscontainer(node->kind) && line_start != line_end) {
			auto formatted = parse_source(line_start, line_end, 0);

			auto pre = depth == 0 ? "##" : "###";
			append("%s %s `%s`\n\n", pre, kstr, node->qualified_name.c_str());
			append("```cpp\n");
			append("%s {\n", formatted.c_str());
			append_child_nodes(node, 4);
			append("};\n");
			append("```\n");

			// Containers with children are never excluded whether
			// they have comments or not
			if (node->doc_range) {
				auto comment = parse_comment(node->doc_range.value());
				append("%s\n\n", comment.c_str());
				comment.clear();
			}

			append_fields(node);
			build(node->children, depth + 1);

			if (depth == 0) {
				append("\n---\n\n");
			}
			continue;
		}
		if (node->doc_range && node->kind != CXCursor_FieldDecl) {
			auto comment = parse_comment(node->doc_range.value());
			// Only declarations that have comments will be documented
			append_decl(depth == 0 ? "##" : "###", kstr, node);
			append("%s\n\n", comment.c_str());
			comment.clear();
		}
	}
}

bool build_docs(const std::string &filename, Options options) {
	auto safe_name = [](std::string path) {
		std::replace(path.begin(), path.end(), '/', '_');
		std::replace(path.begin(), path.end(), '\\', '_');
		return path;
	};

	std::vector<std::string> source;
	auto tmp_header = "dsdoc_tmp_" + safe_name(filename);
	std::ifstream infile{filename};
	std::ofstream outfile{tmp_header};

	std::string line;
	std::string lncpy;
	while (std::getline(infile, line)) {
		lncpy = line;
		ltrim(lncpy, ' ', '\t');
		if (lncpy.size() > 0 && lncpy[0] == '#') {
			// Ideally this step is not necessary but libclang
			// doesn't seem to traverse files with lots of includes.
			continue;
		}
		outfile << line << '\n';
		source.push_back(line);
	}
	outfile.close();

	const char *args[] = {"-x", "c++", 0};
	auto index = clang_createIndex(0, 0);

	auto tu = clang_parseTranslationUnit(
		index, tmp_header.c_str(),
		args, (sizeof(args) / sizeof(*args)) - 1,
		nullptr, 0,
		CXTranslationUnit_SkipFunctionBodies);

	std::remove(tmp_header.c_str());
	if (tu == nullptr) {
		return false;
	}

	auto out_filename = safe_name(filename);
	if (options.trim_path_prefix != "") {
		auto trim_pos = filename.find(options.trim_path_prefix);
		if (trim_pos != std::string::npos) {
			out_filename.erase(trim_pos, options.trim_path_prefix.size());
		}
	}

	Header header{filename, std::move(source), options};
	header.parse(tu);

	out_filename += ".md";
	if (options.output_dir != "") {
		out_filename = options.output_dir + "/" + out_filename;
	}

	std::ofstream md{out_filename};
	for (const auto &compiled : header.build()) {
		md << compiled;
	}
	return true;
}

} // pocdoc

#endif // POCDOC_H
