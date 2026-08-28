# PostgreSQL pg_lib ライブラリ

PostgreSQL 17.10 を前提とした C言語向けデータベースアクセスライブラリです。

## 概要
- Postgresアクセスの主要な関数およびユーティリティを集約しています。
- 接続管理、クエリ実行、型変換、文字列操作などのモジュールで構成されています。

## 前提条件・動作環境
- **PostgreSQL 17.10** 以降
- **CMake**: 3.15 以上
- **Cコンパイラ**: C11 サポート（GCC, Clang など）
- **PostgreSQL (libpq)**: 開発生析パッケージ（例: `libpq-dev` / `postgresql-devel`）

## CMake ビルドオプション

| オプション名 | デフォルト値 | 説明 |
| :--- | :---: | :--- |
| `PG_LIB_BUILD_SAMPLES` | `ON` | `sample/` 配下のサンプルプログラムをビルドする |
| `PG_LIB_BUILD_UTILITIES` | `ON` | `pg_util` ライブラリをビルドする |
| `PG_LIB_ENABLE_SANITIZER` | `ON` | Debugビルド時に Address/UB Sanitizer を有効化する |
| `PG_LIB_WERROR` | `ON` | 警告をエラーとして扱う (`-Werror`) |

## ビルド手順

### 基本ビルド（Debug）

Debug版でモジュールを作成すると、実行バイナリの名前の最後に'**D**'が付きます。

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### リリースビルド

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### サンプルやSanitizerを無効にしてビルドしたい場合

既定では、サニタイザーをバイナリに盛り込んでライブラリやサンプルを作成します。
なお、ライブラリにサニタイザーを設定した場合は、そのライブラリを使用する実行モジュール
にも、サニタイザーの設定が必要になります。ご注意ください。

```bash
cmake -B build -S . \
  -DPG_LIB_BUILD_SAMPLES=OFF \
  -DPG_LIB_ENABLE_SANITIZER=OFF
```

## 動作確認(sample)

sample/にあるコードはすべて、実行結果をファイルに出力するようにしています。コンソールに出力したい場合は、以下の定義を`0`に変更してください。

```c
#define USE_LOG_FILE 0
```

### 1. テスト用データベースの作成

build/に移動して、`sample_execute`を使用して、テスト用の環境を作成します。これにより、データベースに以下のオブジェクトを作成します。

|オブジェクト|名称|
|-----------|----|
|データベース|test_database|
|スキーマ    |test_schema|

また、スキーマ検索パスを`test_user`が、`test_database`にログインした際、`test_schema`に設定します。

```bash
$ cd build/
$ ./sample_execute ../sample/command_create_schema.sql
```

### 2. テスト用テーブルとデータの作成

build/に移動して、`sample_make_data`を使用して、テスト用の環境を作成します。これにより、データベースに以下のオブジェクトを作成します。

|オブジェクト|名称|
|-----------|----|
|テーブル|test_table|

テーブル作成後、ダミーデータを100件作成してテーブルにインサートします。

```bash
$ cd build/
$ ./sample_make_data
```

### 3. テストデータの確認

build/に移動して、データの格納状況を確認します。以下のコマンドを使用してデータの内容を確認します。

|コマンド|パラメータ|備考|
|-------|----|----|
|sample_cursor|なし|SQLでカーソルを宣言、利用するサンプルです。|
|sample_cursor_fetch|なし|独自のカーソル関数を使用するサンプルです。|
|sample_cursor_prepare|なし|★カーソルとEXECUTEの同時宣言が**できない例**です。|
|sample_fetch|なし|各フィールドの型が明確な場合に、その型に合わせた変数でデータを取得します。|
|sample_thread_read|※１|指定したスキーマにあるテーブルをすべてファイルにCSV形式で出力します。|

>※１：パラメータなしで実行すると、パラメータの説明が出力されます。

```bash
./sample_thread_read <parameters>
 parameters are ...
  -H or --host    <database server name>
  -P or --port    <port number>
  -D or --dbname  <database name>
  -S or --schema  <schema name>
  -u or --user    <login user name>
  -p or --pass    <login password>
  -s or --service <service key>
  -o or --output  <output directory> default is '.'
```

実行例です。build/ディレクトリに、export/ディレクトリを作成してください。

```bash
./sample_thread_read --host localhost --port 5432 --dbname test_database --schema test_schema --user test_user --pass testuser --output ./export
```

_最終更新日 2026-08-29_