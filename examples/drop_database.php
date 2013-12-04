<?php

function openDbExclusive(\Transactd\Database $db, $uri)
{
    return $db->open($uri, \Transactd\TYPE_SCHEMA_BDF, \Transactd\TD_OPEN_EXCLUSIVE);
}

$uri = "tdap://localhost/test?dbfile=test.bdf";
$db = new \Transactd\Database;

if (openDbExclusive($db, $uri)) {
    if ($db->drop()) {
        echo "Drop database success\n";
    }
}
