# [deprecated] postgres
# import psycopg2
# from psycopg2 import sql
# from utils.config import config
# from utils.db_commands import DB_NAME, TABLE_NAME
# import utils.db_commands as command

def stuff():
    # current_time = datetime.datetime.now()

    # data = {
    #     "timestamp": current_time,
    #     "binid": binupdate[0],
    #     "bintype": binupdate[1],
    #     "fill": binupdate[2]
    # }

    # insert_query = """
    # INSERT INTO bintable (timestamp, binid, bintype, fill)
    # VALUES (%s, %s, %s, %s)
    # """

    # CUR.execute(insert_query, (data["timestamp"], data["binid"], data["bintype"], data["fill"]))
    # CONN.commit()

# Connect to PostgreSQL database server 
def db_connect():
    conn = None
    try:
        # read connection parameters
        params = config()

        # connect to the PostgreSQL server
        print('Connecting to the PostgreSQL database...')
        conn = psycopg2.connect(**params)
        cur = conn.cursor()
        # cur.execute(command.DROP_DB_IF_EXISTS)
        # cur.execute(command.CREATE_DB)

        # cur.close()
        # conn.close()

        # # New db new me 
        # conn = psycopg2.connect(**params)
        # cur = conn.cursor()

        cur.execute(command.CREATE_TABLE)
        conn.commit()
        
        # Query to list tables in the database
        list_tables_query = """
        SELECT table_name
        FROM information_schema.tables
        WHERE table_schema = 'public'
        """

        # Execute the query
        cur.execute(list_tables_query)

        # Fetch all table names
        table_names = cur.fetchall()

        # Print the list of table names
        for table in table_names:
            print("Table name: " + table[0])

        CONN = conn
        CUR = cur

    except (Exception, psycopg2.DatabaseError) as error:
        print("Error: " + error)

def db_list_tables(): 
    conn = None
    list_tables_query = """
    SELECT table_name
    FROM information_schema.tables
    WHERE table_schema = 'public'
    """

    try:
        params = config()
        conn = psycopg2.connect(**params)
        cur = conn.cursor()

        cur.execute(list_tables_query)

        table_names = cur.fetchall()
        for table in table_names:
            print("Table name: " + table[0])

        CONN = conn
        CUR = cur

    except (Exception, psycopg2.DatabaseError) as error:
        print("Error: " + error)