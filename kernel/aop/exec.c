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
#include "ext/standard/php_string.h"
#include "ext/json/php_json.h"
#include "kernel/aop/exec.h"
#include "kernel/common/anno_parsing.h"
#include "kernel/common/helper_lib.h"
#include "kernel/loader/loader.h"
#include "zend_interfaces.h"
#include "kernel/annotation/annotation.h"
#include "main/SAPI.h"
#include "zend_smart_str.h"

/**
 * {{{ The xaop extension execute Zend OPCode function
 * to instead of the kernel default one
 * NOTE: in xaop the xaop_annotation_execute can collect the performance of each calling.
 */
void xaop_annotation_execute(zend_execute_data *execute_data)
{
    IN_XAOP_OPLINE
    zval *this          = getThis();
    zend_function *func = execute_data->func;

    zval class_annotations, function_annotations, charset, retval/* return value */;
    ZVAL_NULL(&class_annotations);
    ZVAL_NULL(&function_annotations);

    int in_aspect = 0, type = 0;
    zval *before_zval   = NULL, *after_zval     = NULL, 
         *success_zval  = NULL, *failure_zval   = NULL, 
         *api_zval      = NULL, *disable_zval   = NULL;
    
    /* Current in Class calling. */
    IN_CLASS_MODE(func)

    /* Parsing the PHPDoc about the class & the function */
    parse_phpdoc( GET_OBJECT_PHPDOC(this), &class_annotations );
    parse_phpdoc( GET_FUNCTI_PHPDOC(func), &function_annotations );

    IN_ARRAY_CONDITION(class_annotations)
    /* Found all annotations in class phpdoc. */
    zval *annotations = zend_hash_str_find(Z_ARRVAL(class_annotations), ZEND_STRL("annotations"));
    
    IN_ARRAY_CONDITION_P(annotations)
        /* To found the @Aspect exists in class or not. */
        zval *exists_aspect = zend_hash_str_find(Z_ARRVAL_P(annotations), ZEND_STRL("Aspect"));
        if ( exists_aspect ) {
            /* Aspect mode */
            in_aspect = 1;
            /* In @Aspect mode, to parse the function annotation */
            IN_ARRAY_CONDITION(function_annotations)
                /* All current calling phpdoc information about annotations */
                zval *annotations = zend_hash_str_find(Z_ARRVAL(function_annotations), ZEND_STRL("annotations"));
                IN_ARRAY_CONDITION_P(annotations)
                    
                    /* Disable zval event */
                    disable_zval = zend_hash_str_find(Z_ARRVAL_P(annotations), ZEND_STRL("disable"));
                    RETURN_CONTEXT(disable_zval)
                
                    /*Api zval event*/
                    api_zval = zend_hash_str_find(Z_ARRVAL_P(annotations), ZEND_STRL("api"));
                    xaop_api_handler(api_zval, &type, &charset);

                    /* Get the before zval event, calling it */
                    before_zval = zend_hash_str_find(Z_ARRVAL_P(annotations), ZEND_STRL("before"));
                    invoke_kernel_aop_method(before_zval);

                    /* After zval event */
                    after_zval = zend_hash_str_find(Z_ARRVAL_P(annotations), ZEND_STRL("after"));

                    /* Success zval event */
                    success_zval = zend_hash_str_find(Z_ARRVAL_P(annotations), ZEND_STRL("success"));

                    /* Failure zval event */
                    failure_zval = zend_hash_str_find(Z_ARRVAL_P(annotations), ZEND_STRL("failure"));

                    /* Load other annotations */
                    zend_string *a_name;
                    zval *a_value, annotation_obj;
                    zend_class_entry *annotation_ce;
                    ZEND_HASH_FOREACH_STR_KEY_VAL( Z_ARRVAL_P(annotations), a_name, a_value ) {
                        if ( zend_string_equals_literal(a_name, "api") ||
                            zend_string_equals_literal(a_name, "before") || 
                            zend_string_equals_literal(a_name, "after") ||
                            zend_string_equals_literal(a_name, "disable") ||
                            zend_string_equals_literal(a_name, "success") ||
                            zend_string_equals_literal(a_name, "failure")
                        ) {
                            continue;
                        }
                        if ( ZSTR_VAL(a_name)[0] == '\\' ) a_name = strpprintf(0, "%s", ZSTR_VAL(a_name) + 1);
                        annotation_ce = zend_lookup_class(zend_string_tolower(a_name));
                        if ( annotation_ce ) {
                            if ( !instanceof_function(annotation_ce, xaop_annotation_ce) ) {
                                php_error_docref(0, E_ERROR, "Annotation class must implement interface: Xaop\\Annotation.");
                                return ;
                            }
                            xaop_get_object_from_di(&annotation_obj, ZSTR_VAL(a_name), annotation_ce);
                            zend_call_method_with_2_params(&annotation_obj, Z_OBJCE(annotation_obj), NULL, "input", &retval, this, a_value);
                        }
                    } ZEND_HASH_FOREACH_END();
                IN_ARRAY_END
            IN_ARRAY_END
        } else {
            in_aspect = 0;
        }
    IN_ARRAY_END
    IN_ARRAY_END
    IN_CLASS_MODE_END

    EX(return_value) = &retval;
    execute_ex(execute_data);

    /* For JSON & XML parsing */
    OUTPUT_JSON(type)
    IN_ARRAY_CONDITION(retval)
        smart_str http_body = { 0 };
        php_json_encode( &http_body, &retval, PHP_JSON_UNESCAPED_UNICODE);
        smart_str_0(&http_body);
        php_write(ZSTR_VAL(http_body.s), ZSTR_LEN(http_body.s));
        smart_str_free(&http_body);
    IN_ARRAY_END
    OUTPUT_XML(type)
    IN_ARRAY_CONDITION(retval)
        smart_str http_body = { 0 };
        smart_str_appends(&http_body, "<?xml version=\"1.0\" encoding=\"");
        smart_str_appends(&http_body, Z_STRVAL(charset));
        smart_str_appends(&http_body, "\"?>");
        smart_str_appends(&http_body, "<root>");
        xaop_xml_data(&retval, &http_body);
        smart_str_appends(&http_body, "</root>");
        smart_str_0(&http_body);
        php_write(ZSTR_VAL(http_body.s), ZSTR_LEN(http_body.s));
        smart_str_free(&http_body);
    IN_ARRAY_END
    OUTPUT_END

    BACK_TRUE(&retval) invoke_kernel_aop_method(success_zval); } else { invoke_kernel_aop_method(failure_zval); }
    IN_ASPECT_MODE(in_aspect) {
        zval_ptr_dtor(&charset);
        invoke_kernel_aop_method(after_zval);
    }
    zval_ptr_dtor(&retval);
    if ( Z_TYPE(class_annotations) == IS_ARRAY )
        zend_array_destroy(Z_ARRVAL(class_annotations));
    if ( Z_TYPE(function_annotations) == IS_ARRAY )
        zend_array_destroy(Z_ARRVAL(function_annotations));
    IN_XAOP_OPLINE_END
}/*}}}*/

