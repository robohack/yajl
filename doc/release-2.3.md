# Version 2.2.92 (beta)

This (beta) release includes the following major fixes, changes, and
updates:

- major overhaul of the Makefiles to extract the basic core Makefile and to
  allow the wrapper Makefiles to be used in other projects

- avoid UB when passing pointers to slightly incompatible functions

- parse numbers with strtoll() (in the "C" locale) to avoid overrun and underrun
  and any other problems that come from a hand-rolled number parser

- avoid potential buffer overrun  when parsing UTF-8 surrogate characters

- other minor code fixes and cleanups


	**Beta Changelog**: https://github.com/robohack/yajl/compare/release-2.2.91...release-2.2.92
	**Full Changelog**: https://github.com/robohack/yajl/compare/release-2.2...release-2.2.92

# Version 2.2.91 (beta)

This (beta) release includes the following major fixes, changes, and
updates:

- error messages now include the line number and character offset where
  the error was detected

- numbers are always validated for over/under-flow if no parser
  callbacks are used

- invalid leading zeros on numbers now produce a unique new lexer error
  message (and are always correctly identified)

- many fixes and updates to the Makefiles have made the build more
  portable and more robust, and improve the results on macOS
  w.r.t. dynamic libraries.

- tests and examples all include definitions for allocation functions
  that do error checking

Note that the previous release (release-2.2) included a fix for
CVE-2023-33460 (memory leak in yajl_parse_tree()) in commit#38220af, but
this release also includes the related yajl_complete_parse() fixes for
the example and test programs.

Note also MKDOC use has been deprecated.  If you don't have Cxref
available then just set CXREF=true on the make command line.

	**Full Changelog**: https://github.com/robohack/yajl/compare/release-2.2...release-2.2.92
