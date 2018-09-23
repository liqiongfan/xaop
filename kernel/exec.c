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
#include "kernel/exec.h"
#include "kernel/parsing.h"
#include "kernel/helper.h"
#include "ext/standard/php_string.h"
#include "Zend/zend_smart_str.h"
#include "ext/json/php_json.h"
#include "main/SAPI.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_compile.h"
#include "kernel/interface/annotation.h"

/**
 * To execute the zend_op_array to instead of the default one.
 * @param execute_data  current op_array data need to be run.
 */
void xaop_annotation_ex( zend_execute_data *execute_data TSRMLS_DC )
{
    zval          *context = getThis();
    zend_function *func    = execute_data->func;
    
    XAOP_G( aspect )       = 0;
    
    int  output_type  = 0;
    zval charset, func_return, class_annos, function_annos;
    zval *before_zval = NULL, *after_zval = NULL, *success_zval = NULL, *failure_zval = NULL, *disable_zval = NULL,
         *api_zval    = NULL, *deprecated_zval = NULL;
    
    ZVAL_NULL( &class_annos );
    ZVAL_NULL( &function_annos );
    
    IN_ANNOTATION_MODE( context, func ) {
        
        parse_phpdoc( GET_CLASS_DOC( context ), &class_annos );
        parse_phpdoc( GET_FUNCTION_DOC( func ), &function_annos );
        
        ARRAY_MODE( class_annos ) {
            
            zval *annotations = zend_hash_str_find( Z_ARRVAL( class_annos ), ZEND_STRL( ANNOTATIONS_TEXT ) );
            ARRAY_P_MODE( annotations ) {
                
                zval *aspect = zend_hash_str_find( Z_ARRVAL_P( annotations ), ZEND_STRL( ASPECT_TEXT ) );
                
                if ( aspect ) {
                    
                    XAOP_G( aspect ) = 1;
                    
                    ARRAY_MODE( function_annos ) {
                        
                        annotations = zend_hash_str_find( Z_ARRVAL( function_annos ), ZEND_STRL( ANNOTATIONS_TEXT ) );
                        
                        ARRAY_P_MODE( annotations ) {
                            
                            disable_zval = zend_hash_str_find( Z_ARRVAL_P( annotations ), ZEND_STRL( DISABLE_TEXT ) );
                            if ( disable_zval ) return;
                            
                            before_zval = zend_hash_str_find( Z_ARRVAL_P( annotations ), ZEND_STRL( BEFORE_TEXT ) );
                            invoke_kernel_aop_method( before_zval );
                            
                            api_zval = zend_hash_str_find( Z_ARRVAL_P( annotations ), ZEND_STRL( API_TEXT ) );
                            xaop_api_handler( api_zval, &output_type, &charset );
                            
                            after_zval = zend_hash_str_find( Z_ARRVAL_P( annotations ), ZEND_STRL( AFTER_TEXT ) );
                            success_zval    = zend_hash_str_find( Z_ARRVAL_P( annotations ), ZEND_STRL( SUCCESS_TEXT ) );
                            failure_zval    = zend_hash_str_find( Z_ARRVAL_P( annotations ), ZEND_STRL( FAILURE_TEXT ) );
                            
                            deprecated_zval = zend_hash_str_find( Z_ARRVAL_P( annotations ), ZEND_STRL( DEPRECATED_TEXT ) );
                            if ( deprecated_zval ) {
                                XAOP_INFO( E_DEPRECATED, "Function %s%s%s() is deprecated",
                                           func->common.scope ? ZSTR_VAL( func->common.scope->name ) : "",
                                           func->common.scope ? "::" : "", ZSTR_VAL( func->common.function_name ) );
                            }
                            
                            /* for other annotations */
                            zend_string *annotation_name;
                            zval        *annotation_value;
                            ZEND_HASH_FOREACH_STR_KEY_VAL( Z_ARRVAL_P( annotations ), annotation_name, annotation_value ) {
                                if ( zend_string_equals_literal_ci( annotation_name, API_TEXT ) ||
                                     zend_string_equals_literal_ci( annotation_name, DISABLE_TEXT ) ||
                                     zend_string_equals_literal_ci( annotation_name, BEFORE_TEXT ) ||
                                     zend_string_equals_literal_ci( annotation_name, AFTER_TEXT ) ||
                                     zend_string_equals_literal_ci( annotation_name, SUCCESS_TEXT ) ||
                                     zend_string_equals_literal_ci( annotation_name, FAILURE_TEXT ) ||
                                     zend_string_equals_literal_ci( annotation_name, DEPRECATED_TEXT ) ) {
                                    continue;
                                }
                                zend_class_entry
                                    *annotation_entry = zend_lookup_class( zend_string_tolower( annotation_name ) );
                                if ( !annotation_entry ) {
                                    XAOP_INFO( E_ERROR, "Xaop can't found the Annotation class: `%s`.", ZSTR_VAL( annotation_name ) );
                                    XAOP_C_TO( release_memory )
                                }
                                if ( !instanceof_function( annotation_entry, annotation_ce ) ) {
                                    XAOP_INFO( E_ERROR, "Annotation class must implements the " XAOP_PREFIX "Annotations" );
                                    XAOP_C_TO( release_memory )
                                }
                                zval annotation_obj, f_return;
                                xaop_get_object_from_di( &annotation_obj, ZSTR_VAL( annotation_name ), annotation_entry );
                                zend_call_method_with_2_params( &annotation_obj, annotation_entry, NULL, "input", &f_return, context, annotation_value );
                                Z_TRY_DELREF( f_return );
                            } ZEND_HASH_FOREACH_END();
                        } /* function's annotations */
                    } /* function annotations */
                } /* in aspect mode */
            } /* class's annotations */
        } /* class annotations */
    } /* end for annotation mode */
    
    if ( NULL == EX( return_value ) ) {
        EX( return_value ) = &func_return;
    } else {
        ZVAL_NULL( &func_return );
    }
    execute_ex( execute_data );
    if ( EX( return_value ) ) {
        ZVAL_COPY( &func_return, EX( return_value ) );
    }
    
    ARRAY_MODE( func_return ) {
        if ( OUTPUT_JSON == output_type ) {
            smart_str http_body = { 0 };
            php_json_encode( &http_body, &func_return, PHP_JSON_UNESCAPED_UNICODE );
            smart_str_0( &http_body );
            php_write( ZSTR_VAL( http_body.s ), ZSTR_LEN( http_body.s ) );
            smart_str_free( &http_body );
        } else if ( OUTPUT_XML == output_type ) {
            smart_str http_body = { 0 };
            smart_str_appends( &http_body, "<?xml version=\"1.0\" encoding=\"" );
            smart_str_appends( &http_body, Z_STRVAL( charset ) );
            smart_str_appends( &http_body, "\"?>" );
            smart_str_appends( &http_body, "<root>" );
            xaop_xml_data( &func_return, &http_body );
            smart_str_appends( &http_body, "</root>" );
            smart_str_0( &http_body );
            php_write( ZSTR_VAL( http_body.s ), ZSTR_LEN( http_body.s ) );
            smart_str_free( &http_body );
        }
    }
    
    if ( success_zval && ( Z_TYPE( func_return ) == IS_TRUE || Z_TYPE( func_return ) == IS_ARRAY ) ) {
        invoke_kernel_aop_method( success_zval );
    }
    else if ( failure_zval && ( Z_TYPE( func_return ) == IS_FALSE || Z_TYPE( func_return ) == IS_NULL ) ) {
        invoke_kernel_aop_method( failure_zval );
    }
    
    ARRAY_P_MODE( after_zval ) {
        invoke_kernel_aop_method( after_zval );
    }

XAOP_C_LABEL( release_memory )
    
    Z_TRY_DELREF( func_return );
    
    if ( api_zval && IS_ARRAY == Z_TYPE_P(api_zval) && zend_hash_num_elements(Z_ARRVAL_P(api_zval)) && Z_TYPE( charset ) == IS_STRING ) {
        Z_TRY_DELREF( charset );
    }
    
    ARRAY_MODE( class_annos ) {
        zend_array_destroy( Z_ARRVAL( class_annos ) );
    }
    ARRAY_MODE( function_annos ) {
        zend_array_destroy( Z_ARRVAL( function_annos ) );
    }
}

