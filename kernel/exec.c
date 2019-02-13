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

/* guard flags */
#define IN_GET      (1<<0)
#define IN_SET      (1<<1)
#define IN_UNSET    (1<<2)
#define IN_ISSET    (1<<3)

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
    
static void zend_std_call_getter(zval *object, zval *member, zval *retval) /* {{{ */
{
    zend_class_entry *ce = Z_OBJCE_P(object);
    zend_class_entry *orig_fake_scope = EG(fake_scope);

    EG(fake_scope) = NULL;

    /* __get handler is called with one argument:
          property name

       it should return whether the call was successful or not
    */
    zend_call_method_with_1_params(object, ce, &ce->__get, ZEND_GET_FUNC_NAME, retval, member);

    EG(fake_scope) = orig_fake_scope;
}
/* }}} */

static void zend_std_call_issetter(zval *object, zval *member, zval *retval) /* {{{ */
{
    zend_class_entry *ce = Z_OBJCE_P(object);
    zend_class_entry *orig_fake_scope = EG(fake_scope);

    EG(fake_scope) = NULL;

    /* __isset handler is called with one argument:
          property name

       it should return whether the property is set or not
    */

    if (Z_REFCOUNTED_P(member)) Z_ADDREF_P(member);

    zend_call_method_with_1_params(object, ce, &ce->__isset, ZEND_ISSET_FUNC_NAME, retval, member);

    zval_ptr_dtor(member);

    EG(fake_scope) = orig_fake_scope;
}
/* }}} */

static int zend_verify_property_access(zend_property_info *property_info, zend_class_entry *ce) /* {{{ */
{
    zend_class_entry *scope;

    if (property_info->flags & ZEND_ACC_PUBLIC) {
        return 1;
    } else if (property_info->flags & ZEND_ACC_PRIVATE) {
        if (EG(fake_scope)) {
            scope = EG(fake_scope);
        } else {
            scope = zend_get_executed_scope();
        }
        return (ce == scope || property_info->ce == scope);
    } else if (property_info->flags & ZEND_ACC_PROTECTED) {
        if (EG(fake_scope)) {
            scope = EG(fake_scope);
        } else {
            scope = zend_get_executed_scope();
        }
        return zend_check_protected(property_info->ce, scope);
    }
    return 0;
}
/* }}} */

static zend_bool is_derived_class(zend_class_entry *child_class, zend_class_entry *parent_class) /* {{{ */
{
    child_class = child_class->parent;
    while (child_class) {
        if (child_class == parent_class) {
            return 1;
        }
        child_class = child_class->parent;
    }

    return 0;
}
/* }}} */

