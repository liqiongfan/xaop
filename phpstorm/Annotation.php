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
 * Interface Annotation
 *
 * Every annotation class must implement the Annotation interface to do the annotation job
 * such as the following one:
 *
 * class Tag implement Xaop\Annotation {
 *
 *     public function input($object, $annotations) {
 *         foreach($annotations as $key => $val) {
 *             $object->$key = $val;
 *         }
 *     }
 * }
 * @package Xaop
 */
interface Annotation {

    /**
     * Input the values from the annotations
     * @param object|string $objectOrName   In function mode. This means the calling object otherwise the Class name
     * @param array $annotations    The annotations from the outside class or function
     */
    public function input($objectOrName, $annotations);
}