/**
 * To execute the user-defined code
 *
 * Each method injection calling, need set the env. code for overloaded to 1 after running the aop method, reset it to 0
 * @param execute_data      The current execute stack.
 */
void xaop_injection_ex( zend_execute_data *execute_data TSRMLS_DC )
{
    zend_string *funct_name = execute_data->func->common.function_name;
    zend_class_entry *scope  = execute_data->func->common.scope;
    
    if ( 0 == XAOP_G(around_mode) ) {
        INVOKE_METHOD_CONTEXT() {
            if ( funct_name ) {
                /** In function call */
                if ( scope ) {
                    /** Current in class scope */
                    PARSING_SCOPE_AROUND_AOP( 1 );
                    PARSING_SCOPE_AOP( before_aops, 1 );
                IN_CLASS_SCOPE_NOT
                    /** Not in class scope, such as global function */
                    PARSING_SCOPE_AROUND_AOP( 0 );
                    PARSING_SCOPE_AOP( before_aops, 0 );
                IN_CLASS_SCOPE_END
            }
        }
    }
    if ( 0 == XAOP_G(around_mode) ) {
        zval retval;
        ZVAL_NULL(&retval);
        if ( NULL == EX(return_value) ) {
            EX( return_value ) = &retval;
        }
        execute_ex( execute_data );
        if ( EX( return_value ) ) {
            ZVAL_COPY( &retval, EX( return_value ) );
        }
        
        if ( funct_name != NULL ) {
            /** In function call */
            if ( scope != NULL ) {
                /** Current in class scope */
                PARSING_SCOPE_AFTER_AOP( after_aops, 1 );
                if ( IS_NULL != Z_TYPE( retval ) ) { PARSING_SCOPE_AFTER_AOP( after_return_aops, 1 ); }
                if ( EG( exception ) ) {
                    zend_object *exception_object = EG( exception );
                    EG( exception ) = NULL;
                    PARSING_SCOPE_AFTER_AOP( after_throw_aops, 1 );
                    EG( exception )               = exception_object;
                }
            IN_CLASS_SCOPE_NOT
                /** Not in class scope, such as global function */
                PARSING_SCOPE_AFTER_AOP( after_aops, 0 );
                if ( IS_NULL != Z_TYPE( retval ) ) { PARSING_SCOPE_AFTER_AOP( after_return_aops, 0 ); }
                if ( EG( exception ) ) {
                    zend_object *exception_object = EG( exception );
                    EG( exception ) = NULL;
                    PARSING_SCOPE_AFTER_AOP( after_throw_aops, 0 );
                    EG( exception )               = exception_object;
                }
            IN_CLASS_SCOPE_END
        }
    
        Z_TRY_DELREF( retval );
    }
}

/**
 * To execute the zend_execute_data instead of the kernel default
 *
 * Each method injection calling, need set the env. code for overloaded to 1 after running the aop method, reset it to 0
 * @param execute_data      The current execute stack.
 * @param return_value      The return_value
 */
void xaop_injection_internal_ex( zend_execute_data *execute_data TSRMLS_DC, zval *return_value TSRMLS_DC)
{
    zend_string *funct_name = execute_data->func->common.function_name;
    zend_class_entry *scope = execute_data->func->common.scope;
    
    if ( 0 == XAOP_G(around_mode) ) {
        INVOKE_METHOD_CONTEXT() {
            if ( funct_name ) {
                /** In function call */
                if ( scope ) {
                    /** Current in class scope */
                    PARSING_SCOPE_AROUND_AOP( 1 );
                    PARSING_SCOPE_AOP( before_aops, 1 );
                IN_CLASS_SCOPE_NOT
                    /** Not in class scope, such as global function */
                    PARSING_SCOPE_AROUND_AOP( 0 );
                    PARSING_SCOPE_AOP( before_aops, 0 );
                IN_CLASS_SCOPE_END
            }
        }
    }
    if ( 0 == XAOP_G(around_mode) ) {
        execute_internal( execute_data, return_value );
        
        if ( funct_name != NULL ) {
            /** In function call */
            if ( scope != NULL ) {
                /** Current in class scope */
                PARSING_SCOPE_AFTER_AOP( after_aops, 1 );
                if ( return_value && IS_NULL != Z_TYPE_P( return_value ) ) { PARSING_SCOPE_AFTER_AOP( after_return_aops, 1 ); }
                if ( EG( exception ) ) {
                    zend_object *exception_object = EG( exception );
                    EG( exception ) = NULL;
                    PARSING_SCOPE_AFTER_AOP( after_throw_aops, 1 );
                    EG( exception )               = exception_object;
                }
            IN_CLASS_SCOPE_NOT
                /** Not in class scope, such as global function */
                PARSING_SCOPE_AFTER_AOP( after_aops, 0 );
                if ( return_value && IS_NULL != Z_TYPE_P( return_value ) ) { PARSING_SCOPE_AFTER_AOP( after_return_aops, 0 ); }
                if ( EG( exception ) ) {
                    zend_object *exception_object = EG( exception );
                    EG( exception ) = NULL;
                    PARSING_SCOPE_AFTER_AOP( after_throw_aops, 0 );
                    EG( exception )               = exception_object;
                }
            IN_CLASS_SCOPE_END
        }
    }
}

/**
 * This function replace the kernel function's std library function to do the property aop feature
 */
void xaop_property_aop_ex(zval *object, zval *member, zval *value, void **cache_slot)
{
    if ( 2 == XAOP_G(property_aop) ) {
        /** To do the AOP job. */
        PARSING_PROP_AOP(PROPERTY_BEFORE_SET_AOP);
        zend_std_write_property(object, member, value, cache_slot);
        PARSING_PROP_AOP(PROPERTY_AFTER_SET_AOP);
    } else if ( 1 == XAOP_G(property_aop) ) {
        zend_std_write_property(object, member, value, cache_slot);
    }
}/*}}}*/

/** {{{
 *  This function property the aop feature for the property read
 */
zval *xaop_read_property_aop_ex(zval *object, zval *member, int type, void **cache_slot, zval *rv)
{
    if ( 2 == XAOP_G(property_aop) ) {
        /* To do the AOP job. */
        PARSING_PROP_AOP(PROPERTY_BEFORE_READ_AOP);
        zval *result = XAOP_G( std_reader )( object, member, type, cache_slot, rv );
        PARSING_PROP_AOP(PROPERTY_AFTER_READ_AOP);
        return result;
    } else {
        return XAOP_G( std_reader )( object, member, type, cache_slot, rv );
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
