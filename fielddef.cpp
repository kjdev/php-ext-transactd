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
#include "dbdef.h"
#include "tabledef.h"
#include "fielddef.h"
#include "exception.h"

using namespace bzs::db::protocol::tdap::client;
using namespace bzs::db::protocol::tdap;

ZEND_EXTERN_MODULE_GLOBALS(transactd)

zend_class_entry *php_transactd_fielddef_ce;
static zend_object_handlers php_transactd_fielddef_handlers;

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_fielddef___construct, 0, 0, 3)
    ZEND_ARG_INFO(0, dbdef)
    ZEND_ARG_INFO(0, tableIndex)
    ZEND_ARG_INFO(0, insertIndex)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_fielddef___get, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_fielddef___set, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_fielddef_setname, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_fielddef_setlenbycharnum, 0, 0, 1)
    ZEND_ARG_INFO(0, len)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_fielddef_setcharsetindex, 0, 0, 1)
    ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

static dbdef *
php_transactd_get_dbdef(php_transactd_fielddef_t *intern TSRMLS_DC)
{
    php_transactd_tabledef_t *table;
    php_transactd_dbdef_t *db;

    if (!intern || !intern->link) {
        return NULL;
    }

    TRANSACTD_TABLEDEF_OBJ(table, intern->link);
    if (!table->link) {
        return NULL;
    }

    TRANSACTD_DBDEF_OBJ(db, table->link);
    if (db->def) {
        return db->def;
    }

    return NULL;
}

TRANSACTD_ZEND_METHOD(FieldDef, __construct)
{
    int rc;
    php_transactd_fielddef_t *intern;
    zval *link;
    long tableIndex, insertIndex;
    zend_error_handling error_handling;

    zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
    rc = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Oll",
                               &link, php_transactd_tabledef_ce,
                               &tableIndex, &insertIndex);
    zend_restore_error_handling(&error_handling TSRMLS_CC);
    if (rc == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_FIELDDEF_OBJ(intern, getThis());

    intern->link = link;
    zval_add_ref(&intern->link);

    if (php_transactd_fielddef_class_init(intern, tableIndex,
                                          insertIndex TSRMLS_CC) == FAILURE) {
        TRANSACTD_EXCEPTION(0, "Transactd\\FieldDef object has not been "
                            "correctly initialized by its constructor");
        RETURN_FALSE;
    }
}

TRANSACTD_ZEND_METHOD(FieldDef, __get)
{
    char *name;
    int name_len;
    php_transactd_fielddef_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &name, &name_len) == FAILURE) {
        RETURN_FALSE;
    }

    if (!name || name_len <= 0) {
        RETURN_FALSE;
    }

    TRANSACTD_FIELDDEF_OBJ(intern, getThis());

    //TODO
    if (strcmp(name, "len") == 0) {
        RETURN_LONG(intern->def->len);
    } else if (strcmp(name, "type") == 0) {
        RETURN_LONG(intern->def->type);
    }

    TRANSACTD_ERR(E_WARNING, "Unsupported: %s", name);
    RETURN_FALSE;
}

TRANSACTD_ZEND_METHOD(FieldDef, __set)
{
    char *name;
    int name_len;
    zval *value;
    php_transactd_fielddef_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz",
                              &name, &name_len, &value) == FAILURE) {
        RETURN_FALSE;
    }

    if (!name || !value || name_len <= 0) {
        RETURN_FALSE;
    }

    TRANSACTD_FIELDDEF_OBJ(intern, getThis());

    //TODO
    if (strcmp(name, "type") == 0) {
        convert_to_long(value);
        intern->def->type = (ushort_td)Z_LVAL_P(value);
    } else if (strcmp(name, "len") == 0) {
        convert_to_long(value);
        intern->def->len = (ushort_td)Z_LVAL_P(value);
    } else {
        TRANSACTD_ERR(E_WARNING, "Unsupported: %s", name);
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(FieldDef, setName)
{
    char *name;
    int name_len;
    php_transactd_fielddef_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &name, &name_len) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_FIELDDEF_OBJ(intern, getThis());

    if (!name || name_len <= 0) {
        RETURN_FALSE;
    }

    intern->def->setName(name);

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(FieldDef, setLenByCharnum)
{
    long len;
    php_transactd_fielddef_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",
                              &len) == FAILURE) {
        RETURN_FALSE;
    }

    if (len <= 0) {
        RETURN_FALSE;
    }

    TRANSACTD_FIELDDEF_OBJ(intern, getThis());

    intern->def->setLenByCharnum(len);

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(FieldDef, setCharsetIndex)
{
    long index;
    php_transactd_fielddef_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",
                              &index) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_FIELDDEF_OBJ(intern, getThis());

    intern->def->setCharsetIndex(index);

    RETURN_TRUE;
}


