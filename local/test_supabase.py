import os
import supabase
from supabase import create_client, Client

SUPABASE_URL = os.environ.get('SUPABASE_URL')
SUPABASE_API_KEY = os.environ.get('SUPABASE_API_KEY')

supabase_client = create_client(SUPABASE_URL, SUPABASE_API_KEY)

# Query data from a table
def query_data():
    print("Querying data")

    # Replace 'your-table-name' with the name of your table
    table_name = 'bintable'
    response = supabase_client.from_(table_name).select('*').execute()

    data = response.data
    print("Printing rows in data.......")
    for row in data:
        print("ROW:", row, "\n")

# Insert a new record into a table
def insert_data():
    print("Inserting data")
    # Replace 'your-table-name' with the name of your table
    table_name = 'bintable'
    data_to_insert = {
        "photo_path": "temp.jpg",
        "class": 1
    }

    response = supabase_client.table(table_name).insert(data_to_insert).execute()

if __name__ == "__main__":
    query_data()
    insert_data()
    query_data()
