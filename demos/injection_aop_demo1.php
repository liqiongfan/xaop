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

Xaop::addBeforeAop(Swing::class, "__get", function(){
    echo 'Closure before()' . PHP_EOL;
});
Xaop::addBeforeAop(Swing::class, "__get", [ Swing::class, "__getBefore" ] );
Xaop::addAfterAop(Swing::class, "__get", [ Swing::class, "__getAfter" ]);
Xaop::addAfterAop(Swing::class, "__get", function(){
    echo 'Closure after()' . PHP_EOL;
});

echo '<pre>';

$swing = new Swing();
$swing->di;

echo '</pre>';

/** output:
    Closure before()
    Swing::__getBefore()
    Swing::__get()
    Swing::__getAfter()
    Closure after()
 */

