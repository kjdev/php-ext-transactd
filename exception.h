#ifndef PHP_TRANSACTD_EXCEPTION_H
#define PHP_TRANSACTD_EXCEPTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "zend_exceptions.h"

#include "php_transactd.h"

extern PHP_TRANSACTD_API zend_class_entry *php_transactd_except_ce;

PHP_TRANSACTD_API int php_transactd_except_class_register(TSRMLS_D);

#define TRANSACTD_ERR(_flag, ...) \
php_error_docref(NULL TSRMLS_CC, _flag, __VA_ARGS__)

#define TRANSACTD_EXCEPTION(_code, ...) \
zend_throw_exception_ex(php_transactd_except_ce, _code TSRMLS_CC, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif  /* PHP_TRANSACTD_EXCEPTION_H */
