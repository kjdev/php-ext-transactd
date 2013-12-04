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
#include "table.h"
#include "exception.h"

#include <bzs/db/protocol/tdap/client/dbDef.h>

using namespace bzs::db::protocol::tdap::client;
using namespace bzs::db::protocol::tdap;

ZEND_EXTERN_MODULE_GLOBALS(transactd)

zend_class_entry *php_transactd_db_ce;
static zend_object_handlers php_transactd_db_handlers;

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_db___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_db_create, 0, 0, 1)
    ZEND_ARG_INFO(0, uri)
    ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_db_open, 0, 0, 1)
    ZEND_ARG_INFO(0, uri)
    ZEND_ARG_INFO(0, type)
    ZEND_ARG_INFO(0, mode)
    ZEND_ARG_INFO(0, dir)
    ZEND_ARG_INFO(0, owner)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_db_close, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_db_drop, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_db_dbdef, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_db_opentable, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, mode)
    ZEND_ARG_INFO(0, autoCreate)
    ZEND_ARG_INFO(0, owner)
    ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_db_beginTrn, 0, 0, 0)
    ZEND_ARG_INFO(0, bias)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_db_endTrn, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_db_abortTrn, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_transactd_db_stat, 0, 0, 0)
ZEND_END_ARG_INFO()

TRANSACTD_ZEND_METHOD(Database, __construct)
{
    php_transactd_database_t *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DATABASE_OBJ(intern, getThis());

    intern->db = database::create();

    if (!intern->db) {
        TRANSACTD_EXCEPTION(0, "Transactd\\Database object has not been "
                            "correctly initialized by its constructor");
        RETURN_FALSE;
    }
}

TRANSACTD_ZEND_METHOD(Database, create)
{
    char *uri;
    int uri_len;
    long type = TYPE_SCHEMA_BDF;
    php_transactd_database_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l",
                              &uri, &uri_len, &type) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DATABASE_OBJ(intern, getThis());

    intern->db->create(uri, type);
    if (intern->db->stat() != 0) {
        php_transactd_error(E_WARNING, intern->db->stat(), NULL);
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Database, open)
{
    char *uri;
    int uri_len;
    long type = TYPE_SCHEMA_BDF;
    long mode = TD_OPEN_READONLY;
    char *dir = NULL, *owner = NULL;
    int dir_len = 0, owner_len = 0;
    php_transactd_database_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|llss",
                              &uri, &uri_len, &type, &mode,
                              &dir, &dir_len, &owner, &owner_len) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DATABASE_OBJ(intern, getThis());

    if (intern->uri) {
        TRANSACTD_ERR(E_WARNING, "Already open: %s", Z_STRVAL_P(intern->uri));
        RETURN_FALSE;
    }

    if (!uri || uri_len <= 0) {
        TRANSACTD_ERR(E_WARNING, "Empty uri");
        RETURN_FALSE;
    }

    intern->db->open(uri, type, mode, dir, owner);
    if (intern->db->stat() != 0) {
        php_transactd_error(E_WARNING, intern->db->stat(), NULL);
        RETURN_FALSE;
    }

    MAKE_STD_ZVAL(intern->uri);
    ZVAL_STRINGL(intern->uri, uri, uri_len, 1);

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Database, drop)
{
    php_transactd_database_t *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DATABASE_OBJ(intern, getThis());
    if (!intern->uri) {
        TRANSACTD_EXCEPTION(0, "No open uri");
        RETURN_FALSE;
    }

    intern->db->drop();
    if (intern->db->stat() != 0) {
        php_transactd_error(E_WARNING, intern->db->stat(), NULL);
        RETURN_FALSE;
    }

    intern->db->close();

    zval_ptr_dtor(&intern->uri);
    intern->uri = NULL;

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Database, close)
{
    php_transactd_database_t *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DATABASE_OBJ(intern, getThis());
    if (!intern->uri) {
        TRANSACTD_EXCEPTION(0, "No open uri");
        RETURN_FALSE;
    }

    intern->db->close();

    zval_ptr_dtor(&intern->uri);
    intern->uri = NULL;

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Database, dbDef)
{
    php_transactd_database_t *intern;
    php_transactd_dbdef_t *dbdef;

    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DATABASE_OBJ(intern, getThis());
    if (!intern->uri) {
        TRANSACTD_EXCEPTION(0, "No open uri");
        RETURN_FALSE;
    }

    object_init_ex(return_value, php_transactd_dbdef_ce);

    TRANSACTD_DBDEF_OBJ(dbdef, return_value);

    dbdef->link = getThis();
    zval_add_ref(&dbdef->link);

    dbdef->def = intern->db->dbDef();
    if (!dbdef->def) {
        TRANSACTD_EXCEPTION(0, "Transactd\\DbDef object has not been "
                            "correctly initialized by its constructor");
        RETURN_FALSE;
    }
}