static uint32_t zend_get_property_offset(zend_class_entry *ce, zend_string *member, int silent, void **cache_slot) /* {{{ */
{
    zval *zv;
    zend_property_info *property_info = NULL;
    uint32_t flags;
    zend_class_entry *scope;

    if (cache_slot && EXPECTED(ce == CACHED_PTR_EX(cache_slot))) {
        return (uint32_t)(intptr_t)CACHED_PTR_EX(cache_slot + 1);
    }

    if (UNEXPECTED(ZSTR_VAL(member)[0] == '\0' && ZSTR_LEN(member) != 0)) {
        if (!silent) {
            zend_throw_error(NULL, "Cannot access property started with '\\0'");
        }
        return ZEND_WRONG_PROPERTY_OFFSET;
    }

    if (UNEXPECTED(zend_hash_num_elements(&ce->properties_info) == 0)) {
        goto exit_dynamic;
    }

    zv = zend_hash_find(&ce->properties_info, member);
    if (EXPECTED(zv != NULL)) {
        property_info = (zend_property_info*)Z_PTR_P(zv);
        flags = property_info->flags;
        if (UNEXPECTED((flags & ZEND_ACC_SHADOW) != 0)) {
            /* if it's a shadow - go to access it's private */
            property_info = NULL;
        } else {
            if (EXPECTED(zend_verify_property_access(property_info, ce) != 0)) {
                if (UNEXPECTED(!(flags & ZEND_ACC_CHANGED))
                    || UNEXPECTED((flags & ZEND_ACC_PRIVATE))) {
                    if (UNEXPECTED((flags & ZEND_ACC_STATIC) != 0)) {
                        if (!silent) {
                            zend_error(E_NOTICE, "Accessing static property %s::$%s as non static", ZSTR_VAL(ce->name), ZSTR_VAL(member));
                        }
                        return ZEND_DYNAMIC_PROPERTY_OFFSET;
                    }
                    goto exit;
                }
            } else {
                /* Try to look in the scope instead */
                property_info = ZEND_WRONG_PROPERTY_INFO;
            }
        }
    }

    if (EG(fake_scope)) {
        scope = EG(fake_scope);
    } else {
        scope = zend_get_executed_scope();
    }

    if (scope != ce
        && scope
        && is_derived_class(ce, scope)
        && (zv = zend_hash_find(&scope->properties_info, member)) != NULL
        && ((zend_property_info*)Z_PTR_P(zv))->flags & ZEND_ACC_PRIVATE) {
        property_info = (zend_property_info*)Z_PTR_P(zv);
        if (UNEXPECTED((property_info->flags & ZEND_ACC_STATIC) != 0)) {
            return ZEND_DYNAMIC_PROPERTY_OFFSET;
        }
    } else if (UNEXPECTED(property_info == NULL)) {
exit_dynamic:
        if (cache_slot) {
            CACHE_POLYMORPHIC_PTR_EX(cache_slot, ce, (void*)(intptr_t)ZEND_DYNAMIC_PROPERTY_OFFSET);
        }
        return ZEND_DYNAMIC_PROPERTY_OFFSET;
    } else if (UNEXPECTED(property_info == ZEND_WRONG_PROPERTY_INFO)) {
        /* Information was available, but we were denied access.  Error out. */
        if (!silent) {
            zend_throw_error(NULL, "Cannot access %s property %s::$%s", zend_visibility_string(flags), ZSTR_VAL(ce->name), ZSTR_VAL(member));
        }
        return ZEND_WRONG_PROPERTY_OFFSET;
    }

exit:
    if (cache_slot) {
        CACHE_POLYMORPHIC_PTR_EX(cache_slot, ce, (void*)(intptr_t)property_info->offset);
    }
    return property_info->offset;
}
/* }}} */

