<?php

$tablenum_user = 1;
$fieldnum_name = 1;

function changeUserTable(\Transactd\DbDef $def)
{
    global $tablenum_user, $fieldnum_name;

    $td = $def->tableDefs($tablenum_user);

    //Change name size
    $fd = $td->fieldDefs($fieldnum_name);
    $fd->setLenByCharnum(64);

    //Add tel field
    $fd = $td->insertField($td->fieldCount);
    $fd->setName("tel");
    $fd->type = \Transactd\FT_MYCHAR;
    $fd->setCharsetIndex(\Transactd\CHARSET_LATIN1);
    $fd->setLenByCharnum(16);

    //Write user table schema
    return $td->update();
}

function openDbExclusive(\Transactd\Database $db, $uri)
{
    return $db->open($uri, \Transactd\TYPE_SCHEMA_BDF, \Transactd\TD_OPEN_EXCLUSIVE);
}

$uri = "tdap://localhost/test?dbfile=test.bdf";
$db = new \Transactd\Database;

if (openDbExclusive($db, $uri)) {
    //Backup current user table schema
    $db->dbDef()->pushBackup($tablenum_user);

    if (changeUserTable($db->dbDef())) {
        echo "change table success\n";
    } else {
        //Restore user table schema
        $db->dbDef()->popBackup($tablenum_user);
    }
}