/**
 * {{{ 
 * The following two function to run the AOP for the injection AOP programming
 * The AOP mode can be annotation mode & injection mode & raw mode
 * and both of them were against each other in the same time.
 */
void xaop_execute(zend_execute_data *execute_data)
{
    IN_XAOP_OPLINE

    int around = 0;
    zval *this = getThis();
    zend_function *func = execute_data->func;
    IN_FUNCTION_MODE(func)
        zend_string *function_name = func->op_array.function_name;
        LOST_OBJECT
            XAOP_MATCH_FUNC_AROUND_AOPS(1);
        LOST_ELSE
            XAOP_MATCH_CLASS_AROUND_AOPS(1);
        LOST_OBJECT_END

    IN_FUNCTION_MODE_END

    NOT_AROUND_MODE(around)
    IN_FUNCTION_MODE(func)
        zend_string *function_name = func->op_array.function_name;
        
        LOST_OBJECT
            /* In this mode, compare all func_aops */
            /* 1 before_func_aops 2 after_func_aops 3 around_func_aops */
            XAOP_MATCH_FUNC_AOPS(before_func_aops);
        LOST_ELSE
            /* Compare funtions & object aops */
            /* 1 before_aops 2 after_aops 3 around_aops */
            XAOP_MATCH_CLASS_AOPS(before_aops);
        LOST_OBJECT_END

    IN_FUNCTION_MODE_END
    execute_ex(execute_data);
    IN_FUNCTION_MODE(func)
        zend_string *function_name = func->op_array.function_name;
        
        LOST_OBJECT
            /* In this mode, compare all func_aops */
            /* 1 after_func_aops */
            XAOP_MATCH_FUNC_AOPS(after_func_aops);
        LOST_ELSE
            /* Compare funtions & object aops */
            /* 1 after_aops */
            XAOP_MATCH_CLASS_AOPS(after_aops);
        LOST_OBJECT_END

    IN_FUNCTION_MODE_END
    NOT_AROUND_MODE_END
    IN_XAOP_OPLINE_END
}

