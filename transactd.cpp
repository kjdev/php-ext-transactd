#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_CONFIG_H
#    include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend.h"

#ifdef __cplusplus
}
#endif

#include "php_transactd.h"
#include "database.h"
#include "table.h"
#include "dbdef.h"
#include "tabledef.h"
#include "fielddef.h"
#include "keydef.h"
#include "keysegment.h"
#include "flags.h"
#include "exception.h"

ZEND_DECLARE_MODULE_GLOBALS(transactd)

ZEND_INI_BEGIN()
ZEND_INI_END()

static void
transactd_init_globals(zend_transactd_globals *transactd_globals)
{}

ZEND_MINIT_FUNCTION(transactd)
{
    ZEND_INIT_MODULE_GLOBALS(transactd, transactd_init_globals, NULL);
    REGISTER_INI_ENTRIES();

    php_transactd_database_class_register(module_number TSRMLS_CC);
    php_transactd_table_class_register(module_number TSRMLS_CC);
    php_transactd_dbdef_class_register(module_number TSRMLS_CC);
    php_transactd_tabledef_class_register(module_number TSRMLS_CC);
    php_transactd_fielddef_class_register(module_number TSRMLS_CC);
    php_transactd_keydef_class_register(module_number TSRMLS_CC);
    php_transactd_keysegment_class_register(module_number TSRMLS_CC);
    php_transactd_flags_class_register(module_number TSRMLS_CC);
    php_transactd_except_class_register(TSRMLS_C);

    return SUCCESS;
}

ZEND_MSHUTDOWN_FUNCTION(transactd)
{
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}

ZEND_MINFO_FUNCTION(transactd)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "Extension load support", "enabled");
    php_info_print_table_row(2, "Extension Version", TRANSACTD_EXT_VERSION);
    php_info_print_table_end();
}

zend_module_entry transactd_module_entry = {
    STANDARD_MODULE_HEADER,
    "transactd",
    NULL,
    ZEND_MINIT(transactd),
    ZEND_MSHUTDOWN(transactd),
    NULL,
    NULL,
    ZEND_MINFO(transactd),
    TRANSACTD_EXT_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL_TRANSACTD
ZEND_GET_MODULE(transactd)
#endif
