<?php

$fieldnum_id = 0;
$fieldnum_name = 1;
$fieldnum_group = 2;
$fieldnum_tel = 3;
$keynum_group = 1;

function updateUsers(\Transactd\Table $tb)
{
    global $fieldnum_id, $fieldnum_name, $fieldnum_group, $fieldnum_tel, $keynum_group;

    $tb->clearBuffer();
    $tb->setKeyNum($keynum_group); //use group key
    $tb->setFV($fieldnum_group, 1); //set group = 1;
    $tb->seekGreater(true /*orEqual*/);
    while ($tb->stat() == 0) {
        //Check group value.
        if ($tb->getFVint($fieldnum_group) != 1) {
            break;
        }
        //Update group
        $tb->setFV($fieldnum_group, 3); //change group 1 to 3
        if ($tb->update(\Transactd\CHANGE_CURRENT_NCC /*ncc=true*/) !== true) { //Important ncc=true !
            return false;
        } else {
            $tb->seekNext();
        }
    }
    return (($tb->stat() == \Transactd\STATUS_EOF) || ($tb->stat() == 0));
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
        $db->beginTrn();
        if (updateUsers($tbu)) {
            $db->endTrn();
            echo "Update records success\n";
        } else {
            $db->abortTrn();
        }
    }
}