void xaop_execute_internal(zend_execute_data *execute_data, zval *return_value)
{
    IN_XAOP_OPLINE
    int around = 0;
    zval *this = getThis();
    zend_function *func = execute_data->func;

    NOT_AROUND_MODE(around)
    IN_INTERNAL_FUNCTION_MODE(func)
        zend_string *function_name = func->internal_function.function_name;
        
        LOST_OBJECT
            /* In this mode, compare all func_aops */
            /* 1 before_func_aops 2 around_func_aops */
            XAOP_MATCH_FUNC_AOPS(before_func_aops);
        LOST_ELSE
            /* Compare funtions & object aops */
            /* 1 before_aops 2 around_aops */
            XAOP_MATCH_CLASS_AOPS(before_aops);
        LOST_OBJECT_END

    IN_FUNCTION_MODE_END
    execute_internal(execute_data, return_value);  
    IN_INTERNAL_FUNCTION_MODE(func)
        zend_string *function_name = func->internal_function.function_name;
        
        LOST_OBJECT
            /* In this mode, compare all func_aops */
            /* 1 after_func_aops */
            XAOP_MATCH_FUNC_AOPS(after_func_aops);
        LOST_ELSE
            /* Compare funtions & object aops */
            /* 1 after_aops */
            XAOP_MATCH_CLASS_AOPS(after_aops);
        LOST_OBJECT_END

    IN_FUNCTION_MODE_END
    NOT_AROUND_MODE_END
    IN_XAOP_OPLINE_END
}/*}}}*/

/**
 * {{{ Compare the two strings, str1 can has the `*` char to match one or more char
 * the str2 must be the string which waiting to be matched.
 * such as :
 * xaop_match_string("_a*b", "_ahellob")
 * will return SUCCESS;
 */
int xaop_match_string(char *str1, char *str2)
{
    int str1_len = strlen(str1);
    int str2_len = strlen(str2);
    
    if ( !str2_len ) return FAILURE;

    const char *multi_char = strchr(str1, '*');
    if ( NULL == multi_char ) {
        if ( 0 == strncmp(str1, str2, str2_len) && str1_len == str2_len ) return SUCCESS;
        else return FAILURE;
    } else {
        if ( multi_char != strrchr(str1, '*') ) {
            /* contain two or more `*` char */
            return FAILURE;
        }
        if ( 0 == str1_len - 1 ) return SUCCESS;
        if ( *(str1 + str1_len - 1) == '*' ) {
            /* end with the `*` char */
            if ( 0 == strncmp(str1, str2, str1_len - 1) ) return SUCCESS;
            else return FAILURE;
        } else if ( *str1 == '*' ) {
            /* start with the `*` char */
            int cmp_str1_len = str1_len - 1;
            int str2_offset_len = ( str2_len == cmp_str1_len ) ? 0 : str2_len - cmp_str1_len;

            if ( 0 == strncmp(str1 + 1, str2 + str2_offset_len, cmp_str1_len) ) return SUCCESS;
            else return FAILURE;
        } else {
            /* locate among the str1 */
            int after_cmp_str1_len = str1_len - ( multi_char - str1 ) - 1;
            if ( str2_len == after_cmp_str1_len ) return FAILURE;
            int str2_offset_len = str2_len - after_cmp_str1_len;
            if ( ( 0 == strncmp(str1, str2, multi_char - str1)) 
                && ( 0 == strncmp( multi_char + 1, str2 + str2_offset_len, after_cmp_str1_len )) ) return SUCCESS;
            else return FAILURE;
        }
    }
}/*}}}*/

