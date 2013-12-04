<?php

function createDatabase(\Transactd\Database $db, $uri)
{
    return $db->create($uri);
}

function openDbExclusive(\Transactd\Database $db, $uri)
{
    return $db->open($uri, \Transactd\TYPE_SCHEMA_BDF, \Transactd\TD_OPEN_EXCLUSIVE);
}

function createUserTableSchema(\Transactd\DbDef $def)
{
    $td = $def->createTableDef();

    //Insert table
    $td->setTableName("user");
    $td->setFileName("user.dat");
    $td->id = 1;
    $td->charsetIndex = \Transactd\CHARSET_UTF8B4;
    if ($def->insertTable($td) !== true) {
        return false;
    }

    //Insert field
    $fieldNum = 0;
    $fd = $td->insertField($fieldNum);
    $fd->setName("id");
    $fd->type = \Transactd\FT_INTEGER;
    $fd->len = 4;

    $fd = $td->insertField(++$fieldNum);
    $fd->setName("name");
    $fd->type = \Transactd\FT_MYVARCHAR;
    $fd->setLenByCharnum(32);

    $fd = $td->insertField(++$fieldNum);
    $fd->setName("group");
    $fd->type = \Transactd\FT_INTEGER;
    $fd->len = 4;

    //Insert index
    $keyNum = 0;
    $kd = $td->insertKey($keyNum);
    $kd->segments[0]->fieldNum = 0;
    $kd->segments[0]->flags->bit8 = true; //extended type
    $kd->segments[0]->flags->bit1 = true; //updateable
    $kd->segmentCount = 1;
    $td->primaryKeyNum = $keyNum;

    $keyNum = 1;
    $kd = $td->insertKey($keyNum);
    $kd->segments[0]->fieldNum = 2; //group id
    $kd->segments[0]->flags->bit8 = true; //extended type
    $kd->segments[0]->flags->bit1 = true; //updateable
    $kd->segments[0]->flags->bit0 = true; //duplicatable
    $kd->segmentCount = 1;

    //Write schema table
    return $td->update();
}

function createGroupTableSchema(\Transactd\DbDef $def)
{
    $td = $def->createTableDef();

    //Insert table
    $td->setTableName("group1");
    $td->setFileName("group1.dat");
    $td->id = 2;
    $td->charsetIndex = \Transactd\CHARSET_UTF8B4;
    if ($def->insertTable($td) !== true) {
        return false;
    }

    //Insert field
    $fieldNum = 0;
    $fd = $td->insertField($fieldNum);
    $fd->setName("id");
    $fd->type = \Transactd\FT_INTEGER;
    $fd->len = 4;

    $fd = $td->insertField(++$fieldNum);
    $fd->setName("name");
    $fd->type = \Transactd\FT_MYVARCHAR;
    $fd->setLenByCharnum(32);

    //Insert index
    $keyNum = 0;
    $kd = $td->insertKey($keyNum);
    $kd->segments[0]->fieldNum = 0;
    $kd->segments[0]->flags->bit8 = true; //extended type
    $kd->segments[0]->flags->bit1 = true; //updateable
    $kd->segmentCount = 1;
    $td->primaryKeyNum = $keyNum;

    //Write schema table
    return $td->update();
}

function createPictureTableSchema(\Transactd\DbDef $def)
{
    $td = $def->createTableDef();

    //Insert table
    $td->setTableName("picture");
    $td->setFileName("picture.dat");
    $td->id = 3;
    $td->charsetIndex = \Transactd\CHARSET_LATIN1;
    if ($def->insertTable($td) !== true) {
        return false;
    }

    //Insert 3 fields
    $fieldNum = 0;
    $fd = $td->insertField($fieldNum);
    $fd->setName("type");
    $fd->type = \Transactd\FT_INTEGER;
    $fd->len = 2;

    $fd = $td->insertField(++$fieldNum);
    $fd->setName("id");
    $fd->type = \Transactd\FT_INTEGER;
    $fd->len = 4;

    $fd = $td->insertField(++$fieldNum);
    $fd->setName("picture");
    $fd->type = \Transactd\FT_MYBLOB;
    $fd->len = 11;

    //Insert index
    $keyNum = 0;
    $kd = $td->insertKey($keyNum);
    $ks = $kd->segments[0];
    $ks->fieldNum = 0; //type
    $ks->flags->bit8 = true; //extended type
    $ks->flags->bit1 = true; //updateable
    $ks->flags->bit4 = true; //segment part
    $ks = $kd->segments[1];
    $ks->fieldNum = 1; //id
    $ks->flags->bit8 = true; //extended type
    $ks->flags->bit1 = true; //updateable
    $kd->segmentCount = 2;
    $td->primaryKeyNum = $keyNum;

    //Write schema table
    return $td->update();
}

$uri = "tdap://localhost/test?dbfile=test.bdf";
$db = new \Transactd\Database;

if (createDatabase($db, $uri)) {
    if (openDbExclusive($db, $uri)) {
        if (createUserTableSchema($db->dbDef())) {
            if (createGroupTableSchema($db->dbDef())) {
                if (createPictureTableSchema($db->dbDef())) {
                    echo "create database success\n";
                }
            }
        }
    }
}
