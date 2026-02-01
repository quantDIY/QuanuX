import os

source_path = "extensions/cpp/quickfix/source/spec/FIX42.xml"
dest_path = "extensions/cpp/ibkr_fix/config/FIX42-IBKR.xml"

custom_fields = """
  <field name='IBKRLocalSymbol' number='6035' type='STRING' />
  <field name='OrderReference' number='6010' type='STRING' />
  <field name='OrderCapacity' number='6122' type='STRING' />
  <field name='ShortSaleRule' number='6086' type='INT' />
  <field name='ShortSellAvailability' number='6236' type='INT' />
"""

try:
    with open(source_path, 'r') as f:
        content = f.read()
    
    # Inject fields before </fields> (if it exists, FIX42.xml usually has fields then messages)
    # The view_file output showed <header> then <messages>. 
    # Wait, the view_file output did NOT show a <fields> section at the top. 
    # It started with <header>, then <messages>.
    # Standard QuickFIX XMLs usually have <fields> at the end or begin.
    # Let's check if <fields> is in the file.
    
    if "<fields>" in content:
        # It has fields section
        new_content = content.replace("</fields>", custom_fields + "</fields>")
    else:
        # Sometimes fields are defined inline or it's a different structure.
        # But QuickFIX usually requires a <fields> section. 
        # Let's inspect the end of the file.
        # If we can't find it, we will append it inside the root <fix> tag.
        if "</fix>" in content:
             new_content = content.replace("</fix>", "<fields>" + custom_fields + "</fields>\n</fix>")
        else:
             print("Error: Invalid FIX XML structure")
             exit(1)

    os.makedirs(os.path.dirname(dest_path), exist_ok=True)
    with open(dest_path, 'w') as f:
        f.write(new_content)
    print(f"Successfully created {dest_path}")

except Exception as e:
    print(f"Error: {e}")
