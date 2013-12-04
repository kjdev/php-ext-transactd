#ifndef PHP_TRANSACTD_DATABASE_H
#define PHP_TRANSACTD_DATABASE_H

#include <bzs/db/protocol/tdap/client/database.h>
typedef struct {
    zend_object std;
    bzs::db::protocol::tdap::client::database *db;
    zval *uri;
} php_transactd_database_t;

#ifdef __cplusplus
extern "C" {
#endif

#include "php_transactd.h"

extern PHP_TRANSACTD_API zend_class_entry *php_transactd_db_ce;

PHP_TRANSACTD_API int php_transactd_db_class_register(int module_number TSRMLS_DC);
PHP_TRANSACTD_API void php_transactd_error(int flag, short code, char *table);

#define TRANSACTD_DATABASE_OBJ(_self, _obj) \
    _self = (php_transactd_database_t *)zend_object_store_get_object(_obj TSRMLS_CC)

#ifdef __cplusplus
}
#endif

#endif  /* PHP_TRANSACTD_DATABASE_H */
