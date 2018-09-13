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

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_xaop.h"
#include "kernel/common/anno_parsing.h"
#include "kernel/aop/exec.h"

/* If you declare any globals in php_xaop.h uncomment this: */
ZEND_DECLARE_MODULE_GLOBALS(xaop)

/* True global resources - no need for thread safety here */
static int le_xaop;

XAOP_INIT(loader);
XAOP_INIT(annotation);
XAOP_INIT(app);
XAOP_INIT(xaop);

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("xaop.aop_mode", "1", PHP_INI_SYSTEM, OnUpdateLong, aop_mode, zend_xaop_globals, xaop_globals)
PHP_INI_END()

/* }}} */

/* {{{ proto string get_xaop_version()
   Return the version of the xaop */
PHP_FUNCTION(get_xaop_version)
{
	if ( zend_parse_parameters_none() == FAILURE ) {
		return ;
	}

	RETURN_STRING(PHP_XAOP_VERSION);
}
/* }}} */


PHP_FUNCTION(get_phpdoc)
{
	zend_string *str1;
	zend_string *str2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "SS", &str1, &str2) == FAILURE ) {
		return ;
	}

	if ( SUCCESS == xaop_match_string(ZSTR_VAL(str1), ZSTR_VAL(str2)) ) {
		RETURN_TRUE
	} else {
		RETURN_FALSE
	}
}


/* {{{ php_xaop_init_globals
 */
/* Uncomment this function if you have INI entries */
static void php_xaop_init_globals(zend_xaop_globals *xaop_globals)
{
	xaop_globals->aop_mode = 1;
}
/* }}} */


/** {{{ PHP_GINIT_FUNCTION
*/
PHP_GINIT_FUNCTION(xaop)
{
	memset(xaop_globals, 0, sizeof(*xaop_globals));
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(xaop)
{
#if defined(COMPILE_DL_XAOP) && defined(ZTS)
	ZEND_INIT_MODULE_GLOBALS(xaop, NULL, NULL);
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	/* If you have INI entries, uncomment these lines
	*/
	REGISTER_INI_ENTRIES();
	
	if ( 2 == INI_INT("xaop.aop_mode") ) {
		zend_execute_ex = xaop_annotation_execute;
	} else if ( 3 == INI_INT("xaop.aop_mode") ) {
		zend_execute_ex = xaop_execute;
		zend_execute_internal = xaop_execute_internal;
	} else if ( 1 != INI_INT("xaop.aop_mode") ) {
		php_error_docref(0, E_ERROR, "xaop.aop_mode wrong.");
		return FAILURE;
	}
	
	loader_init();
	annotation_init();
	app_init();
	xaop_init();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(xaop)
{
	/* uncomment this line if you have INI entries
	*/
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

	/* Global DI container */
	array_init(&XAOP_G(di));

	/* Global autoload aliases */
	array_init(&XAOP_G(aliases));

	/* all injected aop functions */
	array_init(&XAOP_G(before_aops));
	array_init(&XAOP_G(after_aops));
	array_init(&XAOP_G(around_aops));
	array_init(&XAOP_G(before_func_aops));
	array_init(&XAOP_G(after_func_aops));
	array_init(&XAOP_G(around_func_aops));
	XAOP_G(around_data) = NULL;
	XAOP_G(around_type) = 0;

	/* Some mvc variables */
	XAOP_G(default_module) = zend_string_init( ZEND_STRL("index"), 0 );
	XAOP_G(default_controller) = zend_string_init( ZEND_STRL("index"), 0 );
	XAOP_G(default_action) = zend_string_init( ZEND_STRL("index"), 0 );
	array_init(&XAOP_G(default_modules));
	XAOP_G(url_mode) = 0;
	add_next_index_string(&XAOP_G(default_modules), "index");
	XAOP_G(url_get_str) = zend_string_init( ZEND_STRL("_url"), 0 );
	XAOP_G(view_suffix) = zend_string_init( ZEND_STRL("html"), 0 );
	XAOP_G(url_suffix) = zend_string_init( ZEND_STRL("html"), 0 );
	XAOP_G(application_dir) = zend_string_init( ZEND_STRL("."), 0 );
	XAOP_G(default_namespace) = zend_string_init( ZEND_STRL("app"), 0 );

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
	zend_array_destroy(Z_ARRVAL(XAOP_G(aliases)));
	zend_array_destroy(Z_ARRVAL(XAOP_G(before_aops)));
	zend_array_destroy(Z_ARRVAL(XAOP_G(after_aops)));
	zend_array_destroy(Z_ARRVAL(XAOP_G(around_aops)));
	zend_array_destroy(Z_ARRVAL(XAOP_G(before_func_aops)));
	zend_array_destroy(Z_ARRVAL(XAOP_G(after_func_aops)));
	zend_array_destroy(Z_ARRVAL(XAOP_G(around_func_aops)));

	zend_string_release(XAOP_G(default_module));
	zend_string_release(XAOP_G(default_controller));
	zend_string_release(XAOP_G(default_action));
	zend_string_release(XAOP_G(url_get_str));
	zend_string_release(XAOP_G(view_suffix));
	zend_string_release(XAOP_G(application_dir));
	zend_string_release(XAOP_G(default_namespace));
	zend_array_destroy(Z_ARRVAL(XAOP_G(default_modules)));

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(xaop)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "xaop support", "enabled");
	php_info_print_table_header(2, "xaop author", "Josin <https://www.supjos.cn>");
	php_info_print_table_header(2, "based on ",  "PHP 7.x");
	php_info_print_table_header(2, "LICENSE ", "FreeBSD License");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ xaop_functions[]
 *
 * Every user visible function must have an entry in xaop_functions[].
 */
const zend_function_entry xaop_functions[] = {
	PHP_FE(get_xaop_version,	NULL)		/* For testing, remove later. */
	PHP_FE(get_phpdoc,	NULL)		/* For testing, remove later. */
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
