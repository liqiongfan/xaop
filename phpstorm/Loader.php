<?php
/**
 * Powered by Xaop.
 * Xaop is an High performance PHP extension for WEB develop based on server such as Nginx/Apache
 * User can use, modify, copy Xan only when keep the explain words
 * https://www.supjos.cn All Rights Reserved.
 * License: BSD
 * User: josin
 * Date: 2018/9/10
 * Time: 下午1:21
 */

namespace Xaop;

class Loader {

    /**
     * Loader constructor.
     *
     * The constructor of the Xaop\Loader class
     */
    public function __construct() { }

    /**
     * Set the alias path with the given alias name, such as :
     * setMap('@app', __DIR__ . '/../app');
     * @param string $key       The alias key name, NOTE: must be start with the char: `@`
     * @param string $path      The alias key path
     */
     public function setMap($key, $path) { }

    /**
     * To autoload the class with the full name & namespace, it was Case sensative.
     * @param string $className The class name which current env. can't found.
     */
     final public function loader($className) { }
}