static zend_function_entry php_transactd_fielddef_methods[] = {
    TRANSACTD_ZEND_ME(FieldDef, __construct,
                      arginfo_transactd_fielddef___construct,
                      ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    TRANSACTD_ZEND_ME(FieldDef, __get,
                      arginfo_transactd_fielddef___get, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(FieldDef, __set,
                      arginfo_transactd_fielddef___set, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(FieldDef, setName,
                      arginfo_transactd_fielddef_setname, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(FieldDef, setLenByCharnum,
                      arginfo_transactd_fielddef_setlenbycharnum,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(FieldDef, setCharsetIndex,
                      arginfo_transactd_fielddef_setcharsetindex,
                      ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

static void
php_transactd_fielddef_free_storage(void *object TSRMLS_DC)
{
    php_transactd_fielddef_t *intern = (php_transactd_fielddef_t *)object;

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
php_transactd_fielddef_new_ex(zend_class_entry *ce,
                              php_transactd_fielddef_t **ptr TSRMLS_DC)
{
    php_transactd_fielddef_t *intern;
    zend_object_value retval;

    intern = (php_transactd_fielddef_t *)emalloc(
        sizeof(php_transactd_fielddef_t));
    memset(intern, 0, sizeof(php_transactd_fielddef_t));
    if (ptr) {
        *ptr = intern;
    }

    zend_object_std_init(&intern->std, ce TSRMLS_CC);
    object_properties_init(&intern->std, ce);

    retval.handle = zend_objects_store_put(
        intern, (zend_objects_store_dtor_t)zend_objects_destroy_object,
        (zend_objects_free_object_storage_t)php_transactd_fielddef_free_storage,
        NULL TSRMLS_CC);
    retval.handlers = &php_transactd_fielddef_handlers;

    intern->def = NULL;
    intern->link = NULL;

    return retval;
}

static zend_object_value
php_transactd_fielddef_new(zend_class_entry *ce TSRMLS_DC)
{
    return php_transactd_fielddef_new_ex(ce, NULL TSRMLS_CC);
}

PHP_TRANSACTD_API int
php_transactd_fielddef_class_register(int module_number TSRMLS_DC)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, ZEND_NS_NAME(TRANSACTD_NS, "FieldDef"),
                     php_transactd_fielddef_methods);

    ce.create_object = php_transactd_fielddef_new;

    php_transactd_fielddef_ce = zend_register_internal_class(&ce TSRMLS_CC);
    if (php_transactd_fielddef_ce == NULL) {
        return FAILURE;
    }

    memcpy(&php_transactd_fielddef_handlers, zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));

    php_transactd_fielddef_handlers.clone_obj = NULL;

    return SUCCESS;
}

PHP_TRANSACTD_API int
php_transactd_fielddef_class_init(php_transactd_fielddef_t *intern,
                                  long tableIndex, long insertIndex TSRMLS_DC)
{
    dbdef *def;

    def = php_transactd_get_dbdef(intern TSRMLS_CC);
    if (!def) {
        return FAILURE;
    }

    intern->def = def->insertField(tableIndex, insertIndex);
    if (!intern->def) {
        return FAILURE;
    }

    return SUCCESS;
}
