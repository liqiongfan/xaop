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
#include "main/SAPI.h"
#include "ext/json/php_json.h"
#include "ext/standard/php_string.h"
#include "Zend/zend_smart_str.h"
#include "Zend/zend_exceptions.h"
#include "kernel/helper.h"

/**
 * {{{
 * Call global function with 3 params
 * it mainly means to call the `preg_replace` function
 */
void xaop_method_with_3_char_params(char *function_name, char *param1, char *param2, char *param3, zval *retval)
{
    zval func_name, subject, replace, pattern;
    ZVAL_STRING(&func_name, function_name);
    ZVAL_STRING(&subject, param1);
    ZVAL_STRING(&replace, param2);
    ZVAL_STRING(&pattern, param3);
    zval params[3] = { pattern, replace, subject };
    call_user_function(EG(function_table), NULL, &func_name, retval, 3, params);
    zval_ptr_dtor(&func_name);
    zval_ptr_dtor(&subject);
    zval_ptr_dtor(&replace);
    zval_ptr_dtor(&pattern);
}/*}}}*/

/**
 * {{{ To obtain the object from the global DI container
 * if exists, or not to create the object & store it into the global di container
 * and return
 */
void xaop_get_object_from_di(zval *ret_obj, char *name, zend_class_entry *class_ce)
{
    zval *retval = zend_hash_str_find( Z_ARRVAL(XAOP_G(di)), name, strlen(name) );
    if ( retval && !ZVAL_IS_NULL(retval) ) {
        ZVAL_COPY(ret_obj, retval);
    } else {
        object_init_ex(ret_obj, class_ce);
        add_assoc_zval( &XAOP_G(di), name, ret_obj);
    }
}/*}}}*/

/**
 * {{{ To reverse the slash char to positive slash, such as:
 * `app\model\Tag` will return the following:
 * `app/model/Tag`.
 * NOTE: This operation will modify the source string
 */
void xaop_reverse_slash_string(zend_string *source)
{
    int i = 0, s_len = ZSTR_LEN(source);
    char *str = ZSTR_VAL(source);
    for (; i < s_len; i++) {
        if ( *(str + i) == '\\' ) {
            *(str + i) = '/';
        }
    } 
}/*}}}*/

/**
 * {{{ proto xaop_include_php_file() will include the php file into the current scope
 * if you assign the retval, the xaop will return the result into the retval variable, otherwise please pass NULL to retval
 */
