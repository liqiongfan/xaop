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
#include "kernel/mvc/app.h"

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_app_construct), 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_app_set_default_module), 0, 0, 1)
    ZEND_ARG_INFO(0, defaultModule)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_app_set_default_controller), 0, 0, 1)
    ZEND_ARG_INFO(0, defaultController)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_app_set_default_action), 0, 0, 1)
    ZEND_ARG_INFO(0, defaultAction)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_app_set_allow_modules), 0, 0, 1)
    ZEND_ARG_INFO(0, allowModules)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_app_set_url_mode), 0, 0, 1)
    ZEND_ARG_INFO(0, url_model)
    ZEND_ARG_INFO(0, url_get_str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_app_set_view_suffix), 0, 0, 1)
    ZEND_ARG_INFO(0, viewSuffix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_app_set_url_suffix), 0, 0, 1)
    ZEND_ARG_INFO(0, urlSuffix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_app_set_application_dir), 0, 0, 1)
    ZEND_ARG_INFO(0, applicationDir)
    ZEND_ARG_INFO(0, defaultNamespace)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_app_on), 0, 0, 2)
    ZEND_ARG_INFO(0, statusCode)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_app_bootstrap), 0, 0, 1)
    ZEND_ARG_INFO(0, bootstrapPath)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ARG_INFO(xaop_app_run), 0, 0, 0)
ZEND_END_ARG_INFO()

/**
 * {{{ proto Xaop\App::__construct()
 * Construct the Xaop\App class object from the kernel class env.
 */
XAOP_METHOD(App, __construct)
{

}/*}}}*/

/**
 * {{{ proto Xaop\App::setDefaultModule($defaultModule)
 * To set the default module
 */
XAOP_METHOD(App, setDefaultModule)
{
    zend_string *default_module;
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &default_module) == FAILURE ) {
        return ;
    }

    if ( !ZSTR_LEN(default_module) ) {
        return ;
    }

    XAOP_G(default_module) = default_module;
}/*}}}*/

/**
 * {{{ proto Xaop\App::setDefaultController($defaultController)
 * To set the default controller
 */
XAOP_METHOD(App, setDefaultController)
{
    zend_string *default_controller;
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &default_controller) == FAILURE ) {
        return ;
    }

    if ( !ZSTR_LEN(default_controller) ) {
        return ;
    }

    XAOP_G(default_controller) = default_controller;
}/*}}}*/

/**
 * {{{ proto Xaop\App::setDefaultAction($defaultAction)
 * To set the default Action
 */
XAOP_METHOD(App, setDefaultAction)
{
    zend_string *default_action;
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &default_action) == FAILURE ) {
        return ;
    }

    if ( !ZSTR_LEN(default_action) ) {
        return ;
    }

    XAOP_G(default_action) = default_action;
}/*}}}*/

/**
 * {{{ proto Xaop\App::setAllowModules($modules = [])
 * To set the allowed modules
 */
XAOP_METHOD(App, setAllowModules)
{
    zval *allow_modules;
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &allow_modules) == FAILURE ) {
        return ;
    }
    ZVAL_COPY(&XAOP_G(default_modules), allow_modules);
}/*}}}*/

/**
 * {{{ proto Xaop\App::setUrlMode($urlMode [, $urlGetStr = ""])
 * To set the $_GET key to obtain the URL info.
 */
XAOP_METHOD(App, setUrlMode)
{
    zend_string *url_mode, *url_get_str = NULL;

    if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "S|S", &url_mode, &url_get_str) == FAILURE ) {
        return ;
    }

    if ( zend_string_equals_literal_ci(url_mode, "get") ) {
        XAOP_G(url_mode) = 0;
    } else if ( zend_string_equals_literal_ci(url_mode, "path") ) {
        XAOP_G(url_mode) = 1;
    } else if ( zend_string_equals_literal_ci(url_mode, "auto") ) {
        XAOP_G(url_mode) = 2;
    } else {
        php_error_docref(0, E_ERROR, "Wrong url mode obtain.");
        return ;
    }

    if ( url_get_str ) {
        XAOP_G(url_get_str) = url_get_str;
    }
}/*}}}*/

