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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
