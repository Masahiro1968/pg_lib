@connect host=localhost dbname=postgres user=postgres password=postgres

DROP DATABASE IF EXISTS "test_database";

DROP USER IF EXISTS "test_user";

CREATE USER "test_user" WITH PASSWORD 'testuser';

CREATE DATABASE "test_database" OWNER "test_user";

@disconnect

@connect host=localhost dbname=test_database user=test_user password=testuser

CREATE SCHEMA "test_schema";

ALTER ROLE "test_user" IN DATABASE "test_database" SET search_path TO "test_schema";

@disconnect