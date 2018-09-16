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
#include "kernel/annotations/annotation.h"
#include "kernel/parsing.h"

/**
 * {{{ ARG_INFO for class Xaop\Doc
 */
ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(annotation_get_class_doc), 0, 0, 1)
    ZEND_ARG_INFO(0, className)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(annotation_get_function_doc), 0, 0, 2)
    ZEND_ARG_INFO(0, className)
    ZEND_ARG_INFO(0, functionName)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(annotation_get_class_const_doc), 0, 0, 2)
    ZEND_ARG_INFO(0, className)
    ZEND_ARG_INFO(0, constName)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(annotation_get_class_attribute_doc), 0, 0, 2)
    ZEND_ARG_INFO(0, className)
    ZEND_ARG_INFO(0, attributeName)
ZEND_END_ARG_INFO()
/*}}}*/

/**
 * {{{ proto Doc::getClassDoc($className)
 * to obtain the parse result of the class document
 */
XAOP_METHOD(Doc, getClassDoc)
{
    zval             *val;
    zend_string      *doc;
    zend_class_entry *ce;
    if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "z", &val ) == FAILURE ) {
        return;
    }
    switch ( Z_TYPE_P( val ) ) {
        case IS_STRING:
            ce = zend_lookup_class( zend_string_tolower( Z_STR_P( val ) ) );
            if ( !ce ) {
                XAOP_INFO(E_ERROR, "Class `%s` not exists.", ZSTR_VAL(ce->name));
                return ;
            }
            break;
        case IS_OBJECT:
            ce = Z_OBJCE_P( val );
            break;
        default:
            XAOP_INFO( E_ERROR, "Parameter type wrong." );
            return;
    }
    
    doc = ce->info.user.doc_comment;
    parse_phpdoc( doc, return_value );
} /*}}}*/

/**
 * {{{ proto Doc::getFunctionDoc($className, $functionName)
 * to obtain the function doc. result with the given class & function name
 */
XAOP_METHOD(Doc, getFunctionDoc)
{
    zval             *val;
    zend_string      *doc, *function_name;
    zend_class_entry *ce;
    
    if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "zS", &val, &function_name ) == FAILURE ) {
        return;
    }
    switch ( Z_TYPE_P( val ) ) {
        case IS_STRING:
            ce = zend_lookup_class( zend_string_tolower( Z_STR_P( val ) ) );
            if ( !ce ) {
                XAOP_INFO(E_ERROR, "Class `%s` not exists.", ZSTR_VAL(ce->name));
                return ;
            }
            break;
        case IS_OBJECT:
            ce = Z_OBJCE_P( val );
            break;
        default:
            XAOP_INFO( E_ERROR, "Parameter type wrong." );
            return;
    }
    
    zend_function *func = zend_hash_find_ptr( &ce->function_table, zend_string_tolower( function_name ) );
    if ( !func ) {
        XAOP_INFO( E_ERROR, "function: `%s` not exist in class: `%s`", ZSTR_VAL( function_name ), ZSTR_VAL( ce->name ) );
        return;
    }
    if ( func->type == ZEND_INTERNAL_FUNCTION ) {
        RETURN_NULL();
    }
    
    doc = func->op_array.doc_comment;
    parse_phpdoc( doc, return_value );
}/*}}}*/

/**
 * {{{ proto Doc::getClassConstDoc($className, $constName)
 * to obtain the const doc for the given class & const name
 */
XAOP_METHOD(Doc, getClassConstDoc)
{
    zval             *val;
    zend_string      *const_name, *doc;
    zend_class_entry *ce;
    
    if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "zS", &val, &const_name ) == FAILURE ) {
        return;
    }
    
    switch ( Z_TYPE_P( val ) ) {
        case IS_STRING:
            ce = zend_lookup_class( zend_string_tolower( Z_STR_P( val ) ) );
            if ( !ce ) {
                XAOP_INFO( E_ERROR, "Class `%s` not exists.", ZSTR_VAL( ce->name ) );
                return;
            }
            break;
        case IS_OBJECT:
            ce = Z_OBJCE_P( val );
            break;
        default:
            XAOP_INFO( E_ERROR, "Parameter type wrong." );
            return;
    }
    
    zend_class_constant *constant = zend_hash_find_ptr( &ce->constants_table, const_name );
    if ( !constant ) {
        XAOP_INFO( E_ERROR, "Class constant `%s` not exists.", ZSTR_VAL( const_name ) );
        return;
    }
    parse_phpdoc( constant->doc_comment, return_value );
}/*}}}*/

/**
 * {{{ proto Doc::getAttributeDoc($className, $attributeName)
 * to obtain the attribute doc result.
 */
XAOP_METHOD(Doc, getAttributeDoc)
{
    zval             *val;
    zend_string      *doc, *attribute_name;
    zend_class_entry *ce;
    
    if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "zS", &val, &attribute_name ) == FAILURE ) {
        return;
    }
    switch ( Z_TYPE_P( val ) ) {
        case IS_STRING:
            ce = zend_lookup_class( zend_string_tolower( Z_STR_P( val ) ) );
            if ( !ce ) {
                XAOP_INFO( E_ERROR, "Class `%s` not exists.", ZSTR_VAL( ce->name ) );
                return;
            }
            break;
        case IS_OBJECT:
            ce = Z_OBJCE_P( val );
            break;
        default:
            XAOP_INFO( E_ERROR, "Parameter type wrong." );
            return;
    }
    
    zend_property_info *property_info = zend_hash_find_ptr( &ce->properties_info, zend_string_tolower( attribute_name ) );
    
    if ( !property_info ) {
        XAOP_INFO( E_ERROR, "Class property `%s` not exists.", ZSTR_VAL( attribute_name ) );
        return;
    }
    
    parse_phpdoc( property_info->doc_comment, return_value );
}/*}}}*/

XAOP_FUNCTIONS(doc)
    XAOP_ME(Doc, getClassDoc, arginfo_annotation_get_class_doc, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL )
    XAOP_ME(Doc, getFunctionDoc, arginfo_annotation_get_function_doc, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL )
    XAOP_ME(Doc, getClassConstDoc, arginfo_annotation_get_class_const_doc, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL )
    XAOP_ME(Doc, getAttributeDoc, arginfo_annotation_get_class_attribute_doc, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL )
XAOP_FUNCTIONS_END()

XAOP_INIT(doc)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, XAOP_PREFIX "Doc", doc_functions);
    doc_ce = zend_register_internal_class(&ce);
    doc_ce->ce_flags |= ZEND_ACC_FINAL;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
