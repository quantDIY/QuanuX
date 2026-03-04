# cython: language_level=3, boundscheck=False, wraparound=False

def json_to_markdown(list data_rows, list columns):
    """
    C-level string manipulation to instantly convert massive Hasura JSON dictionaries
    into pristine Markdown tables, entirely bypassing Python formatting overhead.
    """
    cdef str markdown_table = "|"
    cdef str header_divider = "|"
    cdef str col
    cdef dict row
    cdef str val
    
    # 1. Header Construction
    for col in columns:
        markdown_table += f" {col} |"
        header_divider += "---|"
        
    markdown_table += "\n" + header_divider + "\n"
    
    # 2. Row Iteration
    for row in data_rows:
        markdown_table += "|"
        for col in columns:
            val = str(row.get(col, ""))
            markdown_table += f" {val} |"
        markdown_table += "\n"
        
    return markdown_table
