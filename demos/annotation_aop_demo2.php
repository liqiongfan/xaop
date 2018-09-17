<?php

// This is the demo demonstrate the route how the Xaop work.
// The annotation step going:
// before->function()->success|failure->after()

class Super
{
    function _superGet() {
        echo 'Super::_superGet()' . PHP_EOL;
    }

    function _superBefore() {
        echo 'Super::_superBefore()' . PHP_EOL;
    }

    function _superAfter() {
        echo 'Super::_superAfter()' . PHP_EOL;
    }

}

/**
 * Class Son
 * @Aspect
 */
class Son
{
    function _sonGet() {
        echo 'Son::_sonGet()' . PHP_EOL;
    }

    function _sonSet() {
        echo 'Son::_sonSet()' . PHP_EOL;
    }

    /**
     * @before( value = "Super._superBefore" )
     * @after( value = "Super._superAfter" )
     */
    function _before() {
        echo 'Son::_before()' . PHP_EOL;
    }

    function _after() {
        echo 'Son::_after()' . PHP_EOL;
    }

    function _revert() {
        echo 'Son::_revert()' . PHP_EOL;
    }
}


/**
 * Class Login
 *
 * @Aspect
 */
class Login
{
    /**
     * @before( value = "Son._before" )
     * @after( value = "Son._sonGet" )
     * @success( value = "Son._sonSet" )
     * @failure( value = "Son._revert" )
     */
    function _list() {
        echo 'Login::_list()' . PHP_EOL;
    }
}

// echo '<pre>';

$login = new Login();
$login->_list();

// echo '</pre>';

/** output:
    Super::_superBefore()
    Son::_before()
    Super::_superAfter()
    Login::_list()
    Son::_revert()
    Son::_sonGet()
 */
