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

#define PHP_XAOP_VERSION "0.9.9"

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

/**
 * Some macros for the kernel use.
 */
#define NORMAL_AOP 1
#define ANNOTATION_AOP 2
#define INJECTION_AOP  3

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:
*/
ZEND_BEGIN_MODULE_GLOBALS(xaop)
    zval di;                    /* Di container */
    int  aspect;                /* Aspect mode or not, 1 for aspect 0 for not. */
    int  overloaded;            /* overload mode or not 1: overload 0: normal */
    int  aop_mode;              /* The AOP mode for kernel use. 1: normal 2:annotation aop 3: method aop */
    char *method_prefix;        /* The AOP method start with will be excluded */
    
    zval before_aops;           /* The before aop */
    zval after_aops;            /* The after aop */
    zval after_return_aops;     /* The after_return aop */
    zval after_throw_aops;      /* The after_throw aop */
    zval around_aops;           /* The around_aop */
    int  around_mode;           /* In around aop mode */
ZEND_END_MODULE_GLOBALS(xaop)

/* Always refer to the globals in your function as XAOP_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define XAOP_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(xaop, v)

#if defined(ZTS) && defined(COMPILE_DL_XAOP)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

ZEND_EXTERN_MODULE_GLOBALS(xaop)

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
#define XAOP_INFO(type, info...)   php_error_docref(0, type, ##info)
#define XAOP_ENTRY_OBJ(obj)        (Z_OBJCE_P(obj)),(obj)

extern int le_xaop;

XAOP_INIT(annotation);
XAOP_INIT(doc);
XAOP_INIT(xaop);

#endif	/* PHP_XAOP_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
