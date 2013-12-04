<?php

$fieldnum_id = 0;
$fieldnum_name = 1;
$fieldnum_group = 2;
$fieldnum_tel = 3;
$keynum_id = 0;

function deleteUser(\Transactd\Table $tb)
{
    global $keynum_id, $fieldnum_id;

    $tb->clearBuffer();
    $tb->setKeyNum($keynum_id); //use id key
    $tb->setFV($fieldnum_id, 3); //id=3 satoshi
    if ($tb->seek()) {
        return $tb->del();
    }
    return false;
}

function openDatabase(\Transactd\Database $db, $uri)
{
    return $db->open($uri, \Transactd\TYPE_SCHEMA_BDF);
}

$uri = "tdap://localhost/test?dbfile=test.bdf";
$db = new \Transactd\Database;

if (openDatabase($db, $uri)) {
    $tbu = $db->openTable("user");
    if ($tbu) {
        if (deleteUser($tbu)) {
            echo "Delete records success\n";
        }
    }
}
