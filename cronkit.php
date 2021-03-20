<?php
use Cronkit\Application;

/**
 * @name test
 * @cron 0 0 * * * * 
 */
class Foo {
    public function execute() {
        echo "foo\n";
    }
}

/**
 * @name test
 * @cron * * * * * * 
 */
class Bar {
    public function execute() {
        echo "this is bar\n";
    }
}

/**
 * @name testv2
 * @cron * * * * * * 
 */
class FooBar {
    public function execute() {
        
        echo "foo.bar" . (10 + 20) . "----\n";
    }
}

$app = new Application();
$app->register(Foo::class);
$app->register(Bar::class);
$app->register(FooBar::class);
$app->start();