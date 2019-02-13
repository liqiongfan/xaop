<?php

/**
 * @Aspect
 */
class A
{
	/**
	 * 	@api(	type =	"json"	, charset=utf-8)
	 */
	public function getJson() {
		return [
			'a', 'b', 'c', 'd'
		];
	}
}

// var_dump( (new Xaop\Doc())->getFunctionDoc("A", "getJson") );
(new A())->getJson();
