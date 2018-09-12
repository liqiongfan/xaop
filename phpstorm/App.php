<?php
/**
 * Powered by Xaop.
 * Xaop is an High performance PHP extension for WEB develop based on server such as Nginx/Apache
 * User can use, modify, copy Xan only when keep the explain words
 * https://www.supjos.cn All Rights Reserved.
 * License: BSD
 * User: Josin
 * Date: 2018/9/10
 */

namespace Xaop;

/**
 * Class App
 *
 * The APP component for the MVC web develope
 * developer can derived from Xaop\App class or directly use the Xaop\App class
 * will create the App object for the web lifecycle, in the whole lifecycle, xaop will
 * stay the variable Xaop\App::$app for the reference of the Xaop\App object
 *
 * @package Xaop
 */
class App
{

    /**
     * App constructor.
     * Construct the Xaop\App class object
     * NOTE: This is a final function, that means the child class can't rewrite the
     * constructor functon
     */
    final public function __construct() { }

    /**
     * To set the default module
     *
     * @param string $defaultModule The default module
     */
    public function setDefaultModule($defaultModule = "index") { }

    /**
     * To set the default controller
     *
     * @param string $defaultController The default controller
     */
    public function setDefaultController($defaultController = 'index') { }

    /**
     * To set the default action
     *
     * @param string $defaultAction The default action
     */
    public function setDefaultAction($defaultAction = 'index') { }

    /**
     * To set the allowed modules, default the `index` module are allowed, other module
     * will be denied.
     *
     * @param array $defaultModules The allowed modules
     */
    public function setDefaultModules($defaultModules = ['index']) { }

    /**
     * To set the url mode bundled with the GET url key when in get mode
     *
     * @param string $urlMode The mode to obtain the url info.
     * @param string $urlGetStr when mode set to get, the url get key to obtain the info.
     */
    public function setUrlMode($urlMode = "GET", $urlGetStr = "_url") { }

    /**
     * Set the default view suffix
     *
     * @param string $viewSuffix The default view suffix
     */
    public function setViewSuffix($viewSuffix = "html") { }

    /**
     * Set the default url suffix
     * @param string $urlSuffix The default url suffix
     */
    public function setUrlSuffix($urlSuffix = "html") { }

    /**
     * @param string $applicationDir The default application dir
     * @param string $defaultName The bundled namespace with the application dir.
     */
    public function setApplicationDir($applicationDir, $defaultName = "app") { }

    /**
     * Set the callback when the given status code occured
     *
     * @param int      $statusCode The status code when occured, such as 404
     * @param \Closure $callback The callback when the status occured will be invoked.
     */
    public function on($statusCode = 404, $callback) { }

    /**
     * To bootstrap some global job such as IOC and so on.
     *
     * @param string $bootstrapClassPath The bootstrap class path to do the bootstrap job, The path will be based on the setApplicationDir
     * if not setting the applicationDir, the default applicationDir `.` will be used.
     */
    public function bootstrap($bootstrapClassPath = "Bootstrap") { }

    /**
     * Start the MVC process.
     */
    public function run() { }
}