zval *xaop_std_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv) /* {{{ */
{
    zend_object *zobj;
    zval tmp_member, tmp_object;
    zval *retval;
    uint32_t property_offset;
    uint32_t *guard = NULL;

    zobj = Z_OBJ_P(object);

    ZVAL_UNDEF(&tmp_member);
    if (UNEXPECTED(Z_TYPE_P(member) != IS_STRING)) {
        ZVAL_STR(&tmp_member, zval_get_string(member));
        member = &tmp_member;
        cache_slot = NULL;
    }

#if DEBUG_OBJECT_HANDLERS
    fprintf(stderr, "Read object #%d property: %s\n", Z_OBJ_HANDLE_P(object), Z_STRVAL_P(member));
#endif

    /* make zend_get_property_info silent if we have getter - we may want to use it */
    property_offset = zend_get_property_offset(zobj->ce, Z_STR_P(member), (type == BP_VAR_IS) || (zobj->ce->__get != NULL), cache_slot);

    if (EXPECTED(property_offset != ZEND_WRONG_PROPERTY_OFFSET)) {
        if (EXPECTED(property_offset != ZEND_DYNAMIC_PROPERTY_OFFSET)) {
            retval = OBJ_PROP(zobj, property_offset);
            if (EXPECTED(Z_TYPE_P(retval) != IS_UNDEF)) {
                goto exit;
            }
        } else if (EXPECTED(zobj->properties != NULL)) {
            retval = zend_hash_find(zobj->properties, Z_STR_P(member));
            if (EXPECTED(retval)) goto exit;
        }
    } else if (UNEXPECTED(EG(exception))) {
        retval = &EG(uninitialized_zval);
        goto exit;
    }

    ZVAL_UNDEF(&tmp_object);

    /* magic isset */
    if ((type == BP_VAR_IS) && zobj->ce->__isset) {
        zval tmp_result;
        guard = zend_get_property_guard(zobj, Z_STR_P(member));

        if (!((*guard) & IN_ISSET)) {
            if (Z_TYPE(tmp_member) == IS_UNDEF) {
                ZVAL_COPY(&tmp_member, member);
                member = &tmp_member;
            }
            ZVAL_COPY(&tmp_object, object);
            ZVAL_UNDEF(&tmp_result);

            *guard |= IN_ISSET;
            zend_std_call_issetter(&tmp_object, member, &tmp_result);
            *guard &= ~IN_ISSET;

            if (!zend_is_true(&tmp_result)) {
                retval = &EG(uninitialized_zval);
                zval_ptr_dtor(&tmp_object);
                zval_ptr_dtor(&tmp_result);
                goto exit;
            }

            zval_ptr_dtor(&tmp_result);
        }
    }

    /* magic get */
    if (zobj->ce->__get) {
        if (guard == NULL) {
            guard = zend_get_property_guard(zobj, Z_STR_P(member));
        }
        if (!((*guard) & IN_GET)) {
            /* have getter - try with it! */
            if (Z_TYPE(tmp_object) == IS_UNDEF) {
                ZVAL_COPY(&tmp_object, object);
            }
            *guard |= IN_GET; /* prevent circular getting */
            zend_std_call_getter(&tmp_object, member, rv);
            *guard &= ~IN_GET;

            if (Z_TYPE_P(rv) != IS_UNDEF) {
                retval = rv;
                if (!Z_ISREF_P(rv) &&
                    (type == BP_VAR_W || type == BP_VAR_RW  || type == BP_VAR_UNSET)) {
                    SEPARATE_ZVAL(rv);
                    if (UNEXPECTED(Z_TYPE_P(rv) != IS_OBJECT)) {
                        zend_error(E_NOTICE, "Indirect modification of overloaded property %s::$%s has no effect", ZSTR_VAL(zobj->ce->name), Z_STRVAL_P(member));
                    }
                }
            } else {
                retval = &EG(uninitialized_zval);
            }
            zval_ptr_dtor(&tmp_object);
            goto exit;
        } else if (Z_STRVAL_P(member)[0] == '\0' && Z_STRLEN_P(member) != 0) {
            zval_ptr_dtor(&tmp_object);
            zend_throw_error(NULL, "Cannot access property started with '\\0'");
            retval = &EG(uninitialized_zval);
            goto exit;
        }
    }

    zval_ptr_dtor(&tmp_object);

    if ((type != BP_VAR_IS)) {
        zend_error(E_NOTICE,"Undefined property: %s::$%s", ZSTR_VAL(zobj->ce->name), Z_STRVAL_P(member));
    }
    retval = &EG(uninitialized_zval);

exit:
    if (UNEXPECTED(Z_REFCOUNTED(tmp_member))) {
        zval_ptr_dtor(&tmp_member);
    }

    return retval;
}
/* }}} */

/** {{{
 *  This function property the aop feature for the property read
 */
zval *xaop_read_property_aop_ex(zval *object, zval *member, int type, void **cache_slot, zval *rv)
{
    if ( 2 == XAOP_G(property_aop) ) {
        /* To do the AOP job. */
        PARSING_PROP_AOP(PROPERTY_BEFORE_READ_AOP);
        zval *result = xaop_std_read_property( object, member, type, cache_slot, rv );
        PARSING_PROP_AOP(PROPERTY_AFTER_READ_AOP);
        return result;
    } else {
        return xaop_std_read_property( object, member, type, cache_slot, rv );
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
