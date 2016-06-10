#ifndef _POOL4_UNIT_TEST_H
#define _POOL4_UNIT_TEST_H

//#include "nat64/mod/common/types.h"
//#include "nat64/mod/stateful/bib/entry.h"
//#include "nat64/mod/stateful/session/entry.h"

#define ASSERT_PRIMITIVE(expected, actual, specifier, name, ...) ({	\
		/* don't want these to be evaluated multiple times. */	\
		typeof(expected) __expected = expected;			\
		typeof(expected) __actual = actual;			\
		if (__expected != __actual)				\
			pr_info("Test '" name "' failed. Expected:"	\
				specifier " Actual:" specifier,		\
				##__VA_ARGS__, __expected, __actual);	\
		__expected == __actual;					\
	})

/* https://www.kernel.org/doc/Documentation/printk-formats.txt */
#define ASSERT_UINT(expected, actual, name, ...) \
		ASSERT_PRIMITIVE(expected, actual, "%u", name, ##__VA_ARGS__)
#define ASSERT_INT(expected, actual, name, ...) \
		ASSERT_PRIMITIVE(expected, actual, "%d", name, ##__VA_ARGS__)
#define ASSERT_BOOL(expected, actual, name, ...) \
		ASSERT_PRIMITIVE(expected, actual, "%u", name, ##__VA_ARGS__)
#define ASSERT_U64(expected, actual, name, ...) \
		ASSERT_PRIMITIVE(expected, actual, "%llu", name, ##__VA_ARGS__)
#define ASSERT_PTR(expected, actual, name, ...) \
		ASSERT_PRIMITIVE(expected, actual, "%p", name, ##__VA_ARGS__)
#define ASSERT_BE16(expected, actual, name, ...) \
		ASSERT_PRIMITIVE(expected, be16_to_cpu(actual), "%u", name, \
				##__VA_ARGS__)
#define ASSERT_BE32(expected, actual, name, ...) \
		ASSERT_PRIMITIVE(expected, be32_to_cpu(actual), "%u", name, \
				##__VA_ARGS__)

/*
 * Ehh... there aren't macros, but they're still all caps so they're even
 * easier to recognize.
 */

bool ASSERT_ADDR4(const char *expected, const struct in_addr *actual,
		const char *test_name);
bool __ASSERT_ADDR4(const struct in_addr *expected,
		const struct in_addr *actual,
		const char *test_name);
bool ASSERT_ADDR6(const char *expected, const struct in6_addr *actual,
		const char *test_name);
bool __ASSERT_ADDR6(const struct in6_addr *expected,
		const struct in6_addr *actual,
		const char *test_name);
//bool ASSERT_TUPLE(struct tuple *expected, struct tuple *actual,
//		char *test_name);
//bool ASSERT_BIB(struct bib_entry *expected, struct bib_entry *actual,
//		char *test_name);
//bool ASSERT_SESSION(struct session_entry *expected,
//		struct session_entry *actual,
//		char *test_name);

bool init_full(void);
void end_full(void);

/**
 * Macros to be used by the main test function.
 */
#define START_TESTS(module_name)	\
	int test_counter = 0;			\
	int failure_counter = 0;		\
	pr_info("Module '%s': Starting tests...", module_name)

#define CALL_TEST(test, test_name, ...)								\
	pr_info("Test '" test_name "': Starting...", ##__VA_ARGS__);	\
	test_counter++;													\
	if (test) {														\
		pr_info("Test '" test_name "': Success.\n", ##__VA_ARGS__);	\
	} else {														\
		pr_info("Test '" test_name "': Failure.\n", ##__VA_ARGS__);	\
		failure_counter++;											\
	}
#define INIT_CALL_END(init_function, test_function, end_function, test_name)	\
	if (!init_function)															\
		return -EINVAL;															\
	CALL_TEST(test_function, test_name)											\
	end_function
#define END_TESTS \
	pr_info("Finished. Runs: %d; Errors: %d\n\n", test_counter, failure_counter); \
	return (failure_counter > 0) ? -EINVAL : 0;


#endif /* _POOL4_UNIT_TEST_H */
