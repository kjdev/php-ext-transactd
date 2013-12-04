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
#include "exception.h"

using namespace bzs::db::protocol::tdap::client;
using namespace bzs::db::protocol::tdap;

ZEND_EXTERN_MODULE_GLOBALS(transactd)

zend_class_entry *php_transactd_tabledef_ce;
static zend_object_handlers php_transactd_tabledef_handlers;

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_tabledef___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_tabledef___get, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_tabledef___set, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_tabledef_setfilename, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_tabledef_settablename, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_tabledef_insertfield, 0, 0, 1)
    ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_tabledef_insertkey, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_tabledef_fielddefs, 0, 0, 1)
    ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_tabledef_update, 0, 0, 0)
    ZEND_ARG_INFO(0, turbo)
    ZEND_ARG_INFO(0, owner)
ZEND_END_ARG_INFO()

static dbdef *
php_transactd_get_dbdef(php_transactd_tabledef_t *intern TSRMLS_DC)
{
    php_transactd_dbdef_t *dbdef;

    if (!intern || !intern->link) {
        return NULL;
    }

    TRANSACTD_DBDEF_OBJ(dbdef, intern->link);

    if (dbdef->def) {
        return dbdef->def;
    }

    return NULL;
}

static database *
php_transactd_get_database(php_transactd_tabledef_t *intern TSRMLS_DC)
{
    php_transactd_dbdef_t *dbdef;
    php_transactd_database_t *database;

    if (!intern || !intern->link) {
        return NULL;
    }

    TRANSACTD_DBDEF_OBJ(dbdef, intern->link);

    if (!dbdef->link) {
        return NULL;
    }

    TRANSACTD_DATABASE_OBJ(database, dbdef->link);

    if (database->db) {
        return database->db;
    }

    return NULL;
}

TRANSACTD_ZEND_METHOD(TableDef, __construct)
{
    php_transactd_tabledef_t *intern;
    int rc;
    zval *link;
    zend_error_handling error_handling;

    zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
    rc = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O",
                               &link, php_transactd_dbdef_ce);
    zend_restore_error_handling(&error_handling TSRMLS_CC);
    if (rc == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLEDEF_OBJ(intern, getThis());

    intern->link = link;
    zval_add_ref(&intern->link);

    if (php_transactd_tabledef_class_init(intern, 1 TSRMLS_CC) == FAILURE) {
        TRANSACTD_EXCEPTION(0, "Transactd\\TableDef object has not been "
                            "correctly initialized by its constructor");
        RETURN_FALSE;
    }
}

TRANSACTD_ZEND_METHOD(TableDef, __get)
{
    char *name;
    int name_len;
    php_transactd_tabledef_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &name, &name_len) == FAILURE) {
        RETURN_FALSE;
    }

    if (!name || name_len <= 0) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLEDEF_OBJ(intern, getThis());

    //TODO
    if (intern->def_ptr) {
        if (strcmp(name, "id") == 0) {
            RETURN_LONG((*intern->def_ptr)->id);
        } else if (strcmp(name, "fieldCount") == 0) {
            RETURN_LONG((*intern->def_ptr)->fieldCount);
        } else if (strcmp(name, "charsetIndex") == 0) {
            RETURN_LONG((*intern->def_ptr)->charsetIndex);
        } else if (strcmp(name, "primaryKeyNum") == 0) {
            RETURN_LONG((*intern->def_ptr)->primaryKeyNum);
        }
    } else {
        if (strcmp(name, "id") == 0) {
            RETURN_LONG(intern->def->id);
        } else if (strcmp(name, "fieldCount") == 0) {
            RETURN_LONG(intern->def->fieldCount);
        } else if (strcmp(name, "charsetIndex") == 0) {
            RETURN_LONG(intern->def->charsetIndex);
        } else if (strcmp(name, "primaryKeyNum") == 0) {
            RETURN_LONG(intern->def->primaryKeyNum);
        }
    }


    TRANSACTD_ERR(E_WARNING, "Unsupported: %s", name);
    RETURN_FALSE;
}

