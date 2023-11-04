
DROP DATABASE IF EXISTS bindb;

CREATE DATABASE bindb;

\c bindb

CREATE TABLE bintable (
    curtime    TIMESTAMP,
    binid      INTEGER,       -- probably 0
    itemclass  INTEGER,       -- classification
    itempath   VARCHAR(255),  -- upload/<hash>.jpg
    fill_R     FLOAT,
    fill_T     FLOAT
); 

\q
