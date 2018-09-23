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
#include "kernel/xaop.h"

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_add_before_aop), 0, 0, 3)
    ZEND_ARG_INFO(0, className)
    ZEND_ARG_INFO(0, functionName)
    ZEND_ARG_INFO(0, aop)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_add_after_aop), 0, 0, 3)
    ZEND_ARG_INFO(0, className)
    ZEND_ARG_INFO(0, functionName)
    ZEND_ARG_INFO(0, aop)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_add_around_aop), 0, 0, 3)
    ZEND_ARG_INFO(0, className)
    ZEND_ARG_INFO(0, functionName)
    ZEND_ARG_INFO(0, aop)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_add_after_return_aop), 0, 0, 3)
    ZEND_ARG_INFO(0, className)
    ZEND_ARG_INFO(0, functionName)
    ZEND_ARG_INFO(0, aop)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_add_after_throw_aop), 0, 0, 3)
    ZEND_ARG_INFO(0, className)
    ZEND_ARG_INFO(0, functionName)
    ZEND_ARG_INFO(0, aop)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_add_property_before_read_aop), 0, 0, 3)
   ZEND_ARG_INFO(0, className)
   ZEND_ARG_INFO(0, propertyName)
   ZEND_ARG_INFO(0, aop)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_add_property_before_set_aop), 0, 0, 3)
   ZEND_ARG_INFO(0, className)
   ZEND_ARG_INFO(0, propertyName)
   ZEND_ARG_INFO(0, aop)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_add_property_after_set_aop), 0, 0, 3)
   ZEND_ARG_INFO(0, className)
   ZEND_ARG_INFO(0, propertyName)
   ZEND_ARG_INFO(0, aop)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_add_property_after_read_aop), 0, 0, 3)
   ZEND_ARG_INFO(0, className)
   ZEND_ARG_INFO(0, propertyName)
   ZEND_ARG_INFO(0, aop)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_exec), 0, 0, 1)
   ZEND_ARG_INFO(0, xaopExec)
ZEND_END_ARG_INFO()

/* Common parse parameters for aop */

/**
 * {{{ proto Xaop::addBeforeAop($className, $functionName, $aop)
 */
XAOP_METHOD(Xaop, addBeforeAop)
{
    DCL_PARAMS
    
#ifdef FAST_ZPP
    SAVE_PARAMS_ZPP();
#else
    SAVE_PARAMS_OLD();
#endif
    CHECK_PARAM();
    
    zval retval;
    array_init(&retval);

    /**
     * [ class = 'Tag', func = 'getList', aop = (zval *) ]
     */
    add_assoc_zval(&retval, "class", class_name);
    add_assoc_str(&retval, "func", function_name);
    Z_TRY_ADDREF_P(aop);
    add_assoc_zval(&retval, "aop", aop);
    
    add_next_index_zval(&XAOP_G(before_aops), &retval);
}/*}}}*/

/**
 * {{{ proto Xaop::addAfterAop($className, $functionName, $aop)
 */
XAOP_METHOD(Xaop, addAfterAop)
{
    DCL_PARAMS

#ifdef FAST_ZPP
    SAVE_PARAMS_ZPP();
#else
    SAVE_PARAMS_OLD();
#endif
    CHECK_PARAM();
    
    zval retval;
    array_init(&retval);
    
    /**
     * [ class = 'Tag', func = 'getList', aop = (zval *) ]
     */
    add_assoc_zval(&retval, "class", class_name);
    add_assoc_str(&retval, "func", function_name);
    Z_TRY_ADDREF_P(aop);
    add_assoc_zval(&retval, "aop", aop);
    
    add_next_index_zval(&XAOP_G(after_aops), &retval);
}/*}}}*/

/**
 * {{{ proto Xaop::addAfterReturnAop($className, $functionName, $aop)
 */
