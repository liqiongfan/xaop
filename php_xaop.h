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

#ifndef PHP_XAOP_H
#define PHP_XAOP_H

extern zend_module_entry xaop_module_entry;
#define phpext_xaop_ptr &xaop_module_entry

#define PHP_XAOP_VERSION "0.1.12"

#ifdef PHP_WIN32
#	define PHP_XAOP_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_XAOP_API __attribute__ ((visibility("default")))
#else
#	define PHP_XAOP_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:
*/
ZEND_BEGIN_MODULE_GLOBALS(xaop)
    zend_bool enable_aop;
    zval di;
    zval aliases;
    zval pdo_object;

    zval before_func_aops;            /* All aops without class env. */
    zval after_func_aops;             /* All aops without class env. */
    zval around_func_aops;            /* All aops without class env. */
    zval before_aops;                 /* All injected aop functions */
    zval after_aops;                  /* All injected aop functions */
    zval around_aops;                 /* All injected aop functions */
    zend_execute_data *around_data;   /* The current calling function opcodes for running. */
    int around_type;                  /* around type */
    int aop_mode;                     /* aop mode for use */

    zend_string *default_module;      /* index */
    zend_string *default_controller;  /* index */
    zend_string *default_action;      /* index */
    zval default_modules;             /* default [ 'index' ] */
    int url_mode;                     /* 0: GET 1: PATH 2:AUTO */
    zend_string *url_get_str;         /* default: _url */
    zend_string *view_suffix;         /* default html */
    zend_string *url_suffix;          /* default html */
    zend_string *application_dir;     /* default `.` */
    zend_string *default_namespace;   /* default `app` */
ZEND_END_MODULE_GLOBALS(xaop)

/* Always refer to the globals in your function as XAOP_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define XAOP_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(xaop, v)

#if defined(ZTS) && defined(COMPILE_DL_XAOP)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

/* Every XAOP_G calling need this macro. */
extern ZEND_DECLARE_MODULE_GLOBALS(xaop);

/* Check whether the method is exist in object or not. the method_name was `char *` */
#define XAOP_METHOD_EXISTS(object, method_name) ( ( Z_OBJ_HT_P(object)->get_method(&Z_OBJ_P(object), strpprintf(0, "%s", ZEND_STRL(method_name)), NULL)) )
#define XAOP_C_LABEL(name)         name :
#define XAOP_C_TO(name)            goto name;  
#define XAOP_ME                    PHP_ME
#define XAOP_AB_ME                 PHP_ABSTRACT_ME
#define XAOP_METHOD                PHP_METHOD
#define XAOP_INIT(name)            void name##_init()
#define ARG_INFO(name)             arginfo_##name
#define XAOP_MALIAS                PHP_MALIAS
#define XAOP_FUNCTIONS(name)       static const zend_function_entry name##_functions[] = {
#define XAOP_FUNCTIONS_END()       PHP_FE_END };
#define XAOP_PREFIX                "Xaop\\"
#define XAOP_INFO(type, info...)   php_error_docref(NULL, type, ##info)
#define XAOP_ENTRY_OBJ(obj)        (Z_OBJCE_P(obj)),(obj)

#endif	/* PHP_XAOP_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
