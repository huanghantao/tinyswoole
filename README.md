## Description

a tiny [swoole](https://github.com/swoole/swoole-src)

## Create server

```php
<?php

$serv = new TinySwoole\Server('127.0.0.1', 9501, TSWOOLE_TCP);
$serv->start();

```

## Set parameters

```php
$serv->set([
    'reactor_num' => 2,
]);
```

## Add the server start event callback

```php
<?php

function onStart()
{
    print_r("server is running..." . PHP_EOL);
}

$serv = new TinySwoole\Server('127.0.0.1', 9501, TSWOOLE_TCP);
$serv->on("Start", "onStart");
$serv->start();

```

## Add the server connect event callback

```php
<?php

function onConnect($fd)
{
    print_r("client[{$fd}] is connected" . PHP_EOL);
}

$serv = new TinySwoole\Server('127.0.0.1', 9501, TSWOOLE_TCP);
$serv->on("Connect", "onConnect");
$serv->start();

```

## Add the server receive event callback

```php
<?php

function onReceive($serv, $fd, $data)
{
    print_r("receive data from client[{$fd}]: {$data}");
}

$serv = new TinySwoole\Server('127.0.0.1', 9501, TSWOOLE_TCP);
$serv->on("Receive", "onReceive");
$serv->start();

```

## send data to client

```php
$serv->send($fd, "hello world");
```

## Install from source

```shell
git clone git@github.com:huanghantao/tinyswoole.git && \
cd tinyswoole && \
phpize && \
./configure && \
make && make install
```

## Welcome

QQ group：942858122