XAOP_METHOD(Xaop, addAfterReturnAop)
{
    DCL_PARAMS

#ifdef FAST_ZPP
    SAVE_PARAMS_ZPP();
#else
    SAVE_PARAMS_OLD();
#endif
    CHECK_PARAM();
    
    zval retval;
    array_init(&retval);
    
    /**
     * [ class = 'Tag', func = 'getList', aop = (zval *) ]
     */
    add_assoc_zval(&retval, "class", class_name);
    add_assoc_str(&retval, "func", function_name);
    Z_TRY_ADDREF_P(aop);
    add_assoc_zval(&retval, "aop", aop);
    
    add_next_index_zval(&XAOP_G(after_return_aops), &retval);
}/*}}}*/

/**
 * {{{ proto Xaop::addAfterThrowAop($className, $functionName, $aop)
 */
XAOP_METHOD(Xaop, addAfterThrowAop)
{
    DCL_PARAMS

#ifdef FAST_ZPP
    SAVE_PARAMS_ZPP();
#else
    SAVE_PARAMS_OLD();
#endif
    CHECK_PARAM();
    
    zval retval;
    array_init(&retval);
    
    /**
     * [ class = 'Tag', func = 'getList', aop = (zval *) ]
     */
    add_assoc_zval(&retval, "class", class_name);
    add_assoc_str(&retval, "func", function_name);
    Z_TRY_ADDREF_P(aop);
    add_assoc_zval(&retval, "aop", aop);
    
    add_next_index_zval(&XAOP_G(after_throw_aops), &retval);
}/*}}}*/

/**
 * {{{ proto Xaop::addAroundAop($className, $functionName, $aop)
 */
XAOP_METHOD(Xaop, addAroundAop)
{
    DCL_PARAMS

#ifdef FAST_ZPP
    SAVE_PARAMS_ZPP();
#else
    SAVE_PARAMS_OLD();
#endif
    CHECK_PARAM();
    
    zval retval;
    array_init(&retval);
    
    /**
     * [ class = 'Tag', func = 'getList', aop = (zval *) ]
     */
    add_assoc_zval(&retval, "class", class_name);
    add_assoc_str(&retval, "func", function_name);
    Z_TRY_ADDREF_P(aop);
    add_assoc_zval(&retval, "aop", aop);
    
    add_next_index_zval(&XAOP_G(around_aops), &retval);
}/*}}}*/

/**
 * {{{ proto Xaop::addPropertyBeforeAop($className, $propertyName, $aop)
 * [ 'class' => 'xx', 'prop' => 'xx', 'aop' => (zval *), 'type' => xx ]
 */
XAOP_METHOD(Xaop, addPropertyBeforeReadAop)
{
    DCL_PROPERTY_PARAMS
    SAVE_PROPERTY_PARAMS_ZPP();
    CHECK_PROPERTY_PARAMS();
    
    zval retval;
    array_init(&retval);
    add_assoc_str(&retval, "class", class_name );
    add_assoc_str(&retval, "prop", property_name );
    Z_TRY_ADDREF_P(aop);
    add_assoc_zval(&retval, "aop", aop );
    add_assoc_long(&retval, "type", PROPERTY_BEFORE_READ_AOP );
    
    add_next_index_zval(&XAOP_G(property_aops), &retval );
}/*}}}*/

/**
 * {{{ proto Xaop::addPropertyBeforeAop($className, $propertyName, $aop)
 * [ 'class' => 'xx', 'prop' => 'xx', 'aop' => (zval *), 'type' => xx ]
 */
XAOP_METHOD(Xaop, addPropertyBeforeSetAop)
{
    DCL_PROPERTY_PARAMS
    SAVE_PROPERTY_PARAMS_ZPP();
    CHECK_PROPERTY_PARAMS();
    
    zval retval;
    array_init(&retval);
    add_assoc_str(&retval, "class", class_name );
    add_assoc_str(&retval, "prop", property_name );
    Z_TRY_ADDREF_P(aop);
    add_assoc_zval(&retval, "aop", aop );
    add_assoc_long(&retval, "type", PROPERTY_BEFORE_SET_AOP );
    
    add_next_index_zval(&XAOP_G(property_aops), &retval );
}/*}}}*/

/**
 * {{{ proto Xaop::addPropertyAfterReadAop($className, $propertyName, $aop)
 */