TRANSACTD_ZEND_METHOD(Database, openTable)
{
    long mode = 0;
    char *owner = NULL, *uri = NULL;
    zend_bool create = 1;
    zval *name;
    php_transactd_database_t *intern;
    php_transactd_table_t *table;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|lbss",
                              &name, &mode, &create, &owner, &uri) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DATABASE_OBJ(intern, getThis());
    if (!intern->uri) {
        TRANSACTD_EXCEPTION(0, "No open uri");
        RETURN_FALSE;
    }

    object_init_ex(return_value, php_transactd_table_ce);

    TRANSACTD_TABLE_OBJ(table, return_value);

    table->link = getThis();
    zval_add_ref(&table->link);

    if (php_transactd_table_class_init(table, return_value, name, mode, create,
                                       owner, uri TSRMLS_CC) == FAILURE) {
        TRANSACTD_EXCEPTION(0, "Transactd\\Table object has not been "
                            "correctly initialized by its constructor");
        RETURN_FALSE;
    }
}

TRANSACTD_ZEND_METHOD(Database, beginTrn)
{
    long bias = LOCK_SINGLE_NOWAIT + PARALLEL_TRN + NOWAIT_WRITE;
    php_transactd_database_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l",
                              &bias) == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DATABASE_OBJ(intern, getThis());
    if (!intern->uri) {
        TRANSACTD_EXCEPTION(0, "No open uri");
        RETURN_FALSE;
    }

    intern->db->beginTrn((short)bias);

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Database, endTrn)
{
    php_transactd_database_t *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DATABASE_OBJ(intern, getThis());
    if (!intern->uri) {
        TRANSACTD_EXCEPTION(0, "No open uri");
        RETURN_FALSE;
    }

    intern->db->endTrn();

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Database, abortTrn)
{
    php_transactd_database_t *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DATABASE_OBJ(intern, getThis());
    if (!intern->uri) {
        TRANSACTD_EXCEPTION(0, "No open uri");
        RETURN_FALSE;
    }

    intern->db->abortTrn();

    RETURN_TRUE;
}

TRANSACTD_ZEND_METHOD(Database, stat)
{
    php_transactd_database_t *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    TRANSACTD_DATABASE_OBJ(intern, getThis());
    if (!intern->uri) {
        TRANSACTD_EXCEPTION(0, "No open uri");
        RETURN_FALSE;
    }

    RETURN_LONG(intern->db->stat());
}

