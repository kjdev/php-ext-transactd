#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_CONFIG_H
#    include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "zend_extensions.h"

#ifdef __cplusplus
}
#endif

#include "php_transactd.h"
#include "database.h"
#include "table.h"
#include "exception.h"

using namespace bzs::db::protocol::tdap::client;
using namespace bzs::db::protocol::tdap;

ZEND_EXTERN_MODULE_GLOBALS(transactd)

zend_class_entry *php_transactd_table_ce;
static zend_object_handlers php_transactd_table_handlers;

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_table___construct, 0, 0, 2)
    ZEND_ARG_INFO(0, database)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, mode)
    ZEND_ARG_INFO(0, autoCreate)
    ZEND_ARG_INFO(0, owner)
    ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_table_clearbuffer, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_table_setfv, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, data)
    ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_table_getfv, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_table_setfilter, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, reject)
    ZEND_ARG_INFO(0, cache)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_table_setkeynum, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_table_seekgreater, 0, 0, 1)
    ZEND_ARG_INFO(0, equal)
    ZEND_ARG_INFO(0, lock)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_table_seek, 0, 0, 0)
    ZEND_ARG_INFO(0, lock)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_table_stat, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_table_insert, 0, 0, 0)
    ZEND_ARG_INFO(0, ncc)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_table_update, 0, 0, 1)
    ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_table_del, 0, 0, 1)
    ZEND_ARG_INFO(0, inkey)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_table_findnext, 0, 0, 0)
    ZEND_ARG_INFO(0, current)
ZEND_END_ARG_INFO()

TRANSACTD_ZEND_METHOD(Table, __construct)
{
    int rc;
    long mode = 0;
    char *owner = NULL, *uri = NULL;
    zend_bool create = 1;
    zval *link, *name;
    zend_error_handling error_handling;
    php_transactd_table_t *intern;

    zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
    rc = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Oz|lbss",
                               &link, php_transactd_db_ce,
                               &name, &mode, &create, &owner, &uri);
    zend_restore_error_handling(&error_handling TSRMLS_CC);
    if (rc == FAILURE) {
        TRANSACTD_EXCEPTION(0, "Transactd\\Table object has not been "
                            "correctly initialized by its constructor");
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    intern->link = link;
    zval_add_ref(&intern->link);

    if (php_transactd_table_class_init(intern, return_value, name, mode, create,
                                       owner, uri TSRMLS_CC) == FAILURE) {
        TRANSACTD_EXCEPTION(0, "Transactd\\Table object has not been "
                            "correctly initialized by its constructor");
        RETURN_FALSE;
    }
}

TRANSACTD_ZEND_METHOD(Table, clearBuffer)
{
    php_transactd_table_t *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    intern->table->clearBuffer();

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Table, setFV)
{
    zval *key, *data;
    long size = -1;
    php_transactd_table_t *intern;
    short num_key = 0;
    char *str_key = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|l",
                              &key, &data, &size) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    if (Z_TYPE_P(key) == IS_LONG) {
        num_key = (short)Z_LVAL_P(key);
    } else {
        convert_to_string(key);
        str_key = Z_STRVAL_P(key);
    }

    switch (Z_TYPE_P(data)) {
        case IS_BOOL:
            if (str_key) {
                intern->table->setFV(str_key, Z_BVAL_P(data));
            } else {
                intern->table->setFV(num_key, Z_BVAL_P(data));
            }
            break;
        case IS_LONG:
            if (str_key) {
                intern->table->setFV(str_key, (__int64)Z_LVAL_P(data));
            } else {
                intern->table->setFV(num_key, (__int64)Z_LVAL_P(data));
            }
            break;
        case IS_DOUBLE:
            if (str_key) {
                intern->table->setFV(str_key, Z_DVAL_P(data));
            } else {
                intern->table->setFV(num_key, Z_DVAL_P(data));
            }
            break;
        case IS_STRING:
            if (str_key) {
                if (size > 0) {
                    intern->table->setFV(str_key, (void *)Z_STRVAL_P(data),
                                         (uint_td)size);
                } else {
                    intern->table->setFV(str_key, Z_STRVAL_P(data));
                }
            } else {
                if (size > 0) {
                    intern->table->setFV(num_key, (void *)Z_STRVAL_P(data),
                                         (uint_td)size);
                } else {
                    intern->table->setFV(num_key, Z_STRVAL_P(data));
                }
            }
            break;
        case IS_NULL:
        default:
            TRANSACTD_ERR(E_WARNING, "Type is unsupported");
            RETURN_FALSE;
    }

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Table, getFVint)
{
    zval *key;
    php_transactd_table_t *intern;
    short num_key = 0;
    char *str_key = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",
                              &key) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    if (Z_TYPE_P(key) == IS_LONG) {
        num_key = (short)Z_LVAL_P(key);
    } else {
        convert_to_string(key);
        str_key = Z_STRVAL_P(key);
    }

    if (str_key) {
        RETURN_LONG(intern->table->getFVint(str_key));
    } else {
        RETURN_LONG(intern->table->getFVint(num_key));
    }
}

