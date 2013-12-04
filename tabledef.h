#ifndef PHP_TRANSACTD_TABLEDEF_H
#define PHP_TRANSACTD_TABLEDEF_H

#include <bzs/db/protocol/tdap/client/table.h>
typedef struct {
    zend_object std;
    zval *link;
    zend_bool is_new;
    bzs::db::protocol::tdap::tabledef *def;
    bzs::db::protocol::tdap::tabledef **def_ptr;
} php_transactd_tabledef_t;

#ifdef __cplusplus
extern "C" {
#endif

#include "php_transactd.h"

extern PHP_TRANSACTD_API zend_class_entry *php_transactd_tabledef_ce;

PHP_TRANSACTD_API int php_transactd_tabledef_class_register(int module_number TSRMLS_DC);
PHP_TRANSACTD_API int php_transactd_tabledef_class_init(php_transactd_tabledef_t *intern, int is_new TSRMLS_DC);

#define TRANSACTD_TABLEDEF_OBJ(_self, _obj) \
    _self = (php_transactd_tabledef_t *)zend_object_store_get_object(_obj TSRMLS_CC)

#ifdef __cplusplus
}
#endif

#endif  /* PHP_TRANSACTD_TABLEDEF_H */
