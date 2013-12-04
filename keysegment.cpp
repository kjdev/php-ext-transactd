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
#include "keydef.h"
#include "keysegment.h"
#include "flags.h"
#include "exception.h"

using namespace bzs::db::protocol::tdap;

ZEND_EXTERN_MODULE_GLOBALS(transactd)

zend_class_entry *php_transactd_keysegment_ce;
static zend_object_handlers php_transactd_keysegment_handlers;

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_keysegment___get, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_keysegment___set, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static keydef *
php_transactd_get_keydef(php_transactd_keysegment_t *intern TSRMLS_DC)
{
    php_transactd_keydef_t *keydef;
    TRANSACTD_KEYDEF_OBJ(keydef, intern->link);
    if (keydef->def) {
        return keydef->def;
    }
    return NULL;
}

TRANSACTD_ZEND_METHOD(KeySegment, __construct)
{
    TRANSACTD_EXCEPTION(0, "Transactd\\KeySegment object has not been "
                        "correctly initialized by its constructor");
    RETURN_FALSE;
}

TRANSACTD_ZEND_METHOD(KeySegment, __get)
{
    char *name;
    int name_len;
    php_transactd_keysegment_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &name, &name_len) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_KEYSEGMENT_OBJ(intern, getThis());

    if (strcmp(name, "fieldNum") == 0) {
        keydef *def = php_transactd_get_keydef(intern TSRMLS_CC);
        if (def && intern->n >= 0 && intern->n < 8) {
            RETURN_LONG(def->segments[intern->n].fieldNum);
        }
    }

    RETURN_FALSE;
}

TRANSACTD_ZEND_METHOD(KeySegment, __set)
{
    char *name;
    int name_len;
    zval *value;
    php_transactd_keysegment_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz",
                              &name, &name_len, &value) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_KEYSEGMENT_OBJ(intern, getThis());

    if (strcmp(name, "fieldNum") == 0) {
        keydef *def = php_transactd_get_keydef(intern TSRMLS_CC);
        if (def && intern->n >= 0 && intern->n < 8) {
            convert_to_long(value);
            def->segments[intern->n].fieldNum = (uchar_td)Z_LVAL_P(value);
            RETURN_TRUE;
        }
    } else {
        TRANSACTD_ERR(E_WARNING, "Unsupported: %s", name);
    }

    RETURN_FALSE;
}

static zend_function_entry php_transactd_keysegment_methods[] = {
    TRANSACTD_ZEND_ME(KeySegment, __construct, NULL,
                      ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    TRANSACTD_ZEND_ME(KeySegment, __get,
                      arginfo_transactd_keysegment___get, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(KeySegment, __set,
                      arginfo_transactd_keysegment___set, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

static void
php_transactd_keysegment_free_storage(void *object TSRMLS_DC)
{
    php_transactd_keysegment_t *intern = (php_transactd_keysegment_t *)object;

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
php_transactd_keysegment_new_ex(zend_class_entry *ce,
                                php_transactd_keysegment_t **ptr TSRMLS_DC)
{
    php_transactd_keysegment_t *intern;
    zend_object_value retval;

    intern = (php_transactd_keysegment_t *)emalloc(
        sizeof(php_transactd_keysegment_t));
    memset(intern, 0, sizeof(php_transactd_keysegment_t));
    if (ptr) {
        *ptr = intern;
    }

    zend_object_std_init(&intern->std, ce TSRMLS_CC);
    object_properties_init(&intern->std, ce);

    retval.handle = zend_objects_store_put(
        intern, (zend_objects_store_dtor_t)zend_objects_destroy_object,
        (zend_objects_free_object_storage_t)php_transactd_keysegment_free_storage,
        NULL TSRMLS_CC);
    retval.handlers = &php_transactd_keysegment_handlers;

    intern->link = NULL;
    intern->n = 0;

    return retval;
}

static zend_object_value
php_transactd_keysegment_new(zend_class_entry *ce TSRMLS_DC)
{
    return php_transactd_keysegment_new_ex(ce, NULL TSRMLS_CC);
}

PHP_TRANSACTD_API int
php_transactd_keysegment_class_register(int module_number TSRMLS_DC)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, ZEND_NS_NAME(TRANSACTD_NS, "KeySegment"),
                     php_transactd_keysegment_methods);

    ce.create_object = php_transactd_keysegment_new;

    php_transactd_keysegment_ce = zend_register_internal_class(&ce TSRMLS_CC);
    if (php_transactd_keysegment_ce == NULL) {
        return FAILURE;
    }

    memcpy(&php_transactd_keysegment_handlers, zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));

    php_transactd_keysegment_handlers.clone_obj = NULL;

    zend_declare_property_null(php_transactd_keysegment_ce,
                               "flags", sizeof("flags") - 1,
                               ZEND_ACC_PUBLIC TSRMLS_CC);

    return SUCCESS;
}

PHP_TRANSACTD_API int
php_transactd_keysegment_class_init(php_transactd_keysegment_t *intern,
                                    zval *this_ptr, zval *return_value TSRMLS_DC)
{
    zval *obj;
    php_transactd_flags_t *flags;
    keydef *def;

    def = php_transactd_get_keydef(intern TSRMLS_CC);
    if (!def || intern->n < 0 || intern->n >= 8) {
        return FAILURE;
    }

    MAKE_STD_ZVAL(obj);

    object_init_ex(obj, php_transactd_flags_ce);

    TRANSACTD_FLAGS_OBJ(flags, obj);

    flags->flags = &def->segments[intern->n].flags;
    flags->link = this_ptr;
    zval_add_ref(&flags->link);

    zend_update_property(php_transactd_keydef_ce, return_value,
                         "flags", sizeof("flags")-1, obj TSRMLS_CC);

    zval_ptr_dtor(&obj);

    return SUCCESS;
}
