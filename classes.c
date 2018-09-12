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

#ifndef XAOP_CLASSES_H
#define XAOP_CLASSES_H

zend_class_entry *xaop_loader_ce;
zend_class_entry *xaop_annotation_ce;
zend_class_entry *xaop_adapter_ce;
zend_class_entry *xaop_app_ce;
zend_class_entry *xaop_controller_ce;
zend_class_entry *xaop_model_ce;
zend_class_entry *xaop_request_ce;
zend_class_entry *xaop_session_ce;
zend_class_entry *xaop_view_ce;
zend_class_entry *xaop_ce;

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
