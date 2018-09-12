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

#ifndef XAOP_HELPER_H
#define XAOP_HELPER_H

/* Call global function with 3 params */
void xaop_method_with_3_char_params(char *function_name, char *param1, char *param2, char *param3, zval *retval);

/* Get the object from the global DI container */
void xaop_get_object_from_di(zval *ret_obj, char *name, zend_class_entry *class_ce);

/* Reslash the string, NOTE: this will change the source string */
void xaop_reverse_slash_string(zend_string *source);

/* Include PHP file */
int xaop_include_php_file(zend_string *php_file_path, zval *retval);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