static zend_function_entry php_transactd_db_methods[] = {
    TRANSACTD_ZEND_ME(Database, __construct, arginfo_transactd_db___construct,
                      ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    TRANSACTD_ZEND_ME(Database, create, arginfo_transactd_db_create,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Database, open, arginfo_transactd_db_open,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Database, drop, arginfo_transactd_db_drop,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Database, close, arginfo_transactd_db_close,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Database, dbDef, arginfo_transactd_db_dbdef,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Database, openTable, arginfo_transactd_db_opentable,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Database, beginTrn, arginfo_transactd_db_beginTrn,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_MALIAS(Database, beginTransaction,
                          beginTrn, arginfo_transactd_db_beginTrn,
                          ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Database, endTrn, arginfo_transactd_db_endTrn,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_MALIAS(Database, commit, endTrn, arginfo_transactd_db_endTrn,
                          ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Database, abortTrn, arginfo_transactd_db_abortTrn,
                      ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_MALIAS(Database, rollBack,
                          abortTrn, arginfo_transactd_db_abortTrn,
                          ZEND_ACC_PUBLIC)
    TRANSACTD_ZEND_ME(Database, stat, arginfo_transactd_db_stat,
                      ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

static void
php_transactd_db_free_storage(void *object TSRMLS_DC)
{
    php_transactd_database_t *intern = (php_transactd_database_t *)object;

    if (!intern) {
        return;
    }

    if (intern->db) {
        if (intern->uri) {
            intern->db->close();
            zval_ptr_dtor(&intern->uri);
        }
        database::destroy(intern->db);
    }

    zend_object_std_dtor(&intern->std TSRMLS_CC);
    efree(object);
}

static zend_object_value
php_transactd_db_new_ex(zend_class_entry *ce,
                        php_transactd_database_t **ptr TSRMLS_DC)
{
    php_transactd_database_t *intern;
    zend_object_value retval;

    intern = (php_transactd_database_t *)emalloc(
        sizeof(php_transactd_database_t));
    memset(intern, 0, sizeof(php_transactd_database_t));
    if (ptr) {
        *ptr = intern;
    }

    zend_object_std_init(&intern->std, ce TSRMLS_CC);
    object_properties_init(&intern->std, ce);

    retval.handle = zend_objects_store_put(
        intern, (zend_objects_store_dtor_t)zend_objects_destroy_object,
        (zend_objects_free_object_storage_t)php_transactd_db_free_storage,
        NULL TSRMLS_CC);
    retval.handlers = &php_transactd_db_handlers;

    intern->db = NULL;
    intern->uri = NULL;

    return retval;
}

static zend_object_value
php_transactd_db_new(zend_class_entry *ce TSRMLS_DC)
{
    return php_transactd_db_new_ex(ce, NULL TSRMLS_CC);
}

PHP_TRANSACTD_API int
php_transactd_db_class_register(int module_number TSRMLS_DC)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, ZEND_NS_NAME(TRANSACTD_NS, "Database"),
                     php_transactd_db_methods);

    ce.create_object = php_transactd_db_new;

    php_transactd_db_ce = zend_register_internal_class(&ce TSRMLS_CC);
    if (php_transactd_db_ce == NULL) {
        return FAILURE;
    }

    memcpy(&php_transactd_db_handlers, zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));

    php_transactd_db_handlers.clone_obj = NULL;

    /* constant */
    TRANSACTD_LONG_CONSTANT("TYPE_SCHEMA_BDF", TYPE_SCHEMA_BDF);
    TRANSACTD_LONG_CONSTANT("TYPE_SCHEMA_DDF", TYPE_SCHEMA_DDF);
    TRANSACTD_LONG_CONSTANT("TD_OPEN_NORMAL", TD_OPEN_NORMAL);
    TRANSACTD_LONG_CONSTANT("TD_OPEN_READONLY", TD_OPEN_READONLY);
    TRANSACTD_LONG_CONSTANT("TD_OPEN_EXCLUSIVE", TD_OPEN_EXCLUSIVE);

    TRANSACTD_LONG_CONSTANT("MAX_CHAR_INFO", MAX_CHAR_INFO);
    TRANSACTD_LONG_CONSTANT("CHARSET_LATIN1", CHARSET_LATIN1);
    TRANSACTD_LONG_CONSTANT("CHARSET_CP850", CHARSET_CP850);
    TRANSACTD_LONG_CONSTANT("CHARSET_ASCII", CHARSET_ASCII);
    TRANSACTD_LONG_CONSTANT("CHARSET_SJIS", CHARSET_SJIS);
    TRANSACTD_LONG_CONSTANT("CHARSET_UTF8", CHARSET_UTF8);
    TRANSACTD_LONG_CONSTANT("CHARSET_USC2", CHARSET_USC2);
    TRANSACTD_LONG_CONSTANT("CHARSET_UTF8B4", CHARSET_UTF8B4);
    TRANSACTD_LONG_CONSTANT("CHARSET_UTF16LE", CHARSET_UTF16LE);
    TRANSACTD_LONG_CONSTANT("CHARSET_CP932", CHARSET_CP932);
    TRANSACTD_LONG_CONSTANT("CHARSET_EUCJ", CHARSET_EUCJ);

    TRANSACTD_LONG_CONSTANT("FT_STRING", ft_string);
    TRANSACTD_LONG_CONSTANT("FT_INTEGER", ft_integer);
    TRANSACTD_LONG_CONSTANT("FT_FLOAT", ft_float);
    TRANSACTD_LONG_CONSTANT("FT_DATE", ft_date);
    TRANSACTD_LONG_CONSTANT("FT_TIME", ft_time);
    TRANSACTD_LONG_CONSTANT("FT_DECIMAL", ft_decimal);
    TRANSACTD_LONG_CONSTANT("FT_MONEY", ft_money);
    TRANSACTD_LONG_CONSTANT("FT_LOGICAL", ft_logical);
    TRANSACTD_LONG_CONSTANT("FT_NUMERIC", ft_numeric);
    TRANSACTD_LONG_CONSTANT("FT_BFLOAT", ft_bfloat);
    TRANSACTD_LONG_CONSTANT("FT_LSTRING", ft_lstring);
    TRANSACTD_LONG_CONSTANT("FT_ZSTRING", ft_zstring);
    TRANSACTD_LONG_CONSTANT("FT_NOTE", ft_note);
    TRANSACTD_LONG_CONSTANT("FT_LVAR", ft_lvar);
    TRANSACTD_LONG_CONSTANT("FT_UINTEGER", ft_uinteger);
    TRANSACTD_LONG_CONSTANT("FT_AUTOINC", ft_autoinc);
    TRANSACTD_LONG_CONSTANT("FT_BIT", ft_bit);
    TRANSACTD_LONG_CONSTANT("FT_NUMERICSTS", ft_numericsts);
    TRANSACTD_LONG_CONSTANT("FT_NUMERICSA", ft_numericsa);
    TRANSACTD_LONG_CONSTANT("FT_CURRENCY", ft_currency);
    TRANSACTD_LONG_CONSTANT("FT_TIMESTAMP", ft_timestamp);
    TRANSACTD_LONG_CONSTANT("FT_BLOB", ft_blob);
    TRANSACTD_LONG_CONSTANT("FT_RESERVE22", ft_reserve22);
    TRANSACTD_LONG_CONSTANT("FT_RESERVE23", ft_reserve23);
    TRANSACTD_LONG_CONSTANT("FT_RESERVE24", ft_reserve24);
    TRANSACTD_LONG_CONSTANT("FT_WSTRING", ft_wstring);
    TRANSACTD_LONG_CONSTANT("FT_WZSTRING", ft_wzstring);
    TRANSACTD_LONG_CONSTANT("FT_GUID", ft_guid);
    TRANSACTD_LONG_CONSTANT("FT_DATATIME", ft_datatime);
    TRANSACTD_LONG_CONSTANT("FT_MYVARCHAR", ft_myvarchar);
    TRANSACTD_LONG_CONSTANT("FT_MYVARBINARY", ft_myvarbinary);
    TRANSACTD_LONG_CONSTANT("FT_MYWVARCHAR", ft_mywvarchar);
    TRANSACTD_LONG_CONSTANT("FT_MYWVARBINARY", ft_mywvarbinary);
    TRANSACTD_LONG_CONSTANT("FT_MYCHAR", ft_mychar);
    TRANSACTD_LONG_CONSTANT("FT_MYWCHAR", ft_mywchar);
    TRANSACTD_LONG_CONSTANT("FT_MYDATE", ft_mydate);
    TRANSACTD_LONG_CONSTANT("FT_MYTIME", ft_mytime);
    TRANSACTD_LONG_CONSTANT("FT_MYDATETIME", ft_mydatetime);
    TRANSACTD_LONG_CONSTANT("FT_MYTIMESTAMP", ft_mytimestamp);
    TRANSACTD_LONG_CONSTANT("FT_MYTEXT", ft_mytext);
    TRANSACTD_LONG_CONSTANT("FT_MYBLOB", ft_myblob);
    TRANSACTD_LONG_CONSTANT("FT_NULLINDICATOR", ft_nullindicator);

    TRANSACTD_LONG_CONSTANT("CHANGE_CURRENT_CC", nstable::changeCurrentCc);
    TRANSACTD_LONG_CONSTANT("CHANGE_CURRENT_NCC", nstable::changeCurrentNcc);
    TRANSACTD_LONG_CONSTANT("CHANGE_IN_KEY", nstable::changeInKey);

    TRANSACTD_LONG_CONSTANT("STATUS_EOF", STATUS_EOF);

    return SUCCESS;
}

PHP_TRANSACTD_API void
php_transactd_error(int flag, short code, char *table)
{
    char msg[1024] = {0x00};

    nstable::tdapErr(0x00, code, table, msg);
    if (flag < 0) {
        TRANSACTD_EXCEPTION(code, msg);
    } else {
        TRANSACTD_ERR(flag, msg);
    }
}
