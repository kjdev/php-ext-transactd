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
#include "flags.h"
#include "exception.h"

using namespace bzs::db::protocol::tdap;

ZEND_EXTERN_MODULE_GLOBALS(transactd)

zend_class_entry *php_transactd_flags_ce;
static zend_object_handlers php_transactd_flags_handlers;

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_flags___get, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_flags___set, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

TRANSACTD_ZEND_METHOD(Flags, __construct)
{
    TRANSACTD_EXCEPTION(0, "Transactd\\Flags object has not been "
                        "correctly initialized by its constructor");
    RETURN_FALSE;
}

TRANSACTD_ZEND_METHOD(Flags, __get)
{
    char *name;
    int name_len;
    php_transactd_flags_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &name, &name_len) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_FLAGS_OBJ(intern, getThis());

    if (strcmp(name, "bit0") == 0) {
        RETURN_LONG(intern->flags->bit0);
    } else if (strcmp(name, "bit1") == 0) {
        RETURN_LONG(intern->flags->bit1);
    } else if (strcmp(name, "bit2") == 0) {
        RETURN_LONG(intern->flags->bit2);
    } else if (strcmp(name, "bit3") == 0) {
        RETURN_LONG(intern->flags->bit3);
    } else if (strcmp(name, "bit4") == 0) {
        RETURN_LONG(intern->flags->bit4);
    } else if (strcmp(name, "bit5") == 0) {
        RETURN_LONG(intern->flags->bit5);
    } else if (strcmp(name, "bit6") == 0) {
        RETURN_LONG(intern->flags->bit6);
    } else if (strcmp(name, "bit7") == 0) {
        RETURN_LONG(intern->flags->bit7);
    } else if (strcmp(name, "bit8") == 0) {
        RETURN_LONG(intern->flags->bit8);
    } else if (strcmp(name, "bit9") == 0) {
        RETURN_LONG(intern->flags->bit9);
    } else if (strcmp(name, "bitA") == 0) {
        RETURN_LONG(intern->flags->bitA);
    } else if (strcmp(name, "bitB") == 0) {
        RETURN_LONG(intern->flags->bitB);
    } else if (strcmp(name, "bitC") == 0) {
        RETURN_LONG(intern->flags->bitC);
    } else if (strcmp(name, "bitD") == 0) {
        RETURN_LONG(intern->flags->bitD);
    } else if (strcmp(name, "bitE") == 0) {
        RETURN_LONG(intern->flags->bitE);
    } else if (strcmp(name, "bitF") == 0) {
        RETURN_LONG(intern->flags->bitF);
    }

    RETURN_FALSE;
}

TRANSACTD_ZEND_METHOD(Flags, __set)
{
    char *name;
    int name_len;
    zval *value;
    php_transactd_flags_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz",
                              &name, &name_len, &value) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_FLAGS_OBJ(intern, getThis());

    convert_to_long(value);

    if (strcmp(name, "bit0") == 0) {
        intern->flags->bit0 = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bit1") == 0) {
        intern->flags->bit1 = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bit2") == 0) {
        intern->flags->bit2 = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bit3") == 0) {
        intern->flags->bit3 = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bit4") == 0) {
        intern->flags->bit4 = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bit5") == 0) {
        intern->flags->bit5 = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bit6") == 0) {
        intern->flags->bit6 = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bit7") == 0) {
        intern->flags->bit7 = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bit8") == 0) {
        intern->flags->bit8 = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bit9") == 0) {
        intern->flags->bit9 = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bitA") == 0) {
        intern->flags->bitA = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bitB") == 0) {
        intern->flags->bitB = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bitC") == 0) {
        intern->flags->bitC = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bitD") == 0) {
        intern->flags->bitD = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bitE") == 0) {
        intern->flags->bitE = (unsigned short)Z_LVAL_P(value);
    } else if (strcmp(name, "bitF") == 0) {
        intern->flags->bitF = (unsigned short)Z_LVAL_P(value);
    } else {
        TRANSACTD_ERR(E_WARNING, "Unsupported: %s", name);
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

static zend_function_entry php_transactd_flags_methods[] = {
    TRANSACTD_ZEND_ME(Flags, __construct, NULL,
                      ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    TRANSACTD_ZEND_ME(Flags, __get,
                      arginfo_transactd_flags___get, ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Flags, __set,
                      arginfo_transactd_flags___set, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

static void
php_transactd_flags_free_storage(void *object TSRMLS_DC)
{
    php_transactd_flags_t *intern = (php_transactd_flags_t *)object;

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
php_transactd_flags_new_ex(zend_class_entry *ce,
                              php_transactd_flags_t **ptr TSRMLS_DC)
{
    php_transactd_flags_t *intern;
    zend_object_value retval;

    intern = (php_transactd_flags_t *)emalloc(
        sizeof(php_transactd_flags_t));
    memset(intern, 0, sizeof(php_transactd_flags_t));
    if (ptr) {
        *ptr = intern;
    }

    zend_object_std_init(&intern->std, ce TSRMLS_CC);
    object_properties_init(&intern->std, ce);

    retval.handle = zend_objects_store_put(
        intern, (zend_objects_store_dtor_t)zend_objects_destroy_object,
        (zend_objects_free_object_storage_t)php_transactd_flags_free_storage,
        NULL TSRMLS_CC);
    retval.handlers = &php_transactd_flags_handlers;

    intern->link = NULL;
    intern->flags = NULL;

    return retval;
}

static zend_object_value
php_transactd_flags_new(zend_class_entry *ce TSRMLS_DC)
{
    return php_transactd_flags_new_ex(ce, NULL TSRMLS_CC);
}

PHP_TRANSACTD_API int
php_transactd_flags_class_register(int module_number TSRMLS_DC)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, ZEND_NS_NAME(TRANSACTD_NS, "Flags"),
                     php_transactd_flags_methods);

    ce.create_object = php_transactd_flags_new;

    php_transactd_flags_ce = zend_register_internal_class(&ce TSRMLS_CC);
    if (php_transactd_flags_ce == NULL) {
        return FAILURE;
    }

    memcpy(&php_transactd_flags_handlers, zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));

    php_transactd_flags_handlers.clone_obj = NULL;

    return SUCCESS;
}
