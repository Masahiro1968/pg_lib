-- 下記の内容で接続します。
@connect host=localhost dbname=postgres user=postgres password=postgres

-- 過去に作成した環境(データベース)を削除します。
DROP DATABASE IF EXISTS "test_database";

-- 過去に作成した環境(ユーザー)を削除します。
DROP USER IF EXISTS "test_user";

-- ユーザーを作成します。
CREATE USER "test_user" WITH PASSWORD 'testuser';

-- 対象ユーザーにファイル読み込み専用ロールを付与します。
GRANT pg_read_server_files TO "test_user";

-- データベースを作成します。
CREATE DATABASE "test_database" OWNER "test_user";

-- 切断します。
@disconnect

-- 再接続します。
@connect host=localhost dbname=test_database user=test_user password=testuser

-- スキーマを作成します。
CREATE SCHEMA "test_schema";

-- test_userが、test_databaseに接続する場合、スキーマ検索パスにtest_schemaを指定します。
ALTER ROLE "test_user" IN DATABASE "test_database" SET search_path TO "test_schema";

-- 切断します。
@disconnect