from pypdf import PdfReader

reader = PdfReader("research/IBKR-FIX-Specification.pdf")
text = ""
for page in reader.pages:
    text += page.extract_text() + "\n"

with open("research/ibkr_fix.txt", "w") as f:
    f.write(text)

print(f"Extracted {len(text)} characters.")
