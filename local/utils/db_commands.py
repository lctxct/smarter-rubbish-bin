DB_NAME = "bindb"
TABLE_NAME = "bintable"

DROP_DB_IF_EXISTS = f"DROP DATABASE IF EXISTS {DB_NAME}"
CREATE_DB  = f"CREATE DATABASE {DB_NAME}"
CREATE_TABLE = f"""
CREATE TABLE {TABLE_NAME} (
    timestamp TIMESTAMP,
    binid     INTEGER,
    bintype   INTEGER,
    fill      FLOAT
)
"""

