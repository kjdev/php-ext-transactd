#ifndef PHP_TRANSACTD_KEYSEGMENT_H
#define PHP_TRANSACTD_KEYSEGMENT_H

typedef struct {
    zend_object std;
    zval *link;
    int n;
} php_transactd_keysegment_t;

#ifdef __cplusplus
extern "C" {
#endif

#include "php_transactd.h"

extern PHP_TRANSACTD_API zend_class_entry *php_transactd_keysegment_ce;

PHP_TRANSACTD_API int php_transactd_keysegment_class_register(int module_number TSRMLS_DC);
PHP_TRANSACTD_API int php_transactd_keysegment_class_init(php_transactd_keysegment_t *intern, zval *this_ptr, zval *return_value TSRMLS_DC);

#define TRANSACTD_KEYSEGMENT_OBJ(_self, _obj) \
    _self = (php_transactd_keysegment_t *)zend_object_store_get_object(_obj TSRMLS_CC)

#ifdef __cplusplus
}
#endif

#endif  /* PHP_TRANSACTD_KEYSEGMENT_H */