/**
 * {{{ proto Xaop\App::setViewSuffix($viewSuffix)
 * To set the view suffix
 */
XAOP_METHOD(App, setViewSuffix)
{
    zend_string *view_suffix;
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &view_suffix) == FAILURE ) {
        return ;
    }

    if ( !ZSTR_LEN(view_suffix) ) {
        return ;
    }

    XAOP_G(view_suffix) = view_suffix;
}/*}}}*/

/**
 * {{{ proto Xaop\App::setUrlSuffix($urlSuffix)
 * To set the url suffix
 */
XAOP_METHOD(App, setUrlSuffix)
{
    zend_string *url_suffix;
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &url_suffix) == FAILURE ) {
        return ;
    }

    if ( !ZSTR_LEN(url_suffix) ) {
        return ;
    }

    XAOP_G(url_suffix) = url_suffix;
}/*}}}*/

/**
 * {{{ proto Xaop\App::setApplicationDir($applicationDir[, $defaultNamesapce = "app"])
 * To set the applicaton dir bundle with the default namespace
 */
XAOP_METHOD(App, setApplicationDir)
{
    zend_string *application_dir, *default_namespace = NULL;
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|S", &application_dir, &default_namespace) == FAILURE ) {
        return ;
    }
    if ( !ZSTR_LEN(application_dir) ) {
        php_error_docref(0, E_ERROR, "applicationDir msut be valid string path.");
        return ;
    }
    add_assoc_str(&XAOP_G(aliases), default_namespace && ZSTR_LEN(default_namespace) ? ZSTR_VAL(default_namespace) : "app", application_dir );
    
    XAOP_G(application_dir) = application_dir;
}/*}}}*/

/**
 * {{{ proto Xaop\App::on($statusCode, $callback)
 * Bind some status code with some callback.
 */
XAOP_METHOD(App, on)
{

}/*}}}*/

/**
 * {{{ proto Xaop\App::bootstrap($bootstrapPath = "Bootstrap")
 * To bootstrap the kernel
 */
XAOP_METHOD(App, bootstrap)
{

}/*}}}*/

/**
 * {{{ proto Xaop\App::run()
 * To start the MVC process
 */
XAOP_METHOD(App, run)
{

}/*}}}*/


XAOP_FUNCTIONS(app)
    XAOP_ME(App, __construct, arginfo_xaop_app_construct, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL)
    XAOP_ME(App, setDefaultModule, arginfo_xaop_app_set_default_module, ZEND_ACC_PUBLIC)
    XAOP_ME(App, setDefaultController, arginfo_xaop_app_set_default_controller, ZEND_ACC_PUBLIC)
    XAOP_ME(App, setDefaultAction, arginfo_xaop_app_set_default_action, ZEND_ACC_PUBLIC)
    XAOP_ME(App, setAllowModules, arginfo_xaop_app_set_allow_modules, ZEND_ACC_PUBLIC)
    XAOP_ME(App, setUrlMode, arginfo_xaop_app_set_url_mode, ZEND_ACC_PUBLIC)
    XAOP_ME(App, setViewSuffix, arginfo_xaop_app_set_view_suffix, ZEND_ACC_PUBLIC)
    XAOP_ME(App, setUrlSuffix, arginfo_xaop_app_set_url_suffix, ZEND_ACC_PUBLIC)
    XAOP_ME(App, setApplicationDir, arginfo_xaop_app_set_application_dir, ZEND_ACC_PUBLIC)
    XAOP_ME(App, on, arginfo_xaop_app_on, ZEND_ACC_PUBLIC)
    XAOP_ME(App, bootstrap, arginfo_xaop_app_bootstrap, ZEND_ACC_PUBLIC)
    XAOP_ME(App, run, arginfo_xaop_app_run, ZEND_ACC_PUBLIC)
XAOP_FUNCTIONS_END()


XAOP_INIT(app)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, XAOP_PREFIX "App", app_functions);
    xaop_app_ce = zend_register_internal_class(&ce);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
