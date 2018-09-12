/*
 +----------------------------------------------------------------------+
 | Extreme AOP extension for PHP 7                                      |
 +----------------------------------------------------------------------+
 | Copyright (c) 1997-2018 The PHP Group                                |
 +----------------------------------------------------------------------+
 | This source file is subject to version 3.01 of the PHP license,      |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.php.net/license/3_01.txt                                  |
 | If you did not receive a copy of the PHP license and are unable to   |
 | obtain it through the world-wide-web, please send a note to          |
 | license@php.net so we can mail you a copy immediately.               |
 +----------------------------------------------------------------------+
 | Author: Josin https://www.supjos.cn                                  |
 +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_xaop.h"
#include "ext/standard/php_string.h"
#include "ext/json/php_json.h"
#include "kernel/common/helper_lib.h"
#include "zend_smart_str.h"
#include "kernel/loader/loader.h"
#include "Zend/zend_interfaces.h"

/**
 * {{{
 * proto Auto load the class with the given class name plus the full namespace
 * such as :
 * app\annotations\Tag
 * NOTE: the classname with namespace need to be sensative!!!
 * Developer must know to make sure that the class can't be found in the current class_table array
 * if exists in EG(class_table), and you invoke the xaop_auto_load_class, it will let the php engine to 
 * throw the redefinition class error info.
 */
void xaop_auto_load_class(zend_string *classname_with_ns)
{
    xaop_internal_load(classname_with_ns);
}/*}}}*/

/**
 * {{{ 
 * proto Invoke the autoload function to load the class
 */
void xaop_internal_load(zend_string *classname_with_ns)
{
    if ( Z_TYPE(XAOP_G(aliases)) != IS_ARRAY || !zend_hash_num_elements(Z_ARRVAL(XAOP_G(aliases))) ) {
        php_error_docref( 0, E_ERROR, "autoload class: `%s` failed, please setMap() aliases first!", ZSTR_VAL(classname_with_ns) );
        return ;
    }

    if ( ZSTR_VAL(classname_with_ns)[0] == '\\' ) {
        classname_with_ns = strpprintf(0, "%s", ZSTR_VAL(classname_with_ns) + 1 );
    }

    if ( zend_hash_find_ptr(EG(class_table), zend_string_tolower(classname_with_ns)) ) {
        return ;
    }

    char alias_name[256] = { 0 };
    zend_string *name, *real_file_path;
    zval *name_path, retval;
    char *slash_pos = strchr(ZSTR_VAL(classname_with_ns), '\\' );

    /* Not found the '\' char */
    if ( !slash_pos ) {
        real_file_path = strpprintf(0, "./%s.php", ZSTR_VAL(classname_with_ns));
        XAOP_C_TO(include_opcode);
    } else {
        memcpy( alias_name, ZSTR_VAL(classname_with_ns), slash_pos - ZSTR_VAL(classname_with_ns) );
    }

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL(XAOP_G(aliases)), name, name_path) {
        if ( 0 == strncasecmp(ZSTR_VAL(name), ZEND_STRL(alias_name)) ) {
            real_file_path  = strpprintf(0, "%s/%s.php", Z_STRVAL_P(name_path), slash_pos + 1);
            XAOP_C_TO(include_opcode);
            return ;
        }
    } ZEND_HASH_FOREACH_END();

    php_error_docref(0, E_ERROR, "Xaop can't load the class: `%s`.", ZSTR_VAL(classname_with_ns));

XAOP_C_LABEL(include_opcode)
    xaop_reverse_slash_string(real_file_path);
    if ( SUCCESS == xaop_include_php_file(real_file_path, &retval) ) {
        /* If you like, you can add some logical job after include the php file */
    } else {
        php_error_docref(0, E_ERROR, "Can't auto load the class: `%s`.", ZSTR_VAL(classname_with_ns));
    }
}/*}}}*/

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_loader_construct), 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_loader_set_map), 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_loader_loader), 0, 0, 1)
    ZEND_ARG_INFO(0, className)
ZEND_END_ARG_INFO()

/**
 * {{{ proto Xaop\Loader::__construct()
 * to load the class automatically
 */
XAOP_METHOD(Loader, __construct)
{
    /* Enable auto_load */
    zval loader;
    
    /* Construct the calling params */
    array_init(&loader);

    add_next_index_zval(&loader, getThis());
    add_next_index_string(&loader, "loader");

    zend_call_method_with_1_params( 
        NULL, 
        NULL,
        NULL, 
        "spl_autoload_register", 
        return_value, 
        &loader
    );
}/*}}}*/

/**
 * {{{ proto Xaop\Loader::setMap($name, $path)
 * set the path with the given name, let the kernel to found the class with the given alias name
 */
XAOP_METHOD(Loader, setMap)
{
    zend_string *alias_name, *alias_path;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "SS", &alias_name, &alias_path) == FAILURE){
        return ;
    }

    if ( *(ZSTR_VAL(alias_name)) != '@' ){
        php_error_docref(0, E_ERROR, "$aliasName must be start with `@`!");
        return ;
    }

    add_assoc_str(&XAOP_G(aliases), ZSTR_VAL(alias_name) + 1, alias_path);
}/*}}}*/

/**
 * {{{ proto Xaop\Loader::loader($name)
 * to auto load the class with the given name
 */
XAOP_METHOD(Loader, loader)
{
    zend_string *class_name_with_ns;
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &class_name_with_ns) == FAILURE ) {
        return ;
    }
    xaop_auto_load_class(class_name_with_ns);
}/*}}}*/

XAOP_FUNCTIONS(loader)
    XAOP_ME(Loader, __construct, arginfo_xaop_loader_construct, ZEND_ACC_PUBLIC)
    XAOP_ME(Loader, setMap, arginfo_xaop_loader_set_map, ZEND_ACC_PUBLIC)
    XAOP_ME(Loader, loader, arginfo_xaop_loader_loader, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL )
XAOP_FUNCTIONS_END()


XAOP_INIT(loader)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Xaop", "Loader", loader_functions);
    xaop_loader_ce = zend_register_internal_class(&ce);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