TRANSACTD_ZEND_METHOD(TableDef, __set)
{
    char *name;
    int name_len;
    zval *value;
    php_transactd_tabledef_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz",
                              &name, &name_len, &value) == FAILURE) {
        RETURN_FALSE;
    }

    if (!name || !value || name_len <= 0) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLEDEF_OBJ(intern, getThis());

    convert_to_long(value);

    //TODO
    if (intern->def_ptr) {
        if (strcmp(name, "id") == 0) {
            (*intern->def_ptr)->id = (ushort_td)Z_LVAL_P(value);
        } else if (strcmp(name, "fieldCount") == 0) {
            (*intern->def_ptr)->fieldCount = (ushort_td)Z_LVAL_P(value);
        } else if (strcmp(name, "charsetIndex") == 0) {
            (*intern->def_ptr)->charsetIndex = (ushort_td)Z_LVAL_P(value);
        } else if (strcmp(name, "primaryKeyNum") == 0) {
            (*intern->def_ptr)->primaryKeyNum = (ushort_td)Z_LVAL_P(value);
        } else {
            TRANSACTD_ERR(E_WARNING, "Unsupported: %s", name);
            RETURN_FALSE;
        }
    } else {
        if (strcmp(name, "id") == 0) {
            intern->def->id = (ushort_td)Z_LVAL_P(value);
        } else if (strcmp(name, "fieldCount") == 0) {
            intern->def->fieldCount = (ushort_td)Z_LVAL_P(value);
        } else if (strcmp(name, "charsetIndex") == 0) {
            intern->def->charsetIndex = (ushort_td)Z_LVAL_P(value);
        } else if (strcmp(name, "primaryKeyNum") == 0) {
            intern->def->primaryKeyNum = (ushort_td)Z_LVAL_P(value);
        } else {
            TRANSACTD_ERR(E_WARNING, "Unsupported: %s", name);
            RETURN_FALSE;
        }
    }

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(TableDef, setFileName)
{
    char *name;
    int name_len;
    php_transactd_tabledef_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &name, &name_len) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLEDEF_OBJ(intern, getThis());

    if (!name || name_len <= 0) {
        RETURN_FALSE;
    }

    intern->def->setFileName(name);

    if (intern->def_ptr) {
        (*intern->def_ptr)->setFileName(name);
    } else {
        intern->def->setFileName(name);
    }

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(TableDef, setTableName)
{
    char *name;
    int name_len;
    php_transactd_tabledef_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &name, &name_len) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_TABLEDEF_OBJ(intern, getThis());

    if (!name || name_len <= 0) {
        RETURN_FALSE;
    }

    if (intern->def_ptr) {
        (*intern->def_ptr)->setTableName(name);
    } else {
        intern->def->setTableName(name);
    }

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(TableDef, insertField)
{
    php_transactd_tabledef_t *intern;
    php_transactd_fielddef_t *field;
    int tableIndex;
    long fieldIndex;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",
                              &fieldIndex) == FAILURE) {
        return;
    }

    TRANSACTD_TABLEDEF_OBJ(intern, getThis());

    if (intern->def_ptr) {
        tableIndex = (*intern->def_ptr)->id;
    } else {
        tableIndex = intern->def->id;
    }

    object_init_ex(return_value, php_transactd_fielddef_ce);

    TRANSACTD_FIELDDEF_OBJ(field, return_value);

    field->link = getThis();
    zval_add_ref(&field->link);

    if (php_transactd_fielddef_class_init(field, tableIndex,
                                          fieldIndex TSRMLS_CC) == FAILURE) {
        TRANSACTD_EXCEPTION(0, "Transactd\\FieldDef object has not been "
                            "correctly initialized by its constructor");
        RETURN_FALSE;
    }
}

TRANSACTD_ZEND_METHOD(TableDef, insertKey)
{
    php_transactd_tabledef_t *intern;
    php_transactd_keydef_t *key;
    long tableIndex, keyIndex;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",
                              &keyIndex) == FAILURE) {
        return;
    }

    TRANSACTD_TABLEDEF_OBJ(intern, getThis());

    if (intern->def_ptr) {
        tableIndex = (*intern->def_ptr)->id;
    } else {
        tableIndex = intern->def->id;
    }

    object_init_ex(return_value, php_transactd_keydef_ce);

    TRANSACTD_KEYDEF_OBJ(key, return_value);

    key->link = getThis();
    zval_add_ref(&key->link);

    if (php_transactd_keydef_class_init(key, return_value, tableIndex,
                                        keyIndex TSRMLS_CC) == FAILURE) {
        TRANSACTD_EXCEPTION(0, "Transactd\\KeyDef object has not been "
                            "correctly initialized by its constructor");
        RETURN_FALSE;
    }
}

TRANSACTD_ZEND_METHOD(TableDef, fieldDefs)
{
    php_transactd_tabledef_t *intern;
    php_transactd_fielddef_t *field;
    long fieldIndex;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",
                              &fieldIndex) == FAILURE) {
        return;
    }

    TRANSACTD_TABLEDEF_OBJ(intern, getThis());

    object_init_ex(return_value, php_transactd_fielddef_ce);

    TRANSACTD_FIELDDEF_OBJ(field, return_value);

    field->link = getThis();
    zval_add_ref(&field->link);

    if (intern->def_ptr) {
        field->def = &(*intern->def_ptr)->fieldDefs[fieldIndex];
    }

    if (!intern->def_ptr || !field->def) {
        TRANSACTD_EXCEPTION(0, "Transactd\\FieldDef object has not been "
                            "correctly initialized by its constructor");
        RETURN_FALSE;
    }
}

