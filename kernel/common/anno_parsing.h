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

#ifndef XAOP_ANNO_PARSING_H
#define XAOP_ANNO_PARSING_H

/* Parse the PHPDocs and store the result into the value `result` */
void parse_phpdoc(zend_string *phpdoc, zval *result);

/* Parse one line phpdoc. */
void parse_line_phpdoc(zend_string *line_doc, zval *result, int *body_end_pos, int start_pos);

/* Parse the key & value pairs */
void parse_key_value_pairs(zend_string *str, zval *result);

/* Parse the key & value */
void parse_key_value(zend_string *str, zval *result);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
