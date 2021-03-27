#### Cronkit

> Periodic task for PHP


```php
<?php
use Cronkit\Application;

/**
 * @name hello
 * @cron * * * * * * 
 */
class Hello {
    public function execute() {
        echo sprintf("hello\n");
    }
}

/**
 * @name foobar
 */
class Foobar {
    public function execute() {
        echo sprintf("foobar\n");
    }
}

$app = new Application();
$app->register(Hello::class);
$app->register('*/1 * * * * *', Foobar::class);
$app->start();
```