TRANSACTD_ZEND_METHOD(TableDef, update)
{
    php_transactd_tabledef_t *intern;
    int tableIndex;
    zend_bool turbo = 0;
    zval *owner = NULL;
    char *ownerName = NULL;
    dbdef *def;
    database *db;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|bz",
                              &turbo, &owner) == FAILURE) {
        RETURN_FALSE;
    }

    if (owner && Z_TYPE_P(owner) != IS_NULL) {
        convert_to_string(owner);
        ownerName = Z_STRVAL_P(owner);
    }

    TRANSACTD_TABLEDEF_OBJ(intern, getThis());

    if (intern->def_ptr) {
        tableIndex = (*intern->def_ptr)->id;
    } else {
        tableIndex = intern->def->id;
    }

    def = php_transactd_get_dbdef(intern);
    if (!def) {
        TRANSACTD_ERR(E_WARNING, "Invalid Transactd DbDef object");
        RETURN_FALSE;
    }

    db = php_transactd_get_database(intern);
    if (!db) {
        TRANSACTD_ERR(E_WARNING, "Invalid Transactd Database object");
        RETURN_FALSE;
    }

    def->updateTableDef(tableIndex);
    if (def->stat() != 0) {
        php_transactd_error(E_WARNING, def->stat(), NULL);
        RETURN_FALSE;
    }

    if (db->existsTableFile(tableIndex, ownerName)) {
        db->convertTable(tableIndex, turbo, ownerName);
        if (def->stat() != 0) {
            php_transactd_error(E_WARNING, db->stat(), NULL);
            RETURN_FALSE;
        }
    }

    if (intern->is_new) {
        delete intern->def;
    }

    if (intern->def_ptr || intern->is_new) {
        intern->def_ptr = def->tableDefPtr(tableIndex);
        intern->def = (*intern->def_ptr);
    } else {
        intern->def = def->tableDefs(tableIndex);
    }
    intern->is_new = 0;

    RETURN_TRUE;
}


static zend_function_entry php_transactd_tabledef_methods[] = {
    TRANSACTD_ZEND_ME(TableDef, __construct,
                      arginfo_transactd_tabledef___construct,
                      ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    TRANSACTD_ZEND_ME(TableDef, __get,
                      arginfo_transactd_tabledef___get, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(TableDef, __set,
                      arginfo_transactd_tabledef___set, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(TableDef, setFileName,
                      arginfo_transactd_tabledef_setfilename, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(TableDef, setTableName,
                      arginfo_transactd_tabledef_settablename, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(TableDef, insertField,
                      arginfo_transactd_tabledef_insertfield, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(TableDef, insertKey,
                      arginfo_transactd_tabledef_insertkey, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(TableDef, fieldDefs,
                      arginfo_transactd_tabledef_fielddefs, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(TableDef, update,
                      arginfo_transactd_tabledef_update, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_MALIAS(TableDef, write, update,
                          arginfo_transactd_tabledef_update, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

static void
php_transactd_tabledef_free_storage(void *object TSRMLS_DC)
{
    php_transactd_tabledef_t *intern = (php_transactd_tabledef_t *)object;

    if (!intern) {
        return;
    }

    if (intern->link) {
        zval_ptr_dtor(&intern->link);
    }

    if (intern->is_new) {
        delete intern->def;
    }

    zend_object_std_dtor(&intern->std TSRMLS_CC);
    efree(object);
}

static zend_object_value
php_transactd_tabledef_new_ex(zend_class_entry *ce,
                              php_transactd_tabledef_t **ptr TSRMLS_DC)
{
    php_transactd_tabledef_t *intern;
    zend_object_value retval;

    intern = (php_transactd_tabledef_t *)emalloc(
        sizeof(php_transactd_tabledef_t));
    memset(intern, 0, sizeof(php_transactd_tabledef_t));
    if (ptr) {
        *ptr = intern;
    }

    zend_object_std_init(&intern->std, ce TSRMLS_CC);
    object_properties_init(&intern->std, ce);

    retval.handle = zend_objects_store_put(
        intern, (zend_objects_store_dtor_t)zend_objects_destroy_object,
        (zend_objects_free_object_storage_t)php_transactd_tabledef_free_storage,
        NULL TSRMLS_CC);
    retval.handlers = &php_transactd_tabledef_handlers;

    intern->link = NULL;
    intern->def = NULL;
    intern->def_ptr = NULL;
    intern->is_new = 0;

    return retval;
}

static zend_object_value
php_transactd_tabledef_new(zend_class_entry *ce TSRMLS_DC)
{
    return php_transactd_tabledef_new_ex(ce, NULL TSRMLS_CC);
}

PHP_TRANSACTD_API int
php_transactd_tabledef_class_register(int module_number TSRMLS_DC)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, ZEND_NS_NAME(TRANSACTD_NS, "TableDef"),
                     php_transactd_tabledef_methods);

    ce.create_object = php_transactd_tabledef_new;

    php_transactd_tabledef_ce = zend_register_internal_class(&ce TSRMLS_CC);
    if (php_transactd_tabledef_ce == NULL) {
        return FAILURE;
    }

    memcpy(&php_transactd_tabledef_handlers, zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));

    php_transactd_tabledef_handlers.clone_obj = NULL;

    return SUCCESS;
}

PHP_TRANSACTD_API int
php_transactd_tabledef_class_init(php_transactd_tabledef_t *intern,
                                  int is_new TSRMLS_DC)
{
    if (is_new) {
        intern->def = new tabledef();
        if (!intern->def) {
            return FAILURE;
        }
        intern->is_new = 0;
    }
    return SUCCESS;
}