XAOP_METHOD(Xaop, addPropertyAfterReadAop)
{
    DCL_PROPERTY_PARAMS
    SAVE_PROPERTY_PARAMS_ZPP();
    CHECK_PROPERTY_PARAMS();
    
    zval retval;
    array_init(&retval);
    add_assoc_str(&retval, "class", class_name );
    add_assoc_str(&retval, "prop", property_name );
    Z_TRY_ADDREF_P(aop);
    add_assoc_zval(&retval, "aop", aop );
    add_assoc_long(&retval, "type", PROPERTY_AFTER_READ_AOP );
    
    add_next_index_zval(&XAOP_G(property_aops), &retval );
}/*}}}*/

/**
 * {{{ proto Xaop::addPropertyAfterSetAop($className, $propertyName, $aop)
 */
XAOP_METHOD(Xaop, addPropertyAfterSetAop)
{
    DCL_PROPERTY_PARAMS
    SAVE_PROPERTY_PARAMS_ZPP();
    CHECK_PROPERTY_PARAMS();
    
    zval retval;
    array_init(&retval);
    add_assoc_str(&retval, "class", class_name );
    add_assoc_str(&retval, "prop", property_name );
    Z_TRY_ADDREF_P(aop);
    add_assoc_zval(&retval, "aop", aop );
    add_assoc_long(&retval, "type", PROPERTY_AFTER_SET_AOP );
    
    add_next_index_zval(&XAOP_G(property_aops), &retval );
}/*}}}*/

/**{{{ proto Xaop::exec($xaopExecResource)
 * */
XAOP_METHOD(Xaop, exec)
{
    zval *xaop_exec;
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &xaop_exec) == FAILURE ) {
        return ;
    }
    
    zend_execute_data *exec = (zend_execute_data *)zend_fetch_resource( Z_RES_P(xaop_exec), "XaopExec", le_xaop);
    if ( exec ) {
        if ( exec->func->type == ZEND_USER_FUNCTION ) {
            exec->return_value = return_value;
            execute_ex( exec );
        } else if ( exec->func->type == ZEND_INTERNAL_FUNCTION ) {
            execute_internal(exec, return_value);
        }
    }
}/*}}}*/

/**
 * {{{ All functions for Xaop class
 */
XAOP_FUNCTIONS(xaop)
    XAOP_ME(Xaop, addBeforeAop, arginfo_xaop_add_before_aop, XAOP_FUNCTION_DEF )
    XAOP_ME(Xaop, addAfterAop, arginfo_xaop_add_after_aop, XAOP_FUNCTION_DEF )
    XAOP_ME(Xaop, addAroundAop, arginfo_xaop_add_around_aop, XAOP_FUNCTION_DEF )
    XAOP_ME(Xaop, addAfterReturnAop, arginfo_xaop_add_after_return_aop, XAOP_FUNCTION_DEF )
    XAOP_ME(Xaop, addAfterThrowAop, arginfo_xaop_add_after_throw_aop, XAOP_FUNCTION_DEF )
    
    /* Below are some property aop join-point */
    XAOP_ME(Xaop, addPropertyBeforeReadAop, arginfo_xaop_add_property_before_read_aop, XAOP_FUNCTION_DEF )
    XAOP_ME(Xaop, addPropertyBeforeSetAop, arginfo_xaop_add_property_before_set_aop, XAOP_FUNCTION_DEF )
    XAOP_ME(Xaop, addPropertyAfterReadAop, arginfo_xaop_add_property_after_read_aop, XAOP_FUNCTION_DEF )
    XAOP_ME(Xaop, addPropertyAfterSetAop, arginfo_xaop_add_property_after_set_aop, XAOP_FUNCTION_DEF )
    
    XAOP_ME(Xaop, exec, arginfo_xaop_exec, XAOP_FUNCTION_DEF )
XAOP_FUNCTIONS_END()
/*}}}*/

/**
 * {{{ proto xaop_init()
 */
XAOP_INIT(xaop)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Xaop", xaop_functions);
    xaop_ce = zend_register_internal_class(&ce);
    xaop_ce->ce_flags |= ZEND_ACC_FINAL;
}/*}}}*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
