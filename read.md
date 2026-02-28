============================================================
🎵 PROJECT NAME  : MP3 Tag Editor (ID3v2)
📅 DURATION      : Nov 2025 - Dec 2025
🏫 INSTITUTE     : Emertxe Information Technologies
💻 LANGUAGE      : C Programming
============================================================

📌 DESCRIPTION
------------------------------------------------------------
• Developed a command-line MP3 Tag Editor.
• Reads and modifies ID3v2 metadata of MP3 files.
• Allows viewing and editing of Title, Artist, Album,
  Year, Genre, and Comment fields.
• Performs safe file modification using a temporary file.

🎯 OBJECTIVE
------------------------------------------------------------
• Understand MP3 file structure and ID3v2 format.
• Perform metadata manipulation using low-level file handling.
• Implement big-endian and little-endian conversions.

🛠 TECHNOLOGIES USED
------------------------------------------------------------
✔ C Programming
✔ File Handling (fopen, fread, fwrite, fclose)
✔ Command Line Arguments (argc, argv)
✔ Structures
✔ String Handling (strcmp, strcpy, strlen)
✔ Endian Conversion
✔ Bitwise Operations
✔ Binary File Processing

✨ FEATURES
------------------------------------------------------------
✔ View MP3 metadata (-v option)
✔ Edit MP3 metadata (-e option)
✔ Supports ID3v2 frames:
    - TIT2 (Title)
    - TPE1 (Artist)
    - TALB (Album)
    - TYER (Year)
    - TCON (Genre)
    - COMM (Comment)
✔ Validates .mp3 file extension
✔ Validates frame ID format
✔ Sync-safe integer size calculation
✔ Big-endian ↔ Little-endian conversion
✔ Temporary file update mechanism
✔ Preserves original audio data
✔ Simple CLI interface

⚙ WORKING PRINCIPLE
------------------------------------------------------------
➤ Read first 10 bytes to verify "ID3" header.
➤ Extract tag size using sync-safe integer method.
➤ Iterate through frames (Frame ID + Size + Flags + Data).
➤ For view:
     → Print metadata fields.
➤ For edit:
     → Match frame ID.
     → Modify frame content.
     → Update size in big-endian format.
     → Write changes into temp file.
➤ Replace original file after successful update.

============================================================
