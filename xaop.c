/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
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
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_xaop.h"
#include "kernel/exec.h"

/* If you declare any globals in php_xaop.h uncomment this: */
ZEND_DECLARE_MODULE_GLOBALS(xaop)

/* True global resources - no need for thread safety here */
static int le_xaop;

/** {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("xaop.aop_mode", "1", PHP_INI_ALL, OnUpdateLong, aop_mode, zend_xaop_globals, xaop_globals)
PHP_INI_END()
/* }}} */

/* {{{ proto string get_xaop_version(void)
   Return a string contains the xaop version */
PHP_FUNCTION(get_xaop_version)
{
	if (zend_parse_parameters_none() == FAILURE ) {
	    return ;
	}
	
	RETURN_STRING(PHP_XAOP_VERSION);
}
/* }}} */

/* {{{ php_xaop_init_globals
 */
/* Uncomment this function if you have INI entries */
static void php_xaop_init_globals(zend_xaop_globals *xaop_globals)
{
    xaop_globals->aop_mode = 1;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(xaop)
{
	/* If you have INI entries, uncomment these lines */
    ZEND_INIT_MODULE_GLOBALS(xaop, php_xaop_init_globals, NULL);
	REGISTER_INI_ENTRIES();
	
	annotation_init();
	doc_init();
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(xaop)
{
	/* uncomment this line if you have INI entries */
	UNREGISTER_INI_ENTRIES();
	
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(xaop)
{
#if defined(COMPILE_DL_XAOP) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
    /** After INI config read already, to init the kernel aop handler */
    switch ( XAOP_G(aop_mode) ) {
        case ANNOTATION_AOP:
            zend_execute_ex = xaop_annotation_ex;
            break;
        case INJECTION_AOP:
            break;
    }
    XAOP_G(overloaded) = 0;
    array_init(&XAOP_G(di));
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(xaop)
{
#if defined(COMPILE_DL_XAOP) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    zend_array_destroy(Z_ARRVAL(XAOP_G(di)));
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(xaop)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "xaop support", "enabled" );
	php_info_print_table_header(2, "xaop version", PHP_XAOP_VERSION );
	php_info_print_table_header(2, "enable mode", ( ANNOTATION_AOP == XAOP_G(aop_mode) ) ? "Annotation(文档注解) AOP" : (
	    INJECTION_AOP == XAOP_G(aop_mode) ? "Injection(方法注入) AOP" : "Unkown mode" ) );
	php_info_print_table_header(2, "xaop author", "Josin");
	php_info_print_table_header(2, "current php version", PHP_VERSION );
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini */
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ xaop_functions[]
 *
 * Every user visible function must have an entry in xaop_functions[].
 */
const zend_function_entry xaop_functions[] = {
	PHP_FE(get_xaop_version,	NULL)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in xaop_functions[] */
};
/* }}} */

/* {{{ xaop_module_entry
 */
zend_module_entry xaop_module_entry = {
	STANDARD_MODULE_HEADER,
	"xaop",
	xaop_functions,
	PHP_MINIT(xaop),
	PHP_MSHUTDOWN(xaop),
	PHP_RINIT(xaop),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(xaop),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(xaop),
	PHP_XAOP_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XAOP
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(xaop)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