/**
 * Each arg with the format like that:
 * [
 *    [0] => B
 *    [1] => getC
 * ]
 */
void invoke_zval_arg(zval *arg)
{
    if ( Z_TYPE_P(arg) == IS_OBJECT && zend_is_callable(arg, IS_CALLABLE_CHECK_NO_ACCESS, NULL) ) {
        zval retval;
        call_user_function(NULL, arg, arg, &retval, 0, NULL);
        zval_ptr_dtor(&retval);
        return ;
    } else if ( 2 != zend_hash_num_elements(Z_ARRVAL_P(arg)) ) 
        return ;

    zval *class_name = zend_hash_index_find(Z_ARRVAL_P(arg), 0);
    zval *funct_name = zend_hash_index_find(Z_ARRVAL_P(arg), 1);

    zend_class_entry *class_ce = zend_lookup_class(zend_string_tolower(Z_STR_P(class_name)));
    if ( class_ce ) {
        zval obj, retval;
        xaop_get_object_from_di(&obj, Z_STRVAL_P(class_name), class_ce);
        call_user_function(NULL, &obj, funct_name, &retval, 0, NULL);
        zval_ptr_dtor(&retval);
    }
}/*}}}*/

/**
 * {{{ To handle the `@api` annotation
 * steps:
 *  1: exists the @api annotation
 *  2: class has the @Aspect annotation
 *  3: current function has the return value
 *  4: return the value with the given format: JSON or XML
 * an @api annotation looks like the below format:
 * [
 *     'type'    => 'JSON',
 *     'charset' => 'GBK' // Cant be ommited
 * ]
 */
void xaop_api_handler(zval *api_zval, int *tp, zval *c_set)
{
    IN_ARRAY_CONDITION_P(api_zval)

        zval *type      = zend_hash_str_find(Z_ARRVAL_P(api_zval), ZEND_STRL("type"));
        zval *charset   = zend_hash_str_find(Z_ARRVAL_P(api_zval), ZEND_STRL("charset"));
        if ( charset ) ZVAL_COPY_VALUE(c_set, charset); else ZVAL_STRING(c_set, "UTF-8");

        sapi_header_line ctr = {0};
        ctr.line_len = spprintf(
            &(ctr.line), 
            0, 
            "Content-Type: application/%s;charset=%s", 
            !type ? "json" : ZSTR_VAL(zend_string_tolower(Z_STR_P(type))), 
            Z_STRVAL_P(c_set)
        );
        ctr.response_code = 200;                                           
        sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
        efree(ctr.line);

        /* Type */
        if ( type && zend_string_equals_literal_ci(Z_STR_P(type), "json") ) *tp = 1;
        else if ( type && zend_string_equals_literal_ci(Z_STR_P(type), "xml") ) *tp = 2;
        else *tp = 0;

    IN_ARRAY_END
}/*}}}*/

/**
 * {{{ proto xaop_xml_data to generate the XML response data to client
 */
void xaop_xml_data(zval *data, smart_str *result)
{
    if (Z_TYPE_P(data) != IS_ARRAY) return ;
    Bucket *bucket;

    ZEND_HASH_FOREACH_BUCKET(Z_ARRVAL_P(data), bucket) {
        if ( !bucket->key ) bucket->key = strpprintf(0, "%s", "key");
        smart_str_appendc(result, '<');
        smart_str_appends(result, ZSTR_VAL(bucket->key));
        smart_str_appendc(result, '>');
        /* <![CDATA[ ]]> */
        if ( Z_TYPE(bucket->val) == IS_ARRAY ) {
            xaop_xml_data(&(bucket->val), result);
        } else if ( Z_TYPE(bucket->val) == IS_LONG ) {
            smart_str_append_long(result, Z_LVAL(bucket->val));
        } else {
            if (Z_TYPE(bucket->val) != IS_STRING)
                convert_to_string(&bucket->val);
            smart_str_appends(result, "<![CDATA[");
            smart_str_appends(result, Z_STRVAL(bucket->val));
            smart_str_appends(result, "]]>");
        }
        smart_str_appends(result, "</");
        smart_str_appends(result, ZSTR_VAL(bucket->key));
        smart_str_appendc(result, '>');
    } ZEND_HASH_FOREACH_END();
}/*}}}*/

