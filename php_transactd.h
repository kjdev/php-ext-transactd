#ifndef PHP_TRANSACTD_H
#define PHP_TRANSACTD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#if defined(__linux__) & !defined(LINUX)
#define LINUX 1
#endif

#define TRANSACTD_EXT_VERSION "0.0.1"

#define TRANSACTD_NS "Transactd"
#define TRANSACTD_ZEND_METHOD(classname, name) \
ZEND_METHOD(Transactd_##classname, name)
#define TRANSACTD_ZEND_ME(classname, name, arg_info, flags) \
ZEND_ME(Transactd_##classname, name, arg_info, flags)
#define TRANSACTD_ZEND_MALIAS(classname, name, alias, arg_info, flags) \
ZEND_MALIAS(Transactd_##classname, name, alias, arg_info, flags)
#define TRANSACTD_LONG_CONSTANT(name, val) \
REGISTER_NS_LONG_CONSTANT(TRANSACTD_NS, name, val, CONST_CS|CONST_PERSISTENT)
#define TRANSACTD_STRING_CONSTANT(name, val) \
REGISTER_NS_STRING_CONSTANT(TRANSACTD_NS, name, val, CONST_CS|CONST_PERSISTENT)

extern zend_module_entry transactd_module_entry;
#define phpext_transactd_ptr &transactd_module_entry

#ifdef PHP_WIN32
#    define PHP_TRANSACTD_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#    define PHP_TRANSACTD_API __attribute__ ((visibility("default")))
#else
#    define PHP_TRANSACTD_API
#endif

#ifdef ZTS
#    include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(transactd)
ZEND_END_MODULE_GLOBALS(transactd)

#ifdef ZTS
#    define TRANSACTD_G(v) TSRMG(transactd_globals_id, zend_transactd_globals *, v)
#else
#    define TRANSACTD_G(v) (transactd_globals.v)
#endif

#ifdef __cplusplus
}
#endif

#endif  /* PHP_TRANSACTD_H */
