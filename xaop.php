<?php

/**
 *@Aspect
 */
class Swing
{
    public function _before() {
        echo '_before';
    }

    /**
     *@before(value="Swing._before")
     */
    public function goodLists() {
        echo 'goodLists';
    }
}

$swig = new Swing();
$swig->goodLists();