TRANSACTD_ZEND_METHOD(Table, getFVstr)
{
    zval *key;
    php_transactd_table_t *intern;
    short num_key = 0;
    char *str_key = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",
                              &key) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    if (Z_TYPE_P(key) == IS_LONG) {
        num_key = (short)Z_LVAL_P(key);
    } else {
        convert_to_string(key);
        str_key = Z_STRVAL_P(key);
    }

    if (str_key) {
        RETURN_STRING(intern->table->getFVstr(str_key), 1);
    } else {
        RETURN_STRING(intern->table->getFVstr(num_key), 1);
    }
}

TRANSACTD_ZEND_METHOD(Table, setFilter)
{
    char *str;
    int str_len;
    long reject, cache;
    php_transactd_table_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sll",
                              &str, &str_len, &reject, &cache) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    intern->table->setFilter(str, reject, cache);

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Table, setKeyNum)
{
    long key;
    php_transactd_table_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",
                              &key) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    intern->table->setKeyNum((char_td)key);

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Table, seekGreater)
{
    zend_bool equal;
    long lock = 0;
    php_transactd_table_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b|l",
                              &equal, &lock) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    intern->table->seekGreater(equal, lock);

    if (intern->table->stat() != 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Table, seek)
{
    long lock = 0;
    php_transactd_table_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l",
                              &lock) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    intern->table->seek(lock);

    if (intern->table->stat() != 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Table, seekNext)
{
    long lock = 0;
    php_transactd_table_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l",
                              &lock) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    intern->table->seekNext(lock);

    if (intern->table->stat() != 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Table, stat)
{
    php_transactd_table_t *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    RETURN_LONG(intern->table->stat());
}

TRANSACTD_ZEND_METHOD(Table, insert)
{
    zend_bool ncc = 0;
    php_transactd_table_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b",
                              &ncc) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    intern->table->insert(ncc);
    if (intern->table->stat() != 0) {
        php_transactd_error(E_WARNING, intern->table->stat(),
                            (char *)intern->table->tableDef()->tableName());
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Table, update)
{
    long type;
    php_transactd_table_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",
                              &type) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    switch (type) {
        case nstable::changeCurrentNcc:
            intern->table->update(nstable::changeCurrentNcc);
            break;
        case nstable::changeInKey:
            intern->table->update(nstable::changeInKey);
            break;
        case nstable::changeCurrentCc:
        default:
            intern->table->update(nstable::changeCurrentCc);
            break;
    }

    if (intern->table->stat() != 0) {
        php_transactd_error(E_WARNING, intern->table->stat(),
                            (char *)intern->table->tableDef()->tableName());
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Table, del)
{
    zend_bool inkey = 0;
    php_transactd_table_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b",
                              &inkey) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    intern->table->del(inkey);
    if (intern->table->stat() != 0) {
        php_transactd_error(E_WARNING, intern->table->stat(),
                            (char *)intern->table->tableDef()->tableName());
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Table, findNext)
{
    zend_bool notIncCurrent = 1;
    php_transactd_table_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b",
                              &notIncCurrent) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLE_OBJ(intern, getThis());

    intern->table->findNext(notIncCurrent);

    RETURN_TRUE;
}

