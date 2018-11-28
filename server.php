<?php

$serv = new TinySwoole\Server('127.0.0.1', 9501, TSWOOLE_TCP);
$serv->start();
