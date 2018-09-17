<?php

// This is the file demonstrate the InjectionAOP mode which need set the php.ini with the config to 3:
//  `` xaop.aop_mode = 3 ``

class Swing
{
    public function __get($name) {
        echo 'Swing::__get()' . PHP_EOL;
    }

    public function __getBefore() {
        echo 'Swing::__getBefore()' . PHP_EOL;
    }

    public function __getAfter() {
        echo 'Swing::__getAfter()' . PHP_EOL;
    }
}

Xaop::addAroundAop(Swing::class, "__get", function($xaopExec){
    echo '_around_before()' . PHP_EOL;
    Xaop::exec($xaopExec); // To run the Swing::__get() function
    echo '_around_after()' . PHP_EOL;
});

echo '<pre>';

$swing = new Swing();
$swing->di;

echo '</pre>';

/** output:
    _around_before()
    Swing::__get()
    _around_after()
 */

