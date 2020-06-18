#ifndef TEST_H
#define TEST_H

struct EmptyStructDecl {

};

struct StructDeclA {
	std::string name;
	std::vector<uint64_t> items;

};

enum class EnumClassDecl {
	Constant_1,
	Constant_2,
	Constant_3,

	// Same value as Constant_3
	Last_Constant = Constant_3
};

namespace ns {

// A type alias
using u64 = long long;

// Another alias, this doesn't work :(
// since we don't know what std::vector<int>
// is without the include
using vec = std::vector<std::string>;

// A typedef
typedef uint32_t u32;

// This enum is written in one line
enum AllInOneLineEnum { Constant_A, Constant_B };

struct StructDeclB {
	float x, y, z;

	// Constructor for StructDeclB
	StructDeclB(float x, float y, float z)
	   : x{x}, y{y}, z{z} {}

	// Static inline member function in StructDeclB
	static inline StructDeclB member_func(float x, float y) {
		// Make sure variables declared in function don't get parsed
		StructDeclB decl{x, y, 0};
		return decl;
	}
};

namespace ns2 {

// A struct in a nested namespace
struct StructDeclC {
	int x;
	int y;
};

} // namespace ns2

struct StructDeclD {
	union {
		u64 var64;
		u32 var32;
	}
}

class BaseClassDecl {
public:
	// A *virtual* function in BaseClassDecl
	//
	//     // This is a comment in a code block
	//
	// See the example above.
	virtual void virtual_func(const std::unique_ptr<StructDeclB> &decl_b) const;
	virtual ~BaseClassDecl() = default;

	void member_func(int x, int y) noexcept;

	// Some template function in BaseClassDecl
	template<typename T>
	void template_func() const;

	// A template function that takes up many lines
	template<typename... Args>
	std::string
	template_func2(const std::vector<std::shared_ptr> &long_variable_name1,
	               const std::vector<std::shared_ptr> &long_variable_name2);
private:
	int private_variable;
	void private_function() const;

	// This function has a comment, but it won't show up in the docs.
	void private_function_with_comment();
}

class ClassDecl : public BaseClassDecl {
public:
	// This member of ClassDecl is documented
	int documented_member;

	// This will also show up
	std::vector<std::string> vector_of_string;

	// This one isn't being picked up by clang when traversing
	// the AST?? :(
	std::vector<int> vector_of_int;

	int this_one_will_not;
}

void BaseClassDecl::member_func(int x, int y) noexcept {

}

// This comment will not appear in the docs
template<typename T>
void BaseClassDecl::template_func() const {

}

// Some function in namespace ns
int free_function() {

}

// Another function declaration in namespace ns
int free_function_2();

} // namespace ns

#endif // TEST_H
