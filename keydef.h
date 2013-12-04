#ifndef PHP_TRANSACTD_KEYDEF_H
#define PHP_TRANSACTD_KEYDEF_H

#include <bzs/db/protocol/tdap/client/table.h>
typedef struct {
    zend_object std;
    zval *link;
    bzs::db::protocol::tdap::keydef *def;
} php_transactd_keydef_t;

#ifdef __cplusplus
extern "C" {
#endif

#include "php_transactd.h"

extern PHP_TRANSACTD_API zend_class_entry *php_transactd_keydef_ce;

PHP_TRANSACTD_API int php_transactd_keydef_class_register(int module_number TSRMLS_DC);
PHP_TRANSACTD_API int php_transactd_keydef_class_init(php_transactd_keydef_t *intern, zval *return_value, int tableIndex, int insertIndex TSRMLS_DC);

#define TRANSACTD_KEYDEF_OBJ(_self, _obj) \
    _self = (php_transactd_keydef_t *)zend_object_store_get_object(_obj TSRMLS_CC)

#ifdef __cplusplus
}
#endif

#endif  /* PHP_TRANSACTD_KEYDEF_H */
