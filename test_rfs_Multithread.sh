# Now start the client
echo "Starting the client to perform operations..." 

# Test file names
LOCAL_ORIGINAL_FILE="a.txt"
REMOTE_TEST_FILE="b.txt"
LOCAL_COPY_FILE="c.txt"


# Create a test file
echo "Creating test file..."
echo "THIS IS A FILE.
THIS IS A FILE?
THIS IS A FILE!
THIS IS A FILE.
" > "local/a.txt"

# Test 0: Show HELP information
echo "Testing HELP..."
./client rfs HELP

sleep 2

# Test 1: Write a file to the server
echo "Test 1: Writing file to server..."
./client rfs WRITE "$LOCAL_ORIGINAL_FILE" "$REMOTE_TEST_FILE" 1

sleep 5

# Test 2: Get the file back from the server
echo "Test 2: Retrieving file from server..."
./client rfs GET "$REMOTE_TEST_FILE" "$LOCAL_COPY_FILE"

sleep 5

# Test 3: Delete the file from the server
echo "Test 3: Deleting file from server..."
./client rfs RM "$REMOTE_TEST_FILE"

sleep 5

# Test 4: Attempt to retrieve the deleted file (should fail)
echo "Test 4: Attempting to retrieve deleted file..."
./client rfs GET "$REMOTE_TEST_FILE"

sleep 5

# Test 5: Write with read-only permission, attempt delete (should fail)
echo "Test 5: Testing permission and deletion..."
./client rfs WRITE "$LOCAL_ORIGINAL_FILE" "$REMOTE_TEST_FILE" 0
./client rfs RM "$REMOTE_TEST_FILE"

sleep 5

# Test 6: Write with only local file path
echo "Test 6: Writing file to server with only local file path..."
./client rfs WRITE "$LOCAL_ORIGINAL_FILE" 1

sleep 5

# Test 7: Get with only remote file path
echo "Test 7: Retrieving file from server with only remote file path..."
./client rfs GET "$REMOTE_TEST_FILE"

sleep 5

# Test 8: Check file permission
echo "Test 8: Checking file permission..."
./client rfs LS "$REMOTE_TEST_FILE"

# Clean up local test files
echo "Cleaning up test files..."
rm -f "local/$LOCAL_ORIGINAL_FILE" "file/$REMOTE_TEST_FILE" "local/$REMOTE_TEST_FILE" "file/$LOCAL_ORIGINAL_FILE" "local/$LOCAL_COPY_FILE" "meta/$LOCAL_ORIGINAL_FILE" "meta/$REMOTE_TEST_FILE"

echo "Tests completed."


echo "Script completed."
