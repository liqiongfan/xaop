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
#include "kernel/common/helper_lib.h"
#include "kernel/aop/xaop.h"
#include "kernel/aop/exec.h"

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_add_before_aop), 0, 0, 3)
    ZEND_ARG_INFO(0, className)
    ZEND_ARG_INFO(0, functionName)
    ZEND_ARG_INFO(0, pointCallback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_add_after_aop), 0, 0, 3)
    ZEND_ARG_INFO(0, className)
    ZEND_ARG_INFO(0, functionName)
    ZEND_ARG_INFO(0, pointCallback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_add_around_aop), 0, 0, 3)
    ZEND_ARG_INFO(0, className)
    ZEND_ARG_INFO(0, functionName)
    ZEND_ARG_INFO(0, pointCallback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_exec), 0, 0, 0)
ZEND_END_ARG_INFO()

XAOP_METHOD(Xaop, __construct) { }

/**
 * {{{ proto Xaop::addBeforeAop($className, $functionName, $pointCallback)
 * To add the callback calling for the aop before event
 * [
 *     '_a*b' => (zval *)
 * ]
 * or with class & function:
 * [
 *     [ 'class' => 'abc', 'func' => 'ab*', 'closure' => (zval *) ]
 * ]
 */
XAOP_METHOD(Xaop, addBeforeAop)
{
    parse_aop_parameters( INTERNAL_FUNCTION_PARAM_PASSTHRU, &XAOP_G(before_func_aops), &XAOP_G(before_aops) );
}/*}}}*/

/**
 * {{{ proto Xaop::addAfterAop($className, $functionName, $pointCallback)
 * To add the callback calling for the aop after event
 */
XAOP_METHOD(Xaop, addAfterAop)
{
    parse_aop_parameters( INTERNAL_FUNCTION_PARAM_PASSTHRU, &XAOP_G(after_func_aops), &XAOP_G(after_aops) );
}/*}}}*/

#if 0
/**
 * {{{ proto Xaop::addAroundAop($className, $functionName, $pointCallback)
 * Add the around aop feature
 */
XAOP_METHOD(Xaop, addAroundAop)
{
    parse_aop_parameters( INTERNAL_FUNCTION_PARAM_PASSTHRU, &XAOP_G(around_func_aops), &XAOP_G(around_aops) );
}/*}}}*/

/**
 * {{{ proto Xaop::exec()
 */
XAOP_METHOD(Xaop, exec)
{
    if ( NULL != XAOP_G(around_data) ) {
        if ( 1 == XAOP_G(around_type) ) {
            execute_ex(XAOP_G(around_data));
        } else if ( 2 == XAOP_G(around_type) ) {
            execute_internal(XAOP_G(around_data), return_value);
        }
    }
}/*}}}*/
#endif

XAOP_FUNCTIONS(xaop)
    XAOP_ME(Xaop, __construct,  NULL, ZEND_ACC_PRIVATE)
    XAOP_ME(Xaop, addBeforeAop, arginfo_xaop_add_before_aop, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC | ZEND_ACC_FINAL )
    XAOP_ME(Xaop, addAfterAop,  arginfo_xaop_add_after_aop,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC | ZEND_ACC_FINAL )
#if 0
    XAOP_ME(Xaop, addAroundAop, arginfo_xaop_add_around_aop, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC | ZEND_ACC_FINAL )
    XAOP_ME(Xaop, exec, arginfo_xaop_exec, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC | ZEND_ACC_FINAL )
#endif
XAOP_FUNCTIONS_END()


XAOP_INIT(xaop)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Xaop", xaop_functions);
    xaop_ce = zend_register_internal_class(&ce);

    xaop_ce->ce_flags |= ZEND_ACC_FINAL;
}

/**
 * Parse the parameters for the AOP
 * This function make sure the parameters were right & suitable for AOP using.
 * @since 0.1.12
 */
void parse_aop_parameters(INTERNAL_FUNCTION_PARAMETERS, zval *func_aops, zval *aops)
{
    zend_string *function_name;
    zval *class_name, *point_callback;

    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zSz", &class_name, &function_name, &point_callback) == FAILURE ) {
        return ;
    }

    if ( !ZSTR_LEN(function_name) ) {
        php_error_docref(0, E_ERROR, "Parameter wrong.");
        return ;
    }

    if ( Z_TYPE_P(point_callback) == IS_OBJECT && !zend_is_callable(point_callback, IS_CALLABLE_CHECK_NO_ACCESS, NULL) ) {
        php_error_docref(0, E_ERROR, "Parameter 3 must be a valid callback.");
        return ;
    } else if ( Z_TYPE_P(point_callback) == IS_ARRAY && 2 == zend_hash_num_elements(Z_ARRVAL_P(point_callback)) ) {
        zval *object_or_name = zend_hash_index_find(Z_ARRVAL_P(point_callback), 0);
        zval *func_name = zend_hash_index_find(Z_ARRVAL_P(point_callback), 1);

        if ( !func_name || Z_TYPE_P(func_name) != IS_STRING || !Z_STRLEN_P(func_name) ) {
            php_error_docref(0, E_ERROR, "Parameter 3 must be a valid callback.");
            return ;
        }

        if ( Z_TYPE_P(object_or_name) == IS_OBJECT ) {
            if ( XAOP_METHOD_EXISTS(object_or_name, Z_STRVAL_P(func_name)) == NULL ) {
                php_error_docref(0, E_ERROR, "Parameter 3 must be a valid callback.");
                return ;
            }
        } else if ( Z_TYPE_P(object_or_name) == IS_STRING ) {
            zend_class_entry *object_ce  = zend_lookup_class(Z_STR_P(object_or_name));
            zval *method_exists = zend_hash_find_ptr( &object_ce->function_table, zend_string_tolower(Z_STR_P(func_name)) );
            if ( !method_exists ) {
                php_error_docref(0, E_ERROR, "Method not exists in class: `%s`.", ZSTR_VAL(object_ce->name));
                return ;
            }
        }
    } else if ( Z_TYPE_P(point_callback) != IS_OBJECT ) {
        php_error_docref(0, E_ERROR, "Parameter wrong.");
        return ;
    }

    if ( Z_TYPE_P(class_name) == IS_NULL ) {
        add_assoc_zval(func_aops, ZSTR_VAL(function_name), point_callback);
        ZVAL_COPY(return_value, func_aops);
    } else if ( Z_TYPE_P(class_name) == IS_STRING ) {
        zval point;
        array_init(&point);
        
        add_assoc_zval(&point, "class", class_name);
        add_assoc_str(&point, "func" , function_name);
        add_assoc_zval(&point, "closure", point_callback);

        add_next_index_zval(aops, &point);

        ZVAL_COPY(return_value, aops);
    } else {
        php_error_docref(0, E_ERROR, "Parameter wrong.");
        return ; 
    }
}/*}}}*/

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
