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
#include "dbdef.h"
#include "tabledef.h"
#include "fielddef.h"
#include "keydef.h"
#include "keysegment.h"
#include "exception.h"

using namespace bzs::db::protocol::tdap::client;
using namespace bzs::db::protocol::tdap;

ZEND_EXTERN_MODULE_GLOBALS(transactd)

zend_class_entry *php_transactd_dbdef_ce;
static zend_object_handlers php_transactd_dbdef_handlers;

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_dbdef___construct, 0, 0, 1)
    ZEND_ARG_INFO(0, db)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_dbdef_createtabledef, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_dbdef_inserttable, 0, 0, 1)
    ZEND_ARG_INFO(0, db)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_dbdef_tabledefs, 0, 0, 1)
    ZEND_ARG_INFO(0, tableIndex)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_dbdef_pushbackup, 0, 0, 1)
    ZEND_ARG_INFO(0, tableIndex)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_dbdef_popbackup, 0, 0, 1)
    ZEND_ARG_INFO(0, tableIndex)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_dbdef_stat, 0, 0, 0)
    ZEND_ARG_INFO(0, stat)
ZEND_END_ARG_INFO()

static dbdef *
php_transactd_get_dbdef(php_transactd_dbdef_t *intern TSRMLS_DC)
{
    php_transactd_database_t *database;

    if (!intern || !intern->link) {
        return NULL;
    }

    TRANSACTD_DATABASE_OBJ(database, intern->link);

    if (database->db) {
        return database->db->dbDef();
    }

    return NULL;
}


TRANSACTD_ZEND_METHOD(DbDef, __construct)
{
    int rc;
    php_transactd_dbdef_t *intern;
    zval *link;
    zend_error_handling error_handling;

    zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
    rc = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O",
                               &link, php_transactd_database_ce);
    zend_restore_error_handling(&error_handling TSRMLS_CC);
    if (rc == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DBDEF_OBJ(intern, getThis());

    intern->link = link;
    zval_add_ref(&intern->link);

    intern->def = php_transactd_get_dbdef(intern TSRMLS_CC);
    if (!intern->def) {
        TRANSACTD_EXCEPTION(0, "Transactd\\DbDef object has not been "
                            "correctly initialized by its constructor");
        RETURN_FALSE;
    }
}

TRANSACTD_ZEND_METHOD(DbDef, createTableDef)
{
    php_transactd_dbdef_t *intern;
    php_transactd_tabledef_t *table;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    TRANSACTD_DBDEF_OBJ(intern, getThis());

    object_init_ex(return_value, php_transactd_tabledef_ce);

    TRANSACTD_TABLEDEF_OBJ(table, return_value);

    table->link = getThis();
    zval_add_ref(&table->link);

    if (php_transactd_tabledef_class_init(table, 1 TSRMLS_CC) == FAILURE) {
        TRANSACTD_EXCEPTION(0, "Transactd\\TableDef object has not been "
                            "correctly initialized by its constructor");
        RETURN_FALSE;
    }
}

TRANSACTD_ZEND_METHOD(DbDef, insertTable)
{
    php_transactd_dbdef_t *intern;
    php_transactd_tabledef_t *table;
    zval *td;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O",
                              &td, php_transactd_tabledef_ce) == FAILURE) {
        return;
    }

    TRANSACTD_DBDEF_OBJ(intern, getThis());
    TRANSACTD_TABLEDEF_OBJ(table, td);

    intern->def->insertTable(table->def);
    if (intern->def->stat() != 0) {
        php_transactd_error(E_WARNING, intern->def->stat(), NULL);
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(DbDef, tableDefs)
{
    php_transactd_dbdef_t *intern;
    php_transactd_tabledef_t *table;
    long tableIndex;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",
                              &tableIndex) == FAILURE) {
        return;
    }

    TRANSACTD_DBDEF_OBJ(intern, getThis());

    object_init_ex(return_value, php_transactd_tabledef_ce);

    TRANSACTD_TABLEDEF_OBJ(table, return_value);

    table->link = getThis();
    zval_add_ref(&table->link);

    table->def_ptr = intern->def->tableDefPtr(tableIndex);
    if (!table->def_ptr) {
        TRANSACTD_EXCEPTION(0, "Transactd\\TableDef object has not been "
                            "correctly initialized by its constructor");
        RETURN_FALSE;
    }
    table->def = (*table->def_ptr);
}

