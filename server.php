<?php

function onStart()
{
    print_r("server is running..." . PHP_EOL);
}

function onConnect()
{
    print_r("a client is connected" . PHP_EOL);
}

$serv = new TinySwoole\Server('127.0.0.1', 9501, TSWOOLE_TCP);
$serv->on("Start", "onStart");
$serv->on("Connect", "onConnect");
$serv->start();
