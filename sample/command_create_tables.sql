@connect host=localhost dbname=test_database user=test_user password=testuser

CREATE TABLE sample_table (
   id              INTEGER PRIMARY KEY,
   int_value       INTEGER,
   bigint_value    BIGINT,
   numeric_value   NUMERIC(8,5),
   real_value      REAL,
   double_value    DOUBLE PRECISION,
   char_value      CHAR(10),
   varchar_value   VARCHAR(100),
   text_value      TEXT,
   date_value      DATE,
   timestamp_value TIMESTAMP,
   bool_value      BOOLEAN
);

INSERT INTO sample_table VALUES (
    1,
    101,
    10101,
    3.14,
    1.41421356,
    12.121212,
    'char001',
    'varchar 001',
    'text No.001',
    '2026-08-22',
    '2026-08-22 12:13:14',
    'f'
);

@disconnect