TRANSACTD_ZEND_METHOD(DbDef, pushBackup)
{
    php_transactd_dbdef_t *intern;
    long index;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",
                              &index) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DBDEF_OBJ(intern, getThis());

    intern->def->pushBackup(index);


    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(DbDef, popBackup)
{
    php_transactd_dbdef_t *intern;
    long index;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",
                              &index) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DBDEF_OBJ(intern, getThis());

    intern->def->popBackup(index);

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(DbDef, stat)
{
    php_transactd_dbdef_t *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DBDEF_OBJ(intern, getThis());

    RETURN_LONG(intern->def->stat());
}


static zend_function_entry php_transactd_dbdef_methods[] = {
    TRANSACTD_ZEND_ME(DbDef, __construct,
                      arginfo_transactd_dbdef___construct,
                      ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    TRANSACTD_ZEND_ME(DbDef, insertTable,
                      arginfo_transactd_dbdef_inserttable, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(DbDef, createTableDef,
                      arginfo_transactd_dbdef_createtabledef, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(DbDef, tableDefs,
                      arginfo_transactd_dbdef_tabledefs, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(DbDef, pushBackup,
                      arginfo_transactd_dbdef_pushbackup, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(DbDef, popBackup,
                      arginfo_transactd_dbdef_popbackup, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(DbDef, stat,
                      arginfo_transactd_dbdef_stat, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

static void
php_transactd_dbdef_free_storage(void *object TSRMLS_DC)
{
    php_transactd_dbdef_t *intern = (php_transactd_dbdef_t *)object;

    if (!intern) {
        return;
    }

    if (intern->link) {
        zval_ptr_dtor(&intern->link);
    }

    zend_object_std_dtor(&intern->std TSRMLS_CC);
    efree(object);
}

static zend_object_value
php_transactd_dbdef_new_ex(zend_class_entry *ce,
                           php_transactd_dbdef_t **ptr TSRMLS_DC)
{
    php_transactd_dbdef_t *intern;
    zend_object_value retval;

    intern = (php_transactd_dbdef_t *)emalloc(sizeof(php_transactd_dbdef_t));
    memset(intern, 0, sizeof(php_transactd_dbdef_t));
    if (ptr) {
        *ptr = intern;
    }

    zend_object_std_init(&intern->std, ce TSRMLS_CC);
    object_properties_init(&intern->std, ce);

    retval.handle = zend_objects_store_put(
        intern, (zend_objects_store_dtor_t)zend_objects_destroy_object,
        (zend_objects_free_object_storage_t)php_transactd_dbdef_free_storage,
        NULL TSRMLS_CC);
    retval.handlers = &php_transactd_dbdef_handlers;

    intern->link = NULL;
    intern->def = NULL;

    return retval;
}

static zend_object_value
php_transactd_dbdef_new(zend_class_entry *ce TSRMLS_DC)
{
    return php_transactd_dbdef_new_ex(ce, NULL TSRMLS_CC);
}

PHP_TRANSACTD_API int
php_transactd_dbdef_class_register(int module_number TSRMLS_DC)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, ZEND_NS_NAME(TRANSACTD_NS, "DbDef"),
                     php_transactd_dbdef_methods);

    ce.create_object = php_transactd_dbdef_new;

    php_transactd_dbdef_ce = zend_register_internal_class(&ce TSRMLS_CC);
    if (php_transactd_dbdef_ce == NULL) {
        return FAILURE;
    }

    memcpy(&php_transactd_dbdef_handlers, zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));

    php_transactd_dbdef_handlers.clone_obj = NULL;

    return SUCCESS;
}
