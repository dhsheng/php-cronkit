<?php
use Cronkit\Application;

/**
 * @name hellosada
 * @cron 0 * *  * * * 
 */
class Foo {}

/**
 * @name baraa
 * @cron *\/7 * * * * * 
 */
class Bar {}

/**
 * @cron *\/5 * * * * * 
 */
class FooBar {}

$app = new Application();
$app->register(Foo::class);
$app->register(Bar::class);
$app->register(FooBar::class);
$app->start();

$a = <<<eof
/**
 * @name foo.bar.dfadfadsfadsfadsafdsad
 * @cron *\/5 * * * * * 
 */
eof;

#/@name\\s+(?P<name>[^\\s]+)/

$preg = '/@cron\\s+(?P<cron>[^\r\n]+)/';
$result = NULL;
preg_match($preg, $a, $result);
print_r($result);