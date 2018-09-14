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
#define IN_ANNOTATION_MODE(context, func) if ( (context) && (GET_CLASS_DOC(context)) && (func) && GET_FUNCTION_DOC(func) )
#define INVOKE_METHOD_CONTEXT() if ( 0 == XAOP_G( overloaded ) )
#define INIT_CONTEXT_FOR_CALLING() XAOP_G( overloaded ) = 1;
#define INIT_CONTEXT_FOR_CALLING_END() XAOP_G( overloaded ) = 0;
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
#define OUTPUT_JSON  1
#define OUTPUT_XML   2

/* Execute the annotation aop instead of the default `zend_execute` function */
void xaop_annotation_ex(zend_execute_data *execute_data TSRMLS_CC);

#endif //EXEC_H
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
