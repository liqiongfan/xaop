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
#ifndef EXEC_H
#define EXEC_H

#define GET_CLASS_DOC(object) Z_OBJCE_P(object)->info.user.doc_comment
#define GET_FUNCTION_DOC(func) (func)->op_array.doc_comment
#define IN_ANNOTATION_MODE(context, func) if ( (context) && (GET_CLASS_DOC(context)) && (func) && GET_FUNCTION_DOC(func) && \
( XAOP_G(method_prefix)[0] == '\0' || strncmp( ZSTR_VAL((func)->op_array.function_name), XAOP_G(method_prefix), strlen(XAOP_G(method_prefix))\
)) )
#define INVOKE_METHOD_CONTEXT() if ( 0 == XAOP_G( overloaded ) )
#define INVOKE_METHOD_CONTEXT_AFTER() if ( 1 == XAOP_G( overloaded ) )
#define INIT_CONTEXT_FOR_CALLING(v) XAOP_G( overloaded ) = v;
#define INIT_CONTEXT_FOR_CALLING_END(v) XAOP_G( overloaded ) = v;
#define ARRAY_MODE(arr) if ( Z_TYPE(arr) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL(arr)) )
#define ARRAY_P_MODE(arr) if ( arr && Z_TYPE_P(arr) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(arr)) )
#define ANNOTATIONS_TEXT "annotations"
#define ASPECT_TEXT  "Aspect"
#define API_TEXT     "api"
#define DISABLE_TEXT "disable"
#define BEFORE_TEXT  "before"
#define AFTER_TEXT   "after"
#define SUCCESS_TEXT "success"
#define FAILURE_TEXT "failure"
#define DEPRECATED_TEXT "deprecated"
#define OUTPUT_JSON  1
#define OUTPUT_XML   2
#define DCL_EXECUTE_PARAMS       zend_function *func = execute_data->func;\
        zend_class_entry *scope = func->common.scope;
#define IN_FUNCTION_CALL( ) if ( func )
#define IN_CLASS_SCOPE( )  if ( scope ) {
#define IN_CLASS_SCOPE_NOT } else {
#define IN_CLASS_SCOPE_END }

#define PARSING_SCOPE_AOP( aop_type, class_scope) do { \
    zval *join_point;\
    ZEND_HASH_FOREACH_VAL( Z_ARRVAL(XAOP_G(aop_type)), join_point ) { \
        if ( Z_TYPE_P(join_point) == IS_ARRAY ) {\
            zval *class_name = zend_hash_str_find( Z_ARRVAL_P(join_point), ZEND_STRL("class"));\
            if ( !class_name || ZVAL_IS_NULL(class_name) ) {\
                if ( class_scope ) continue;\
            }\
            zval *func_name  = zend_hash_str_find( Z_ARRVAL_P(join_point), ZEND_STRL("func"));\
            zval *aop        = zend_hash_str_find( Z_ARRVAL_P(join_point), ZEND_STRL("aop"));\
            /** to compare the class_name & function_name with the current scope */\
            if ( ( ( !class_scope) && SUCCESS == xaop_match_string(Z_STRVAL_P(func_name), ZSTR_VAL(EX(func)->common.function_name)) ) || \
                ( ( class_scope) && SUCCESS == xaop_match_string(Z_STRVAL_P(class_name), ZSTR_VAL(EX(func)->common.scope->name)) && SUCCESS == xaop_match_string(Z_STRVAL_P(func_name), ZSTR_VAL(EX(func)->common.function_name)) )\
            ) {\
                INIT_CONTEXT_FOR_CALLING(1)\
                invoke_zval_arg(aop);\
                INIT_CONTEXT_FOR_CALLING_END(0)\
            }\
        }\
    } ZEND_HASH_FOREACH_END();\
} while(0)

#define PARSING_SCOPE_AFTER_AOP( aop_type, class_scope ) do {\
    zval *join_point;\
    ZEND_HASH_FOREACH_VAL( Z_ARRVAL( XAOP_G( aop_type ) ), join_point ) {\
        if ( Z_TYPE_P( join_point ) == IS_ARRAY ) {\
            zval *class_name = zend_hash_str_find( Z_ARRVAL_P( join_point ), ZEND_STRL( "class" ) );\
            if ( !class_name || ZVAL_IS_NULL( class_name ) ) {\
                if ( class_scope ) continue;\
            }\
            zval *func_name = zend_hash_str_find( Z_ARRVAL_P( join_point ), ZEND_STRL( "func" ) );\
            zval *aop       = zend_hash_str_find( Z_ARRVAL_P( join_point ), ZEND_STRL( "aop" ) );\
            /** to compare the class_name & function_name with the current scope */\
            if ( ((!class_scope) && SUCCESS == xaop_match_string( Z_STRVAL_P( func_name ), ZSTR_VAL( funct_name ) )) || \
               ( ( class_scope) && SUCCESS == xaop_match_string( Z_STRVAL_P( class_name ), ZSTR_VAL( scope->name ) ) && SUCCESS == xaop_match_string( Z_STRVAL_P( func_name ), ZSTR_VAL( funct_name ) ) )\
            ) {\
                invoke_zval_arg( aop );\
            }\
        }\
    } ZEND_HASH_FOREACH_END();\
} while (0)

#define PARSING_SCOPE_AROUND_AOP( class_scope ) do {\
    zval *join_point;\
    ZEND_HASH_FOREACH_VAL( Z_ARRVAL( XAOP_G( around_aops ) ), join_point )\
    {\
        if ( Z_TYPE_P( join_point ) == IS_ARRAY ) {\
            zval *class_name = zend_hash_str_find( Z_ARRVAL_P( join_point ), ZEND_STRL( "class" ) );\
            if ( !class_name || ZVAL_IS_NULL( class_name ) ) {\
                if ( class_scope ) continue;\
            }\
            zval *func_name = zend_hash_str_find( Z_ARRVAL_P( join_point ), ZEND_STRL( "func" ) );\
            zval *aop       = zend_hash_str_find( Z_ARRVAL_P( join_point ), ZEND_STRL( "aop" ) );\
            /** to compare the class_name & function_name with the current scope */\
            if ( ( ( !class_scope ) && SUCCESS == xaop_match_string( Z_STRVAL_P( func_name ), ZSTR_VAL( funct_name ) ) ) ||\
            ( ( class_scope ) && SUCCESS == xaop_match_string( Z_STRVAL_P( class_name ), ZSTR_VAL( scope->name ) ) &&\
            SUCCESS == xaop_match_string( Z_STRVAL_P( func_name ), ZSTR_VAL( funct_name ) ) ) ) {\
                XAOP_G( around_mode ) = 0;\
                invoke_zval_arg_with_execute_data( aop, execute_data );\
                XAOP_G( around_mode ) = 1;\
            }\
        }\
    } ZEND_HASH_FOREACH_END();\
} while(0)


/* Execute the annotation aop instead of the default `zend_execute` function */
void xaop_annotation_ex(zend_execute_data *execute_data TSRMLS_CC);

/* Execute the execute stack for method injection aop */
void xaop_injection_ex( zend_execute_data *execute_data TSRMLS_DC );
void xaop_injection_internal_ex( zend_execute_data *execute_data TSRMLS_DC, zval *return_value TSRMLS_DC);

#endif //EXEC_H
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
