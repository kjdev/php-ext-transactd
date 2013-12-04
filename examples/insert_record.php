<?php

$fieldnum_id = 0;
$fieldnum_name = 1;
$fieldnum_group = 2;
$fieldnum_tel = 3;
$fieldnum_pic_type = 0;
$fieldnum_pic_id = 1;
$fieldnum_pic_pic = 2;

function insertUser(\Transactd\Table $tb, $id, $name, $groupid, $tel)
{
    global $fieldnum_id, $fieldnum_name, $fieldnum_group, $fieldnum_tel;
    $tb->clearBuffer();
    $tb->setFV($fieldnum_id, $id);
    $tb->setFV($fieldnum_name, $name);
    $tb->setFV($fieldnum_group, $groupid);
    $tb->setFV($fieldnum_tel, $tel);
    return $tb->insert();
}

function insertUsers(\Transactd\Table $tb)
{
    if (!insertUser($tb, 1, "akio", 1, "81-3-2222-3569")) {
        return false;
    }
    if (!insertUser($tb, 2, "yoko" , 2, "81-263-80-5555")) {
        return false;
    }
    if (!insertUser($tb, 3, "satoshi", 1, "81-3-1111-1234")) {
        return false;
    }
    if (!insertUser($tb, 4, "keiko" , 2, "81-26-222-3569")) {
        return false;
    }
    if (!insertUser($tb, 5, "john" , 3, "81-26-222-3565")) {
        return false;
    }
    return true;
}

function insertGroup(\Transactd\Table $tb, $id, $name)
{
    global $fieldnum_id, $fieldnum_name;
    $tb->clearBuffer();
    $tb->setFV($fieldnum_id, $id);
    $tb->setFV($fieldnum_name, $name);
    return $tb->insert();
}

function insertGroups(\Transactd\Table $tb)
{
    if (!insertGroup($tb, 1, "develop")) {
        return false;
    }
    if (!insertGroup($tb, 2, "sales")) {
        return false;
    }
    if (!insertGroup($tb, 3, "finance")) {
        return false;
    }
    return true;
}

function insertPicture(\Transactd\Table $tb, $type, $id, $img, $size)
{
    global $fieldnum_pic_type, $fieldnum_pic_id, $fieldnum_pic_pic;
    $tb->clearBuffer();
    $tb->setFV($fieldnum_pic_type, $type);
    $tb->setFV($fieldnum_pic_id, $id);
    $tb->setFV($fieldnum_pic_pic, $img, $size);
    return $tb->insert();
}

function readImage($path)
{
    return file_get_contents($path);
}

function openDatabase(\Transactd\Database $db, $uri)
{
    return $db->open($uri, \Transactd\TYPE_SCHEMA_BDF);
}

$uri = "tdap://localhost/test?dbfile=test.bdf";
$db = new \Transactd\Database;

if (openDatabase($db, $uri)) {
    $tbg = $db->openTable("group1");
    if ($tbg) {
        if (insertGroups($tbg)) {
            $tbu = $db->openTable("user");
            if ($tbu) {
                if (insertUsers($tbu)) {
                    $tbp = $db->openTable("picture");
                    if ($tbp) {
                        $s = readImage($argv[0]);
                        if (insertPicture($tbp, 1, 1, $s, strlen($s))) {
                            echo "Insert records success\n";
                        }
                    }
                }
            }
        }
    }
}
