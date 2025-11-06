## CGI Script to handle file uploads

import cgi
import os
import sys

print("Content-Type: text/plain")
print()  # End of headers

def main():
    form = cgi.FieldStorage()

    # Directory to save uploaded files
    upload_dir = "/home/qmennen/Documents/webserv/htdocs/site-1/uploads"
    if not os.path.exists(upload_dir):
        os.makedirs(upload_dir)

    # Process each file in the form
    for field in form.keys():
        field_item = form[field]
        if field_item.filename:
            # Get the filename and file data
            filename = os.path.basename(field_item.filename)
            file_data = field_item.file.read()

            # Save the file to the upload directory
            with open(os.path.join(upload_dir, filename), 'wb') as f:
                f.write(file_data)

            print(f"Uploaded file: {filename} ({len(file_data)} bytes) to {upload_dir}")
        else:
            print(f"No file uploaded for field: {field}")

if __name__ == "__main__":
    main()