static zend_function_entry php_transactd_table_methods[] = {
    TRANSACTD_ZEND_ME(Table, __construct, arginfo_transactd_table___construct,
                      ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    TRANSACTD_ZEND_ME(Table, clearBuffer, arginfo_transactd_table_clearbuffer,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Table, setFV, arginfo_transactd_table_setfv,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Table, getFVint, arginfo_transactd_table_getfv,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Table, getFVstr, arginfo_transactd_table_getfv,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Table, setFilter, arginfo_transactd_table_setfilter,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Table, setKeyNum, arginfo_transactd_table_setkeynum,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Table, seekGreater, arginfo_transactd_table_seekgreater,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Table, seek, arginfo_transactd_table_seek,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Table, seekNext, arginfo_transactd_table_seek,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Table, stat, arginfo_transactd_table_stat,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Table, insert, arginfo_transactd_table_insert,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Table, update, arginfo_transactd_table_update,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Table, del, arginfo_transactd_table_del,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_MALIAS(Table, delete, del, arginfo_transactd_table_del,
                          ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Table, findNext, arginfo_transactd_table_findnext,
                      ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

static void
php_transactd_table_free_storage(void *object TSRMLS_DC)
{
    php_transactd_table_t *intern = (php_transactd_table_t *)object;

    if (!intern) {
        return;
    }

    if (intern->table) {
        intern->table->release();
    }

    if (intern->link) {
        zval_ptr_dtor(&intern->link);
    }

    zend_object_std_dtor(&intern->std TSRMLS_CC);
    efree(object);
}

static zend_object_value
php_transactd_table_new_ex(zend_class_entry *ce,
                              php_transactd_table_t **ptr TSRMLS_DC)
{
    php_transactd_table_t *intern;
    zend_object_value retval;

    intern = (php_transactd_table_t *)emalloc(
        sizeof(php_transactd_table_t));
    memset(intern, 0, sizeof(php_transactd_table_t));
    if (ptr) {
        *ptr = intern;
    }

    zend_object_std_init(&intern->std, ce TSRMLS_CC);
    object_properties_init(&intern->std, ce);

    retval.handle = zend_objects_store_put(
        intern, (zend_objects_store_dtor_t)zend_objects_destroy_object,
        (zend_objects_free_object_storage_t)php_transactd_table_free_storage,
        NULL TSRMLS_CC);
    retval.handlers = &php_transactd_table_handlers;

    intern->link = NULL;
    intern->table = NULL;

    return retval;
}

static zend_object_value
php_transactd_table_new(zend_class_entry *ce TSRMLS_DC)
{
    return php_transactd_table_new_ex(ce, NULL TSRMLS_CC);
}

PHP_TRANSACTD_API int
php_transactd_table_class_register(int module_number TSRMLS_DC)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, ZEND_NS_NAME(TRANSACTD_NS, "Table"),
                     php_transactd_table_methods);

    ce.create_object = php_transactd_table_new;

    php_transactd_table_ce = zend_register_internal_class(&ce TSRMLS_CC);
    if (php_transactd_table_ce == NULL) {
        return FAILURE;
    }

    memcpy(&php_transactd_table_handlers, zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));

    php_transactd_table_handlers.clone_obj = NULL;

    return SUCCESS;
}

PHP_TRANSACTD_API int
php_transactd_table_class_init(php_transactd_table_t *intern,
                               zval *return_value, zval *name, short mode,
                               zend_bool create, char *owner,
                               char *uri TSRMLS_DC)
{
    php_transactd_database_t *database;

    TRANSACTD_DATABASE_OBJ(database, intern->link);

    if (Z_TYPE_P(name) == IS_LONG) {
        intern->table = database->db->openTable(Z_LVAL_P(name), mode,
                                                create, owner, uri);
    } else {
        convert_to_string(name);
        if (is_numeric_string(Z_STRVAL_P(name), Z_STRLEN_P(name),
                              NULL, NULL, 0) == IS_LONG) {
            convert_to_long(name);
            intern->table = database->db->openTable(Z_LVAL_P(name), mode,
                                                    create, owner, uri);
        } else {
            intern->table = database->db->openTable(Z_STRVAL_P(name), mode,
                                                    create, owner, uri);
        }
    }

    if (!intern->table || database->db->stat() != 0) {
        php_transactd_error(E_WARNING, database->db->stat(), NULL);
        return FAILURE;
    }

    return SUCCESS;
}
