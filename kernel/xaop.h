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
#ifndef XAOP_H
#define XAOP_H

extern zend_class_entry *xaop_ce;

#define DCL_PARAMS      zval *class_name, *aop; zend_string *function_name;
#define SAVE_PARAMS_ZPP()     ZEND_PARSE_PARAMETERS_START(3, 3)\
    Z_PARAM_ZVAL(class_name)\
    Z_PARAM_STR(function_name)\
    Z_PARAM_ZVAL(aop)\
    ZEND_PARSE_PARAMETERS_END()

#define SAVE_PARAMS_OLD()  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zSz", &class_name, &function_name, &aop) == FAILURE ) { \
    return ; \
    }

#define CHECK_PARAM() do {\
    if ( Z_TYPE_P(class_name) != IS_STRING && Z_TYPE_P(class_name) != IS_NULL ) { \
        php_error_docref(NULL, E_ERROR, "First argument need to be a valid class name or NULL");\
        return ;\
    }\
    if ( !zend_is_callable(aop, IS_CALLABLE_CHECK_NO_ACCESS, NULL) ) {\
        php_error_docref(NULL, E_ERROR, "Third argument is expected to be a valid callback");\
        return ;\
    }\
} while(0)

#endif //XAOP_H
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
