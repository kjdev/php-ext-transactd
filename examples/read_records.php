<?php

$fieldnum_id = 0;
$fieldnum_name = 1;
$fieldnum_group = 2;
$fieldnum_tel = 3;
$keynum_group = 1;

function dumpUser(array $user)
{
    printf("id    %d\n", $user['id']);
    printf("name  %s\n", $user['name']);
    printf("group %d\n", $user['group']);
    printf("tel   %s\n\n", $user['tel']);
}

function readUsers(\Transactd\Table $tb, &$users)
{
    global $fieldnum_id, $fieldnum_name, $fieldnum_group, $fieldnum_tel, $keynum_group;

    $users = array();

    $tb->clearBuffer();
    $tb->setKeyNum($keynum_group); //use group key
    $tb->setFV($fieldnum_group, 3); //set group = 3;
    $tb->seekGreater(true /*orEqual*/);
    if ($tb->stat() == 0) {
        $tb->setFilter("group = 3", 1/*rejectCount*/, 0/*max records*/);
    }
    while (1) {
        if ($tb->stat() != 0) {
            if ($tb->stat() == \Transactd\STATUS_EOF) {
                return true;
            }
            return false;
        }

        $u = array(
            'id' => $tb->getFVint($fieldnum_id),
            'name' => $tb->getFVstr($fieldnum_name),
            'group' => $tb->getFVint($fieldnum_group),
            'tel' => $tb->getFVstr($fieldnum_tel),
        );

        array_push($users, $u);

        $tb->findNext();
    }
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
        if (readUsers($tbu, $users)) {
            foreach ($users as $u) {
                dumpUser($u);
            }
        }
    }
}
