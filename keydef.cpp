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
#include "keydef.h"
#include "keysegment.h"
#include "exception.h"

using namespace bzs::db::protocol::tdap;

ZEND_EXTERN_MODULE_GLOBALS(transactd)

zend_class_entry *php_transactd_keydef_ce;
static zend_object_handlers php_transactd_keydef_handlers;

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_keydef___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_keydef___get, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_keydef___set, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

TRANSACTD_ZEND_METHOD(KeyDef, __construct)
{
    int rc;
    php_transactd_keydef_t *intern;
    long tableIndex, keyIndex;
    zval *link;
    zend_error_handling error_handling;

    zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
    rc = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Oll",
                               &link, php_transactd_tabledef_ce,
                               &tableIndex, &keyIndex);
    zend_restore_error_handling(&error_handling TSRMLS_CC);
    if (rc == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_KEYDEF_OBJ(intern, getThis());

    intern->link = link;
    zval_add_ref(&intern->link);

    if (php_transactd_keydef_class_init(intern, return_value, tableIndex,
                                        keyIndex TSRMLS_CC) == FAILURE) {
        TRANSACTD_EXCEPTION(0, "Transactd\\KeyDef object has not been "
                            "correctly initialized by its constructor");
        RETURN_FALSE;
    }
}

TRANSACTD_ZEND_METHOD(KeyDef, __get)
{
    char *name;
    int name_len;
    php_transactd_keydef_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &name, &name_len) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_KEYDEF_OBJ(intern, getThis());

    if (strcmp(name, "keyNumber") == 0) {
        RETURN_LONG(intern->def->keyNumber);
    } else if (strcmp(name, "segmentCount") == 0) {
        RETURN_LONG(intern->def->segmentCount);
    }

    RETURN_FALSE;
}

TRANSACTD_ZEND_METHOD(KeyDef, __set)
{
    char *name;
    int name_len;
    zval *value;
    php_transactd_keydef_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz",
                              &name, &name_len, &value) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_KEYDEF_OBJ(intern, getThis());

    if (strcmp(name, "segmentCount") == 0) {
        convert_to_long(value);
        intern->def->segmentCount = (uchar_td)Z_LVAL_P(value);
    } else {
        TRANSACTD_ERR(E_WARNING, "Unsupported: %s", name);
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

static zend_function_entry php_transactd_keydef_methods[] = {
    TRANSACTD_ZEND_ME(KeyDef, __construct,
                      arginfo_transactd_keydef___construct,
                      ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    TRANSACTD_ZEND_ME(KeyDef, __get,
                      arginfo_transactd_keydef___get, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(KeyDef, __set,
                      arginfo_transactd_keydef___set, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

static void
php_transactd_keydef_free_storage(void *object TSRMLS_DC)
{
    php_transactd_keydef_t *intern = (php_transactd_keydef_t *)object;

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
php_transactd_keydef_new_ex(zend_class_entry *ce,
                              php_transactd_keydef_t **ptr TSRMLS_DC)
{
    php_transactd_keydef_t *intern;
    zend_object_value retval;

    intern = (php_transactd_keydef_t *)emalloc(
        sizeof(php_transactd_keydef_t));
    memset(intern, 0, sizeof(php_transactd_keydef_t));
    if (ptr) {
        *ptr = intern;
    }

    zend_object_std_init(&intern->std, ce TSRMLS_CC);
    object_properties_init(&intern->std, ce);

    retval.handle = zend_objects_store_put(
        intern, (zend_objects_store_dtor_t)zend_objects_destroy_object,
        (zend_objects_free_object_storage_t)php_transactd_keydef_free_storage,
        NULL TSRMLS_CC);
    retval.handlers = &php_transactd_keydef_handlers;

    intern->def = NULL;
    intern->link = NULL;

    return retval;
}

static zend_object_value
php_transactd_keydef_new(zend_class_entry *ce TSRMLS_DC)
{
    return php_transactd_keydef_new_ex(ce, NULL TSRMLS_CC);
}

PHP_TRANSACTD_API int
php_transactd_keydef_class_register(int module_number TSRMLS_DC)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, ZEND_NS_NAME(TRANSACTD_NS, "KeyDef"),
                     php_transactd_keydef_methods);

    ce.create_object = php_transactd_keydef_new;

    php_transactd_keydef_ce = zend_register_internal_class(&ce TSRMLS_CC);
    if (php_transactd_keydef_ce == NULL) {
        return FAILURE;
    }

    memcpy(&php_transactd_keydef_handlers, zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));

    php_transactd_keydef_handlers.clone_obj = NULL;

    /* property */
    zend_declare_property_null(php_transactd_keydef_ce,
                               "segments", sizeof("segments") - 1,
                               ZEND_ACC_PUBLIC TSRMLS_CC);

    return SUCCESS;
}

PHP_TRANSACTD_API int
php_transactd_keydef_class_init(php_transactd_keydef_t *intern,
                                zval *return_value, int tableIndex,
                                int keyIndex TSRMLS_DC)
{
    php_transactd_tabledef_t *table;
    php_transactd_dbdef_t *db;
    zval *segments;
    int i;

    TRANSACTD_TABLEDEF_OBJ(table, intern->link);
    if (!table->link) {
        return FAILURE;
    }

    TRANSACTD_DBDEF_OBJ(db, table->link);
    if (!db->def) {
        return FAILURE;
    }

    intern->def = db->def->insertKey(tableIndex, keyIndex);
    if (!intern->def) {
        return FAILURE;
    }

    MAKE_STD_ZVAL(segments);
    array_init(segments);

    for (i = 0; i < 8; i++) {
        zval *obj;
        php_transactd_keysegment_t *keysegment;

        MAKE_STD_ZVAL(obj);
        object_init_ex(obj, php_transactd_keysegment_ce);

        TRANSACTD_KEYSEGMENT_OBJ(keysegment, obj);

        keysegment->n = i;

        keysegment->link = return_value;
        zval_add_ref(&keysegment->link);

        if (php_transactd_keysegment_class_init(
                keysegment, return_value, obj TSRMLS_CC) == FAILURE) {
            return FAILURE;
        }

        add_index_zval(segments, i, obj);
    }

    zend_update_property(php_transactd_keydef_ce, return_value,
                         "segments", sizeof("segments")-1, segments TSRMLS_CC);

    zval_ptr_dtor(&segments);

    return SUCCESS;
}
