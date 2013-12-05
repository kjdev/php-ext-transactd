# PHP Extension for Transactd Plugin

This extension allows to access the functionality provided by Transactd Plugin
for MySQL/MariaDB.

More information about the Transactd Plugin for MySQL/MariaDB can be found at
[» http://www.bizstation.jp/en/transactd/](http://www.bizstation.jp/en/transactd/)

## Build

```
% phpize
% ./configure
% make
% make install
```

### Build option

* --with-transactd-includedir

  transactd include directory.
  (ex: /path/to/transactd/source)

* --with-transactd-libdir

  transactd library (libtdclcpp\_64m.so.1.1, libtdclc\_64.so.1.1) directory.
  (ex: /path/to/transactd/libs)

## Configration

transactd.ini:

```
extension=transactd.so
```

## Caution

現状、サンプル版なので Transactd のプロジェクトページの
[サンプル](http://www.bizstation.jp/ja/transactd/client/sdk/doc/page_4exsample.html) と同等のものしか動作確認していません。

クラスのメソッド、プロパティのほとんどが未実装状態です。

## Examples

Transactd のプロジェクトページにある
[C++コードサンプル](http://www.bizstation.jp/ja/transactd/client/sdk/doc/page_4exsample.html) と同等のサンプルになります。

DDL操作

* [データベースとテーブルの作成](examples/create_database.php)
* [データベースの削除](examples/drop_database.php)
* [テーブル定義の変更と変換](examples/update_table_def.php)

DML操作

* [レコードの挿入](examples/insert_record.php)
* [レコードの読み取りと更新](examples/update_record.php)
* [レコードの削除](examples/delete_record.php)
* [レコードのセットの読み取り](examples/read_records.php)
* [トランザクション処理](examples/transaction.php)

## Related

* [api document](http://api.at-ninja.jp/php-ext-transactd/)