/**
 * {{{
 * Invoke the method with the given aop_zval parameters
 * The aop_zval like the following:
 * [
 *     'value'      => 'app\models\User.getName',
 *     'parameters' => 'Xan||Fast||Compatiable'
 * ]
 * or [ ]
 * or only have the value node:
 * [
 *     'value'      => 'app\annotations\Tag.setValue'
 * ]
 */
void invoke_kernel_aop_method(zval *aop_zval)
{
    /** 
     * if not given the aop_zval or aop_zval is NULL, empty array or not array value
     * return directly */
    if ( !aop_zval || Z_TYPE_P(aop_zval) != IS_ARRAY 
        || !zend_hash_num_elements(Z_ARRVAL_P(aop_zval)) ) return ;

    zval *value = NULL, *param = NULL;

    value = zend_hash_str_find(Z_ARRVAL_P(aop_zval), ZEND_STRL("value"));
    param = zend_hash_str_find(Z_ARRVAL_P(aop_zval), ZEND_STRL("parameters"));

    if ( !value || Z_TYPE_P(value) != IS_STRING ) return ;

    zval class_function;
    array_init(&class_function);
    php_explode(
        strpprintf(0, "%c", '.'),
        Z_STR_P(value),
        &class_function,
        ZEND_LONG_MAX
    );

    /* Class & function not fited or function name not given */
    if ( zend_hash_num_elements(Z_ARRVAL(class_function)) != 2 ) {
        zend_array_destroy(Z_ARRVAL(class_function));
        return ;
    }

    zval *class_name = zend_hash_index_find(Z_ARRVAL(class_function), 0);
    zval *funct_name = zend_hash_index_find(Z_ARRVAL(class_function), 1);

    zend_class_entry *class_entry = zend_lookup_class(zend_string_tolower(Z_STR_P(class_name)));
    if ( class_entry ) {
        zval class_obj, retval;
        xaop_get_object_from_di(&class_obj, Z_STRVAL_P(class_name), class_entry);
        xaop_call_method_with_php_params(&class_obj, Z_STRVAL_P(funct_name), param, &retval);
        zval_ptr_dtor(&retval);
    }
}/*}}}*/

/**
 * {{{
 * This method call the function of the proxy object with the given params
 * If your parameters were php array, please use this function to invoke the method
 * otherwise use the another one. and params must be string
 */
void xaop_call_method_with_php_params(zval *object, char *method_name, zval *params, zval *retval)
{
    if ( params )  {
        if ( Z_TYPE_P(params) != IS_STRING ) {
            convert_to_string(params);
        }
        zval parameters;
        array_init(&parameters);
        php_explode(
            strpprintf(0, "%s", "||"),
            Z_STR_P(params),
            &parameters,
            ZEND_LONG_MAX
        );

        uint32_t n = 0, p_counts = zend_hash_num_elements(Z_ARRVAL(parameters));
        zval *val_para;

        zval *params_temp = (zval *)emalloc(sizeof(zval) * p_counts );
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL(parameters), val_para) {
            ZVAL_COPY(&params_temp[n], val_para);
            n++;
        } ZEND_HASH_FOREACH_END();
        
        xaop_call_method_with_c_params(object, method_name, p_counts, params_temp, retval);
        efree(params_temp);
        zend_array_destroy(Z_ARRVAL(parameters));
    } else {
        xaop_call_method_with_c_params(object, method_name, 0, NULL, retval);
    }
}/*}}}*/

/**
 * {{{
 * If you call the method with the C params, you must invoke the method with this function call.
 * otherwise to use the above one.
 */
void xaop_call_method_with_c_params(zval *object, char *method_name, int param_count, zval *params, zval *retval)
{
    if ( XAOP_METHOD_EXISTS(object, method_name) != NULL ) {
        zval function_name;
        ZVAL_STRING(&function_name, method_name);
        call_user_function( NULL, object, &function_name, retval, param_count, params );
        zval_ptr_dtor(&function_name);
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
