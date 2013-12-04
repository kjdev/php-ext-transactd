#ifndef PHP_TRANSACTD_DBDEF_H
#define PHP_TRANSACTD_DBDEF_H

#include <bzs/db/protocol/tdap/client/database.h>
#include <bzs/db/protocol/tdap/client/dbDef.h>
typedef struct {
    zend_object std;
    zval *link;
    bzs::db::protocol::tdap::client::dbdef *def;
} php_transactd_dbdef_t;

#ifdef __cplusplus
extern "C" {
#endif

#include "php_transactd.h"

extern PHP_TRANSACTD_API zend_class_entry *php_transactd_dbdef_ce;

PHP_TRANSACTD_API int php_transactd_dbdef_class_register(int module_number TSRMLS_DC);

#define TRANSACTD_DBDEF_OBJ(_self, _obj) \
    _self = (php_transactd_dbdef_t *)zend_object_store_get_object(_obj TSRMLS_CC)

#ifdef __cplusplus
}
#endif

#endif  /* PHP_TRANSACTD_DBDEF_H */