int xaop_include_php_file(zend_string *php_file_path, zval *retval)
{
    if ( access(ZSTR_VAL(php_file_path), F_OK) == -1 ){
        php_error_docref(0, E_ERROR, "File: `%s` not exists.", ZSTR_VAL(php_file_path));
        return FAILURE;
    }

    zend_file_handle phpfile_handle;
    
    phpfile_handle.filename = ZSTR_VAL(php_file_path);
    phpfile_handle.free_filename = 0;
    phpfile_handle.handle.fp = NULL;
    phpfile_handle.opened_path = NULL;
    phpfile_handle.type = ZEND_HANDLE_FILENAME;

    zend_op_array *new_array = zend_compile_file(&phpfile_handle, ZEND_INCLUDE);
	
    if (new_array && phpfile_handle.handle.stream.handle) {
		if (!phpfile_handle.opened_path) {
			phpfile_handle.opened_path = zend_string_copy(php_file_path);
		}
		zend_hash_add_empty_element(&EG(included_files), phpfile_handle.opened_path);
	}
    zend_destroy_file_handle(&phpfile_handle);
    
    if ( new_array ) {
        zend_execute(new_array, retval);
        destroy_op_array(new_array);
        efree(new_array);
        return SUCCESS;
    } else {
        return FAILURE;
    }
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
        call_user_function(NULL, NULL, arg, &retval, 0, NULL);
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
 * To execute the given function with the given execute_data
 */
void invoke_zval_arg_with_execute_data(zval *arg, zend_execute_data *jmp_buf)
{
    if ( Z_TYPE_P(arg) == IS_OBJECT && zend_is_callable(arg, IS_CALLABLE_CHECK_NO_ACCESS, NULL) ) {
        zval retval, data;
        ZVAL_RES(&data, zend_register_resource(jmp_buf, le_xaop));
        zval params[1] = { data };
        call_user_function(NULL, NULL, arg, &retval, 1, params);
        jmp_buf->opline = jmp_buf->func->op_array.opcodes;
        if ( EG(exception) ) {
            zend_exception_error(EG(exception), E_WARNING);
        }
        zval_ptr_dtor(&retval);
        return ;
    } else if ( 2 != zend_hash_num_elements(Z_ARRVAL_P(arg)) )
        return ;
    
    zval *class_name = zend_hash_index_find(Z_ARRVAL_P(arg), 0);
    zval *funct_name = zend_hash_index_find(Z_ARRVAL_P(arg), 1);
    
    zend_class_entry *class_ce = zend_lookup_class(zend_string_tolower(Z_STR_P(class_name)));
    if ( class_ce ) {
        zval obj, retval, data;
        ZVAL_RES(&data, zend_register_resource(jmp_buf, le_xaop));
        zval params[1] = { data };
        xaop_get_object_from_di(&obj, Z_STRVAL_P(class_name), class_ce);
        call_user_function(NULL, &obj, funct_name, &retval, 1, params);
        if ( EG(exception) ) {
            zend_exception_error(EG(exception), E_WARNING);
        }
        jmp_buf->opline = jmp_buf->func->op_array.opcodes;
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
    if ( api_zval && Z_TYPE_P(api_zval) == IS_ARRAY
         && zend_hash_num_elements(Z_ARRVAL_P(api_zval)) ) {
    
        zval *type    = zend_hash_str_find( Z_ARRVAL_P( api_zval ), ZEND_STRL( "type" ) );
        zval *charset = zend_hash_str_find( Z_ARRVAL_P( api_zval ), ZEND_STRL( "charset" ) );
        if ( charset ) ZVAL_COPY_VALUE( c_set, charset ); else ZVAL_STRING( c_set, "UTF-8" );
    
        sapi_header_line ctr = { 0 };
        ctr.line_len  = spprintf(
            &( ctr.line ), 0,
            "Content-Type: application/%s;charset=%s",
            !type ? "json" : ZSTR_VAL( zend_string_tolower( Z_STR_P( type ) ) ),
            Z_STRVAL_P( c_set )
        );
        ctr.response_code = 200;
        sapi_header_op( SAPI_HEADER_REPLACE, &ctr );
        efree( ctr.line );
    
        /* Type */
        if ( type && zend_string_equals_literal_ci( Z_STR_P( type ), "json" ) ) *tp = 1;
        else if ( type && zend_string_equals_literal_ci( Z_STR_P( type ), "xml" ) ) *tp = 2;
        else *tp = 0;
    }
}/*}}}*/

/**
 * {{{ proto xaop_xml_data to generate the XML response data to client
 */
void xaop_xml_data(zval *data, smart_str *result)
{
    Bucket *bucket;
    if (Z_TYPE_P(data) != IS_ARRAY) return ;
    
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
    if ( !aop_zval || Z_TYPE_P(aop_zval) != IS_ARRAY || !zend_hash_num_elements(Z_ARRVAL_P(aop_zval)) ) return ;
    
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
    zend_array_destroy(Z_ARRVAL(class_function));
}/*}}}*/

/**
 * {{{
 * This method call the function of the proxy object with the given params
 * If your parameters were php array, please use this function to invoke the method
 * otherwise use the another one. and params must be string
 */
void xaop_call_method_with_php_params(zval *object, char *method_name, zval *params, zval *retval)
{
    if ( params && IS_STRING == Z_TYPE_P(params) )  {
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
